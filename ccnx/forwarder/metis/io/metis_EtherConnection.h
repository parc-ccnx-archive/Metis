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
 * @file metis_EtherConnection.h
 * @brief Represents an ethernet pair (source address, destination address) in the connection table
 *
 * Ethernet connections are never local.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#ifndef Metis_metis_EtherConnection_h
#define Metis_metis_EtherConnection_h

#include <ccnx/forwarder/metis/io/metis_IoOperations.h>
#include <ccnx/forwarder/metis/core/metis_Forwarder.h>
#include <ccnx/forwarder/metis/io/metis_AddressPair.h>
#include <ccnx/api/control/cpi_Address.h>
#include <ccnx/forwarder/metis/io/metis_GenericEther.h>
#include <ccnx/forwarder/metis/io/metis_HopByHopFragmenter.h>
#include <ccnx/forwarder/metis/core/metis_Connection.h>

/**
 * @function metisEtherConnection_Create
 * @abstract <#OneLineDescription#>
 * @discussion
 *   <#Discussion#>
 *
 * @param pair the address pair that uniquely identifies the connection.  Takes ownership of this memory.
 * @return <#return#>
 */
MetisIoOperations *metisEtherConnection_Create(MetisForwarder *metis, MetisGenericEther *ether, MetisAddressPair *pair);

/**
 * If the IO Operats are of type MetisEtherConnection, return its fragmenter
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] conn An allocated MetisConnection
 *
 * @return non-null The fragmenter associated with this conneciton
 * @return null There is no such fragmenter or the ops is not a MetisEtherConnection
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 */
MetisHopByHopFragmenter *metisEtherConnection_GetFragmenter(const MetisConnection *conn);

/**
 * Tests if MetisEtherConnection is the underlying implementation of the connection
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] conn An allocated MetisConnection
 *
 * @return true The connection is of type MetisEtherConnection
 * @return false The connection is of other type
 *
 * Example:
 * @code
 * {
 *    MetisHopByHopFragmenter *
 *    metisEtherConnection_GetFragmenter(const MetisConnection *conn)
 *    {
 *        MetisHopByHopFragmenter *fragmenter = NULL;
 *
 *        if (metisEtherConnection_IsInstanceOf(conn)) {
 *            MetisIoOperations *ops = metisConnection_GetIoOperations(conn);
 *            _MetisEtherState *state = (_MetisEtherState *) ops->context;
 *            fragmenter = state->fragmenter;
 *        }
 *        return fragmenter;
 *    }
 * }
 * @endcode
 */
bool metisEtherConnection_IsInstanceOf(const MetisConnection *conn);

#endif // Metis_metis_EtherConnection_h
