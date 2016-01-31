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
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */


#include <config.h>
#include <stdio.h>

#include <ccnx/forwarder/metis/io/metis_IoOperations.h>
#include <ccnx/forwarder/metis/core/metis_Connection.h>
#include <ccnx/forwarder/metis/io/metis_AddressPair.h>
#include <parc/algol/parc_Memory.h>

#include <LongBow/runtime.h>

struct metis_connection {
    const MetisAddressPair *addressPair;
    MetisIoOperations *ops;

    unsigned refCount;
};

MetisConnection *
metisConnection_Create(MetisIoOperations *ops)
{
    assertNotNull(ops, "Parameter ops must be non-null");
    MetisConnection *conn = parcMemory_AllocateAndClear(sizeof(MetisConnection));
    assertNotNull(conn, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisConnection));
    conn->addressPair = metisIoOperations_GetAddressPair(ops);
    conn->ops = ops;
    conn->refCount = 1;
    return conn;
}

MetisConnection *
metisConnection_Acquire(MetisConnection *connection)
{
    assertNotNull(connection, "Parameter conn must be non-null");
    connection->refCount++;
    return connection;
}

void
metisConnection_Release(MetisConnection **connectionPtr)
{
    assertNotNull(connectionPtr, "Parameter must be non-null double pointer");
    assertNotNull(*connectionPtr, "Parameter must dereference to non-null pointer");
    MetisConnection *conn = *connectionPtr;

    assertTrue(conn->refCount > 0, "Invalid state, connection reference count should be positive, got 0.");
    conn->refCount--;
    if (conn->refCount == 0) {
        // don't destroy addressPair, its part of ops.
        metisIoOperations_Release(&conn->ops);
        parcMemory_Deallocate((void **) &conn);
    }
    *connectionPtr = NULL;
}

bool
metisConnection_Send(const MetisConnection *conn, MetisMessage *message)
{
    assertNotNull(conn, "Parameter conn must be non-null");
    assertNotNull(message, "Parameter message must be non-null");

    if (metisIoOperations_IsUp(conn->ops)) {
        return metisIoOperations_Send(conn->ops, NULL, message);
    }
    return false;
}

MetisIoOperations *
metisConnection_GetIoOperations(const MetisConnection *conn)
{
    return conn->ops;
}

unsigned
metisConnection_GetConnectionId(const MetisConnection *conn)
{
    assertNotNull(conn, "Parameter conn must be non-null");
    return metisIoOperations_GetConnectionId(conn->ops);
}

const MetisAddressPair *
metisConnection_GetAddressPair(const MetisConnection *conn)
{
    assertNotNull(conn, "Parameter conn must be non-null");
    return metisIoOperations_GetAddressPair(conn->ops);
}

bool
metisConnection_IsUp(const MetisConnection *conn)
{
    assertNotNull(conn, "Parameter conn must be non-null");
    return metisIoOperations_IsUp(conn->ops);
}

bool
metisConnection_IsLocal(const MetisConnection *conn)
{
    assertNotNull(conn, "Parameter conn must be non-null");
    return metisIoOperations_IsLocal(conn->ops);
}

const void *
metisConnection_Class(const MetisConnection *conn)
{
    assertNotNull(conn, "Parameter conn must be non-null");
    return metisIoOperations_Class(conn->ops);
}

