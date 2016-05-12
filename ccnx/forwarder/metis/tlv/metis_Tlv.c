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

#include <LongBow/runtime.h>

#include <stdio.h>


#include <parc/algol/parc_Memory.h>

#include <ccnx/forwarder/metis/tlv/metis_Tlv.h>
#include <ccnx/forwarder/metis/tlv/metis_TlvSchemaV0.h>
#include <ccnx/forwarder/metis/tlv/metis_TlvSchemaV1.h>
#include <ccnx/forwarder/metis/core/metis_Forwarder.h>

// a reasonably large enough number that we capture name parsing without
// needing to re-alloc.  Not a big deal if this is wrong, it just means
// we have to do one pass to count and another pass to fillin, and waste
// one malloc and free.
static const size_t _initialLengthForNameExtents = 24;


// -----------------------------

size_t
metisTlv_FixedHeaderLength(void)
{
    // at some point this will no longer be true and we will have to refactor
    return 8;
}


size_t
metisTlv_TotalHeaderLength(const uint8_t *packet)
{
    size_t length = 0;
    uint8_t version = packet[0];
    switch (version) {
        case 0:
            length = MetisTlvSchemaV0_Ops.totalHeaderLength(packet); // Deprecated
            break;

        case 1:
            length = MetisTlvSchemaV1_Ops.totalHeaderLength(packet);
            break;

        default:
            break;
    }
    return length;
}

size_t
metisTlv_TotalPacketLength(const uint8_t *packet)
{
    size_t length = 0;
    uint8_t version = packet[0];
    switch (version) {
        case 0:
            length = MetisTlvSchemaV0_Ops.totalPacketLength(packet); // Deprecated
            break;

        case 1:
            length = MetisTlvSchemaV1_Ops.totalPacketLength(packet);
            break;

        default:
            break;
    }
    return length;
}

PARCBuffer *
metisTlv_EncodeControlPlaneInformation(const CCNxControl *cpiControlMessage)
{
    PARCBuffer *encoded = NULL;
    CCNxTlvDictionary_SchemaVersion version = ccnxTlvDictionary_GetSchemaVersion(cpiControlMessage);
    switch (version) {
        case CCNxTlvDictionary_SchemaVersion_V0:
            encoded = MetisTlvSchemaV0_Ops.encodeControlPlaneInformation(cpiControlMessage);
            break;

        case CCNxTlvDictionary_SchemaVersion_V1:
            encoded = MetisTlvSchemaV1_Ops.encodeControlPlaneInformation(cpiControlMessage);
            break;

        default:
            break;
    }
    return encoded;
}

/**
 * @function metisTlv_ParseName
 * @abstract Parse a name into the provided output array, ensuring it does not exceed outputLength
 * @discussion
 *   <#Discussion#>
 *
 * @param outputArray may be NULL to count the number of name elements.
 * @para outputLength is the maximum number of name segments to parse in to outputArray
 * @return The number of name elements parsed
 */
static size_t
_metisTlv_ParseName(uint8_t *name, size_t nameLength, MetisTlvExtent *outputArray, size_t outputLength)
{
    size_t offset = 0;
    size_t count = 0;
    const size_t tl_length = 4;
    while (offset < nameLength) {
        MetisTlvType *tlv = (MetisTlvType *) (name + offset);
        uint16_t v_length = htons(tlv->length);

        if (count < outputLength) {
            outputArray[count].offset = offset;
            outputArray[count].length = tl_length + v_length;
        }

        // skip past the TL and V
        offset += tl_length + v_length;
        count++;
    }
    return count;
}

void
metisTlv_NameSegments(uint8_t *name, size_t nameLength, MetisTlvExtent **outputArrayPtr, size_t *outputLengthPtr)
{
    // allocate an array that's kind of big.  if name does not fit, we'll need to re-alloc.
    MetisTlvExtent *output = parcMemory_Allocate(_initialLengthForNameExtents * sizeof(MetisTlvExtent));
    assertNotNull(output, "parcMemory_Allocate(%zu) returned NULL", _initialLengthForNameExtents * sizeof(MetisTlvExtent));

    size_t actualLength = _metisTlv_ParseName(name, nameLength, output, _initialLengthForNameExtents);
    if (actualLength > _initialLengthForNameExtents) {
        // Oops, do over
        parcMemory_Deallocate((void **) &output);
        output = parcMemory_Allocate(actualLength * sizeof(MetisTlvExtent));
        assertNotNull(output, "parcMemory_Allocate(%zu) returned NULL", actualLength * sizeof(MetisTlvExtent));
        _metisTlv_ParseName(name, nameLength, output, actualLength);
    }

    *outputArrayPtr = output;
    *outputLengthPtr = actualLength;
}

bool
metisTlv_ExtentToVarInt(const uint8_t *packet, const MetisTlvExtent *extent, uint64_t *output)
{
    assertNotNull(packet, "Parameter buffer must be non-null");
    assertNotNull(extent, "Parameter output must be non-null");

    bool success = false;
    if (extent->length >= 1 && extent->length <= 8) {
        uint64_t value = 0;
        for (int i = 0; i < extent->length; i++) {
            value = value << 8 | packet[extent->offset + i];
        }
        *output = value;
        success = true;
    }
    return success;
}

