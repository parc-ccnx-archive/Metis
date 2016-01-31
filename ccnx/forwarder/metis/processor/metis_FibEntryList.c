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
//
//  metis_FibEntryList.c
//  Metis
//
//  Created by Mosko, Marc <Marc.Mosko@parc.com> on 12/18/13.

#include <config.h>
#include <stdio.h>
#include <stdlib.h>

#include <parc/algol/parc_ArrayList.h>
#include <parc/algol/parc_Memory.h>
#include <ccnx/forwarder/metis/processor/metis_FibEntryList.h>
#include <LongBow/runtime.h>

struct metis_fib_entry_list {
    PARCArrayList *listOfFibEntries;
};

static void
metisFibEntryList_ListDestroyer(void **voidPtr)
{
    MetisFibEntry **entryPtr = (MetisFibEntry **) voidPtr;
    metisFibEntry_Release(entryPtr);
}

MetisFibEntryList *
metisFibEntryList_Create()
{
    MetisFibEntryList *fibEntryList = parcMemory_AllocateAndClear(sizeof(MetisFibEntryList));
    assertNotNull(fibEntryList, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisFibEntryList));
    fibEntryList->listOfFibEntries = parcArrayList_Create(metisFibEntryList_ListDestroyer);
    return fibEntryList;
}

void
metisFibEntryList_Destroy(MetisFibEntryList **listPtr)
{
    assertNotNull(listPtr, "Parameter must be non-null double pointer");
    assertNotNull(*listPtr, "Parameter must dereference to non-null pointer");

    MetisFibEntryList *list = *listPtr;
    parcArrayList_Destroy(&list->listOfFibEntries);
    parcMemory_Deallocate((void **) &list);
    listPtr = NULL;
}

void
metisFibEntryList_Append(MetisFibEntryList *list, MetisFibEntry *fibEntry)
{
    assertNotNull(list, "Parameter list must be non-null pointer");
    assertNotNull(fibEntry, "Parameter fibEntry must be non-null pointer");

    MetisFibEntry *copy = metisFibEntry_Acquire(fibEntry);
    parcArrayList_Add(list->listOfFibEntries, copy);
}

size_t
metisFibEntryList_Length(const MetisFibEntryList *list)
{
    assertNotNull(list, "Parameter list must be non-null pointer");
    return parcArrayList_Size(list->listOfFibEntries);
}


const MetisFibEntry *
metisFibEntryList_Get(const MetisFibEntryList *list, size_t index)
{
    assertNotNull(list, "Parameter list must be non-null pointer");
    MetisFibEntry *entry = parcArrayList_Get(list->listOfFibEntries, index);
    return entry;
}
