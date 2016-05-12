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
 * @file generic_ether.h
 * @brief Generic interface to working with Ethernet frames.
 *
 * Wraps platform-specific code.  The implementation is found in the metis/platforms directory.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_GenericEther_h
#define Metis_metis_GenericEther_h

#include <stdbool.h>
#include <parc/algol/parc_EventBuffer.h>
#include <parc/algol/parc_Buffer.h>
#include <ccnx/forwarder/metis/core/metis_Forwarder.h>

struct metis_generic_ether;
typedef struct metis_generic_ether MetisGenericEther;

/**
 * Create a generic ethernet object
 *
 * Hides system dependent ethernet.  Creates an ethernet object that is ready to send and
 * receive ethernet frames on a given device and ether type.  There may be system limits
 * to the number of these you can open (i.e. 4 BPF devices on Mac OS).
 *
 * If the device name is NULL, it will not bind to a specific interface.
 *
 * You generally need elevated permissions to access an Ethernet device.  This function
 * may return NULL due to a permissions error.
 *
 * @param [in] deviceName The name of the device, e.g. "eth0" or "en1"
 * @param [in] etherType The host-byte-order ethertype (i.e. 0x0801)
 * @param [in] logger The MetisLogger to use
 *
 * @retval non-null An allocated ethernet object
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisGenericEther *metisGenericEther_Create(MetisForwarder *metis, const char *deviceName, uint16_t etherType);

/**
 * Acquire a reference counted copy
 *
 * Returns a reference counted copy of the generic Ethernet object
 *
 * @param [in] ether An allocated Ethernet object
 *
 * @retval non-null A reference counted copy
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisGenericEther *metisGenericEther_Acquire(const MetisGenericEther *ether);

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
void metisGenericEther_Release(MetisGenericEther **etherPtr);

/**
 * Returns the descriptor for i/o
 *
 * Returns a system descriptor that can be used to select, poll, etc.
 *
 * Do not close the socket.  It will be closed when metisGenericEther_Release() is called.
 *
 * @param [in] ether An allocated generic ethernet
 *
 * @retval non-negative The system descriptor
 * @retval negative An error (use errno)
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
int metisGenericEther_GetDescriptor(const MetisGenericEther *ether);


/**
 * Reads the next ethernet frame in to the provided buffer
 *
 * The frame will have the Ethernet header
 *
 * @param [in] ether An allocated generic ethernet
 * @param [in] buffer The allocated buffer to put the packet in
 *
 * @retval true A frame was ready and put in the buffer
 * @retval false No frame is ready
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisGenericEther_ReadNextFrame(MetisGenericEther *ether, PARCEventBuffer *buffer);

/**
 * Sends an Ethernet frame out the device
 *
 * The frame must have an Ethernet header filled in with all values.
 *
 * @param [in] ether An allocated GenericEther object
 * @param [in] buffer The buffer to send, including the Ethernet header
 *
 * @retval true The frame was sent
 * @retval false An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisGenericEther_SendFrame(MetisGenericEther *ether, PARCEventBuffer *buffer);

/**
 * Return the MAC address the object is bound to
 *
 * Returns a PARCBuffer with the 6-byte mac address of the interface
 *
 * @param [in] ether An allocated GenericEther object
 *
 * @retval non-null The MAC address of the interface
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCBuffer *metisGenericEther_GetMacAddress(const MetisGenericEther *ether);

/**
 * Returns the ethertype associated with this object
 *
 * Returns the ethertype, in host byte order.
 *
 * @param [in] ether An allocated GenericEther object
 *
 * @retval number Ethertype (host byte order)
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
uint16_t metisGenericEther_GetEtherType(const MetisGenericEther *ether);

/**
 * Returns the maximum transmission unit (MTU)
 *
 * The MTU is the largest user payload allowed in a frame
 *
 * @param [<#in#> | <#out#> | <#in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 */
unsigned metisGenericEther_GetMTU(const MetisGenericEther *ether);
#endif // Metis_metis_GenericEther_h
