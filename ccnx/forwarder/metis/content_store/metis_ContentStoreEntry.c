/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX OR PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ################################################################################
 * #
 * # PATENT NOTICE
 * #
 * # This software is distributed under the BSD 2-clause License (see LICENSE
 * # file).  This BSD License does not make any patent claims and as such, does
 * # not act as a patent grant.  The purpose of this section is for each contributor
 * # to define their intentions with respect to intellectual property.
 * #
 * # Each contributor to this source code is encouraged to state their patent
 * # claims and licensing mechanisms for any contributions made. At the end of
 * # this section contributors may each make their own statements.  Contributor's
 * # claims and grants only apply to the pieces (source code, programs, text,
 * # media, etc) that they have contributed directly to this software.
 * #
 * # There is no guarantee that this section is complete, up to date or accurate. It
 * # is up to the contributors to maintain their portion of this section and up to
 * # the user of the software to verify any claims herein.
 * #
 * # Do not remove this header notification.  The contents of this section must be
 * # present in all distributions of the software.  You may only modify your own
 * # intellectual property statements.  Please provide contact information.
 *
 * - Palo Alto Research Center, Inc
 * This software distribution does not grant any rights to patents owned by Palo
 * Alto Research Center, Inc (PARC). Rights to these patents are available via
 * various mechanisms. As of January 2016 PARC has committed to FRAND licensing any
 * intellectual property used by its contributions to this software. You may
 * contact PARC at cipo@parc.com for more information or visit http://www.ccnx.org
 */
/**
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>

#include <parc/algol/parc_Memory.h>
#include <ccnx/forwarder/metis/content_store/metis_ContentStoreEntry.h>

#include <LongBow/runtime.h>

const uint64_t metisContentStoreEntry_MaxExpiryTime = UINT64_MAX;
const uint64_t metisContentStoreEntry_MaxRecommendedCacheTime = UINT64_MAX;

struct metis_contentstore_entry {
    MetisMessage *message;
    MetisLruListEntry *lruEntry;
    unsigned refcount;

    bool hasRecommendedCacheTimeTicks;
    uint64_t recommendedCacheTimeTicks;

    bool hasExpiryTimeTicks;
    uint64_t expiryTimeTicks;
};

MetisContentStoreEntry *
metisContentStoreEntry_Create(MetisMessage *contentMessage, MetisLruList *lruList)
{
    assertNotNull(contentMessage, "Parameter objectMessage must be non-null");

    MetisContentStoreEntry *entry = parcMemory_AllocateAndClear(sizeof(MetisContentStoreEntry));
    assertNotNull(entry, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisContentStoreEntry));
    entry->message = metisMessage_Acquire(contentMessage);
    entry->refcount = 1;
    if (lruList != NULL) {
        entry->lruEntry = metisLruList_NewHeadEntry(lruList, entry);
    }

    entry->hasExpiryTimeTicks = metisMessage_HasExpiryTime(contentMessage);
    entry->hasRecommendedCacheTimeTicks = metisMessage_HasRecommendedCacheTime(contentMessage);

    if (entry->hasExpiryTimeTicks) {
        entry->expiryTimeTicks = metisMessage_GetExpiryTimeTicks(contentMessage);
    }

    if (entry->hasRecommendedCacheTimeTicks) {
        entry->recommendedCacheTimeTicks = metisMessage_GetRecommendedCacheTimeTicks(contentMessage);
    }

    return entry;
}

MetisContentStoreEntry *
metisContentStoreEntry_Acquire(const MetisContentStoreEntry *original)
{
    assertNotNull(original, "Parameter must be non-null");
    ((MetisContentStoreEntry *) original)->refcount++;  // cast to break the const.
    return (MetisContentStoreEntry *) original;
}

void
metisContentStoreEntry_Release(MetisContentStoreEntry **entryPtr)
{
    assertNotNull(entryPtr, "Parameter must be non-null double pointer");
    assertNotNull(*entryPtr, "Parameter must dereference to non-null pointer");

    MetisContentStoreEntry *entry = *entryPtr;
    assertTrue(entry->refcount > 0, "Illegal state: has refcount of 0");

    entry->refcount--;
    if (entry->refcount == 0) {
        if (entry->lruEntry) {
            metisLruList_EntryDestroy(&entry->lruEntry);
        }
        metisMessage_Release(&entry->message);
        parcMemory_Deallocate((void **) &entry);
    }
    *entryPtr = NULL;
}

MetisMessage *
metisContentStoreEntry_GetMessage(const MetisContentStoreEntry *storeEntry)
{
    assertNotNull(storeEntry, "Parameter must be non-null");
    return storeEntry->message;
}

bool
metisContentStoreEntry_HasExpiryTimeTicks(const MetisContentStoreEntry *storeEntry)
{
    assertNotNull(storeEntry, "Parameter must be non-null");
    return storeEntry->hasExpiryTimeTicks;
}

uint64_t
metisContentStoreEntry_GetExpiryTimeTicks(const MetisContentStoreEntry *storeEntry)
{
    assertNotNull(storeEntry, "Parameter must be non-null");
    assertTrue(storeEntry->hasExpiryTimeTicks,
               "storeEntry has no ExpiryTimeTicks. Did you call metisContentStoreEntry_HasExpiryTimeTicks() first?");
    return storeEntry->expiryTimeTicks;
}

bool
metisContentStoreEntry_HasRecommendedCacheTimeTicks(const MetisContentStoreEntry *storeEntry)
{
    assertNotNull(storeEntry, "Parameter must be non-null");
    return storeEntry->hasRecommendedCacheTimeTicks;
}

uint64_t
metisContentStoreEntry_GetRecommendedCacheTimeTicks(const MetisContentStoreEntry *storeEntry)
{
    assertNotNull(storeEntry, "Parameter must be non-null");
    assertTrue(storeEntry->hasRecommendedCacheTimeTicks,
               "storeEntry has no RecommendedCacheTimeTicks. Did you call metisContentStoreEntry_HasRecommendedCacheTimeTicks() first?");
    return storeEntry->recommendedCacheTimeTicks;
}

int
metisContentStoreEntry_CompareRecommendedCacheTime(const MetisContentStoreEntry *value1, const MetisContentStoreEntry *value2)
{
    // A signum comparison. negative if key 1 is smaller, 0 if key1 == key2, greater than 0 if key1 is bigger.

    MetisContentStoreEntry *v1 = (MetisContentStoreEntry *) value1;
    MetisContentStoreEntry *v2 = (MetisContentStoreEntry *) value2;

    if (v1->recommendedCacheTimeTicks < v2->recommendedCacheTimeTicks) {
        return -1;
    } else if (v1->recommendedCacheTimeTicks > v2->recommendedCacheTimeTicks) {
        return +1;
    } else {
        // At this point, the times are the same. Use the address of the MetisMessage as the decider.
        // This allows us to store multiple MetisMessages with the same expiry/cache time.
        if (v1->message < v2->message) {
            return -1;
        } else if (v1->message > v2->message) {
            return +1;
        }
    }

    return 0; // The same MetisMessage has been encountered.
}

int
metisContentStoreEntry_CompareExpiryTime(const MetisContentStoreEntry *value1, const MetisContentStoreEntry *value2)
{
    // A signum comparison. negative if key 1 is smaller, 0 if key1 == key2, greater than 0 if key1 is bigger.

    MetisContentStoreEntry *v1 = (MetisContentStoreEntry *) value1;
    MetisContentStoreEntry *v2 = (MetisContentStoreEntry *) value2;

    if (v1->expiryTimeTicks < v2->expiryTimeTicks) {
        return -1;
    } else if (v1->expiryTimeTicks > v2->expiryTimeTicks) {
        return +1;
    } else {
        // At this point, the times are the same. Use the address of the MetisMessage as the decider.
        // This allows us to store multiple MetisMessages with the same expiry/cache time.
        if (v1->message < v2->message) {
            return -1;
        } else if (v1->message > v2->message) {
            return +1;
        }
    }

    return 0; // The same MetisMessage has been encountered.
}

void
metisContentStoreEntry_MoveToHead(MetisContentStoreEntry *storeEntry)
{
    assertNotNull(storeEntry, "Parameter must be non-null");
    assertNotNull(storeEntry->lruEntry, "MetisContentStoreEntry is not attached to an LRUList");
    if (storeEntry->lruEntry) {
        metisLruList_EntryMoveToHead(storeEntry->lruEntry);
    }
}
