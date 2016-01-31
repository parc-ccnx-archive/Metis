/*
 * Copyright (c) 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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
 * @author Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#include <config.h>
#include <LongBow/runtime.h>

#include <ccnx/forwarder/metis/content_store/metis_TimeOrderedList.h>

#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_TreeRedBlack.h>

/**
 * A list of MetisContentStoreEntrys, kept in sorted order by time. The ordering is calculated by a
 * key compare function (e.g. {@link MetisTimeOrderList_KeyCompare}), passed in.
 *
 * This container does not hold references to the objects that it contains. In other words, it does not Acquire()
 * the MetisMessages that are placed in it. That reference count is managed by the owning ContentStore. This is
 * purely an index, and provides an easy to way index MetisMessages based on a specified time value. Typically,
 * that would be their Recommended Cache Time, or Expiration Time.
 *
 * It maintains a tree, sorted by the time values passed in to the Add() function. It does not manage capacity,
 * and can grow uncontrollably if the owning ContentStore does not manage it. Items are indexed first by time, then
 * address of the MetisMessage (just as a distringuishing attribute). This allows us to store multiple items with
 * the same expiration time.
 */

struct metis_timeordered_list {
    PARCTreeRedBlack *timeOrderedTree;
};

static void
_finalRelease(MetisTimeOrderedList **listP)
{
    MetisTimeOrderedList *list = *listP;
    parcTreeRedBlack_Destroy(&list->timeOrderedTree);
}

parcObject_ExtendPARCObject(MetisTimeOrderedList, _finalRelease, NULL, NULL, NULL, NULL, NULL, NULL);

parcObject_ImplementAcquire(metisTimeOrderedList, MetisTimeOrderedList);

parcObject_ImplementRelease(metisTimeOrderedList, MetisTimeOrderedList);


MetisTimeOrderedList *
metisTimeOrderedList_Create(MetisTimeOrderList_KeyCompare *keyCompareFunction)
{
    MetisTimeOrderedList *result = parcObject_CreateInstance(MetisTimeOrderedList);
    if (NULL != result) {
        result->timeOrderedTree = parcTreeRedBlack_Create(keyCompareFunction, // keyCompare
                                                          NULL,               // keyFree
                                                          NULL,               // keyCopy
                                                          NULL,               // valueEquals
                                                          NULL,               // valueFree
                                                          NULL);              // valueCopy
    }
    return result;
}

void
metisTimeOrderedList_Add(MetisTimeOrderedList *list, MetisContentStoreEntry *entry)
{
    parcTreeRedBlack_Insert(list->timeOrderedTree, entry, entry);
}

MetisContentStoreEntry *
metisTimeOrderedList_GetOldest(MetisTimeOrderedList *list)
{
    return parcTreeRedBlack_FirstKey(list->timeOrderedTree);
}

bool
metisTimeOrderedList_Remove(MetisTimeOrderedList *list, MetisContentStoreEntry *storeEntry)
{
    bool result = false;

    MetisContentStoreEntry *entry = (MetisContentStoreEntry *) parcTreeRedBlack_Remove(list->timeOrderedTree, storeEntry);
    if (entry != NULL) {
        result = true;
    }
    return result;
}

size_t
metisTimeOrderedList_Length(MetisTimeOrderedList *list)
{
    return (size_t) parcTreeRedBlack_Size(list->timeOrderedTree);
}


