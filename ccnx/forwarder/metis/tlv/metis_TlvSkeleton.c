/*
 * Copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <string.h>
#include <LongBow/runtime.h>
#include <ccnx/forwarder/metis/core/metis_Forwarder.h>

#include <ccnx/forwarder/metis/tlv/metis_TlvSkeleton.h>

#include <ccnx/forwarder/metis/tlv/metis_TlvSchemaV0.h>
#include <ccnx/forwarder/metis/tlv/metis_TlvSchemaV1.h>

#define INDEX_NAME 0
#define INDEX_KEYID 1
#define INDEX_OBJHASH 2
#define INDEX_HOPLIMIT 3
#define INDEX_INTLIFETIME 4
#define INDEX_CACHETIME 5
#define INDEX_EXPIRYTIME 6
#define INDEX_CPI 7
#define INDEX_FRAGMENTPAYLOAD 8
#define INDEX_CERTIFICATE 9
#define INDEX_PUBKEY 10

/**
 * The non-opaque representation of the MetisTlvSkeleton.
 *
 * IMPORTANT: if you change this structure, you must make sure the corresponding
 * opaque structure in metis_TlvSkeleton.h has at least that much memory in it.
 */
typedef struct internal_skeleton {
    const struct metis_tlv_ops *tlvOps;
    uint8_t *packet;
    MetisLogger *logger;

    MetisTlvExtent array[MetisTlvSkeleton_ArrayLength];
} _InternalSkeleton;

static void
_assertInvariants(const _InternalSkeleton *skeleton)
{
    assertNotNull(skeleton->tlvOps, "Invalid skeleton, does not have a schema ops");
    assertNotNull(skeleton->packet, "Invalid skeleton, does not have a packet buffer");
}

/**
 * Initialize the skeleton memory
 *
 * Clears all the extents to {0, 0} and sets the tlvOps and packet members for further parsing.
 *
 * @param [in] skeleton The skeleton to initialize
 * @param [in] tlvOps The parser operations to use
 * @param [in] packet the packet buffer (points to byte "0" of the fixed header)
 *
 * Example:
 * @code
 * {
 *    MetisTlvSkeleton skeleton;
 *    _initialize(&skeleton, &MetisTlvSchemaV0_Ops, packet);
 * }
 * @endcode
 */
static void
_initialize(_InternalSkeleton *skeleton, const struct metis_tlv_ops *tlvOps, uint8_t *packet, MetisLogger *logger)
{
    memset(skeleton, 0, sizeof(MetisTlvSkeleton));
    skeleton->packet = packet;
    skeleton->tlvOps = tlvOps;
    skeleton->logger = logger;
    _assertInvariants(skeleton);
}


bool
metisTlvSkeleton_Parse(MetisTlvSkeleton *opaque, uint8_t *packet, MetisLogger *logger)
{
    // do not assert invariants here.  Parse will setup the invariants.
    _InternalSkeleton *skeleton = (_InternalSkeleton *) opaque;
    uint8_t version = packet[0];

    switch (version) {
        case 0:
            _initialize(skeleton, &MetisTlvSchemaV0_Ops, packet, logger);
            return MetisTlvSchemaV0_Ops.parse(opaque);

        case 1:
            _initialize(skeleton, &MetisTlvSchemaV1_Ops, packet, logger);
            return MetisTlvSchemaV1_Ops.parse(opaque);

        default:
            if (metisLogger_IsLoggable(logger, MetisLoggerFacility_Message, PARCLogLevel_Warning)) {
                metisLogger_Log(logger, MetisLoggerFacility_Message, PARCLogLevel_Warning, __func__,
                                "Parsing unknown packet version %u", version);
            }
            break;
    }
    return false;
}

// ==========================================================
// Setters

void
metisTlvSkeleton_SetName(MetisTlvSkeleton *opaque, size_t offset, size_t length)
{
    _InternalSkeleton *skeleton = (_InternalSkeleton *) opaque;
    skeleton->array[INDEX_NAME].offset = offset;
    skeleton->array[INDEX_NAME].length = length;

    if (metisLogger_IsLoggable(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
        metisLogger_Log(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                        "Set name extent {%u, %u}", offset, length);
    }
}

void
metisTlvSkeleton_SetKeyId(MetisTlvSkeleton *opaque, size_t offset, size_t length)
{
    _InternalSkeleton *skeleton = (_InternalSkeleton *) opaque;
    skeleton->array[INDEX_KEYID].offset = offset;
    skeleton->array[INDEX_KEYID].length = length;

    if (metisLogger_IsLoggable(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
        metisLogger_Log(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                        "Set keyid extent {%u, %u}", offset, length);
    }
}

void
metisTlvSkeleton_SetCertificate(MetisTlvSkeleton *opaque, size_t offset, size_t length)
{
    _InternalSkeleton *skeleton = (_InternalSkeleton *) opaque;
    skeleton->array[INDEX_CERTIFICATE].offset = offset;
    skeleton->array[INDEX_CERTIFICATE].length = length;

    if (metisLogger_IsLoggable(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
        metisLogger_Log(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                        "Set certificate extent {%u, %u}", offset, length);
    }
}

void
metisTlvSkeleton_SetPublicKey(MetisTlvSkeleton *opaque, size_t offset, size_t length)
{
    _InternalSkeleton *skeleton = (_InternalSkeleton *) opaque;
    skeleton->array[INDEX_PUBKEY].offset = offset;
    skeleton->array[INDEX_PUBKEY].length = length;

    if (metisLogger_IsLoggable(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
        metisLogger_Log(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                        "Set public key extent {%u, %u}", offset, length);
    }
}

void
metisTlvSkeleton_SetObjectHash(MetisTlvSkeleton *opaque, size_t offset, size_t length)
{
    _InternalSkeleton *skeleton = (_InternalSkeleton *) opaque;
    skeleton->array[INDEX_OBJHASH].offset = offset;
    skeleton->array[INDEX_OBJHASH].length = length;

    if (metisLogger_IsLoggable(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
        metisLogger_Log(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                        "Set objhash extent {%u, %u}", offset, length);
    }
}

void
metisTlvSkeleton_SetHopLimit(MetisTlvSkeleton *opaque, size_t offset, size_t length)
{
    _InternalSkeleton *skeleton = (_InternalSkeleton *) opaque;
    skeleton->array[INDEX_HOPLIMIT].offset = offset;
    skeleton->array[INDEX_HOPLIMIT].length = length;

    if (metisLogger_IsLoggable(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
        metisLogger_Log(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                        "Set hoplimit extent {%u, %u}", offset, length);
    }
}

void
metisTlvSkeleton_SetInterestLifetime(MetisTlvSkeleton *opaque, size_t offset, size_t length)
{
    _InternalSkeleton *skeleton = (_InternalSkeleton *) opaque;
    skeleton->array[INDEX_INTLIFETIME].offset = offset;
    skeleton->array[INDEX_INTLIFETIME].length = length;

    if (metisLogger_IsLoggable(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
        metisLogger_Log(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                        "Set int lifetime extent {%u, %u}", offset, length);
    }
}

void
metisTlvSkeleton_SetCacheTimeHeader(MetisTlvSkeleton *opaque, size_t offset, size_t length)
{
    _InternalSkeleton *skeleton = (_InternalSkeleton *) opaque;
    skeleton->array[INDEX_CACHETIME].offset = offset;
    skeleton->array[INDEX_CACHETIME].length = length;

    if (metisLogger_IsLoggable(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
        metisLogger_Log(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                        "Set cachetime extent {%u, %u}", offset, length);
    }
}

void
metisTlvSkeleton_SetExpiryTime(MetisTlvSkeleton *opaque, size_t offset, size_t length)
{
    _InternalSkeleton *skeleton = (_InternalSkeleton *) opaque;
    skeleton->array[INDEX_EXPIRYTIME].offset = offset;
    skeleton->array[INDEX_EXPIRYTIME].length = length;

    if (metisLogger_IsLoggable(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
        metisLogger_Log(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                        "Set expirytime extent {%u, %u}", offset, length);
    }
}

void
metisTlvSkeleton_SetCPI(MetisTlvSkeleton *opaque, size_t offset, size_t length)
{
    _InternalSkeleton *skeleton = (_InternalSkeleton *) opaque;
    skeleton->array[INDEX_CPI].offset = offset;
    skeleton->array[INDEX_CPI].length = length;

    if (metisLogger_IsLoggable(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
        metisLogger_Log(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                        "Set cpi extent {%u, %u}", offset, length);
    }
}

bool
metisTlvSkeleton_UpdateHopLimit(MetisTlvSkeleton *opaque, uint8_t hoplimit)
{
    _InternalSkeleton *skeleton = (_InternalSkeleton *) opaque;
    bool updated = false;
    if (!metisTlvExtent_Equals(&skeleton->array[INDEX_HOPLIMIT], &metisTlvExtent_NotFound)) {
        if (skeleton->array[INDEX_HOPLIMIT].length == 1) {
            updated = true;
            uint8_t *value = skeleton->packet + skeleton->array[INDEX_HOPLIMIT].offset;
            *value = hoplimit;

            if (metisLogger_IsLoggable(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
                metisLogger_Log(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                                "set hoplimit %u", hoplimit);
            }
        }
    }
    return updated;
}

void
metisTlvSkeleton_SetFragmentPayload(MetisTlvSkeleton *opaque, size_t offset, size_t length)
{
    _InternalSkeleton *skeleton = (_InternalSkeleton *) opaque;
    skeleton->array[INDEX_FRAGMENTPAYLOAD].offset = offset;
    skeleton->array[INDEX_FRAGMENTPAYLOAD].length = length;

    if (metisLogger_IsLoggable(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug)) {
        metisLogger_Log(skeleton->logger, MetisLoggerFacility_Message, PARCLogLevel_Debug, __func__,
                        "Set fragment payload extent {%u, %u}", offset, length);
    }
}



// ==========================================================

MetisTlvExtent
metisTlvSkeleton_GetName(const MetisTlvSkeleton *skeleton)
{
    return skeleton->array[INDEX_NAME];
}

MetisTlvExtent
metisTlvSkeleton_GetKeyId(const MetisTlvSkeleton *skeleton)
{
    return skeleton->array[INDEX_KEYID];
}

MetisTlvExtent
metisTlvSkeleton_GetCertificate(const MetisTlvSkeleton *skeleton)
{
    return skeleton->array[INDEX_CERTIFICATE];
}

MetisTlvExtent
metisTlvSkeleton_GetPublicKey(const MetisTlvSkeleton *skeleton)
{
    return skeleton->array[INDEX_PUBKEY];
}

MetisTlvExtent
metisTlvSkeleton_GetObjectHash(const MetisTlvSkeleton *skeleton)
{
    return skeleton->array[INDEX_OBJHASH];
}

MetisTlvExtent
metisTlvSkeleton_GetHopLimit(const MetisTlvSkeleton *skeleton)
{
    return skeleton->array[INDEX_HOPLIMIT];
}

MetisTlvExtent
metisTlvSkeleton_GetInterestLifetime(const MetisTlvSkeleton *skeleton)
{
    return skeleton->array[INDEX_INTLIFETIME];
}

MetisTlvExtent
metisTlvSkeleton_GetCacheTimeHeader(const MetisTlvSkeleton *skeleton)
{
    return skeleton->array[INDEX_CACHETIME];
}

MetisTlvExtent
metisTlvSkeleton_GetExpiryTime(const MetisTlvSkeleton *skeleton)
{
    return skeleton->array[INDEX_EXPIRYTIME];
}

MetisTlvExtent
metisTlvSkeleton_GetCPI(const MetisTlvSkeleton *skeleton)
{
    return skeleton->array[INDEX_CPI];
}

MetisTlvExtent
metisTlvSkeleton_GetFragmentPayload(const MetisTlvSkeleton *skeleton)
{
    return skeleton->array[INDEX_FRAGMENTPAYLOAD];
}


const uint8_t *
metisTlvSkeleton_GetPacket(const MetisTlvSkeleton *opaque)
{
    const _InternalSkeleton *skeleton = (const _InternalSkeleton *) opaque;
    _assertInvariants(skeleton);
    return skeleton->packet;
}

PARCCryptoHash *
metisTlvSkeleton_ComputeContentObjectHash(const MetisTlvSkeleton *opaque)
{
    const _InternalSkeleton *skeleton = (const _InternalSkeleton *) opaque;
    _assertInvariants(skeleton);
    return skeleton->tlvOps->computeContentObjectHash(skeleton->packet);
}

size_t
metisTlvSkeleton_TotalPacketLength(const MetisTlvSkeleton *opaque)
{
    const _InternalSkeleton *skeleton = (const _InternalSkeleton *) opaque;
    _assertInvariants(skeleton);
    return skeleton->tlvOps->totalPacketLength(skeleton->packet);
}

bool
metisTlvSkeleton_IsPacketTypeInterest(const MetisTlvSkeleton *opaque)
{
    const _InternalSkeleton *skeleton = (const _InternalSkeleton *) opaque;
    _assertInvariants(skeleton);
    return skeleton->tlvOps->isPacketTypeInterest(skeleton->packet);
}

bool
metisTlvSkeleton_IsPacketTypeContentObject(const MetisTlvSkeleton *opaque)
{
    const _InternalSkeleton *skeleton = (const _InternalSkeleton *) opaque;
    _assertInvariants(skeleton);
    return skeleton->tlvOps->isPacketTypeContentObject(skeleton->packet);
}

bool
metisTlvSkeleton_IsPacketTypeControl(const MetisTlvSkeleton *opaque)
{
    const _InternalSkeleton *skeleton = (const _InternalSkeleton *) opaque;
    _assertInvariants(skeleton);
    return skeleton->tlvOps->isPacketTypeControl(skeleton->packet);
}

bool
metisTlvSkeleton_IsPacketTypeInterestReturn(const MetisTlvSkeleton *opaque)
{
    const _InternalSkeleton *skeleton = (const _InternalSkeleton *) opaque;
    _assertInvariants(skeleton);
    return skeleton->tlvOps->isPacketTypeInterestReturn(skeleton->packet);
}

bool
metisTlvSkeleton_IsPacketTypeHopByHopFragment(const MetisTlvSkeleton *opaque)
{
    const _InternalSkeleton *skeleton = (const _InternalSkeleton *) opaque;
    _assertInvariants(skeleton);
    return skeleton->tlvOps->isPacketTypeHopByHopFragment(skeleton->packet);
}

MetisLogger *
metisTlvSkeleton_GetLogger(const MetisTlvSkeleton *opaque)
{
    const _InternalSkeleton *skeleton = (const _InternalSkeleton *) opaque;
    _assertInvariants(skeleton);
    return skeleton->logger;
}
