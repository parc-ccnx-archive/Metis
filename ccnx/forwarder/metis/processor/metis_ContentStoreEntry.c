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
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>

#include <parc/algol/parc_Memory.h>
#include <ccnx/forwarder/metis/processor/metis_ContentStoreEntry.h>

#include <LongBow/runtime.h>

struct metis_content_store_entry {
    MetisMessage *message;
    MetisLruListEntry *lruEntry;
    unsigned refcount;
};

MetisContentStoreEntry *
metisContentStoreEntry_Create(MetisMessage *objectMessage, MetisLruList *lruList)
{
    assertNotNull(objectMessage, "Parameter objectMessage must be non-null");

    MetisContentStoreEntry *entry = parcMemory_AllocateAndClear(sizeof(MetisContentStoreEntry));
    assertNotNull(entry, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisContentStoreEntry));
    entry->message = metisMessage_Acquire(objectMessage);
    entry->refcount = 1;
    entry->lruEntry = metisLruList_NewHeadEntry(lruList, entry);

    return entry;
}

MetisContentStoreEntry *
metisContentStoreEntry_Acquire(MetisContentStoreEntry *original)
{
    assertNotNull(original, "Parameter must be non-null");
    original->refcount++;
    return original;
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
        metisLruList_EntryDestroy(&entry->lruEntry);
        metisMessage_Release(&entry->message);
        parcMemory_Deallocate((void **) &entry);
    }
    *entryPtr = NULL;
}

/**
 * @function metisContentStoreEntry_GetMessage
 * @abstract Returns a reference counted copy of the message.
 * @discussion
 *   Caller must use <code>metisMessage_Release()</code> on the returned message
 *
 * @param <#param1#>
 * @return <#return#>
 */
MetisMessage *
metisContentStoreEntry_GetMessage(MetisContentStoreEntry *storeEntry)
{
    assertNotNull(storeEntry, "Parameter must be non-null");
    return metisMessage_Acquire(storeEntry->message);
}

void
metisContentStoreEntry_MoveToHead(MetisContentStoreEntry *storeEntry)
{
    assertNotNull(storeEntry, "Parameter must be non-null");
    metisLruList_EntryMoveToHead(storeEntry->lruEntry);
}
