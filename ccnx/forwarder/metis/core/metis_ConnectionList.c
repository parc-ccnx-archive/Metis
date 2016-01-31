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
//  metis_ConnectionList.c
//  CCNx Control API
//
//  Created by Mosko, Marc <Marc.Mosko@parc.com> on 12/18/13.
//  Copyright (c) 2013 Computer Science Laboratory. All rights reserved.
//
#include <config.h>
#include <stdio.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_ArrayList.h>

#include <ccnx/forwarder/metis/core/metis_ConnectionList.h>
#include <LongBow/runtime.h>

struct metis_connection_list {
    PARCArrayList *listOfConnections;
};

/**
 * PARCArrayList entry destroyer
 */
static void
metisConnectionList_ArrayDestroyer(void **voidPtr)
{
    MetisConnection **entryPtr = (MetisConnection **) voidPtr;
    metisConnection_Release(entryPtr);
}

MetisConnectionList *
metisConnectionList_Create()
{
    MetisConnectionList *list = parcMemory_AllocateAndClear(sizeof(MetisConnectionList));
    assertNotNull(list, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisConnectionList));
    list->listOfConnections = parcArrayList_Create(metisConnectionList_ArrayDestroyer);
    return list;
}

void
metisConnectionList_Destroy(MetisConnectionList **listPtr)
{
    assertNotNull(listPtr, "Parameter must be non-null double pointer");
    assertNotNull(*listPtr, "Parameter must dereference to non-null pointer");
    MetisConnectionList *list = *listPtr;
    parcArrayList_Destroy(&list->listOfConnections);
    parcMemory_Deallocate((void **) &list);
    *listPtr = NULL;
}

void
metisConnectionList_Append(MetisConnectionList *list, MetisConnection *entry)
{
    assertNotNull(list, "Parameter list must be non-null");
    assertNotNull(entry, "Parameter entry must be non-null");

    parcArrayList_Add(list->listOfConnections, metisConnection_Acquire(entry));
}

size_t
metisConnectionList_Length(const MetisConnectionList *list)
{
    assertNotNull(list, "Parameter list must be non-null");
    return parcArrayList_Size(list->listOfConnections);
}

MetisConnection *
metisConnectionList_Get(MetisConnectionList *list, size_t index)
{
    assertNotNull(list, "Parameter list must be non-null");
    MetisConnection *original = (MetisConnection *) parcArrayList_Get(list->listOfConnections, index);
    return original;
}
