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
 * @file metis_Listener.h
 * @brief Provides the function abstraction of all Listeners.
 *
 * A listener accepts in coming packets.  A Stream listener will accept the connection
 * then pass it off to the {@link MetisStreamConnection} class.  A datagram listener
 * will have to have its own way to multiplex packets.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_Listener_h
#define Metis_metis_Listener_h

#include <ccnx/api/control/cpi_Address.h>

struct metis_listener_ops;
typedef struct metis_listener_ops MetisListenerOps;

typedef enum {
    METIS_ENCAP_TCP,  /**< TCP encapsulation type */
    METIS_ENCAP_UDP,   /**< UDP encapsulation type */
    METIS_ENCAP_ETHER,  /**< Ethernet encapsulation type */
    METIS_ENCAP_LOCAL  /**< A connection to a local protocol stack */
} MetisEncapType;

struct metis_listener_ops {
    /**
     * A user-defined parameter
     */
    void *context;

    /**
     * Called to destroy the Listener.
     *
     * @param [in] listenerOpsPtr Double pointer to this structure
     */
    void (*destroy)(MetisListenerOps **listenerOpsPtr);

    /**
     * Returns the interface index of the listener.
     *
     * @param [in] ops Pointer to this structure
     *
     * @return the interface index of the listener
     */
    unsigned (*getInterfaceIndex)(const MetisListenerOps *ops);

    /**
     * Returns the address pair that defines the listener (local, remote)
     *
     * @param [in] ops Pointer to this structure
     *
     * @return the (local, remote) pair of addresses
     */
    const CPIAddress * (*getListenAddress)(const MetisListenerOps *ops);

    /**
     * Returns the encapsulation type of the listener (e.g. TCP, UDP, Ethernet)
     *
     * @param [in] ops Pointer to this structure
     *
     * @return the listener encapsulation type
     */
    MetisEncapType (*getEncapType)(const MetisListenerOps *ops);

    /**
     * Returns the underlying socket associated with the listener
     *
     * Not all listeners are capable of returning a useful socket.  In those
     * cases, this function pointer is NULL.
     *
     * TCP does not support this operation (function is NULL).  UDP returns its local socket.
     *
     * The caller should never close this socket, the listener will do that when its
     * destroy method is called.
     *
     * @param [in] ops Pointer to this structure
     *
     * @retval integer The socket descriptor
     *
     * Example:
     * @code
     * <#example#>
     * @endcode
     */
    int (*getSocket)(const MetisListenerOps *ops);
};
#endif // Metis_metis_Listener_h
