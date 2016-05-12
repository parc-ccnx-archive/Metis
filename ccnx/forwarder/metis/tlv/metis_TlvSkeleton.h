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
 * @file metis_TlvSkeleton.h
 * @brief The structure used to store the set of fields used by the forwarder
 *
 * The TLV skeleton is the parsed form of the packet.  It contains the TLV extents of each
 * field relevant to the forwarder.
 *
 * To use MetisTlvSkeleton, you first parse a packet in to the skeleton and can then access the various
 * TlvExtents from the getters.  The Tlv parsers use the Setters.
 *
 * The TlvSkeleton is not allocated memory (in general).  It is defined as a sized opaque struct so one
 * can use it as a member of another struct without deep allocations.  The general use is as shown in
 * the example below.
 *
 * @code
 * typedef struct my_data_s {
 *      unsigned id;
 *      MetisTlvSkeleton skeleton;
 * } MyData;
 *
 * void foo(uint8_t *packetBuffer)
 * {
 *    MyData *mydata = parcMemory_Allocate(sizeof(MyData));
 *    mydata->id = _global_id++;
 *    metisTlvSkeleton_Parse(&mydata->skeleton, packetBuffer);
 *    // now forward the packet using the data in the skeleton
 *    parcMemory_Deallocate(&mydata);
 * }
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_TlvSkeleton_h
#define Metis_metis_TlvSkeleton_h

#include <ccnx/forwarder/metis/tlv/metis_TlvExtent.h>
#include <ccnx/forwarder/metis/core/metis_Logger.h>

#define MetisTlvSkeleton_ArrayLength 11

/**
 * The MetisTlvSkeleton is an opaque object defined in the header so it
 * can be pre-allocated as part of another data structure.  The user should have
 * no direct access to any of the fields.
 */
typedef struct tlv_skeleton {
    void *field1;
    void *field2;
    void *field3;

    MetisTlvExtent array[MetisTlvSkeleton_ArrayLength];
} MetisTlvSkeleton;

// ===================================
// Setters

/**
 * Fills in the packet TLV skeleton
 *
 * Sets the skeleton's tlv operations to the correct value and sets the packet buffer.
 * Will call metisTlvSkeleton_Initialize().
 *
 * Because the MetisTlvSkeleton is meant to be allocated as part of a larger object, it does
 * not store its own reference to the logger, as there is no _Destroy or _Release method.  Therefore,
 * the caller must ensure the logger stays allocated for the lifetime of the skeleton.
 *
 * @param [in] packet Packet memory, pointing to byte 0 of the fixed header
 * @param [in] skeleton An allocated MetisTlvSkeleton to fill in
 * @param [in] logger The logger to use
 *
 * @retval true Good parse
 * @retval false Error
 *
 * Example:
 * @code
 * {
 *    MetisTlvSkeleton skeleton;
 *    bool success = metisTlvSkeleton_Parse(&skeleton, packet);
 * }
 * @endcode
 */
bool metisTlvSkeleton_Parse(MetisTlvSkeleton *skeleton, uint8_t *packet, MetisLogger *logger);

/**
 * Sets the Name extent
 *
 * Sets the name extent to the specified offset and length.
 *
 * @param [in] skeleton A MetisTlvSkeleton structure
 * @param [in] offset The byte offset of the beginning of the 'value'
 * @param [in] length The byte length of the 'value'
 *
 * Example:
 * @code
 * {
 *    MetisTlvType *tlv = (MetisTlvType *) (packet + offset);
 *    uint16_t type = htons(tlv->type);
 *    uint16_t v_length = htons(tlv->length);
 *    offset += sizeof(MetisTlvType);
 *
 *    if (type == _ParserNameType) {
 *        metisTlvSkeleton_SetName(skeleton, offset, v_length);
 *    }
 * }
 * @endcode
 */
void metisTlvSkeleton_SetName(MetisTlvSkeleton *skeleton, size_t offset, size_t length);

/**
 * Sets the KeyId extent
 *
 * For an Interest, the KeyId extent is the KeyIdRestriction.  For a ContentObject, it is
 * the KeyId in the Validation Algorithm.
 *
 * @param [in] skeleton A MetisTlvSkeleton structure
 * @param [in] offset The byte offset of the beginning of the 'value'
 * @param [in] length The byte length of the 'value'
 *
 * Example:
 * @code
 * {
 *    MetisTlvType *tlv = (MetisTlvType *) (packet + offset);
 *    uint16_t type = htons(tlv->type);
 *    uint16_t v_length = htons(tlv->length);
 *    offset += sizeof(MetisTlvType);
 *
 *    if (type == _ParserKeyIdType) {
 *        metisTlvSkeleton_SetKeyId(skeleton, offset, v_length);
 *    }
 * }
 * @endcode
 */
void metisTlvSkeleton_SetKeyId(MetisTlvSkeleton *skeleton, size_t offset, size_t length);

/*
 * Sets the Certificate extent.
 *
 * In a ContentObject, in the Validation dependent data may contain a certificate.
 * Use this to set the extents of the certificate.
 *
 * @param [in] skeleton A MetisTlvSkeleton structure
 * @param [in] offset The byte offset of the beginning of the certificate.
 * @param [in] length The byte length of the certificate.
 */
void metisTlvSkeleton_SetCertificate(MetisTlvSkeleton *skeleton, size_t offset, size_t length);

/*
 * Sets the Public Key extent.
 *
 * In a ContentObject, in the Validation dependent data may contain a public key.
 * Use this to set the extents of the certificate.
 *
 * @param [in] skeleton A MetisTlvSkeleton structure
 * @param [in] offset The byte offset of the beginning of the public key.
 * @param [in] length The byte length of the public key.
 */
void metisTlvSkeleton_SetPublicKey(MetisTlvSkeleton *skeleton, size_t offset, size_t length);

/**
 * Sets the Content Object Hash extent
 *
 * For an Interest, this is the Content Object hash restriction
 *
 * @param [in] skeleton A MetisTlvSkeleton structure
 * @param [in] offset The byte offset of the beginning of the 'value'
 * @param [in] length The byte length of the 'value'
 *
 * Example:
 * @code
 * {
 *    MetisTlvType *tlv = (MetisTlvType *) (packet + offset);
 *    uint16_t type = htons(tlv->type);
 *    uint16_t v_length = htons(tlv->length);
 *    offset += sizeof(MetisTlvType);
 *
 *    if (type == _ParserKeyIdType) {
 *        metisTlvSkeleton_SetObjectHash(skeleton, offset, v_length);
 *    }
 * }
 * @endcode
 */
void metisTlvSkeleton_SetObjectHash(MetisTlvSkeleton *skeleton, size_t offset, size_t length);

/**
 * Sets the Hop Limit extent
 *
 * For an Interest, the hoplimit is found in the fixed header (v1) or in a perhop header (v0)
 *
 * @param [in] skeleton A MetisTlvSkeleton structure
 * @param [in] offset The byte offset of the beginning of the 'value'
 * @param [in] length The byte length of the 'value'
 *
 * Example:
 * @code
 * {
 *    // For a version 1 packet, point it to the field in the fixed header
 *    metisTlvSkeleton_SetHopLimit(skeleton, 4, 1);
 * }
 * @endcode
 */
void metisTlvSkeleton_SetHopLimit(MetisTlvSkeleton *skeleton, size_t offset, size_t length);

/**
 * Sets the Interest Lifetime extent
 *
 * Sets the extent for the Interest lifetime
 *
 * @param [in] skeleton A MetisTlvSkeleton structure
 * @param [in] offset The byte offset of the beginning of the 'value'
 * @param [in] length The byte length of the 'value'
 *
 * Example:
 * @code
 * {
 *    MetisTlvType *tlv = (MetisTlvType *) (packet + offset);
 *    uint16_t type = htons(tlv->type);
 *    uint16_t v_length = htons(tlv->length);
 *    offset += sizeof(MetisTlvType);
 *
 *    if (type == _ParserInterestLifetimeType) {
 *        metisTlvSkeleton_SetInterestLifetime(skeleton, offset, v_length);
 *    }
 * }
 * @endcode
 */
void metisTlvSkeleton_SetInterestLifetime(MetisTlvSkeleton *skeleton, size_t offset, size_t length);

/**
 * Sets the Recommended Cache Time extent
 *
 * Sets the extent for the Recommended Cache Time for a Content Object
 *
 * @param [in] skeleton A MetisTlvSkeleton structure
 * @param [in] offset The byte offset of the beginning of the 'value'
 * @param [in] length The byte length of the 'value'
 *
 * Example:
 * @code
 * {
 *    MetisTlvType *tlv = (MetisTlvType *) (packet + offset);
 *    uint16_t type = htons(tlv->type);
 *    uint16_t v_length = htons(tlv->length);
 *    offset += sizeof(MetisTlvType);
 *
 *    if (type == _ParserRecommendedCacheTimeType) {
 *        metisTlvSkeleton_SetCacheTimeHeader(skeleton, offset, v_length);
 *    }
 * }
 * @endcode
 */
void metisTlvSkeleton_SetCacheTimeHeader(MetisTlvSkeleton *skeleton, size_t offset, size_t length);

/**
 * Sets the ExpiryTime extent
 *
 * Sets the extent for the Expiry Time for a Content Object
 *
 * @param [in] skeleton A MetisTlvSkeleton structure
 * @param [in] offset The byte offset of the beginning of the 'value'
 * @param [in] length The byte length of the 'value'
 *
 * Example:
 * @code
 * {
 *    MetisTlvType *tlv = (MetisTlvType *) (packet + offset);
 *    uint16_t type = htons(tlv->type);
 *    uint16_t v_length = htons(tlv->length);
 *    offset += sizeof(MetisTlvType);
 *
 *    if (type == _ParserExpiryTimeType) {
 *        metisTlvSkeleton_SetExpiryTime(skeleton, offset, v_length);
 *    }
 * }
 * @endcode
 */
void metisTlvSkeleton_SetExpiryTime(MetisTlvSkeleton *skeleton, size_t offset, size_t length);

/**
 * Sets the Control Plane Interface (CPI) extent
 *
 * Sets the extent for the CPI value.
 *
 * @param [in] skeleton A MetisTlvSkeleton structure
 * @param [in] offset The byte offset of the beginning of the 'value'
 * @param [in] length The byte length of the 'value'
 *
 * Example:
 * @code
 * {
 *    MetisTlvType *tlv = (MetisTlvType *) (packet + offset);
 *    uint16_t type = htons(tlv->type);
 *    uint16_t v_length = htons(tlv->length);
 *    offset += sizeof(MetisTlvType);
 *
 *    if (type == _ParserControlType) {
 *        metisTlvSkeleton_SetCPI(skeleton, offset, v_length);
 *    }
 * }
 * @endcode
 */
void metisTlvSkeleton_SetCPI(MetisTlvSkeleton *skeleton, size_t offset, size_t length);

/**
 * Sets the Fragment Payload extent
 *
 * This is the payload of the fragment, i.e. part of the original packet.
 *
 * @param [in] skeleton A MetisTlvSkeleton structure
 * @param [in] offset The byte offset of the beginning of the 'value'
 * @param [in] length The byte length of the 'value'
 *
 * Example:
 * @code
 * {
 *    MetisTlvType *tlv = (MetisTlvType *) (packet + offset);
 *    uint16_t type = htons(tlv->type);
 *    uint16_t v_length = htons(tlv->length);
 *    offset += sizeof(MetisTlvType);
 *
 *    if (type == _ParserFragmentPayloadType) {
 *        metisTlvSkeleton_SetFragmentPayload(skeleton, offset, v_length);
 *    }
 * }
 * @endcode
 */
void metisTlvSkeleton_SetFragmentPayload(MetisTlvSkeleton *skeleton, size_t offset, size_t length);

/**
 * Updates the HopLimit inside the packet buffer
 *
 * If the HopLimit extent is not metisTlvExtent_NotFound, it will update the specified location
 * with the new hoplimit.  The call will fail if the HopLimit extent is not exactly 1 byte.
 *
 * @param [in] skeleton A parsed MetisTlvSkeleton
 * @param [in] hoplimit The new value
 *
 * @retval true Updated
 * @retval false Not updated (likely was metisTlvExtent_NotFound)
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisTlvSkeleton_UpdateHopLimit(MetisTlvSkeleton *skeleton, uint8_t hoplimit);

// ====================================
// Getters

/**
 * Returns the extent of the Name
 *
 * Returns the previously set Name extent or metisTlvExtent_NotFound if not set.
 *
 * @param [in] skeleton An initialized and parsed skeleton
 *
 * @retval {0,0} Not Present
 * @retval other The set value of the extent
 *
 * Example:
 * @code
 * {
 *    uint8_t *packet = // read from network
 *    MetisTlvSkeleton skeleton;
 *    if (metisTlvSkeleton_Parse(&skeleton, packet)) {
 *       MetisTlvExtent extent = metisTlvSkeleton_GetName(&skeleton);
 *    }
 * }
 * @endcode
 */
MetisTlvExtent metisTlvSkeleton_GetName(const MetisTlvSkeleton *skeleton);

/**
 * Returns the extent of the KeyId
 *
 * Returns the previously set KeyId extent or metisTlvExtent_NotFound if not set.
 *
 * @param [in] skeleton An initialized and parsed skeleton
 *
 * @retval {0,0} Not Present
 * @retval other The set value of the extent
 *
 * Example:
 * @code
 * {
 *    uint8_t *packet = // read from network
 *    MetisTlvSkeleton skeleton;
 *    if (metisTlvSkeleton_Parse(&skeleton, packet)) {
 *       MetisTlvExtent extent = metisTlvSkeleton_GetKeyId(&skeleton);
 *    }
 * }
 * @endcode
 */
MetisTlvExtent metisTlvSkeleton_GetKeyId(const MetisTlvSkeleton *skeleton);

/**
 * Returns the extent of the Public Key.
 *
 * Returns the previously set Public Key extent or metisTlvExtent_NotFound if not set.
 *
 * @param [in] skeleton An initialized and parsed skeleton
 *
 * @retval {0,0} Not Present
 * @retval other The set value of the extent
 *
 * Example:
 * @code
 * {
 *    uint8_t *packet = // read from network
 *    MetisTlvSkeleton skeleton;
 *    if (metisTlvSkeleton_Parse(&skeleton, packet)) {
 *       MetisTlvExtent extent = metisTlvSkeleton_GetPublicKey(&skeleton);
 *    }
 * }
 * @endcode
 */
MetisTlvExtent metisTlvSkeleton_GetPublicKey(const MetisTlvSkeleton *skeleton);

/**
 * Returns the extent of the Certificate.
 *
 * Returns the previously set Certificate extent or metisTlvExtent_NotFound if not set.
 *
 * @param [in] skeleton An initialized and parsed skeleton
 *
 * @retval {0,0} Not Present
 * @retval other The set value of the extent
 *
 * Example:
 * @code
 * {
 *    uint8_t *packet = // read from network
 *    MetisTlvSkeleton skeleton;
 *    if (metisTlvSkeleton_Parse(&skeleton, packet)) {
 *       MetisTlvExtent extent = metisTlvSkeleton_GetCertificate(&skeleton);
 *    }
 * }
 * @endcode
 */
MetisTlvExtent metisTlvSkeleton_GetCertificate(const MetisTlvSkeleton *skeleton);

/**
 * Returns the extent of the Object Hash
 *
 * Returns the previously set Object hash extent or metisTlvExtent_NotFound if not set.
 *
 * @param [in] skeleton An initialized and parsed skeleton
 *
 * @retval {0,0} Not Present
 * @retval other The set value of the extent
 *
 * Example:
 * @code
 * {
 *    uint8_t *packet = // read from network
 *    MetisTlvSkeleton skeleton;
 *    if (metisTlvSkeleton_Parse(&skeleton, packet)) {
 *       MetisTlvExtent extent = metisTlvSkeleton_GetObjectHash(&skeleton);
 *    }
 * }
 * @endcode
 */
MetisTlvExtent metisTlvSkeleton_GetObjectHash(const MetisTlvSkeleton *skeleton);

/**
 * Returns the extent of the Hop Limit
 *
 * Returns the previously set Hop Limit extent or metisTlvExtent_NotFound if not set.
 *
 * @param [in] skeleton An initialized and parsed skeleton
 *
 * @retval {0,0} Not Present
 * @retval other The set value of the extent
 *
 * Example:
 * @code
 * {
 *    uint8_t *packet = // read from network
 *    MetisTlvSkeleton skeleton;
 *    if (metisTlvSkeleton_Parse(&skeleton, packet)) {
 *       MetisTlvExtent extent = metisTlvSkeleton_GetObjectHash(&skeleton);
 *    }
 * }
 * @endcode
 */
MetisTlvExtent metisTlvSkeleton_GetHopLimit(const MetisTlvSkeleton *skeleton);

/**
 * Returns the extent of the Interest Lifetime
 *
 * Returns the previously set Interest Lifetime extent or metisTlvExtent_NotFound if not set.
 *
 * @param [in] skeleton An initialized and parsed skeleton
 *
 * @retval {0,0} Not Present
 * @retval other The set value of the extent
 *
 * Example:
 * @code
 * {
 *    uint8_t *packet = // read from network
 *    MetisTlvSkeleton skeleton;
 *    if (metisTlvSkeleton_Parse(&skeleton, packet)) {
 *       MetisTlvExtent extent = metisTlvSkeleton_GetInterestLifetime(&skeleton);
 *    }
 * }
 * @endcode
 */
MetisTlvExtent metisTlvSkeleton_GetInterestLifetime(const MetisTlvSkeleton *skeleton);

/**
 * Returns the extent of the Recommended Cache Time
 *
 * Returns the previously set Recommended Cache Time extent or metisTlvExtent_NotFound if not set.
 *
 * @param [in] skeleton An initialized and parsed skeleton
 *
 * @retval {0,0} Not Present
 * @retval other The set value of the extent
 *
 * Example:
 * @code
 * {
 *    uint8_t *packet = // read from network
 *    MetisTlvSkeleton skeleton;
 *    if (metisTlvSkeleton_Parse(&skeleton, packet)) {
 *       MetisTlvExtent extent = metisTlvSkeleton_GetCacheTimeHeader(&skeleton);
 *    }
 * }
 * @endcode
 */
MetisTlvExtent metisTlvSkeleton_GetCacheTimeHeader(const MetisTlvSkeleton *skeleton);

/**
 * Returns the extent of the Expiry Time
 *
 * Returns the previously set Expiry Time extent or metisTlvExtent_NotFound if not set.
 *
 * @param [in] skeleton An initialized and parsed skeleton
 *
 * @retval {0,0} Not Present
 * @retval other The set value of the extent
 *
 * Example:
 * @code
 * {
 *    uint8_t *packet = // read from network
 *    MetisTlvSkeleton skeleton;
 *    if (metisTlvSkeleton_Parse(&skeleton, packet)) {
 *       MetisTlvExtent extent = metisTlvSkeleton_GetExpiryTime(&skeleton);
 *    }
 * }
 * @endcode
 */
MetisTlvExtent metisTlvSkeleton_GetExpiryTime(const MetisTlvSkeleton *skeleton);

/**
 * Returns the extent of the CPI payload
 *
 * Returns the previously set CPI payload extent or metisTlvExtent_NotFound if not set.
 *
 * @param [in] skeleton An initialized and parsed skeleton
 *
 * @retval {0,0} Not Present
 * @retval other The set value of the extent
 *
 * Example:
 * @code
 * {
 *    uint8_t *packet = // read from network
 *    MetisTlvSkeleton skeleton;
 *    if (metisTlvSkeleton_Parse(&skeleton, packet)) {
 *       MetisTlvExtent extent = metisTlvSkeleton_GetCPI(&skeleton);
 *    }
 * }
 * @endcode
 */
MetisTlvExtent metisTlvSkeleton_GetCPI(const MetisTlvSkeleton *skeleton);

/**
 * Returns the extent of the fragment payload
 *
 * Returns the previously set fragment payload extent or metisTlvExtent_NotFound if not set.
 *
 * @param [in] skeleton An initialized and parsed skeleton
 *
 * @retval {0,0} Not Present
 * @retval other The set value of the extent
 *
 * Example:
 * @code
 * {
 *    uint8_t *packet = // read from network
 *    MetisTlvSkeleton skeleton;
 *    if (metisTlvSkeleton_Parse(&skeleton, packet)) {
 *       MetisTlvExtent extent = metisTlvSkeleton_GetFragmentPayload(&skeleton);
 *    }
 * }
 * @endcode
 */
MetisTlvExtent metisTlvSkeleton_GetFragmentPayload(const MetisTlvSkeleton *skeleton);

/**
 * Returns the pointer to the packet buffer
 *
 * Returns pointer to byte 0 of the fixed header
 *
 * @param [in] skeleton A parsed MetisTlvSkeleton
 *
 * @retval non-null The packet buffer
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
const uint8_t *metisTlvSkeleton_GetPacket(const MetisTlvSkeleton *skeleton);

/**
 * The total packet length based on the fixed header
 *
 * Parses the fixed header and returns the total packet length
 * Will return "0" for unknown packet version
 *
 * @param [in] packet Packet memory pointer
 *
 * @retval number Total packet length
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
size_t metisTlvSkeleton_TotalPacket(const MetisTlvSkeleton *skeleton);

/**
 * @function metisTlv_ComputeContentObjectHash
 * @abstract Computes the ContentObjectHash over a packet
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return NULL if not a content object, otherwise the SHA256 hash
 */
PARCCryptoHash *metisTlvSkeleton_ComputeContentObjectHash(const MetisTlvSkeleton *skeleton);

/**
 * Determines if the packet type is Interest
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] packet Packet memory pointer
 *
 * @retval true It is an Interest
 * @retval false Not an Interest
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisTlvSkeleton_IsPacketTypeInterest(const MetisTlvSkeleton *skeleton);

/**
 * Determines if the packet type is Content Object
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] packet Packet memory pointer
 *
 * @retval true It is a ContentObject
 * @retval false Not a ContentObject
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisTlvSkeleton_IsPacketTypeContentObject(const MetisTlvSkeleton *skeleton);

/**
 * Determines if the packet type is Control
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] packet Packet memory pointer
 *
 * @retval true It is a Control
 * @retval false Not a Control
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisTlvSkeleton_IsPacketTypeControl(const MetisTlvSkeleton *skeleton);

/**
 * Determines if the packet type is InterestReturn
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] packet Packet memory pointer
 *
 * @retval true It is a InterestReturn
 * @retval false Not a InterestReturn
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisTlvSkeleton_IsPacketTypeInterestReturn(const MetisTlvSkeleton *skeleton);

/**
 * Determines if the packet type is HopByHop Fragment
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] packet Packet memory pointer
 *
 * @retval true It is a InterestReturn
 * @retval false Not a InterestReturn
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisTlvSkeleton_IsPacketTypeHopByHopFragment(const MetisTlvSkeleton *skeleton);

/**
 * Returns the logger associated with the skeleton
 *
 * Returns the logger the user passed to the skeleton, which may be NULL
 *
 * @param [in] skeleton An initialized skeleton
 *
 * @retval non-null An allocated logger
 * @retval null No logger associated with skeleton
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisLogger *metisTlvSkeleton_GetLogger(const MetisTlvSkeleton *skeleton);
#endif // Metis_metis_TlvSkeleton_h
