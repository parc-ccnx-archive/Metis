/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Patent rights are not granted under this agreement. Patent rights are
 *       available under FRAND terms.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * Right now, the FIB table is sparse.  There can be an entry for /a and for /a/b/c, but
 * not for /a/b.  This means we need to exhastively lookup all the components to make sure
 * there's not a route for it.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>

#include <ccnx/forwarder/metis/processor/metis_FIB.h>
#include <ccnx/forwarder/metis/processor/metis_FibEntry.h>
#include <ccnx/forwarder/metis/processor/metis_HashTableFunction.h>
#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_TreeRedBlack.h>

#include <LongBow/runtime.h>

// =====================================================

/**
 * @function hashTableFunction_FibEntryDestroyer
 * @abstract Used in the hash table to destroy the data pointer when an item's removed
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
static void
_hashTableFunction_FibEntryDestroyer(void **dataPtr)
{
    metisFibEntry_Release((MetisFibEntry **) dataPtr);
}

/**
 * @function hashTableFunction_TlvNameDestroyer
 * @abstract Used in the hash table to destroy the key pointer when an item's removed
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
static void
_hashTableFunction_TlvNameDestroyer(void **dataPtr)
{
    metisTlvName_Release((MetisTlvName **) dataPtr);
}

// =====================================================

struct metis_fib {
    // KEY = tlvName, VALUE = FibEntry
    PARCHashCodeTable *tableByName;

    // KEY = tlvName.  We use a tree for the keys because that
    // has the same average insert and remove time.  The tree
    // is only used by GetEntries, which in turn is used by things
    // that want to enumerate the FIB
    PARCTreeRedBlack *tableOfKeys;

    MetisLogger *logger;

    // If there are no forward paths, we return an emtpy set.  Allocate this
    // once and return a reference to it whenever we need an empty set.
    MetisNumberSet *emptySet;
};

static MetisFibEntry *_metisFIB_CreateFibEntry(MetisFIB *fib, MetisTlvName *tlvName);

// =====================================================
// Public API

MetisFIB *
metisFIB_Create(MetisLogger *logger)
{
    unsigned initialSize = 1024;

    MetisFIB *fib = parcMemory_AllocateAndClear(sizeof(MetisFIB));
    assertNotNull(fib, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisFIB));
    fib->emptySet = metisNumberSet_Create();
    fib->logger = metisLogger_Acquire(logger);
    fib->tableByName = parcHashCodeTable_Create_Size(metisHashTableFunction_TlvNameEquals,
                                                     metisHashTableFunction_TlvNameHashCode,
                                                     _hashTableFunction_TlvNameDestroyer,
                                                     _hashTableFunction_FibEntryDestroyer,
                                                     initialSize);

    fib->tableOfKeys =
        parcTreeRedBlack_Create(metisHashTableFunction_TlvNameCompare, NULL, NULL, NULL, NULL, NULL);

    if (metisLogger_IsLoggable(fib->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
        metisLogger_Log(fib->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                        "FIB %p created with initialSize %u",
                        (void *) fib, initialSize);
    }

    return fib;
}

void
metisFIB_Destroy(MetisFIB **fibPtr)
{
    assertNotNull(fibPtr, "Parameter must be non-null double pointer");
    assertNotNull(*fibPtr, "Parameter must dereference to non-null pointer");

    MetisFIB *fib = *fibPtr;

    if (metisLogger_IsLoggable(fib->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
        metisLogger_Log(fib->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                        "FIB %p destroyed",
                        (void *) fib);
    }

    metisNumberSet_Release(&fib->emptySet);
    metisLogger_Release(&fib->logger);
    parcTreeRedBlack_Destroy(&fib->tableOfKeys);
    parcHashCodeTable_Destroy(&fib->tableByName);
    parcMemory_Deallocate((void **) &fib);
    *fibPtr = NULL;
}

const MetisNumberSet *
metisFIB_Match(MetisFIB *fib, const MetisMessage *interestMessage)
{
    assertNotNull(fib, "Parameter fib must be non-null");
    assertNotNull(interestMessage, "Parameter interestMessage must be non-null");

    if (metisMessage_HasName(interestMessage)) {
        // this is NOT reference counted, don't destroy it
        MetisTlvName *tlvName = metisMessage_GetName(interestMessage);
        MetisFibEntry *longestMatchingFibEntry = NULL;

        // because the FIB table is sparse, we need to scan all the name segments in order.
        for (size_t i = 0; i < metisTlvName_SegmentCount(tlvName); i++) {
            MetisTlvName *prefixName = metisTlvName_Slice(tlvName, i + 1);
            MetisFibEntry *fibEntry = parcHashCodeTable_Get(fib->tableByName, prefixName);
            if (fibEntry != NULL) {

                // we can accept the FIB entry if it does not contain the ingress connection id or if
                // there is more than one forward path besides the ingress connection id.
                const MetisNumberSet *nexthops = metisFibEntry_GetNexthops(fibEntry);
                if (!metisNumberSet_Contains(nexthops, metisMessage_GetIngressConnectionId(interestMessage)) || metisNumberSet_Length(nexthops) > 1) {
                    longestMatchingFibEntry = fibEntry;
                }
            }
            metisTlvName_Release(&prefixName);
        }

        if (longestMatchingFibEntry != NULL) {
            // this returns a reference counted copy of the next hops
            return metisFibEntry_GetNexthops(longestMatchingFibEntry);
        }
    }

    // return an empty set
    return fib->emptySet;
}

bool
metisFIB_AddOrUpdate(MetisFIB *fib, CPIRouteEntry *route)
{
    assertNotNull(fib, "Parameter fib must be non-null");
    assertNotNull(route, "Parameter route must be non-null");

    const CCNxName *ccnxName = cpiRouteEntry_GetPrefix(route);
    unsigned interfaceIndex = cpiRouteEntry_GetInterfaceIndex(route);
    MetisTlvName *tlvName = metisTlvName_CreateFromCCNxName(ccnxName);

    MetisFibEntry *fibEntry = parcHashCodeTable_Get(fib->tableByName, tlvName);
    if (fibEntry == NULL) {
        fibEntry = _metisFIB_CreateFibEntry(fib, tlvName);
    }

    metisFibEntry_AddNexthop(fibEntry, interfaceIndex);

    // if anyone saved the name in a table, they copied it.
    metisTlvName_Release(&tlvName);

    return true;
}

bool
metisFIB_Remove(MetisFIB *fib, CPIRouteEntry *route)
{
    assertNotNull(fib, "Parameter fib must be non-null");
    assertNotNull(route, "Parameter route must be non-null");

    bool routeRemoved = false;

    const CCNxName *ccnxName = cpiRouteEntry_GetPrefix(route);
    unsigned interfaceIndex = cpiRouteEntry_GetInterfaceIndex(route);
    MetisTlvName *tlvName = metisTlvName_CreateFromCCNxName(ccnxName);

    MetisFibEntry *fibEntry = parcHashCodeTable_Get(fib->tableByName, tlvName);
    if (fibEntry != NULL) {
        metisFibEntry_RemoveNexthop(fibEntry, interfaceIndex);
        if (metisFibEntry_NexthopCount(fibEntry) == 0) {
            parcTreeRedBlack_Remove(fib->tableOfKeys, tlvName);

            // this will de-allocate the key, so must be done last
            parcHashCodeTable_Del(fib->tableByName, tlvName);

            routeRemoved = true;
        }
    }

    metisTlvName_Release(&tlvName);
    return routeRemoved;
}

size_t
metisFIB_Length(const MetisFIB *fib)
{
    assertNotNull(fib, "Parameter fib must be non-null");
    return parcHashCodeTable_Length(fib->tableByName);
}

MetisFibEntryList *
metisFIB_GetEntries(const MetisFIB *fib)
{
    assertNotNull(fib, "Parameter fib must be non-null");
    MetisFibEntryList *list = metisFibEntryList_Create();

    PARCArrayList *values = parcTreeRedBlack_Values(fib->tableOfKeys);
    for (size_t i = 0; i < parcArrayList_Size(values); i++) {
        MetisFibEntry *original = (MetisFibEntry *) parcArrayList_Get(values, i);
        metisFibEntryList_Append(list, original);
    }
    parcArrayList_Destroy(&values);
    return list;
}

void
metisFIB_RemoveConnectionIdFromRoutes(MetisFIB *fib, unsigned connectionId)
{
    assertNotNull(fib, "Parameter fib must be non-null");

    // NB: This is very inefficient.  We need a reverse-index from connectionId to FibEntry (case 814)

    // Walk the entire tree and remove the connection id from every entry.
    PARCArrayList *values = parcTreeRedBlack_Values(fib->tableOfKeys);
    for (size_t i = 0; i < parcArrayList_Size(values); i++) {
        MetisFibEntry *original = (MetisFibEntry *) parcArrayList_Get(values, i);
        metisFibEntry_RemoveNexthop(original, connectionId);
    }
    parcArrayList_Destroy(&values);
}

// =========================================================================
// Private API

/**
 * @function metisFib_CreateFibEntry
 * @abstract Create the given FIB entry
 * @discussion
 *    PRECONDITION: You know that the FIB entry does not exist already
 *
 * @param <#param1#>
 * @return <#return#>
 */
static MetisFibEntry *
_metisFIB_CreateFibEntry(MetisFIB *fib, MetisTlvName *tlvName)
{
    MetisFibEntry *entry = metisFibEntry_Create(tlvName);

    // add a reference counted name, as we specified a key destroyer when we
    // created the table.
    MetisTlvName *copy = metisTlvName_Acquire(tlvName);
    parcHashCodeTable_Add(fib->tableByName, copy, entry);

    // this is an index structure.  It does not have its own destroyer functions in
    // the data structure.  The data in this table is the same pointer as in the hash table.
    parcTreeRedBlack_Insert(fib->tableOfKeys, copy, entry);

    return entry;
}
