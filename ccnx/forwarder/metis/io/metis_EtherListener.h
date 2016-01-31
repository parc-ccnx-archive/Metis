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
 * @file metis_EtherListener.h
 * @brief Listen for raw ethernet frames on an interface
 *
 * <#Detailed Description#>
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#ifndef Metis_metis_EtherListener_h
#define Metis_metis_EtherListener_h

#include <ccnx/forwarder/metis/io/metis_GenericEther.h>

struct metis_listener_ether;
typedef struct metis_listener_ether MetisListenerEther;

/**
 * @function metisListenerEther_Create
 * @abstract Create a L2 listener on a raw ethertype
 * @discussion
 *   Requires root, will send/receive ethernet frames on the specified device.
 *   The exact mechanism varies by system.
 *
 * @param deviceName is the system name of the interface (e.g. "en0")
 * @return <#return#>
 */
MetisListenerOps *metisEtherListener_Create(MetisForwarder *metis, const char *deviceName, uint16_t ethertype);

/**
 * Return the underlying GenericEther of the listener
 *
 * The MetisGenericEther wraps the platform-specific IO operations of the ethernet connection.
 * Will assert if the listenerOps is not of type METIS_ENCAP_ETHER.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @retval non-null The underlying generic ethernet
 * @retval null An error
 *
 * Example:
 * @code
 * {
 *     MetisListenerSet *listenerSet = metisForwarder_GetListenerSet(metis);
 *     MetisListenerOps *listenerOps = metisListenerSet_Find(listenerSet, METIS_ENCAP_ETHER, linkAddress);
 *     if (listenerOps) {
 *         MetisGenericEther *ether = metisEtherListener_GetGenericEtherFromListener(listenerOps);
 *     }
 * }
 * @endcode
 */
MetisGenericEther *metisEtherListener_GetGenericEtherFromListener(MetisListenerOps *listenerOps);
#endif // Metis_metis_EtherListener_h
