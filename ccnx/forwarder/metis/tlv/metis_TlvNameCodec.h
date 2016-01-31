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
 * @file tlv_NameCodec.h
 * @brief Encode/Decode a Name tlv
 *
 * Encodes a CCNxName to a Name TLV container plus one NameComponent TLV container
 * per name segment.
 *
 * Decodes a buffer as a Name TLV that contains one NameComponent TLV per name segment.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
#ifndef Metis_metis_tlv_NameCodec_h
#define Metis_metis_tlv_NameCodec_h

#include <ccnx/common/ccnx_Name.h>

/**
 * Decodes a byte array as the segments of a Name.
 *
 * The (buffer + offset) should point to the beginning of the first NameSegment.
 *
 * The length (end - offset) may be 0 length, in which case an empty name is returned.
 * Otherwise, it must be at least 4 bytes long.
 *
 * @param [in] buffer The byte array
 * @param [in] offset The starting location of the Name
 * @param [in] end The location just past the end of the name
 *
 * @return non-null the Name
 *
 * Example:
 * @code
 * {
 *            // offset        0     1     2     3     4     5     6     7     8   9   10   11   12     13
 *                               |-- type --|-- length --|-- type --|-- length --| ----- value -----|
 *     uint8_t buffer[] = { 0xFF, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, 0x00, 0x04, 'a', 'b', 'c', 'd', 0xFF };
 *
 *     // skip the two 0xFF bytes
 *     // name = "lci:/%02=abcd"
 *     CCNxName * name = tlvName_Decode(buffer, 5, 13);
 * }
 * @endcode
 *
 */
CCNxName *metisTlvNameCodec_Decode(uint8_t *buffer, size_t offset, size_t end);
#endif // Metis_metis_tlv_NameCodec_h
