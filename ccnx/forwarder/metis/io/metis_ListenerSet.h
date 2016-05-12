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
 * @file metis_ListenerSet.h
 * @brief A listener set is unique on (MetisEncapType, localAddress)
 *
 * Keeps track of all the running listeners.  The set is unique on the
 * encapsulation type and the local address.  For example, with TCP encapsulation and
 * local address 127.0.0.1 or Ethernet encapsulation and MAC address 00:11:22:33:44:55.
 *
 * NOTE: This does not allow multiple EtherType on the same interface because the CPIAddress for
 * a LINK address does not include an EtherType.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_ListenerSet_h
#define Metis_metis_ListenerSet_h

#include <ccnx/forwarder/metis/io/metis_Listener.h>

struct metis_listener_set;
typedef struct metis_listener_set MetisListenerSet;

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
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
MetisListenerSet *metisListenerSet_Create(void);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
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
void metisListenerSet_Destroy(MetisListenerSet **setPtr);

/**
 * @function metisListenerSet_Add
 * @abstract Adds the listener to the set
 * @discussion
 *     Unique set based on pair (MetisEncapType, localAddress).
 *     Takes ownership of the ops memory if added.
 *
 * @param <#param1#>
 * @return true if added, false if not
 */
bool metisListenerSet_Add(MetisListenerSet *set, MetisListenerOps *ops);

/**
 * The number of listeners in the set
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] set An allocated listener set
 *
 * @retval <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
size_t metisListenerSet_Length(const MetisListenerSet *set);

/**
 * Returns the listener at the given index
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] set An allocated listener set
 * @param [in] index The index position (0 <= index < metisListenerSet_Lenght)
 *
 * @retval non-null The listener at index
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisListenerOps *metisListenerSet_Get(const MetisListenerSet *set, size_t index);

/**
 * Looks up a listener by its key (EncapType, LocalAddress)
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] set An allocated listener set
 * @param [in] encapType the listener type
 * @param [in] localAddress The local bind address (e.g. MAC address or TCP socket)
 *
 * @retval non-null The listener matching the query
 * @retval null Does not exist
 *
 * Example:
 * @code
 *
 * @endcode
 */
MetisListenerOps *metisListenerSet_Find(const MetisListenerSet *set, MetisEncapType encapType, const CPIAddress *localAddress);
#endif
