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

#include <arpa/inet.h>

#include <parc/algol/parc_Memory.h>
#include <LongBow/runtime.h>

#include "metis_TlvNameCodec.h"
#include "metis_Tlv.h"

CCNxName *
metisTlvNameCodec_Decode(uint8_t *buffer, size_t offset, size_t end)
{
    assertNotNull(buffer, "Parameter buffer must be non-null");
    assertTrue(end >= offset, "Buffer must be at least 4 bytes");

    CCNxName *ccnxName = ccnxName_Create();

    while (offset < end) {
        trapIllegalValueIf(end < offset + 4, "Buffer must be at least 4 bytes")
        {
            ccnxName_Release(&ccnxName);
        }

        MetisTlvType *tlv = (MetisTlvType *) (buffer + offset);
        uint16_t type = htons(tlv->type);
        uint16_t length = htons(tlv->length);

        offset += sizeof(MetisTlvType);

        trapIllegalValueIf(offset + length > end, "name component extends beyond end of name")
        {
            ccnxName_Release(&ccnxName);
        }

        PARCBuffer *nameValue = parcBuffer_Wrap(&buffer[offset], length, 0, length);
        CCNxNameSegment *segment = ccnxNameSegment_CreateTypeValue(type, nameValue);
        parcBuffer_Release(&nameValue);

        ccnxName_Append(ccnxName, segment);
        ccnxNameSegment_Release(&segment);

        offset += length;
    }

    return ccnxName;
}

