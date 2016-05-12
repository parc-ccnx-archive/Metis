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

//
//  metis_MatchingRulesTable.c
//  Metis
//
//  Created by Mosko, Marc <Marc.Mosko@parc.com> on 11/29/13.

#include <config.h>
#include <stdio.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Hash.h>

#include <ccnx/forwarder/metis/processor/metis_HashTableFunction.h>
#include <ccnx/forwarder/metis/processor/metis_MatchingRulesTable.h>
#include <LongBow/runtime.h>

struct metis_matching_rules_table {
    // we maintain three hash tables indexed by the different ways
    // one could ask for something.  THis means a content object needs
    // to do three lookups.  We can optimize this later.

    PARCHashCodeTable *tableByName;
    PARCHashCodeTable *tableByNameAndKeyId;
    PARCHashCodeTable *tableByNameAndObjectHash;

    PARCHashCodeTable_Destroyer dataDestroyer;
};

static PARCHashCodeTable *metisMatchingRulesTable_GetTableForMessage(const MetisMatchingRulesTable *pit, const MetisMessage *interestMessage);

// ======================================================================

MetisMatchingRulesTable *
metisMatchingRulesTable_Create(PARCHashCodeTable_Destroyer dataDestroyer)
{
    size_t initialSize = 65535;

    MetisMatchingRulesTable *table = parcMemory_AllocateAndClear(sizeof(MetisMatchingRulesTable));
    assertNotNull(table, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisMatchingRulesTable));
    table->dataDestroyer = dataDestroyer;

    // There is not a Key destroyer because we use the message from the MetisPitEntry as the key

    table->tableByName = parcHashCodeTable_Create_Size(metisHashTableFunction_MessageNameEquals,
                                                       metisHashTableFunction_MessageNameHashCode,
                                                       NULL,
                                                       dataDestroyer,
                                                       initialSize);

    table->tableByNameAndKeyId = parcHashCodeTable_Create_Size(metisHashTableFunction_MessageNameAndKeyIdEquals,
                                                               metisHashTableFunction_MessageNameAndKeyIdHashCode,
                                                               NULL,
                                                               dataDestroyer,
                                                               initialSize);

    table->tableByNameAndObjectHash = parcHashCodeTable_Create_Size(metisHashTableFunction_MessageNameAndObjectHashEquals,
                                                                    metisHashTableFunction_MessageNameAndObjectHashHashCode,
                                                                    NULL,
                                                                    dataDestroyer,
                                                                    initialSize);
    return table;
}

void
metisMatchingRulesTable_Destroy(MetisMatchingRulesTable **tablePtr)
{
    assertNotNull(tablePtr, "Parameter must be non-null double pointer");
    assertNotNull(*tablePtr, "Parameter must dereference to non-null pointer");

    MetisMatchingRulesTable *table = *tablePtr;

    parcHashCodeTable_Destroy(&table->tableByNameAndObjectHash);
    parcHashCodeTable_Destroy(&table->tableByNameAndKeyId);
    parcHashCodeTable_Destroy(&table->tableByName);

    parcMemory_Deallocate((void **) &table);
    *tablePtr = NULL;
}

void *
metisMatchingRulesTable_Get(const MetisMatchingRulesTable *rulesTable, const MetisMessage *message)
{
    assertNotNull(rulesTable, "Parameter rulesTable must be non-null");
    assertNotNull(message, "Parameter message must be non-null");

    PARCHashCodeTable *hashTable = metisMatchingRulesTable_GetTableForMessage(rulesTable, message);
    return parcHashCodeTable_Get(hashTable, message);
}

PARCArrayList *
metisMatchingRulesTable_GetUnion(const MetisMatchingRulesTable *table, const MetisMessage *message)
{
    // we can have at most 3 results, so create with that capacity
    PARCArrayList *list = parcArrayList_Create_Capacity(NULL, NULL, 3);

    void *dataByName = parcHashCodeTable_Get(table->tableByName, message);
    if (dataByName) {
        parcArrayList_Add(list, dataByName);
    }

    if (metisMessage_HasKeyId(message)) {
        void *dataByNameAndKeyId = parcHashCodeTable_Get(table->tableByNameAndKeyId, message);
        if (dataByNameAndKeyId) {
            parcArrayList_Add(list, dataByNameAndKeyId);
        }
    }

    if (metisMessage_HasContentObjectHash(message)) {
        void *dataByNameAndObjectHash = parcHashCodeTable_Get(table->tableByNameAndObjectHash, message);
        if (dataByNameAndObjectHash) {
            parcArrayList_Add(list, dataByNameAndObjectHash);
        }
    }

    return list;
}

void
metisMatchingRulesTable_RemoveFromBest(MetisMatchingRulesTable *rulesTable, const MetisMessage *message)
{
    assertNotNull(rulesTable, "Parameter rulesTable must be non-null");
    assertNotNull(message, "Parameter message must be non-null");

    PARCHashCodeTable *hashTable = metisMatchingRulesTable_GetTableForMessage(rulesTable, message);
    parcHashCodeTable_Del(hashTable, message);
}

void
metisMatchingRulesTable_RemoveFromAll(MetisMatchingRulesTable *rulesTable, const MetisMessage *message)
{
    assertNotNull(rulesTable, "Parameter rulesTable must be non-null");
    assertNotNull(message, "Parameter message must be non-null");

    parcHashCodeTable_Del(rulesTable->tableByName, message);

    // not all messages have a keyid any more
    if (metisMessage_HasKeyId(message)) {
        parcHashCodeTable_Del(rulesTable->tableByNameAndKeyId, message);
    }

    if (metisMessage_HasContentObjectHash(message)) {
        parcHashCodeTable_Del(rulesTable->tableByNameAndObjectHash, message);
    }
}

bool
metisMatchingRulesTable_AddToBestTable(MetisMatchingRulesTable *rulesTable, MetisMessage *key, void *data)
{
    assertNotNull(rulesTable, "Parameter rulesTable must be non-null");
    assertNotNull(key, "Parameter key must be non-null");
    assertNotNull(data, "Parameter data must be non-null");

    PARCHashCodeTable *hashTable = metisMatchingRulesTable_GetTableForMessage(rulesTable, key);

    bool success = parcHashCodeTable_Add(hashTable, key, data);

    return success;
}

void
metisMatchingRulesTable_AddToAllTables(MetisMatchingRulesTable *rulesTable, MetisMessage *key, void *data)
{
    assertNotNull(rulesTable, "Parameter rulesTable must be non-null");
    assertNotNull(key, "Parameter key must be non-null");
    assertNotNull(data, "Parameter data must be non-null");

    parcHashCodeTable_Add(rulesTable->tableByName, key, data);

    // not all messages have a keyid any more
    if (metisMessage_HasKeyId(key)) {
        parcHashCodeTable_Add(rulesTable->tableByNameAndKeyId, key, data);
    }

    parcHashCodeTable_Add(rulesTable->tableByNameAndObjectHash, key, data);
}

// ========================================================================================

static PARCHashCodeTable *
metisMatchingRulesTable_GetTableForMessage(const MetisMatchingRulesTable *pit, const MetisMessage *interestMessage)
{
    PARCHashCodeTable *table;
    if (metisMessage_HasContentObjectHash(interestMessage)) {
        table = pit->tableByNameAndObjectHash;
    } else if (metisMessage_HasKeyId(interestMessage)) {
        table = pit->tableByNameAndKeyId;
    } else {
        table = pit->tableByName;
    }

    return table;
}
