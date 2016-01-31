/*
 * Copyright (c) 2013-2014, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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
 * The TCP out-bound tunnel is almost identical to the in-bound tunnel.
 * We use MetisStreamConneciton for the out-bound tunnels too.  We call a different
 * constructor than the in-bound so the MetisStreamConneciton knows that it is starting
 * unconnected and needs to wait for the Connected event before putting it in the UP state.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2014, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <ccnx/forwarder/metis/io/metis_TcpTunnel.h>
#include <ccnx/forwarder/metis/io/metis_StreamConnection.h>

#include <LongBow/runtime.h>

MetisIoOperations *
metisTcpTunnel_Create(MetisForwarder *metis, const CPIAddress *localAddress, const CPIAddress *remoteAddress)
{
    MetisAddressPair *pair = metisAddressPair_Create(localAddress, remoteAddress);

    // Determine if its localhost (case 820)
    bool isLocal = false;

    // this takes ownership of the address pair
    return metisStreamConnection_OpenConnection(metis, pair, isLocal);
}
