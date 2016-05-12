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
 * Implements a listener that works with stream connections over a named pipe.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>

#include <ccnx/forwarder/metis/core/metis_ConnectionTable.h>
#include <ccnx/forwarder/metis/core/metis_Forwarder.h>
#include <ccnx/forwarder/metis/io/metis_Listener.h>
#include <ccnx/forwarder/metis/io/metis_LocalListener.h>
#include <ccnx/forwarder/metis/io/metis_StreamConnection.h>

#include <LongBow/runtime.h>
#include <parc/algol/parc_Memory.h>

struct metis_local_listener {
    MetisForwarder *metis;
    MetisLogger *logger;
    PARCEventSocket *listener;
    CPIAddress *localAddress;
    unsigned id;
};

static void               _metisLocalListener_OpsDestroy(MetisListenerOps **listenerOpsPtr);
static unsigned           _metisLocalListener_OpsGetInterfaceIndex(const MetisListenerOps *ops);
static const CPIAddress  *_metisLocalListener_OpsGetListenAddress(const MetisListenerOps *ops);
static MetisEncapType     _metisLocalListener_OpsGetEncapType(const MetisListenerOps *ops);

static MetisListenerOps localTemplate = {
    .context           = NULL,
    .destroy           = &_metisLocalListener_OpsDestroy,
    .getInterfaceIndex = &_metisLocalListener_OpsGetInterfaceIndex,
    .getListenAddress  = &_metisLocalListener_OpsGetListenAddress,
    .getEncapType      = &_metisLocalListener_OpsGetEncapType,
};

// STREAM daemon listener callback
static void metisListenerLocal_Listen(int, struct sockaddr *, int socklen, void *localVoid);

MetisListenerOps *
metisLocalListener_Create(MetisForwarder *metis, const char *path)
{
    MetisLocalListener *local = parcMemory_AllocateAndClear(sizeof(MetisLocalListener));
    assertNotNull(local, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisLocalListener));
    local->metis = metis;
    local->logger = metisLogger_Acquire(metisForwarder_GetLogger(metis));

    struct sockaddr_un addr_unix;
    memset(&addr_unix, 0, sizeof(addr_unix));

    addr_unix.sun_family = PF_UNIX;
    strcpy(addr_unix.sun_path, path);

    unlink(path);

    local->listener = metisDispatcher_CreateListener(metisForwarder_GetDispatcher(metis), metisListenerLocal_Listen,
                                                     (void *) local, -1, (struct sockaddr*) &addr_unix, sizeof(addr_unix));

    assertNotNull(local->listener, "Got null listener from metisDispatcher_CreateListener: (%d) %s", errno, strerror(errno));

    struct sockaddr_un addr_un;
    memset(&addr_un, 0, sizeof(addr_un));
    addr_un.sun_family = AF_UNIX;
    strcpy(addr_un.sun_path, path);

    local->localAddress = cpiAddress_CreateFromUnix(&addr_un);
    local->id = metisForwarder_GetNextConnectionId(metis);

    MetisListenerOps *ops = parcMemory_AllocateAndClear(sizeof(MetisListenerOps));
    assertNotNull(ops, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisListenerOps));
    memcpy(ops, &localTemplate, sizeof(MetisListenerOps));
    ops->context = local;

    return ops;
}

void
metisLocalListener_Destroy(MetisLocalListener **listenerPtr)
{
    assertNotNull(listenerPtr, "Parameter must be non-null double pointer");
    assertNotNull(*listenerPtr, "Parameter must dereference to non-null pointer");

    MetisLocalListener *local = *listenerPtr;

    metisLogger_Release(&local->logger);

    cpiAddress_Destroy(&local->localAddress);
    metisDispatcher_DestroyListener(metisForwarder_GetDispatcher(local->metis), &local->listener);

    parcMemory_Deallocate((void **) &local);
    *listenerPtr = NULL;
}

// ==================================================

/**
 * @function metisListenerLocal_Listen
 * @abstract Called when a client connects to the server socket
 * @discussion
 *   Accepts a client connection.  Creates a new Stream connection and adds it
 *   to the connection table.
 *
 * @param fd the remote client socket (it will be AF_UNIX type)
 * @param sa the remote client address
 * @param socklen the bytes of sa
 * @param localVoid a void point to the MetisLocalListener that owns the server socket
 */
static void
metisListenerLocal_Listen(int fd,
                          struct sockaddr *sa, int socklen, void *localVoid)
{
    MetisLocalListener *local = (MetisLocalListener *) localVoid;
    assertTrue(sa->sa_family == AF_UNIX, "Got wrong address family, expected %d got %d", AF_UNIX, sa->sa_family);

    CPIAddress *remote = cpiAddress_CreateFromUnix((struct sockaddr_un *) sa);
    MetisAddressPair *pair = metisAddressPair_Create(local->localAddress, remote);

    MetisIoOperations *ops = metisStreamConnection_AcceptConnection(local->metis, fd, pair, true);
    MetisConnection *conn = metisConnection_Create(ops);

    metisConnectionTable_Add(metisForwarder_GetConnectionTable(local->metis), conn);

    if (metisLogger_IsLoggable(local->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug)) {
        char *str = metisAddressPair_ToString(pair);
        metisLogger_Log(local->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug, __func__,
                        "Listener %p started on address pair %s", (void *) local, str);
        free(str);
    }

    cpiAddress_Destroy(&remote);
}

static void
_metisLocalListener_OpsDestroy(MetisListenerOps **listenerOpsPtr)
{
    MetisListenerOps *ops = *listenerOpsPtr;
    MetisLocalListener *local = (MetisLocalListener *) ops->context;

    if (metisLogger_IsLoggable(local->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug)) {
        metisLogger_Log(local->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug, __func__,
                        "Listener %p destroyed", (void *) local);
    }

    metisLocalListener_Destroy(&local);
    parcMemory_Deallocate((void **) &ops);
    *listenerOpsPtr = NULL;
}

static unsigned
_metisLocalListener_OpsGetInterfaceIndex(const MetisListenerOps *ops)
{
    MetisLocalListener *local = (MetisLocalListener *) ops->context;
    return local->id;
}

static const CPIAddress *
_metisLocalListener_OpsGetListenAddress(const MetisListenerOps *ops)
{
    MetisLocalListener *local = (MetisLocalListener *) ops->context;
    return local->localAddress;
}

static MetisEncapType
_metisLocalListener_OpsGetEncapType(const MetisListenerOps *ops)
{
    return METIS_ENCAP_LOCAL;
}
