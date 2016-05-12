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
 * The pending interest table.
 *
 * Interest aggregation strategy:
 * - The first Interest for a name is forwarded
 * - A second Interest for a name from a different reverse path may be aggregated
 * - A second Interest for a name from an existing Interest is forwarded
 * - The Interest Lifetime is like a subscription time.  A reverse path entry is removed once the lifetime
 *   is exceeded.
 * - Whan an Interest arrives or is aggregated, the Lifetime for that reverse hop is extended.  As a simplification,
 *   we only keep a single lifetime not per reverse hop.
 *
 * Caveats:
 * - Does not support multiple MTUs yet (case 218)
 * - Does not handle the case when an interest comes in two different interfaces that are both
 *   proper forward routes (case 817).
 * - metisPit_SatisfyInterest should verify the Object came in from one of the egress ports
 *   used by one of the interests (case 818)
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <ccnx/forwarder/metis/processor/metis_PIT.h>
#include <ccnx/forwarder/metis/processor/metis_MatchingRulesTable.h>

#include <ccnx/forwarder/metis/core/metis_Ticks.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_HashCodeTable.h>
#include <parc/algol/parc_Hash.h>

#include <ccnx/forwarder/metis/core/metis_Forwarder.h>

#include <LongBow/runtime.h>

struct metis_standard_pit;
typedef struct metis_standard_pit MetisStandardPIT;

struct metis_standard_pit {
    MetisForwarder *metis;
    MetisLogger *logger;

    MetisMatchingRulesTable *table;

    // counters to track how many of each type of Interest we get
    unsigned insertCounterByName;
    unsigned insertCounterByKeyId;
    unsigned insertCounterByObjectHash;
};

static void _metisPIT_StoreInTable(MetisStandardPIT *pit, MetisMessage *interestMessage);

static void
_metisPIT_PitEntryDestroyer(void **dataPtr)
{
    metisPitEntry_Release((MetisPitEntry **) dataPtr);
}

static bool
_metisPIT_IngressSetContains(MetisPitEntry *pitEntry, unsigned connectionId)
{
    const MetisNumberSet *set = metisPitEntry_GetIngressSet(pitEntry);
    bool numberInSet = metisNumberSet_Contains(set, connectionId);
    return numberInSet;
}

static MetisTicks
_metisPIT_CalculateLifetime(MetisStandardPIT *pit, MetisMessage *interestMessage)
{
    // Should use the lifetime from the interest (case 694)
    uint64_t interestLifetimeTicks = 0;

    if (metisMessage_HasInterestLifetime(interestMessage)) {
        interestLifetimeTicks = metisMessage_GetInterestLifetimeTicks(interestMessage);
    } else {
        interestLifetimeTicks = metisForwarder_NanosToTicks(4000000000ULL);
    }

    MetisTicks expiryTime = metisForwarder_GetTicks(pit->metis) + interestLifetimeTicks;
    return expiryTime;
}

static void
_metisPIT_StoreInTable(MetisStandardPIT *pit, MetisMessage *interestMessage)
{
    MetisMessage *key = metisMessage_Acquire(interestMessage);

    MetisTicks expiryTime = _metisPIT_CalculateLifetime(pit, interestMessage);

    MetisPitEntry *pitEntry = metisPitEntry_Create(key, expiryTime);
    // this is done in metisPitEntry_Create
    //    metisPitEntry_AddIngressId(pitEntry, metisMessage_GetIngressConnectionId(interestMessage));

    metisMatchingRulesTable_AddToBestTable(pit->table, key, pitEntry);

    if (metisLogger_IsLoggable(pit->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
        metisLogger_Log(pit->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                        "Message %p added to PIT (expiry %" PRIu64 ") ingress %u",
                        (void *) interestMessage,
                        metisPitEntry_GetExpiryTime(pitEntry),
                        metisMessage_GetIngressConnectionId(interestMessage));
    }
}

static void
_metisPIT_ExtendLifetime(MetisStandardPIT *pit, MetisPitEntry *pitEntry, MetisMessage *interestMessage)
{
    MetisTicks expiryTime = _metisPIT_CalculateLifetime(pit, interestMessage);
    metisPitEntry_SetExpiryTime(pitEntry, expiryTime);
}

// this appears to only be used in some unit tests
__attribute__((unused))
static void
_metisPIT_AddEgressConnectionId(MetisPIT *generic, const MetisMessage *interestMessage, unsigned connectionId)
{
    assertNotNull(generic, "Parameter pit must be non-null");
    assertNotNull(interestMessage, "Parameter interestMessage must be non-null");

    MetisStandardPIT *pit = metisPIT_Closure(generic);

    MetisPitEntry *entry = metisMatchingRulesTable_Get(pit->table, interestMessage);
    if (entry) {
        metisPitEntry_AddEgressId(entry, connectionId);
    }
}


// ======================================================================
// Interface API

static void
_metisStandardPIT_Destroy(MetisPIT **pitPtr)
{
    assertNotNull(pitPtr, "Parameter must be non-null double pointer");
    assertNotNull(*pitPtr, "Parameter must dereference to non-null pointer");

    MetisStandardPIT *pit = metisPIT_Closure(*pitPtr);

    if (metisLogger_IsLoggable(pit->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
        metisLogger_Log(pit->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                        "PIT %p destroyed",
                        (void *) pit);
    }

    metisMatchingRulesTable_Destroy(&pit->table);
    metisLogger_Release(&pit->logger);
    parcMemory_Deallocate(pitPtr);
}

// There's a bit too much going on in this function, need to break it
// apart for testability and style.
static MetisPITVerdict
_metisStandardPIT_ReceiveInterest(MetisPIT *generic, MetisMessage *interestMessage)
{
    assertNotNull(generic, "Parameter pit must be non-null");
    assertNotNull(interestMessage, "Parameter interestMessage must be non-null");

    MetisStandardPIT *pit = metisPIT_Closure(generic);

    MetisPitEntry *pitEntry = metisMatchingRulesTable_Get(pit->table, interestMessage);

    if (pitEntry) {
        // has it expired?
        MetisTicks now = metisForwarder_GetTicks(pit->metis);
        if (now < metisPitEntry_GetExpiryTime(pitEntry)) {
            // what should we do about extending the lifetime? (case 819)

            _metisPIT_ExtendLifetime(pit, pitEntry, interestMessage);

            // Is the reverse path already in the PIT entry?
            if (_metisPIT_IngressSetContains(pitEntry, metisMessage_GetIngressConnectionId(interestMessage))) {
                // It is already in the PIT entry, so this is a retransmission, so forward it.

                if (metisLogger_IsLoggable(pit->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
                    metisLogger_Log(pit->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                                    "Message %p existing entry (expiry %" PRIu64 ") and reverse path, forwarding",
                                    (void *) interestMessage,
                                    metisPitEntry_GetExpiryTime(pitEntry));
                }

                return MetisPITVerdict_Forward;
            }

            // It is in the PIT but this is the first interest for the reverse path
            metisPitEntry_AddIngressId(pitEntry, metisMessage_GetIngressConnectionId(interestMessage));

            if (metisLogger_IsLoggable(pit->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
                metisLogger_Log(pit->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                                "Message %p existing entry (expiry %" PRIu64 ") and reverse path is new, aggregate",
                                (void *) interestMessage,
                                metisPitEntry_GetExpiryTime(pitEntry));
            }

            return MetisPITVerdict_Aggregate;
        }

        // it's an old entry, remove it
        metisMatchingRulesTable_RemoveFromBest(pit->table, interestMessage);
    }

    _metisPIT_StoreInTable(pit, interestMessage);

    return MetisPITVerdict_Forward;
}

static MetisNumberSet *
_metisStandardPIT_SatisfyInterest(MetisPIT *generic, const MetisMessage *objectMessage)
{
    assertNotNull(generic, "Parameter pit must be non-null");
    assertNotNull(objectMessage, "Parameter objectMessage must be non-null");

    MetisStandardPIT *pit = metisPIT_Closure(generic);

    // we need to look in all three tables to see if there's anything
    // to satisy in each of them and take the union of the reverse path sets.

    MetisNumberSet *ingressSetUnion = metisNumberSet_Create();

    PARCArrayList *list = metisMatchingRulesTable_GetUnion(pit->table, objectMessage);
    for (size_t i = 0; i < parcArrayList_Size(list); i++) {
        MetisPitEntry *pitEntry = (MetisPitEntry *) parcArrayList_Get(list, i);

        // this is a reference counted return
        const MetisNumberSet *ingressSet = metisPitEntry_GetIngressSet(pitEntry);
        metisNumberSet_AddSet(ingressSetUnion, ingressSet);

        // and remove it from the PIT.  Key is a reference counted copy of the pit entry message
        MetisMessage *key = metisPitEntry_GetMessage(pitEntry);
        metisMatchingRulesTable_RemoveFromBest(pit->table, key);
        metisMessage_Release(&key);
    }
    parcArrayList_Destroy(&list);

    return ingressSetUnion;
}

static void
_metisStandardPIT_RemoveInterest(MetisPIT *generic, const MetisMessage *interestMessage)
{
    assertNotNull(generic, "Parameter pit must be non-null");
    assertNotNull(interestMessage, "Parameter interestMessage must be non-null");

    MetisStandardPIT *pit = metisPIT_Closure(generic);

    if (metisLogger_IsLoggable(pit->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
        metisLogger_Log(pit->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                        "Message %p removed from PIT",
                        (void *) interestMessage);
    }

    metisMatchingRulesTable_RemoveFromBest(pit->table, interestMessage);
}

static MetisPitEntry *
_metisStandardPIT_GetPitEntry(const MetisPIT *generic, const MetisMessage *interestMessage)
{
    assertNotNull(generic, "Parameter pit must be non-null");
    assertNotNull(interestMessage, "Parameter interestMessage must be non-null");

    MetisStandardPIT *pit = metisPIT_Closure(generic);

    MetisPitEntry *entry = metisMatchingRulesTable_Get(pit->table, interestMessage);
    if (entry) {
        return metisPitEntry_Acquire(entry);
    }
    return NULL;
}


// ======================================================================
// Public API

MetisPIT *
metisStandardPIT_Create(MetisForwarder *metis)
{
    assertNotNull(metis, "Parameter must be non-null");

    size_t allocation = sizeof(MetisPIT) + sizeof(MetisStandardPIT);

    MetisPIT *generic = parcMemory_AllocateAndClear(allocation);
    assertNotNull(generic, "parcMemory_AllocateAndClear(%zu) returned NULL", allocation);
    generic->closure = (uint8_t *) generic + sizeof(MetisPIT);

    MetisStandardPIT *pit = metisPIT_Closure(generic);
    pit->metis = metis;
    pit->logger = metisLogger_Acquire(metisForwarder_GetLogger(metis));
    pit->table = metisMatchingRulesTable_Create(_metisPIT_PitEntryDestroyer);

    if (metisLogger_IsLoggable(pit->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
        metisLogger_Log(pit->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                        "PIT %p created",
                        (void *) pit);
    }

    generic->getPitEntry = _metisStandardPIT_GetPitEntry;
    generic->receiveInterest = _metisStandardPIT_ReceiveInterest;
    generic->release = _metisStandardPIT_Destroy;
    generic->removeInterest = _metisStandardPIT_RemoveInterest;
    generic->satisfyInterest = _metisStandardPIT_SatisfyInterest;

    return generic;
}

