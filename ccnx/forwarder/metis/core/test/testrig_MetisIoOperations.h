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
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#ifndef Metis_testrig_MetisIoOperations_h
#define Metis_testrig_MetisIoOperations_h

/**
 * Setup a test rig around a MetisIoOperation so we have visibility in to
 * what the connection table is doing
 *
 * Usage: Use <code>mockIoOperationsData_Create()</code> or <code>mockIoOperationsData_CreateSimple()</code>
 *        to create the MetisIoOperations.  You can then inspect the TestData inside the context
 *        by mapping <code>TestData *data = (TestData *) metisIoOperations_GetClosure(ops)</code>.
 *
 * IMPORTANT: ops->destroy(&ops) will not destroy the test rig.  It will increment a counter.
 *            you must call <code>testdata_Destroy(&ops)</code> yourself.  You should call this
 *            as the very last thing, even after <code>metisForwarder_Destroy()</code>, if you put
 *            the MetisIoOpereations in the connection table.
 */
static bool                     mockIoOperations_Send(MetisIoOperations *ops, const CPIAddress *nexthop, MetisMessage *message);
static const CPIAddress        *mockIoOperations_GetRemoteAddress(const MetisIoOperations *ops);
static const MetisAddressPair  *mockIoOperations_GetAddressPair(const MetisIoOperations *ops);
static bool                     mockIoOperations_IsUp(const MetisIoOperations *ops);
static bool                     mockIoOperations_IsLocal(const MetisIoOperations *ops);
static unsigned                 mockIoOperations_GetConnectionId(const MetisIoOperations *ops);
static void                     mockIoOperations_Destroy(MetisIoOperations **opsPtr);
static CPIConnectionType        mockIoOperations_GetConnectionType(const MetisIoOperations *ops);
static const void *mockIoOperations_Class(const MetisIoOperations *ops);

static MetisIoOperations mockIoOperationsTemplate = {
    .closure           = NULL,
    .send              = &mockIoOperations_Send,
    .getRemoteAddress  = &mockIoOperations_GetRemoteAddress,
    .getAddressPair    = &mockIoOperations_GetAddressPair,
    .isUp              = &mockIoOperations_IsUp,
    .isLocal           = &mockIoOperations_IsLocal,
    .getConnectionId   = &mockIoOperations_GetConnectionId,
    .destroy           = &mockIoOperations_Destroy,
    .getConnectionType = &mockIoOperations_GetConnectionType,
    .class             = &mockIoOperations_Class
};

typedef struct mock_io_operations_data {
    // counters for each call
    unsigned sendCount;
    unsigned getRemoteAddressCount;
    unsigned getAddressPairCount;
    unsigned isUpCount;
    unsigned isLocalCount;
    unsigned getConnectionIdCount;
    unsigned destroyCount;
    unsigned getConnectionTypeCount;
    unsigned classCount;

    MetisMessage *lastMessage;
    MetisAddressPair *addressPair;
    unsigned id;
    bool isUp;
    bool isLocal;
    bool sendResult;                // what to return when send() called
    CPIConnectionType connType;
} MockIoOperationsData;

/**
 * @function testdata_Create
 * @abstract Creates a data set for testing MetisIoOperations
 * @discussion
 *   Caller must explicitly use <code>testdata_Destroy()</code> when done.  Calling the destroyer through
 *   the io operations only increments counters, it does not destroy the object.
 *
 * @param <#param1#>
 * @return <#return#>
 */
static MetisIoOperations *
mockIoOperationsData_Create(MetisAddressPair *pair, unsigned id, bool isUp, bool sendResult, bool isLocal, CPIConnectionType connType)
{
    MockIoOperationsData *data = parcMemory_AllocateAndClear(sizeof(MockIoOperationsData));
    data->addressPair = pair;
    data->id = id;
    data->isUp = isUp;
    data->sendResult = sendResult;
    data->lastMessage = NULL;
    data->isLocal = isLocal;
    data->connType = connType;

    MetisIoOperations *ops = parcMemory_AllocateAndClear(sizeof(MetisIoOperations));
    memcpy(ops, &mockIoOperationsTemplate, sizeof(MetisIoOperations));
    ops->closure = data;

    return ops;
}

/**
 * @function testdata_CreateSimple
 * @abstract Creates a data set for testing MetisIoOperations
 * @discussion
 *   Caller must explicitly use <code>testdata_Destroy()</code> when done.  Calling the destroyer through
 *   the io operations only increments counters, it does not destroy the object.
 *
 * @param <#param1#>
 * @return <#return#>
 */
static MetisIoOperations *
mockIoOperationsData_CreateSimple(unsigned addressLocal, unsigned addressRemote, unsigned id, bool isUp, bool sendResult, bool isLocal)
{
    CPIAddress *local = cpiAddress_CreateFromInterface(addressLocal);
    CPIAddress *remote = cpiAddress_CreateFromInterface(addressRemote);
    MetisAddressPair *pair = metisAddressPair_Create(local, remote);
    MetisIoOperations *ops = mockIoOperationsData_Create(pair, id, isUp, sendResult, isLocal, cpiConnection_UDP);
    cpiAddress_Destroy(&local);
    cpiAddress_Destroy(&remote);
    return ops;
}

static void
mockIoOperationsData_Destroy(MetisIoOperations **opsPtr)
{
    MetisIoOperations *ops = *opsPtr;
    MockIoOperationsData *data = (MockIoOperationsData *) metisIoOperations_GetClosure(ops);

    metisAddressPair_Release(&data->addressPair);
    if (data->lastMessage) {
        metisMessage_Release(&data->lastMessage);
    }
    parcMemory_Deallocate((void **) &data);
    ops->closure = NULL;
    parcMemory_Deallocate((void **) &ops);
    *opsPtr = NULL;
}

static bool
mockIoOperations_Send(MetisIoOperations *ops, const CPIAddress *nexthop, MetisMessage *message)
{
    MockIoOperationsData *data = (MockIoOperationsData *) metisIoOperations_GetClosure(ops);
    data->sendCount++;

    if (message) {
        if (data->lastMessage) {
            metisMessage_Release(&data->lastMessage);
        }

        data->lastMessage = metisMessage_Acquire(message);
    }

    return data->sendResult;
}

static const CPIAddress *
mockIoOperations_GetRemoteAddress(const MetisIoOperations *ops)
{
    MockIoOperationsData *data = (MockIoOperationsData *) metisIoOperations_GetClosure(ops);
    data->getRemoteAddressCount++;
    return metisAddressPair_GetRemote(data->addressPair);
}

static const MetisAddressPair *
mockIoOperations_GetAddressPair(const MetisIoOperations *ops)
{
    MockIoOperationsData *data = (MockIoOperationsData *) metisIoOperations_GetClosure(ops);
    data->getAddressPairCount++;
    return data->addressPair;
}

static bool
mockIoOperations_IsUp(const MetisIoOperations *ops)
{
    MockIoOperationsData *data = (MockIoOperationsData *) metisIoOperations_GetClosure(ops);
    data->isUpCount++;
    return data->isUp;
}

static bool
mockIoOperations_IsLocal(const MetisIoOperations *ops)
{
    MockIoOperationsData *data = (MockIoOperationsData *) metisIoOperations_GetClosure(ops);
    data->isLocalCount++;
    return data->isLocal;
}


static unsigned
mockIoOperations_GetConnectionId(const MetisIoOperations *ops)
{
    MockIoOperationsData *data = (MockIoOperationsData *) metisIoOperations_GetClosure(ops);
    data->getConnectionIdCount++;
    return data->id;
}

static void
mockIoOperations_Destroy(MetisIoOperations **opsPtr)
{
    MockIoOperationsData *data = (MockIoOperationsData *) (*opsPtr)->closure;
    data->destroyCount++;
    *opsPtr = NULL;
}

static CPIConnectionType
mockIoOperations_GetConnectionType(const MetisIoOperations *ops)
{
    MockIoOperationsData *data = (MockIoOperationsData *) metisIoOperations_GetClosure(ops);
    data->getConnectionTypeCount++;
    return data->connType;
}

static const void *
mockIoOperations_Class(const MetisIoOperations *ops)
{
    MockIoOperationsData *data = (MockIoOperationsData *) metisIoOperations_GetClosure(ops);
    data->classCount++;
    return __FILE__;
}
#endif // Metis_testrig_MetisIoOperations_h
