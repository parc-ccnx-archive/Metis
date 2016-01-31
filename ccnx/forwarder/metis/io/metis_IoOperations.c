/*
 * Copyright (c) 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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
 * @copyright 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>

#include <ccnx/forwarder/metis/io/metis_IoOperations.h>

#include <LongBow/runtime.h>

void *
metisIoOperations_GetClosure(const MetisIoOperations *ops)
{
    assertNotNull(ops, "Parameter ops must be non-null");
    return ops->closure;
}

bool
metisIoOperations_Send(MetisIoOperations *ops, const CPIAddress *nexthop, MetisMessage *message)
{
    return ops->send(ops, nexthop, message);
}

const CPIAddress *
metisIoOperations_GetRemoteAddress(const MetisIoOperations *ops)
{
    return ops->getRemoteAddress(ops);
}

const MetisAddressPair *
metisIoOperations_GetAddressPair(const MetisIoOperations *ops)
{
    return ops->getAddressPair(ops);
}

bool
metisIoOperations_IsUp(const MetisIoOperations *ops)
{
    return ops->isUp(ops);
}

bool
metisIoOperations_IsLocal(const MetisIoOperations *ops)
{
    return ops->isLocal(ops);
}

unsigned
metisIoOperations_GetConnectionId(const MetisIoOperations *ops)
{
    return ops->getConnectionId(ops);
}

void
metisIoOperations_Release(MetisIoOperations **opsPtr)
{
    MetisIoOperations *ops = *opsPtr;
    ops->destroy(opsPtr);
}

const void *
metisIoOperations_Class(const MetisIoOperations *ops)
{
    return ops->class(ops);
}

CPIConnectionType
metisIoOperations_GetConnectionType(const MetisIoOperations *ops)
{
    return ops->getConnectionType(ops);
}
