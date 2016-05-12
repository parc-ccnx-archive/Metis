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
 * The implementation of metisMessage_Slice() copies data, it needs to do this by reference.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <config.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <ccnx/forwarder/metis/core/metis_Message.h>
#include <ccnx/forwarder/metis/core/metis_StreamBuffer.h>
#include <ccnx/forwarder/metis/tlv/metis_Tlv.h>
#include <ccnx/forwarder/metis/core/metis_Forwarder.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Hash.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_EventBuffer.h>

struct metis_message {
    MetisLogger *logger;

    MetisTicks receiveTime;
    unsigned ingressConnectionId;

    PARCEventBuffer *messageBytes;
    uint8_t *messageHead;

    unsigned refcount;

    struct tlv_skeleton skeleton;

    bool hasKeyId;
    uint32_t keyIdHash;
    bool isKeyIdVerified;

    bool hasContentObjectHash;
    // may be null, even if hasContentObjectHash true due to lazy calculation
    PARCBuffer *contentObjectHash;

    PARCBuffer *certificate;

    PARCBuffer *publicKey;

    bool hasInterestLifetime;
    uint64_t interestLifetimeTicks;

    bool hasExpiryTimeTicks;
    uint64_t expiryTimeTicks;

    bool hasRecommendedCacheTimeTicks;
    uint64_t recommendedCacheTimeTicks;

    bool hasName;
    MetisTlvName *name;

    bool hasFragmentPayload;

    MetisMessagePacketType packetType;
};

static void
_setupName(MetisMessage *message)
{
    MetisTlvExtent extent = metisTlvSkeleton_GetName(&message->skeleton);
    if (extent.offset > 0) {
        message->hasName = true;
        message->name = metisTlvName_Create(&message->messageHead[extent.offset], extent.length);
    } else {
        message->hasName = false;
        message->name = NULL;
    }
}

static void
_setupValidationParams(MetisMessage *message)
{
    MetisTlvExtent extent = metisTlvSkeleton_GetKeyId(&message->skeleton);
    if (extent.offset > 0) {
        message->hasKeyId = true;
        message->keyIdHash = parcHash32_Data(&message->messageHead[extent.offset], extent.length);
    } else {
        message->hasKeyId = false;
        message->keyIdHash = 0;
    }
    message->isKeyIdVerified = false;

    extent = metisTlvSkeleton_GetCertificate(&message->skeleton);
    if (extent.offset > 0) {
        message->certificate = parcBuffer_Flip(parcBuffer_CreateFromArray(&message->messageHead[extent.offset], extent.length));
    } else {
        message->certificate = NULL;
    }

    extent = metisTlvSkeleton_GetPublicKey(&message->skeleton);
    if (extent.offset > 0) {
        message->publicKey = parcBuffer_Flip(parcBuffer_CreateFromArray(&message->messageHead[extent.offset], extent.length));
    } else {
        message->publicKey = NULL;
    }
}

static void
_setupContentObjectHash(MetisMessage *message)
{
    if (metisTlvSkeleton_IsPacketTypeInterest(&message->skeleton)) {
        MetisTlvExtent extent = metisTlvSkeleton_GetObjectHash(&message->skeleton);
        // pre-compute it for an interest
        if (extent.offset > 0) {
            message->hasContentObjectHash = true;
            message->contentObjectHash =
                parcBuffer_Flip(parcBuffer_CreateFromArray(&message->messageHead[extent.offset], extent.length));
        } else {
            message->hasContentObjectHash = false;
        }
    } else if (metisTlvSkeleton_IsPacketTypeContentObject(&message->skeleton)) {
        // we will compute this on demand
        message->hasContentObjectHash = true;
        message->contentObjectHash = NULL;
    } else {
        message->hasContentObjectHash = false;
    }
}

static void
_setupInterestLifetime(MetisMessage *message)
{
    MetisTlvExtent extent = metisTlvSkeleton_GetInterestLifetime(&message->skeleton);
    message->hasInterestLifetime = false;
    if (metisTlvSkeleton_IsPacketTypeInterest(&message->skeleton) && extent.offset > 0) {
        message->hasInterestLifetime = true;
        uint64_t lifetimeMilliseconds;
        metisTlv_ExtentToVarInt(message->messageHead, &extent, &lifetimeMilliseconds);
        message->interestLifetimeTicks = metisForwarder_NanosToTicks(lifetimeMilliseconds * 1000000);
    }
}

static void
_setupFragmentPayload(MetisMessage *message)
{
    MetisTlvExtent extent = metisTlvSkeleton_GetFragmentPayload(&message->skeleton);
    if (extent.offset > 0) {
        message->hasFragmentPayload = true;
    } else {
        message->hasFragmentPayload = true;
    }
}

static void
_setupExpiryTime(MetisMessage *message)
{
    MetisTlvExtent expiryTimeExtent = metisTlvSkeleton_GetExpiryTime(&message->skeleton);

    message->hasExpiryTimeTicks = false;

    if (metisTlvSkeleton_IsPacketTypeContentObject(&message->skeleton)) {
        if (!metisTlvExtent_Equals(&expiryTimeExtent, &metisTlvExtent_NotFound)) {
            uint64_t expiryTimeUTC = 0;
            if (metisTlv_ExtentToVarInt(metisTlvSkeleton_GetPacket(&message->skeleton), &expiryTimeExtent, &expiryTimeUTC)) {
                message->hasExpiryTimeTicks = true;

                // Convert it to ticks that we can use for expiration checking.
                uint64_t metisWallClockTime = parcClock_GetTime(parcClock_Wallclock());
                uint64_t currentTimeInTicks = parcClock_GetTime(parcClock_Monotonic());

                message->expiryTimeTicks = expiryTimeUTC - metisWallClockTime + currentTimeInTicks;
            }
        }
    }
}

static void
_setupRecommendedCacheTime(MetisMessage *message)
{
    MetisTlvExtent cacheTimeExtent = metisTlvSkeleton_GetCacheTimeHeader(&message->skeleton);

    message->hasRecommendedCacheTimeTicks = false;

    if (metisTlvSkeleton_IsPacketTypeContentObject(&message->skeleton)) {
        if (!metisTlvExtent_Equals(&cacheTimeExtent, &metisTlvExtent_NotFound)) {
            uint64_t recommendedCacheTime = 0;
            if (metisTlv_ExtentToVarInt(metisTlvSkeleton_GetPacket(&message->skeleton), &cacheTimeExtent, &recommendedCacheTime)) {
                message->hasRecommendedCacheTimeTicks = true;

                // Convert it to ticks that we can use for expiration checking.
                uint64_t metisWallClockTime = parcClock_GetTime(parcClock_Wallclock());
                uint64_t currentTimeInTicks = parcClock_GetTime(parcClock_Monotonic());

                message->recommendedCacheTimeTicks = recommendedCacheTime - metisWallClockTime + currentTimeInTicks;
            }
        }
    }
}

/**
 * Parse the TLV skeleton and setup message pointers
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] message An allocated messae with the message->messagaeHead pointer setup.
 *
 * @retval false Error parsing message
 * @retval true Good parse
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
static bool
_setupInternalData(MetisMessage *message)
{
    // -1 means linearize the whole buffer
    message->messageHead = parcEventBuffer_Pullup(message->messageBytes, -1);
    message->packetType = MetisMessagePacketType_Unknown;

    bool goodSkeleton = metisTlvSkeleton_Parse(&message->skeleton, message->messageHead, message->logger);

    if (goodSkeleton) {
        _setupName(message);
        _setupValidationParams(message);
        _setupContentObjectHash(message);
        _setupInterestLifetime(message);
        _setupFragmentPayload(message);
        _setupExpiryTime(message);
        _setupRecommendedCacheTime(message);

        // set the packet type
        bool requiresName = false;
        if (metisTlvSkeleton_IsPacketTypeInterest(&message->skeleton)) {
            message->packetType = MetisMessagePacketType_Interest;
            requiresName = true;
        } else if (metisTlvSkeleton_IsPacketTypeContentObject(&message->skeleton)) {
            message->packetType = MetisMessagePacketType_ContentObject;
            requiresName = true;
        } else if (metisTlvSkeleton_IsPacketTypeHopByHopFragment(&message->skeleton)) {
            message->packetType = MetisMessagePacketType_HopByHopFrag;
        } else if (metisTlvSkeleton_IsPacketTypeControl(&message->skeleton)) {
            message->packetType = MetisMessagePacketType_Control;
        } else if (metisTlvSkeleton_IsPacketTypeInterestReturn(&message->skeleton)) {
            message->packetType = MetisMessagePacketType_InterestReturn;
        }

        if (requiresName && !metisMessage_HasName(message)) {
            goodSkeleton = false;
        }
    }

    return goodSkeleton;
}

MetisMessage *
metisMessage_Acquire(const MetisMessage *message)
{
    MetisMessage *copy = (MetisMessage *) message;
    copy->refcount++;
    return copy;
}

MetisMessage *
metisMessage_CreateFromParcBuffer(PARCBuffer *buffer, unsigned ingressConnectionId, MetisTicks receiveTime, MetisLogger *logger)
{
    MetisMessage *message = parcMemory_AllocateAndClear(sizeof(MetisMessage));
    assertNotNull(message, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisMessage));
    message->receiveTime = receiveTime;
    message->ingressConnectionId = ingressConnectionId;
    message->messageBytes = parcEventBuffer_Create();
    message->refcount = 1;
    message->logger = metisLogger_Acquire(logger);

    // this copies the data
    int failure = parcEventBuffer_Append(message->messageBytes, parcBuffer_Overlay(buffer, 0), parcBuffer_Remaining(buffer));
    assertFalse(failure, "Got failure copying data into buffer: (%d) %s", errno, strerror(errno));

    bool goodSkeleton = _setupInternalData(message);
    if (goodSkeleton) {
        if (metisLogger_IsLoggable(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
            metisLogger_Log(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                            "Message %p created ingress %u",
                            (void *) message, ingressConnectionId);
        }
    } else {
        if (metisLogger_IsLoggable(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Warning)) {
            metisLogger_Log(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Warning, __func__,
                            "Error setting up skeleton for buffer %p ingress %u",
                            (void *) buffer, ingressConnectionId);
        }

        metisMessage_Release(&message);
    }
    return message;
}

MetisMessage *
metisMessage_CreateFromArray(const uint8_t *data, size_t dataLength, unsigned ingressConnectionId, MetisTicks receiveTime, MetisLogger *logger)
{
    MetisMessage *message = parcMemory_AllocateAndClear(sizeof(MetisMessage));
    assertNotNull(message, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisMessage));
    message->receiveTime = receiveTime;
    message->ingressConnectionId = ingressConnectionId;
    message->messageBytes = parcEventBuffer_Create();
    message->refcount = 1;
    message->logger = metisLogger_Acquire(logger);

    // this copies the data
    int failure = parcEventBuffer_Append(message->messageBytes, (void *) data, dataLength);
    assertFalse(failure, "Got failure copying data into PARCEventBuffer: (%d) %s", errno, strerror(errno));

    bool goodSkeleton = _setupInternalData(message);
    if (goodSkeleton) {
        if (metisLogger_IsLoggable(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
            metisLogger_Log(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                            "Message %p created ingress %u",
                            (void *) message, ingressConnectionId);
        }
    } else {
        if (metisLogger_IsLoggable(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Warning)) {
            metisLogger_Log(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Warning, __func__,
                            "Error setting up skeleton for array %p ingress %u",
                            (void *) data, ingressConnectionId);
        }

        metisMessage_Release(&message);
    }

    return message;
}

MetisMessage *
metisMessage_ReadFromBuffer(unsigned ingressConnectionId, MetisTicks receiveTime, PARCEventBuffer *input, size_t bytesToRead, MetisLogger *logger)
{
    MetisMessage *message = parcMemory_AllocateAndClear(sizeof(MetisMessage));
    assertNotNull(message, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisMessage));
    message->receiveTime = receiveTime;
    message->ingressConnectionId = ingressConnectionId;
    message->messageBytes = parcEventBuffer_Create();
    message->refcount = 1;
    message->logger = metisLogger_Acquire(logger);

    // dequeue into packet buffer.  This is a near-zero-copy operation from
    // one buffer to another.  It only copies if the message falls across iovec
    // boundaries.
    int bytesRead = parcEventBuffer_ReadIntoBuffer(input, message->messageBytes, bytesToRead);
    assertTrue(bytesRead == bytesToRead, "Partial read, expected %zu got %d", bytesToRead, bytesRead);

    bool goodSkeleton = _setupInternalData(message);
    if (goodSkeleton) {
        if (metisLogger_IsLoggable(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
            metisLogger_Log(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                            "Message %p created ingress %u",
                            (void *) message, ingressConnectionId);
        }
    } else {
        if (metisLogger_IsLoggable(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Warning)) {
            metisLogger_Log(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Warning, __func__,
                            "Error setting up skeleton for buffer %p ingress %u",
                            (void *) input, ingressConnectionId);
        }

        metisMessage_Release(&message);
    }
    return message;
}

MetisMessage *
metisMessage_CreateFromBuffer(unsigned ingressConnectionId, MetisTicks receiveTime, PARCEventBuffer *input, MetisLogger *logger)
{
    assertNotNull(input, "Parameter input must be non-null");
    MetisMessage *message = parcMemory_AllocateAndClear(sizeof(MetisMessage));
    assertNotNull(message, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisMessage));
    message->receiveTime = receiveTime;
    message->ingressConnectionId = ingressConnectionId;
    message->messageBytes = input;
    message->refcount = 1;
    message->logger = metisLogger_Acquire(logger);

    bool goodSkeleton = _setupInternalData(message);
    if (goodSkeleton) {
        if (metisLogger_IsLoggable(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
            metisLogger_Log(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                            "Message %p created ingress %u",
                            (void *) message, ingressConnectionId);
        }
    } else {
        if (metisLogger_IsLoggable(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Warning)) {
            metisLogger_Log(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Warning, __func__,
                            "Error setting up skeleton for buffer %p ingress %u",
                            (void *) input, ingressConnectionId);
        }

        metisMessage_Release(&message);
    }
    return message;
}

void
metisMessage_Release(MetisMessage **messagePtr)
{
    assertNotNull(messagePtr, "Parameter must be non-null double pointer");
    assertNotNull(*messagePtr, "Parameter must dereference to non-null pointer");

    MetisMessage *message = *messagePtr;
    assertTrue(message->refcount > 0, "Invalid state: metisMessage_Release called on message with 0 references %p", (void *) message);

    message->refcount--;
    if (message->refcount == 0) {
        if (metisLogger_IsLoggable(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
            metisLogger_Log(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                            "Message %p destroyed",
                            (void *) message);
        }

        if (message->contentObjectHash) {
            parcBuffer_Release(&message->contentObjectHash);
        }

        if (message->name) {
            metisTlvName_Release(&message->name);
        }

        if (message->publicKey) {
            parcBuffer_Release(&message->publicKey);
        }

        if (message->certificate) {
            parcBuffer_Release(&message->certificate);
        }

        metisLogger_Release(&message->logger);
        parcEventBuffer_Destroy(&(message->messageBytes));
        parcMemory_Deallocate((void **) &message);
    }
    *messagePtr = NULL;
}

bool
metisMessage_Write(PARCEventQueue *parcEventQueue, const MetisMessage *message)
{
    assertNotNull(message, "Message parameter must be non-null");
    assertNotNull(parcEventQueue, "Buffer arameter must be non-null");
    return parcEventQueue_Write(parcEventQueue, message->messageHead, parcEventBuffer_GetLength(message->messageBytes));
}

bool
metisMessage_Append(PARCEventBuffer *writeBuffer, const MetisMessage *message)
{
    assertNotNull(message, "Message parameter must be non-null");
    assertNotNull(writeBuffer, "Buffer arameter must be non-null");
    return parcEventBuffer_Append(writeBuffer, message->messageHead, metisMessage_Length(message));
}

size_t
metisMessage_Length(const MetisMessage *message)
{
    assertNotNull(message, "Parameter must be non-null");
    return parcEventBuffer_GetLength(message->messageBytes);
}

unsigned
metisMessage_GetIngressConnectionId(const MetisMessage *message)
{
    assertNotNull(message, "Parameter must be non-null");
    return message->ingressConnectionId;
}

MetisTicks
metisMessage_GetReceiveTime(const MetisMessage *message)
{
    assertNotNull(message, "Parameter must be non-null");
    return message->receiveTime;
}

bool
metisMessage_HasHopLimit(const MetisMessage *message)
{
    assertNotNull(message, "Parameter must be non-null");
    MetisTlvExtent extent = metisTlvSkeleton_GetHopLimit(&message->skeleton);

    if (extent.offset > 0) {
        return true;
    }
    return false;
}

uint8_t
metisMessage_GetHopLimit(const MetisMessage *message)
{
    bool hasHopLimit = metisMessage_HasHopLimit(message);
    assertTrue(hasHopLimit, "Message does not have a HopLimit field");

    MetisTlvExtent extent = metisTlvSkeleton_GetHopLimit(&message->skeleton);
    uint8_t hopLimit = message->messageHead[extent.offset];
    return hopLimit;
}

void
metisMessage_SetHopLimit(MetisMessage *message, uint8_t hoplimit)
{
    assertNotNull(message, "Parameter must be non-null");
    metisTlvSkeleton_UpdateHopLimit(&message->skeleton, hoplimit);
}

MetisMessagePacketType
metisMessage_GetType(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return message->packetType;
}

MetisTlvName *
metisMessage_GetName(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return message->name;
}

bool
metisMessage_GetKeyIdHash(const MetisMessage *message, uint32_t *hashOutput)
{
    assertNotNull(message, "Parameter message must be non-null");
    if (message->hasKeyId) {
        *hashOutput = message->keyIdHash;
        return true;
    }
    return false;
}

PARCBuffer *
metisMessage_GetCertificate(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return message->certificate;
}

PARCBuffer *
metisMessage_GetPublicKey(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return message->publicKey;
}

bool
metisMessage_KeyIdEquals(const MetisMessage *a, const MetisMessage *b)
{
    assertNotNull(a, "Parameter a must be non-null");
    assertNotNull(b, "Parameter b must be non-null");

    if (a->hasKeyId && b->hasKeyId) {
        MetisTlvExtent ae = metisTlvSkeleton_GetKeyId(&a->skeleton);
        MetisTlvExtent be = metisTlvSkeleton_GetKeyId(&b->skeleton);

        if (ae.length == be.length) {
            return memcmp(&a->messageHead[ae.offset], &b->messageHead[be.offset], ae.length) == 0;
        }
    }
    return false;
}

bool
metisMessage_ObjectHashEquals(MetisMessage *a, MetisMessage *b)
{
    assertNotNull(a, "Parameter a must be non-null");
    assertNotNull(b, "Parameter b must be non-null");

    if (a->hasContentObjectHash && b->hasContentObjectHash) {
        if (a->contentObjectHash == NULL) {
            PARCCryptoHash *hash = metisTlvSkeleton_ComputeContentObjectHash(&a->skeleton);
            a->contentObjectHash = parcBuffer_Acquire(parcCryptoHash_GetDigest(hash));
            parcCryptoHash_Release(&hash);
        }

        if (b->contentObjectHash == NULL) {
            PARCCryptoHash *hash = metisTlvSkeleton_ComputeContentObjectHash(&b->skeleton);
            b->contentObjectHash = parcBuffer_Acquire(parcCryptoHash_GetDigest(hash));
            parcCryptoHash_Release(&hash);
        }

        return parcBuffer_Equals(a->contentObjectHash, b->contentObjectHash);
    }

    return false;
}

bool
metisMessage_GetContentObjectHashHash(MetisMessage *message, uint32_t *hashOutput)
{
    assertNotNull(message, "Parameter message must be non-null");
    assertNotNull(hashOutput, "Parameter hashOutput must be non-null");

    if (message->hasContentObjectHash) {
        if (message->contentObjectHash == NULL) {
            PARCCryptoHash *hash = metisTlvSkeleton_ComputeContentObjectHash(&message->skeleton);
            message->contentObjectHash = parcBuffer_Acquire(parcCryptoHash_GetDigest(hash));
            parcCryptoHash_Release(&hash);
        }

        *hashOutput = (uint32_t) parcBuffer_HashCode(message->contentObjectHash);
        return true;
    }
    return false;
}

bool
metisMessage_HasPublicKey(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return (message->publicKey != NULL);
}

bool
metisMessage_HasCertificate(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return (message->certificate != NULL);
}

bool
metisMessage_HasName(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return message->hasName;
}

bool
metisMessage_HasKeyId(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return message->hasKeyId;
}

bool
metisMessage_IsKeyIdVerified(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return message->isKeyIdVerified;
}

bool
metisMessage_HasContentObjectHash(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return message->hasContentObjectHash;
}

CCNxControl *
metisMessage_CreateControlMessage(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    assertTrue(metisMessage_GetType(message) == MetisMessagePacketType_Control,
               "Wrong type of message, expected %02X got %02X",
               MetisMessagePacketType_Control,
               metisMessage_GetType(message));

    MetisTlvExtent extent = metisTlvSkeleton_GetCPI(&message->skeleton);
    assertTrue(extent.offset > 0, "Message does not have a CPI TLV field!");

    PARCJSON *json = parcJSON_ParseString((char *) &message->messageHead[ extent.offset ]);
    CCNxControl *control = ccnxControl_CreateCPIRequest(json);
    parcJSON_Release(&json);
    return control;
}

bool
metisMessage_HasInterestLifetime(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return message->hasInterestLifetime;
}

uint64_t
metisMessage_GetInterestLifetimeTicks(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return message->interestLifetimeTicks;
}

bool
metisMessage_HasFragmentPayload(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return message->hasFragmentPayload;
}

size_t
metisMessage_AppendFragmentPayload(const MetisMessage *message, PARCEventBuffer *buffer)
{
    size_t bytesAppended = 0;
    if (message->hasFragmentPayload) {
        MetisTlvExtent extent = metisTlvSkeleton_GetFragmentPayload(&message->skeleton);
        parcEventBuffer_Append(buffer, message->messageHead + extent.offset, extent.length);
        bytesAppended = extent.length;
    }
    return bytesAppended;
}

const uint8_t *
metisMessage_FixedHeader(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return message->messageHead;
}

MetisMessage *
metisMessage_Slice(const MetisMessage *original, size_t offset, size_t length, size_t headerLength, const uint8_t header[headerLength])
{
    assertNotNull(original, "Parameter original must be non-null");
    assertTrue(length > 0, "Parameter length must be positive");
    assertTrue(offset + length <= parcEventBuffer_GetLength(original->messageBytes),
               "Slice extends beyond end, maximum %zu got %zu",
               parcEventBuffer_GetLength(original->messageBytes),
               offset + length);

    MetisMessage *message = parcMemory_AllocateAndClear(sizeof(MetisMessage));
    assertNotNull(message, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisMessage));
    message->receiveTime = original->receiveTime;
    message->ingressConnectionId = original->ingressConnectionId;
    message->messageBytes = parcEventBuffer_Create();
    message->refcount = 1;
    message->logger = metisLogger_Acquire(original->logger);

    if (headerLength > 0) {
        assertNotNull(header, "Cannot have a positive headerLength and NULL header");

        // this copies the data
        int failure = parcEventBuffer_Append(message->messageBytes, (void *) header, headerLength);
        assertFalse(failure, "Got failure adding header data into PARCEventBuffer: (%d) %s", errno, strerror(errno));
    }

    // this copies the data
    int failure = parcEventBuffer_Append(message->messageBytes, (uint8_t *) original->messageHead + offset, length);
    assertFalse(failure, "Got failure adding slice data into PARCEventBuffer: (%d) %s", errno, strerror(errno));

    bool goodSkeleton = _setupInternalData(message);
    if (goodSkeleton) {
        if (metisLogger_IsLoggable(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
            metisLogger_Log(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                            "Message %p created slice(%p, %zu, %zu)",
                            (void *) message, (void *) original, offset, length);
        }
    } else {
        if (metisLogger_IsLoggable(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Warning)) {
            metisLogger_Log(message->logger, MetisLoggerFacility_Message, PARCLogLevel_Warning, __func__,
                            "Error setting up skeleton for original %p and header %p",
                            (void *) original, (void *) header);
        }

        metisMessage_Release(&message);
    }

    return message;
}

bool
metisMessage_HasRecommendedCacheTime(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return message->hasRecommendedCacheTimeTicks;
}

uint64_t
metisMessage_GetRecommendedCacheTimeTicks(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    assertTrue(message->hasRecommendedCacheTimeTicks, "MetisMessage does not have a RecommendedCacheTime. Call metisMessage_HasRecommendedCacheTime() first.");
    return message->recommendedCacheTimeTicks;
}

void
metisMessage_SetRecommendedCacheTimeTicks(MetisMessage *message, uint64_t recommendedCacheTimeTicks)
{
    assertNotNull(message, "Parameter message must be non-null");
    message->recommendedCacheTimeTicks = recommendedCacheTimeTicks;
    message->hasRecommendedCacheTimeTicks = true;
}

bool
metisMessage_HasExpiryTime(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    return message->hasExpiryTimeTicks;
}

uint64_t
metisMessage_GetExpiryTimeTicks(const MetisMessage *message)
{
    assertNotNull(message, "Parameter message must be non-null");
    assertTrue(message->hasExpiryTimeTicks, "MetisMessage does not have an ExpiryTime. Call metisMessage_HasExpiryTime() first.");
    return message->expiryTimeTicks;
}

void
metisMessage_SetExpiryTimeTicks(MetisMessage *message, uint64_t expiryTimeTicks)
{
    assertNotNull(message, "Parameter message must be non-null");
    message->expiryTimeTicks = expiryTimeTicks;
    message->hasExpiryTimeTicks = true;
}
