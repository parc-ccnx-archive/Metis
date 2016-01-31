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
#include <string.h>

#include <ccnx/forwarder/metis/processor/metis_MessageProcessor.h>
#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_ArrayList.h>

#include <ccnx/forwarder/metis/processor/metis_StandardPIT.h>
#include <ccnx/forwarder/metis/processor/metis_FIB.h>

#include <ccnx/forwarder/metis/content_store/metis_ContentStoreInterface.h>
#include <ccnx/forwarder/metis/content_store/metis_LRUContentStore.h>

#include <LongBow/runtime.h>

/**
 * @typedef MetisProcessorStats
 * @abstract MessageProcessor         event counters
 *
 * @constant countReceived            All received messages, the good, the bad, the ugly
 * @constant countInterestsReceived   Count of received interests
 * @constant countObjectsReceived     Count of received content objects
 *
 * @constant countInterestsAggregated         Number of Interests suppressed via PIT table aggregation
 * @constant countInterestForwarded           Number of Interests forwarded, for each outbound interface
 * @constant countObjectsForwarded            Number of Content Objects forwarded, for each outbound interface
 * @constant countInterestsSatisfiedFromStore Number of Interests satisfied from the Content Store
 *
 * @constant countDropped              Number of messages dropped, for any reason
 * @constant countInterestsDropped     Number of Interests dropped, for any reason
 * @constant countDroppedNoRoute       Number of Interests dropped because no FIB entry
 * @constant countDroppedNoReversePath Number of Content Objects dropped because no PIT entry
 * @constant countDroppedNoHopLimit    Number of Interests without a HopLimit
 * @constant countDroppedZeroHopLimitFromRemote Number of Interest from a remote node with a 0 hoplimit
 *
 * @constant countDroppedZeroHopLimitToRemote Number of Interest not forwarded to a FIB entry because hoplimit is 0 and its remote
 * @constant countSendFailures         Number of send failures (problems using MetisIoOperations)
 *
 * @discussion <#Discussion#>
 */
typedef struct metis_processor_stats {
    uint32_t countReceived;
    uint32_t countInterestsReceived;
    uint32_t countObjectsReceived;

    uint32_t countInterestsAggregated;

    uint32_t countDropped;
    uint32_t countInterestsDropped;
    uint32_t countDroppedNoRoute;
    uint32_t countDroppedNoReversePath;

    uint32_t countDroppedConnectionNotFound;
    uint32_t countObjectsDropped;

    uint32_t countSendFailures;
    uint32_t countInterestForwarded;
    uint32_t countObjectsForwarded;
    uint32_t countInterestsSatisfiedFromStore;

    uint32_t countDroppedNoHopLimit;
    uint32_t countDroppedZeroHopLimitFromRemote;
    uint32_t countDroppedZeroHopLimitToRemote;
} _MetisProcessorStats;

struct metis_message_processor {
    MetisForwarder *metis;
    MetisLogger *logger;
    MetisTap *tap;

    MetisPIT *pit;
    MetisContentStoreInterface *contentStore;
    MetisFIB *fib;

    _MetisProcessorStats stats;
};

static void metisMessageProcessor_Drop(MetisMessageProcessor *processor, MetisMessage *message);
static void metisMessageProcessor_ReceiveInterest(MetisMessageProcessor *processor, MetisMessage *interestMessage);
static void metisMessageProcessor_ReceiveContentObject(MetisMessageProcessor *processor, MetisMessage *objectMessage);
static unsigned metisMessageProcessor_ForwardToNexthops(MetisMessageProcessor *processor, MetisMessage *message, const MetisNumberSet *nexthops);

static void metisMessageProcessor_ForwardToInterfaceId(MetisMessageProcessor *processor, MetisMessage *message, unsigned interfaceId);

// ============================================================
// Public API

MetisMessageProcessor *
metisMessageProcessor_Create(MetisForwarder *metis)
{
    size_t objectStoreSize = metisConfiguration_GetObjectStoreSize(metisForwarder_GetConfiguration(metis));

    MetisMessageProcessor *processor = parcMemory_AllocateAndClear(sizeof(MetisMessageProcessor));
    assertNotNull(processor, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisMessageProcessor));
    memset(processor, 0, sizeof(MetisMessageProcessor));

    processor->metis = metis;
    processor->logger = metisLogger_Acquire(metisForwarder_GetLogger(metis));
    processor->pit = metisStandardPIT_Create(metis);

    processor->fib = metisFIB_Create(processor->logger);

    if (metisLogger_IsLoggable(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
        metisLogger_Log(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                        "MessageProcessor %p created",
                        (void *) processor);
    }

    MetisContentStoreConfig contentStoreConfig = {
        .objectCapacity = objectStoreSize,
    };

    // Currently, this will instantiate an LRUContentStore. Perhaps someday it'll switch stores
    // based on the MetisContentStoreConfig passed to it.
    processor->contentStore = metisLRUContentStore_Create(&contentStoreConfig, processor->logger);

    return processor;
}

void
metisMessageProcessor_SetContentObjectStoreSize(MetisMessageProcessor *processor, size_t maximumContentStoreSize)
{
    assertNotNull(processor, "Parameter processor must be non-null");
    metisContentStoreInterface_Release(&processor->contentStore);

    MetisContentStoreConfig contentStoreConfig = {
        .objectCapacity = maximumContentStoreSize
    };

    processor->contentStore = metisLRUContentStore_Create(&contentStoreConfig, processor->logger);
}

MetisContentStoreInterface *
metisMessageProcessor_GetContentObjectStore(const MetisMessageProcessor *processor)
{
    assertNotNull(processor, "Parameter processor must be non-null");
    return processor->contentStore;
}

void
metisMessageProcessor_Destroy(MetisMessageProcessor **processorPtr)
{
    assertNotNull(processorPtr, "Parameter must be non-null double pointer");
    assertNotNull(*processorPtr, "Parameter dereference to non-null pointer");

    MetisMessageProcessor *processor = *processorPtr;

    if (metisLogger_IsLoggable(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
        metisLogger_Log(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                        "MessageProcessor %p destroyed",
                        (void *) processor);
    }

    metisLogger_Release(&processor->logger);
    metisFIB_Destroy(&processor->fib);
    metisContentStoreInterface_Release(&processor->contentStore);
    metisPIT_Release(&processor->pit);

    parcMemory_Deallocate((void **) &processor);
    *processorPtr = NULL;
}

void
metisMessageProcessor_Receive(MetisMessageProcessor *processor, MetisMessage *message)
{
    assertNotNull(processor, "Parameter processor must be non-null");
    assertNotNull(message, "Parameter message must be non-null");

    processor->stats.countReceived++;

    if (processor->tap != NULL && processor->tap->isTapOnReceive(processor->tap)) {
        processor->tap->tapOnReceive(processor->tap, message);
    }

    if (metisLogger_IsLoggable(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
        char *nameString = "NONAME";
        if (metisMessage_HasName(message)) {
            CCNxName *name = metisTlvName_ToCCNxName(metisMessage_GetName(message));
            nameString = ccnxName_ToString(name);

            metisLogger_Log(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                            "Message %p ingress %3u length %5u received name %s",
                            (void *) message,
                            metisMessage_GetIngressConnectionId(message),
                            metisMessage_Length(message),
                            nameString);

            parcMemory_Deallocate((void **) &nameString);
            ccnxName_Release(&name);
        }
    }

    switch (metisMessage_GetType(message)) {
        case MetisMessagePacketType_Interest:
            metisMessageProcessor_ReceiveInterest(processor, message);
            break;

        case MetisMessagePacketType_ContentObject:
            metisMessageProcessor_ReceiveContentObject(processor, message);
            break;

        default:
            metisMessageProcessor_Drop(processor, message);
            break;
    }

    // if someone wanted to save it, they made a copy
    metisMessage_Release(&message);
}

void
metisMessageProcessor_AddTap(MetisMessageProcessor *processor, MetisTap *tap)
{
    assertNotNull(processor, "Parameter processor must be non-null");
    assertNotNull(tap, "Parameter tap must be non-null");

    processor->tap = tap;
}

void
metisMessageProcessor_RemoveTap(MetisMessageProcessor *processor, const MetisTap *tap)
{
    assertNotNull(processor, "Parameter processor must be non-null");
    assertNotNull(tap, "Parameter tap must be non-null");

    if (processor->tap == tap) {
        processor->tap = NULL;
    }
}

bool
metisMessageProcessor_AddOrUpdateRoute(MetisMessageProcessor *processor, CPIRouteEntry *route)
{
    return metisFIB_AddOrUpdate(processor->fib, route);
}

bool
metisMessageProcessor_RemoveRoute(MetisMessageProcessor *processor, CPIRouteEntry *route)
{
    return metisFIB_Remove(processor->fib, route);
}

void
metisMessageProcessor_RemoveConnectionIdFromRoutes(MetisMessageProcessor *processor, unsigned connectionId)
{
    metisFIB_RemoveConnectionIdFromRoutes(processor->fib, connectionId);
}

MetisFibEntryList *
metisMessageProcessor_GetFibEntries(MetisMessageProcessor *processor)
{
    assertNotNull(processor, "Parameter processor must be non-null");
    return metisFIB_GetEntries(processor->fib);
}

// ============================================================
// Internal API

/**
 * @function metisMessageProcessor_Drop
 * @abstract Whenever we "drop" a message, notify the OnDrop tap and increment countes
 * @discussion
 *   This is a bookkeeping function.  It notifies the tap, if its an onDrop tap, and
 *   it increments the appropriate counters.
 *
 *   The default action for a message is to destroy it in <code>metisMessageProcessor_Receive()</code>,
 *   so this function does not need to do that.
 *
 * @param <#param1#>
 */
static void
metisMessageProcessor_Drop(MetisMessageProcessor *processor, MetisMessage *message)
{
    if (processor->tap != NULL && processor->tap->isTapOnDrop && processor->tap->isTapOnDrop(processor->tap)) {
        processor->tap->tapOnDrop(processor->tap, message);
    }

    processor->stats.countDropped++;

    switch (metisMessage_GetType(message)) {
        case MetisMessagePacketType_Interest:
            processor->stats.countInterestsDropped++;
            break;

        case MetisMessagePacketType_ContentObject:
            processor->stats.countObjectsDropped++;
            break;

        default:
            break;
    }

    // dont destroy message here, its done at end of receive
}

/**
 * @function metisMessageProcessor_AggregateInterestInPit
 * @abstract Try to aggregate the interest in the PIT
 * @discussion
 *   Tries to aggregate the interest with another interest.
 *
 * @param <#param1#>
 * @return true if interest aggregagted (no more forwarding needed), false if need to keep processing it.
 */
static bool
metisMessageProcessor_AggregateInterestInPit(MetisMessageProcessor *processor, MetisMessage *interestMessage)
{
    MetisPITVerdict verdict = metisPIT_ReceiveInterest(processor->pit, interestMessage);

    if (verdict == MetisPITVerdict_Aggregate) {
        // PIT has it, we're done
        processor->stats.countInterestsAggregated++;

        if (metisLogger_IsLoggable(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
            metisLogger_Log(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                            "Message %p aggregated in PIT (aggregated count %u)",
                            (void *) interestMessage,
                            processor->stats.countInterestsAggregated);
        }

        return true;
    }

    if (metisLogger_IsLoggable(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
        metisLogger_Log(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                        "Message %p not aggregated in PIT (aggregated count %u)",
                        (void *) interestMessage,
                        processor->stats.countInterestsAggregated);
    }

    return false;
}

static bool
_satisfyFromContentStore(MetisMessageProcessor *processor, MetisMessage *interestMessage)
{
    bool result = false;

    // See if there's a match in the store.
    MetisMessage *objectMessage = metisContentStoreInterface_MatchInterest(processor->contentStore, interestMessage);

    if (objectMessage) {
        // If the Interest specified a KeyId restriction and we had a match, check to see if the ContentObject's KeyId
        // has been verified. If not, we don't respond with it.
        if (metisMessage_HasKeyId(interestMessage) && !metisMessage_IsKeyIdVerified(objectMessage)) {
            // We don't match if they specified a KeyId restriction and we haven't yet verified it.
            objectMessage = NULL;
        }
    }

    if (objectMessage != NULL) {
        bool hasExpired = false;
        bool hasExceededRCT = false;

        uint64_t currentTimeTicks = metisForwarder_GetTicks(processor->metis);

        // Check for ExpiryTime exceeded.
        if (metisMessage_HasExpiryTime(objectMessage)
            && (currentTimeTicks > metisMessage_GetExpiryTimeTicks(objectMessage))) {
            hasExpired = true;
        }

        // Check for RCT exceeded.
        if (metisMessage_HasRecommendedCacheTime(objectMessage)
            && (currentTimeTicks > metisMessage_GetRecommendedCacheTimeTicks(objectMessage))) {
            hasExceededRCT = true;
        }

        if (!hasExpired) { // && !hasExceededRCT ? It's up to us.
            // Remove it from the PIT.  nexthops is allocated, so need to destroy
            MetisNumberSet *nexthops = metisPIT_SatisfyInterest(processor->pit, objectMessage);
            assertNotNull(nexthops, "Illegal state: got a null nexthops for an interest we just inserted.");

            // send message in reply, then done
            processor->stats.countInterestsSatisfiedFromStore++;

            if (metisLogger_IsLoggable(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
                metisLogger_Log(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                                "Message %p satisfied from content store (satisfied count %u)",
                                (void *) interestMessage,
                                processor->stats.countInterestsSatisfiedFromStore);
            }

            metisMessageProcessor_ForwardToNexthops(processor, objectMessage, nexthops);
            metisNumberSet_Release(&nexthops);

            result = true;
        }

        // Remove the retrieved ContentObject from the ContentStore if it has expired, or exceeded its RCT.
        if (hasExpired || hasExceededRCT) {
            metisContentStoreInterface_RemoveContent(processor->contentStore, objectMessage);
        }
    }

    return result;
}

/**
 * @function metisMessageProcessor_ForwardViaFib
 * @abstract Try to forward the interest via the FIB
 * @discussion
 *   This calls <code>metisMessageProcessor_ForwardToNexthops()</code>, so if we find any nexthops,
 *   the interest will be sent on its way.  Depending on the MetisIoOperations of each nexthop,
 *   it may be a deferred write and bump up the <code>interestMessage</code> refernce count, or it
 *   may copy the data out.
 *
 *   A TRUE return means we did our best to forward it via the routes.  If those routes are actually
 *   down or have errors, we still return TRUE.  A FALSE return means there were no routes to try.
 *
 * @param <#param1#>
 * @return true if we found a route and tried to forward it, false if no route
 */
static bool
metisMessageProcessor_ForwardViaFib(MetisMessageProcessor *processor, MetisMessage *interestMessage)
{
    bool forwarded = false;

    // Look in the FIB.
    // nexthops will not be NULL, but may be empty.

    const MetisNumberSet *nexthops = metisFIB_Match(processor->fib, interestMessage);

    if (metisMessageProcessor_ForwardToNexthops(processor, interestMessage, nexthops) > 0) {
        forwarded = true;
    } else {
        if (metisLogger_IsLoggable(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
            metisLogger_Log(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                            "Message %p returned an emtpy next hop set", (void *) interestMessage);
        }
    }

    return forwarded;
}

static bool
metisMessageProcessor_IsIngressConnectionLocal(MetisMessageProcessor *processor, MetisMessage *interestMessage)
{
    MetisConnectionTable *connTable = metisForwarder_GetConnectionTable(processor->metis);
    unsigned ingressConnId = metisMessage_GetIngressConnectionId(interestMessage);
    const MetisConnection *ingressConn = metisConnectionTable_FindById(connTable, ingressConnId);

    bool isLocal = false;
    if (ingressConn) {
        isLocal = metisConnection_IsLocal(ingressConn);
    }
    return isLocal;
}

/**
 * On ingress, a remote connection must have hop limit > 0.  All interests must have a hop limit.
 *
 * This function will log the error, if any, but it does not drop the message.
 *
 * If Interest is from a local application, the hop limit is not decremented and may be 0.
 *
 * If Interest is from a remote connection, the hop limit must be greater than 0 and will be decremented.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @retval true The interest passes the hop limit check
 * @retval false The interest fails the hop limit check, should be dropped
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
static bool
metisMessageProcessor_CheckAndDecrementHopLimitOnIngress(MetisMessageProcessor *processor, MetisMessage *interestMessage)
{
    bool success = true;
    if (!metisMessage_HasHopLimit(interestMessage)) {
        processor->stats.countDroppedNoHopLimit++;

        if (metisLogger_IsLoggable(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
            metisLogger_Log(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                            "Message %p did not have a hop limit (count %u)",
                            (void *) interestMessage,
                            processor->stats.countDroppedNoHopLimit);
        }

        success = false;
    } else {
        // Is the ingress connection remote?  If so check for non-zero and decrement
        if (!metisMessageProcessor_IsIngressConnectionLocal(processor, interestMessage)) {
            uint8_t hoplimit = metisMessage_GetHopLimit(interestMessage);
            if (hoplimit == 0) {
                processor->stats.countDroppedZeroHopLimitFromRemote++;

                if (metisLogger_IsLoggable(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
                    metisLogger_Log(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                                    "Message %p from remote host has 0 hop limit (count %u)",
                                    (void *) interestMessage,
                                    processor->stats.countDroppedZeroHopLimitFromRemote);
                }

                success = false;
            } else {
                hoplimit--;
                metisMessage_SetHopLimit(interestMessage, hoplimit);
            }
        }
    }
    return success;
}

/**
 * @function metisMessageProcessor_ReceiveInterest
 * @abstract Receive an interest from the network
 * @discussion
 *   (0) It must have a HopLimit and pass the hoplimit checks
 *   (1) if interest in the PIT, aggregate in PIT
 *   (2) if interest in the ContentStore, reply
 *   (3) if in the FIB, forward
 *   (4) drop
 *
 * @param <#param1#>
 * @return <#return#>
 */
static void
metisMessageProcessor_ReceiveInterest(MetisMessageProcessor *processor, MetisMessage *interestMessage)
{
    processor->stats.countInterestsReceived++;

    if (!metisMessageProcessor_CheckAndDecrementHopLimitOnIngress(processor, interestMessage)) {
        metisMessageProcessor_Drop(processor, interestMessage);
        return;
    }

    // (1) Try to aggregate in PIT
    if (metisMessageProcessor_AggregateInterestInPit(processor, interestMessage)) {
        // done
        return;
    }

    // At this point, we just created a PIT entry.  If we don't forward the interest, we need
    // to remove the PIT entry.

    // (2) Try to satisfy from content store
    if (_satisfyFromContentStore(processor, interestMessage)) {
        // done
        // If we found a content object in the CS, metisMessageProcess_SatisfyFromContentStore already
        // cleared the PIT state
        return;
    }

    // (3) Try to forward it
    if (metisMessageProcessor_ForwardViaFib(processor, interestMessage)) {
        // done
        return;
    }

    // Remove the PIT entry?
    processor->stats.countDroppedNoRoute++;

    if (metisLogger_IsLoggable(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
        metisLogger_Log(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                        "Message %p did not match FIB, no route (count %u)",
                        (void *) interestMessage,
                        processor->stats.countDroppedNoRoute);
    }

    metisMessageProcessor_Drop(processor, interestMessage);
}

/**
 * @function metisMessageProcessor_ReceiveContentObject
 * @abstract Process an in-bound content object
 * @discussion
 *   (1) If it does not match anything in the PIT, drop it
 *   (2) Add to Content Store
 *   (3) Reverse path forward via PIT entries
 *
 * @param <#param1#>
 */
static void
metisMessageProcessor_ReceiveContentObject(MetisMessageProcessor *processor, MetisMessage *message)
{
    processor->stats.countObjectsReceived++;

    MetisNumberSet *ingressSetUnion = metisPIT_SatisfyInterest(processor->pit, message);

    if (metisNumberSet_Length(ingressSetUnion) == 0) {
        // (1) If it does not match anything in the PIT, drop it
        processor->stats.countDroppedNoReversePath++;

        if (metisLogger_IsLoggable(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
            metisLogger_Log(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                            "Message %p did not match PIT, no reverse path (count %u)",
                            (void *) message,
                            processor->stats.countDroppedNoReversePath);
        }

        metisMessageProcessor_Drop(processor, message);
    } else {
        // (2) Add to Content Store. Store may remove expired content, if necessary, depending on store policy.
        uint64_t currentTimeTicks = metisForwarder_GetTicks(processor->metis);
        metisContentStoreInterface_PutContent(processor->contentStore, message, currentTimeTicks);

        // (3) Reverse path forward via PIT entries
        metisMessageProcessor_ForwardToNexthops(processor, message, ingressSetUnion);
    }

    metisNumberSet_Release(&ingressSetUnion);
}

/**
 * @function metisMessageProcessor_ForwardToNexthops
 * @abstract Try to forward to each nexthop listed in the MetisNumberSet
 * @discussion
 *   Will not forward to the ingress connection.
 *
 * @param <#param1#>
 * @return The number of nexthops tried
 */
static unsigned
metisMessageProcessor_ForwardToNexthops(MetisMessageProcessor *processor, MetisMessage *message, const MetisNumberSet *nexthops)
{
    unsigned forwardedCopies = 0;

    size_t length = metisNumberSet_Length(nexthops);

    unsigned ingressId = metisMessage_GetIngressConnectionId(message);
    for (size_t i = 0; i < length; i++) {
        unsigned egressId = metisNumberSet_GetItem(nexthops, i);
        if (egressId != ingressId) {
            forwardedCopies++;
            metisMessageProcessor_ForwardToInterfaceId(processor, message, egressId);
        }
    }
    return forwardedCopies;
}

/**
 * caller has checked that the hop limit is ok.  Try to send out the connection.
 */
static void
metisMessageProcessor_SendWithGoodHopLimit(MetisMessageProcessor *processor, MetisMessage *message, unsigned interfaceId, const MetisConnection *conn)
{
    bool success = metisConnection_Send(conn, message);
    if (success) {
        switch (metisMessage_GetType(message)) {
            case MetisMessagePacketType_Interest:
                processor->stats.countInterestForwarded++;
                break;

            case MetisMessagePacketType_ContentObject:
                processor->stats.countObjectsForwarded++;
                break;

            default:
                break;
        }

        if (metisLogger_IsLoggable(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
            metisLogger_Log(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                            "forward message %p to interface %u (int %u, obj %u)",
                            (void *) message,
                            interfaceId,
                            processor->stats.countInterestForwarded,
                            processor->stats.countObjectsForwarded);
        }
    } else {
        processor->stats.countSendFailures++;

        if (metisLogger_IsLoggable(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
            metisLogger_Log(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                            "forward message %p to interface %u send failure (count %u)",
                            (void *) message,
                            interfaceId,
                            processor->stats.countSendFailures);
        }
        metisMessageProcessor_Drop(processor, message);
    }
}

/*
 *   If the hoplimit is equal to 0, then we may only forward it to local applications.  Otherwise,
 *   we may forward it off the system.
 *
 */
static void
metisMessageProcessor_ForwardToInterfaceId(MetisMessageProcessor *processor, MetisMessage *message, unsigned interfaceId)
{
    MetisConnectionTable *connectionTable = metisForwarder_GetConnectionTable(processor->metis);
    const MetisConnection *conn = metisConnectionTable_FindById(connectionTable, interfaceId);


    if (conn != NULL) {
        /*
         * We can send the message if:
         * a) If the message does not carry a hop limit (e.g. content object)
         * b) It has a hoplimit and it is positive
         * c) Or if the egress connection is local (i.e. it has a hoplimit and it's 0, but this is ok for a local app)
         */
        if ((!metisMessage_HasHopLimit(message)) || (metisMessage_GetHopLimit(message) > 0) || metisConnection_IsLocal(conn)) {
            metisMessageProcessor_SendWithGoodHopLimit(processor, message, interfaceId, conn);
        } else {
            // To reach here, the message has to have a hop limit, it has to be 0 and and going to a remote target
            processor->stats.countDroppedZeroHopLimitToRemote++;

            if (metisLogger_IsLoggable(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
                metisLogger_Log(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                                "forward message %p to interface %u hop limit 0 and not local (count %u)",
                                (void *) message,
                                interfaceId,
                                processor->stats.countDroppedZeroHopLimitToRemote);
            }
        }
    } else {
        processor->stats.countDroppedConnectionNotFound++;

        if (metisLogger_IsLoggable(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug)) {
            metisLogger_Log(processor->logger, MetisLoggerFacility_Processor, PARCLogLevel_Debug, __func__,
                            "forward message %p to interface %u not found (count %u)",
                            (void *) message,
                            interfaceId,
                            processor->stats.countDroppedConnectionNotFound);
        }

        metisMessageProcessor_Drop(processor, message);
    }
}
