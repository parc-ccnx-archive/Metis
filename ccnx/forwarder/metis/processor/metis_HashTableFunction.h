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
 * @file metis_HashTableFunction.h
 * @brief These functions are used in PARCHashCodeTables by the
 * MatchingRulesTable and ContentStore and PIT. They perform the equality
 * and has generation needed by the PARCHashCodeTable.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#ifndef Metis_metis_HashTableFunction_h
#define Metis_metis_HashTableFunction_h

#include <parc/algol/parc_HashCodeTable.h>

// ==========================================================
// These functions operate on a MetisMessage as the key in the HashTable.
// The functions use void * rather than MetisMessage instances in the function
// signature because it is using generic has code tables from PARC Library

/**
 * Determine if the Names of two `MetisMessage` instances are equal.
 *
 * The following equivalence relations on non-null `MetisMessage` instances are maintained:
 *
 *  * It is reflexive: for any non-null reference value x, `metisHashTableFunction_MessageNameEquals(x, x)`
 *      must return true.
 *
 *  * It is symmetric: for any non-null reference values x and y,
 *    `MetisMessage_Equals(x, y)` must return true if and only if
 *        `metisHashTableFunction_MessageNameEquals(y, x)` returns true.
 *
 *  * It is transitive: for any non-null reference values x, y, and z, if
 *        `metisHashTableFunction_MessageNameEquals(x, y)` returns true and
 *        `metisHashTableFunction_MessageNameEquals(y, z)` returns true,
 *        then  `metisHashTableFunction_MessageNameEquals(x, z)` must return true.
 *
 *  * It is consistent: for any non-null reference values x and y, multiple
 *      invocations of `metisHashTableFunction_MessageNameEquals(x, y)` consistently return true or
 *      consistently return false.
 *
 *  * For any non-null reference value x, `metisHashTableFunction_MessageNameEquals(x, NULL)` must
 *      return false.
 *
 * @param a A pointer to a `MetisMessage` instance.
 * @param b A pointer to a `MetisMessage` instance.
 * @return true if the names of the two `MetisMessage` instances are equal.
 *
 * Example:
 * @code
 * {
 *    MetisMessage *a = MetisMessage_Create();
 *    MetisMessage *b = MetisMessage_Create();
 *
 *    if (metisHashTableFunction_MessageNameEquals(a, b)) {
 *        // true
 *    } else {
 *        // false
 *    }
 * }
 * @endcode
 */
bool metisHashTableFunction_MessageNameEquals(const void *metisMessageA, const void *metisMessageB);

/**
 * @function hashTableFunction_NameHashCode
 * @abstract Computes the hash of the entire name in a MetisMessage
 * @discussion
 *   <#Discussion#>
 *
 * @param metisMessageA is a MetisMessage
 * @return A non-cryptographic hash of Name
 */
HashCodeType metisHashTableFunction_MessageNameHashCode(const void *metisMessageA);

/**
 * Determine if the Names and KeyIds of two MetisMessage instances are equal.
 *
 *
 * The following equivalence relations on non-null `MetisMessage` instances are maintained:
 *
 *  * It is reflexive: for any non-null reference value x, `metisHashTableFunction_MessageNameAndKeyIdEquals(x, x)`
 *      must return true.
 *
 *  * It is symmetric: for any non-null reference values x and y,
 *    `metisHashTableFunction_MessageNameAndKeyIdEquals(x, y)` must return true if and only if
 *        `metisHashTableFunction_MessageNameAndKeyIdEquals(y, x)` returns true.
 *
 *  * It is transitive: for any non-null reference values x, y, and z, if
 *        `metisHashTableFunction_MessageNameAndKeyIdEquals(x, y)` returns true and
 *        `metisHashTableFunction_MessageNameAndKeyIdEquals(y, z)` returns true,
 *        then  `metisHashTableFunction_MessageNameAndKeyIdEquals(x, z)` must return true.
 *
 *  * It is consistent: for any non-null reference values x and y, multiple
 *      invocations of `metisHashTableFunction_MessageNameAndKeyIdEquals(x, y)` consistently
 *      return true or consistently return false.
 *
 *  * For any non-null reference value x, `metisHashTableFunction_MessageNameAndKeyIdEquals(x, NULL)`
 *      must return false.
 *
 * @param a A pointer to a `MetisMessage` instance.
 * @param b A pointer to a `MetisMessage` instance.
 * @return true if the Name and KeyId tuple of the two `MetisMessage` instances are equal.
 *
 * Example:
 * @code
 * {
 *    MetisMessage *a = MetisMessage_Create();
 *    MetisMessage *b = MetisMessage_Create();
 *
 *    if (metisHashTableFunction_MessageNameAndKeyIdEquals(a, b)) {
 *        // true
 *    } else {
 *        // false
 *    }
 * }
 * @endcode
 */

bool metisHashTableFunction_MessageNameAndKeyIdEquals(const void *metisMessageA, const void *metisMessageB);

/**
 * @function hashTableFunction_NameAndKeyIdHashCode
 * @abstract Generates a hash code on the tuple (Name, KeyId)
 * @discussion
 *   <#Discussion#>
 *
 * @param metisMessageA is a MetisMessage
 * @return A non-cryptographic hash of (Name, KeyId)
 */
HashCodeType metisHashTableFunction_MessageNameAndKeyIdHashCode(const void *metisMessageA);

/**
 * Determine if the (Name, ContentObjectHash) tuple of two `MetisMessage` instances are equal.
 *
 * The following equivalence relations on non-null `MetisMessage` instances are maintained:
 *
 *  * It is reflexive: for any non-null reference value x, `metisHashTableFunction_MessageNameAndObjectHashEquals(x, x)`
 *      must return true.
 *
 *  * It is symmetric: for any non-null reference values x and y,
 *    `metisHashTableFunction_MessageNameAndObjectHashEquals(x, y)` must return true if and only if
 *        `metisHashTableFunction_MessageNameAndObjectHashEquals(y, x)` returns true.
 *
 *  * It is transitive: for any non-null reference values x, y, and z, if
 *        `metisHashTableFunction_MessageNameAndObjectHashEquals(x, y)` returns true and
 *        `metisHashTableFunction_MessageNameAndObjectHashEquals(y, z)` returns true,
 *        then  `metisHashTableFunction_MessageNameAndObjectHashEquals(x, z)` must return true.
 *
 *  * It is consistent: for any non-null reference values x and y, multiple
 *      invocations of `metisHashTableFunction_MessageNameAndObjectHashEquals(x, y)` consistently
 *      return true or consistently return false.
 *
 *  * For any non-null reference value x, `metisHashTableFunction_MessageNameAndObjectHashEquals(x, NULL)`
 *      must return false.
 *
 * @param a A pointer to a `MetisMessage` instance.
 * @param b A pointer to a `MetisMessage` instance.
 * @return true if the (Name, ContentObjectHash)tuple of the two `MetisMessage` instances are equal.
 *
 * Example:
 * @code
 * {
 *    MetisMessage *a = MetisMessage_Create();
 *    MetisMessage *b = MetisMessage_Create();
 *
 *    if (metisHashTableFunction_MessageNameAndObjectHashEquals(a, b)) {
 *        // true
 *    } else {
 *        // false
 *    }
 * }
 * @endcode
 */
bool metisHashTableFunction_MessageNameAndObjectHashEquals(const void *metisMessageA, const void *metisMessageB);

/**
 * @function hashTableFunction_NameAndObjectHashHashCode
 * @abstract <#OneLineDescription#>
 * @discussion
 *   <#Discussion#>
 *
 * @param metisMessageA is a MetisMessage
 * @return A non-cryptographic hash of (Name, ContentObjectHash)
 */
HashCodeType metisHashTableFunction_MessageNameAndObjectHashHashCode(const void *metisMessageA);

// ==========================================================
// These functions operate on a MetisTlvName as the key of the hash table

/**
 * Determine if two `MetisTlvName` instances in the keys of the hash table are equal.
 *
 * The following equivalence relations on non-null `MetisTlvName` instances are maintained:
 *
 *  * It is reflexive: for any non-null reference value x, `metisHashTableFunction_TlvNameEquals(x, x)`
 *      must return true.
 *
 *  * It is symmetric: for any non-null reference values x and y,
 *    `metisHashTableFunction_TlvNameEquals(x, y)` must return true if and only if
 *        `metisHashTableFunction_TlvNameEquals(y, x)` returns true.
 *
 *  * It is transitive: for any non-null reference values x, y, and z, if
 *        `metisHashTableFunction_TlvNameEquals(x, y)` returns true and
 *        `metisHashTableFunction_TlvNameEquals(y, z)` returns true,
 *        then  `metisHashTableFunction_TlvNameEquals(x, z)` must return true.
 *
 *  * It is consistent: for any non-null reference values x and y, multiple
 *      invocations of `metisHashTableFunction_TlvNameEquals(x, y)` consistently
 *      return true or consistently return false.
 *
 *  * For any non-null reference value x, `metisHashTableFunction_TlvNameEquals(x, NULL)`
 *      must return false.
 *
 * @param a A pointer to a `MetisTlvName` instance.
 * @param b A pointer to a `MetisTlvName` instance.
 * @return true if the two `MetisTlvName` instances are equal.
 *
 * Example:
 * @code
 * {
 *    MetisTlvName *a = metisTlvName_Create();
 *    MetisTlvName *b = metisTlvName_Create();
 *
 *    if (metisHashTableFunction_TlvNameEquals(a, b)) {
 *        // true
 *    } else {
 *        // false
 *    }
 * }
 * @endcode
 */
bool metisHashTableFunction_TlvNameEquals(const void *metisTlvNameA, const void *metisTlvNameB);

/**
 * @function hashTableFunction_TlvNameCompare
 * @abstract The key is a MetisTlvName.  Returns the order comparison of two names.
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return A < B -> -1, A = B -> 0, A > B -> +1
 */
int metisHashTableFunction_TlvNameCompare(const void *keyA, const void *keyB);

/**
 * @function hashTableFunction_TlvNameHashCode
 * @abstract Computes the hash of the entire name in a MetisTlvName
 * @discussion
 *   <#Discussion#>
 *
 * @param keyA is a MetisTlvName
 * @return A non-cryptographic hash of Name
 */
HashCodeType metisHashTableFunction_TlvNameHashCode(const void *keyA);
#endif // Metis_metis_HashTableFunction_h
