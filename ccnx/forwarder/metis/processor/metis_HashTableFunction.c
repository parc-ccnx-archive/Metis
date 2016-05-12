/*
 * Copyright (c) 2013, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
//  metis_HashTableFunction.c
//  Metis
//
//  Created by Mosko, Marc <Marc.Mosko@parc.com> on 11/29/13.
//
//

#include <config.h>
#include <stdio.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Hash.h>

#include <ccnx/forwarder/metis/processor/metis_HashTableFunction.h>
#include <ccnx/forwarder/metis/core/metis_Message.h>

#include <LongBow/runtime.h>

// ======================================================================
// Hash table key functions
// We use a MetisMessage as the key data type

bool
metisHashTableFunction_MessageNameEquals(const void *keyA, const void *keyB)
{
    const MetisMessage *a = (const MetisMessage  *) keyA;
    const MetisMessage *b = (const MetisMessage  *) keyB;

    return metisTlvName_Equals(metisMessage_GetName(a), metisMessage_GetName(b));
}

HashCodeType
metisHashTableFunction_MessageNameHashCode(const void *keyA)
{
    const MetisMessage *message = (const MetisMessage *) keyA;
    MetisTlvName *name = metisMessage_GetName(message);

    // we want the cumulative hash for the whole name
    uint32_t hash = metisTlvName_HashCode(name);

    return hash;
}

bool
metisHashTableFunction_MessageNameAndKeyIdEquals(const void *keyA, const void *keyB)
{
    const MetisMessage *a = (const MetisMessage  *) keyA;
    const MetisMessage *b = (const MetisMessage  *) keyB;

    if (metisMessage_KeyIdEquals(a, b)) {
        if (metisTlvName_Equals(metisMessage_GetName(a), metisMessage_GetName(b))) {
            return true;
        }
    }
    return false;
}

HashCodeType
metisHashTableFunction_MessageNameAndKeyIdHashCode(const void *keyA)
{
    const MetisMessage *message = (const MetisMessage  *) keyA;

    uint32_t keyIdHash;

    bool hasKeyId = metisMessage_GetKeyIdHash(message, &keyIdHash);
    assertTrue(hasKeyId, "Called NameAndKeyIdHashCode for a message without a keyid");

    // we want the cumulative hash for the whole name
    MetisTlvName *name = metisMessage_GetName(message);
    uint32_t nameHash = metisTlvName_HashCode(name);

    // now combine the two hashes.  The KeyId hash is mixed in to the name hash.
    uint32_t hash = parcHash32_Data_Cumulative(&keyIdHash, sizeof(keyIdHash), nameHash);
    return hash;
}

bool
metisHashTableFunction_MessageNameAndObjectHashEquals(const void *keyA, const void *keyB)
{
    const MetisMessage *a = (const MetisMessage  *) keyA;
    const MetisMessage *b = (const MetisMessage  *) keyB;

    // due to lazy calculation of hash in content objects, need non-const
    if (metisMessage_ObjectHashEquals((MetisMessage  *) a, (MetisMessage  *) b)) {
        if (metisTlvName_Equals(metisMessage_GetName(a), metisMessage_GetName(b))) {
            return true;
        }
    }
    return false;
}

HashCodeType
metisHashTableFunction_MessageNameAndObjectHashHashCode(const void *keyA)
{
    const MetisMessage *message = (const MetisMessage  *) keyA;

    uint32_t contentObjectHashHash;

    bool hasObjectHash = metisMessage_GetContentObjectHashHash((MetisMessage *) message, &contentObjectHashHash);
    assertTrue(hasObjectHash, "Called metisPit_NameAndObjectHashHashCode for an interest without a ContentObjectHash restriction");

    // we want the cumulative hash for the whole name
    MetisTlvName *name = metisMessage_GetName(message);
    uint32_t nameHash = metisTlvName_HashCode(name);

    // now combine the two hashes
    uint32_t hash = parcHash32_Data_Cumulative(&contentObjectHashHash, sizeof(contentObjectHashHash), nameHash);
    return hash;
}

// ======================================================================
// TlvName variety

bool
metisHashTableFunction_TlvNameEquals(const void *keyA, const void *keyB)
{
    const MetisTlvName *a = (const MetisTlvName  *) keyA;
    const MetisTlvName *b = (const MetisTlvName  *) keyB;

    return metisTlvName_Equals(a, b);
}

int
metisHashTableFunction_TlvNameCompare(const void *keyA, const void *keyB)
{
    const MetisTlvName *a = (const MetisTlvName  *) keyA;
    const MetisTlvName *b = (const MetisTlvName  *) keyB;

    return metisTlvName_Compare(a, b);
}

HashCodeType
metisHashTableFunction_TlvNameHashCode(const void *keyA)
{
    MetisTlvName *name = (MetisTlvName *) keyA;

    // we want the cumulative hash for the whole name
    uint32_t hash = metisTlvName_HashCode(name);

    return hash;
}
