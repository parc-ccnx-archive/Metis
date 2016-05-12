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
 * Used to identify a connection between a specific local address and
 * a specific remote address.
 */

#ifndef Metis_metis_AddressPair_h
#define Metis_metis_AddressPair_h

#include <ccnx/api/control/cpi_Address.h>

struct metis_address_pair;
typedef struct metis_address_pair MetisAddressPair;

/**
 * @function metisAddressPair_Create
 * @abstract Creates and address pair.  There is no restriction on the address types.
 * @discussion
 *   Creates an ordered pair of addresses, where the first is considered the "local" address
 *   and the second is the "remote" address.  Those designations are purely a convention used
 *   to name them, and does not imply any specifici types of operations.
 *
 *   The two addresses may be of any address types (e.g. IPv4, IPv6, Local, Ethernet).
 *   However, some functions that use an AddressPair may require that the local and remote
 *   addresses be the same type.
 *
 * @param <#param1#>
 * @return <#return#>
 */
MetisAddressPair *metisAddressPair_Create(const CPIAddress *local, const CPIAddress *remote);

/**
 * Returns a reference counted copy of the address pair
 *
 * Increments the reference count and returns the same address pair
 *
 * @param [in] addressPair An allocated address pair
 *
 * @retval non-null A reference counted copy
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisAddressPair *metisAddressPair_Acquire(const MetisAddressPair *addressPair);


/**
 * Releases a reference count to the object
 *
 * Decrements the reference count and destroys the object when it reaches 0.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @retval <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisAddressPair_Release(MetisAddressPair **pairPtr);

/**
 * Determine if two MetisAddressPair instances are equal.
 *
 * Two MetisAddressPair instances are equal if, and only if, the local and remote addresses are identical.
 * Equality is determined by cpiAddress_Equals(a->local, b->local) and
 * cpiAdress_Equals(a->remote, b->remote).
 *
 * The following equivalence relations on non-null `MetisAddressPair` instances are maintained:
 *
 *  * It is reflexive: for any non-null reference value x, `MetisAddressPair_Equals(x, x)`
 *      must return true.
 *
 *  * It is symmetric: for any non-null reference values x and y,
 *    `metisAddressPair_Equals(x, y)` must return true if and only if
 *        `metisAddressPair_Equals(y, x)` returns true.
 *
 *  * It is transitive: for any non-null reference values x, y, and z, if
 *        `metisAddressPair_Equals(x, y)` returns true and
 *        `metisAddressPair_Equals(y, z)` returns true,
 *        then  `metisAddressPair_Equals(x, z)` must return true.
 *
 *  * It is consistent: for any non-null reference values x and y, multiple
 *      invocations of `metisAddressPair_Equals(x, y)` consistently return true or
 *      consistently return false.
 *
 *  * For any non-null reference value x, `metisAddressPair_Equals(x, NULL)` must
 *      return false.
 *
 * @param a A pointer to a `MetisAddressPair` instance.
 * @param b A pointer to a `MetisAddressPair` instance.
 * @return true if the two `MetisAddressPair` instances are equal.
 *
 * Example:
 * @code
 * {
 *    MetisAddressPair *a = metisAddressPair_Create();
 *    MetisAddressPair *b = metisAddressPair_Create();
 *
 *    if (metisAddressPair_Equals(a, b)) {
 *        // true
 *    } else {
 *        // false
 *    }
 * }
 * @endcode
 */
bool metisAddressPair_Equals(const MetisAddressPair *a, const MetisAddressPair *b);

/**
 * @function metisAddressPair_EqualsAddresses
 * @abstract As MetisAddressEquals, but "b" is broken out
 * @discussion
 *   Equality is determined by cpiAddress_Equals(a->local, local) and
 *   cpiAdress_Equals(a->remote, remote).
 *
 * @param <#param1#>
 * @return <#return#>
 */
bool metisAddressPair_EqualsAddresses(const MetisAddressPair *a, const CPIAddress *local, const CPIAddress *remote);

const CPIAddress *metisAddressPair_GetLocal(const MetisAddressPair *pair);
const CPIAddress *metisAddressPair_GetRemote(const MetisAddressPair *pair);

/**
 * @function metisAddressPair_HashCode
 * @abstract Hash useful for tables.  Consistent with Equals.
 * @discussion
 *   Returns a non-cryptographic hash that is consistent with equals.  That is,
 *   if a == b, then hash(a) == hash(b).
 *
 * @param <#param1#>
 * @return <#return#>
 */
PARCHashCode metisAddressPair_HashCode(const MetisAddressPair *pair);

/**
 * @function metisAddressPair_ToString
 * @abstract Human readable string representation.  Caller must use free(3).
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
char *metisAddressPair_ToString(const MetisAddressPair *pair);
#endif // Metis_metis_AddressPair_h
