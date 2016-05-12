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
 * @file metis_Connection.h
 * @brief Wrapper for different types of connections
 *
 * A connection wraps a specific set of {@link MetisIoOperations}.  Those operations
 * allow for input and output.  Connections get stored in the Connection Table.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_Connection_h
#define Metis_metis_Connection_h

#include <ccnx/api/control/cpi_Address.h>
#include <ccnx/forwarder/metis/core/metis_Message.h>
#include <ccnx/forwarder/metis/io/metis_IoOperations.h>

struct metis_connection;
typedef struct metis_connection MetisConnection;

/**
 * Creates a connection object.
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisConnection *metisConnection_Create(MetisIoOperations *ops);

/**
 * @function metisConnection_Release
 * @abstract Releases a reference count, destroying on last release
 * @discussion
 *   Only frees the memory on the final reference count.  The pointer will
 *   always be NULL'd.
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisConnection_Release(MetisConnection **connectionPtr);

/**
 * @function metisConnection_Acquire
 * @abstract A reference counted copy.
 * @discussion
 *   A shallow copy, they share the same memory.
 *
 * @param <#param1#>
 * @return <#return#>
 */
MetisConnection *metisConnection_Acquire(MetisConnection *connection);

/**
 * @function metisConnection_Send
 * @abstract Sends the ccnx message on the connection
 * @discussion
 *
 * @param <#param1#>
 * @return true if message sent, false if connection not up
 */
bool metisConnection_Send(const MetisConnection *conn, MetisMessage *message);


/**
 * Return the `MetisIoOperations` instance associated with the specified `MetisConnection` instance.
 *
 * @param [in] connection The allocated connection
 *
 * @return a pointer to the MetisIoOperations instance associated by th specified connection.
 *
 * Example:
 * @code
 * {
 *     MetisIoOperations *ioOps = metisConnection_GetIoOperations(conn);
 * }
 * @endcode
 */
MetisIoOperations *metisConnection_GetIoOperations(const MetisConnection *conn);

/**
 * Returns the unique identifier of the connection
 *
 * Calls the underlying MetisIoOperations to fetch the connection id
 *
 * @param [in] connection The allocated connection
 *
 * @return unsigned The unique connection id
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
unsigned metisConnection_GetConnectionId(const MetisConnection *conn);

/**
 * Returns the (remote, local) address pair that describes the connection
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] connection The allocated connection
 *
 * @return non-null The connection's remote and local address
 * @return null Should never return NULL
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
const MetisAddressPair *metisConnection_GetAddressPair(const MetisConnection *conn);

/**
 * Tests if the connection is in the "up" state
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] connection The allocated connection
 *
 * @return true The connection is in the "up" state
 * @return false The connection is not in the "up" state
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisConnection_IsUp(const MetisConnection *conn);

/**
 * Tests if the connection is to a Local/Loopback address
 *
 * A local connection is PF_LOCAL (PF_UNIX) and a loopback connection is
 * 127.0.0.0/8 or ::1 for IPv6.
 *
 * @param [in] connection The allocated connection
 *
 * @retval true The connection is local or loopback
 * @retval false The connection is not local or loopback
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisConnection_IsLocal(const MetisConnection *conn);

/**
 * Returns an opaque pointer representing the class of the Io Operations
 *
 * Returns an opaque pointer that an implementation can use to detect if
 * the connection is based on that class.
 *
 * @param [in] conn The MetisConnection to test
 *
 * @return non-null An opaque pointer for each concrete implementation
 *
 * Example:
 * @code
 * {
 *    bool
 *    metisEtherConnection_IsClass(const MetisConnection *conn)
 *    {
 *        bool result = false;
 *        const void *class = metisConnection_Class(conn);
 *        if (class == _metisIoOperationsGuid) {
 *            result = true;
 *        }
 *        return result;
 *    }
 *
 *    MetisHopByHopFragmenter *
 *    metisEtherConnection_GetFragmenter(const MetisConnection *conn)
 *    {
 *        MetisHopByHopFragmenter *fragmenter = NULL;
 *
 *        if (metisEtherConnection_IsClass(conn)) {
 *            MetisIoOperations *ops = metisConnection_GetIoOperations(conn);
 *            _MetisEtherState *state = (_MetisEtherState *) ops->context;
 *            fragmenter = state->fragmenter;
 *        }
 *        return fragmenter;
 *    }
 * }
 * @endcode
 */
const void *metisConnection_Class(const MetisConnection *conn);

#endif // Metis_metis_Connection_h
