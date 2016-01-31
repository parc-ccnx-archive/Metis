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
 * This is a mock for MetisListenerOps
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

/**
 * You should #include this C file in your unit test code
 */

// ===============================
// Setup a mock for the MetisListenerOps

typedef struct mock_listener_data {
    unsigned destroyCount;
    unsigned getInterfaceIndexCount;
    unsigned getListenAddressCount;
    unsigned getEncapTypeCount;

    // These values will be returned by the appropriate getter
    unsigned interfaceIndex;
    CPIAddress *listenAddress;
    MetisEncapType encapType;
} MockListenerData;

static void
mockListener_Destroy(MetisListenerOps **opsPtr)
{
    // Don't actually destroy the data, we want to keep the counts
    MetisListenerOps *ops = *opsPtr;
    MockListenerData *data = ops->context;
    data->destroyCount++;
    parcMemory_Deallocate((void **) &ops);
    *opsPtr = NULL;
}

static unsigned
mockListener_GetInterfaceIndex(const MetisListenerOps *ops)
{
    MockListenerData *data = ops->context;
    data->getInterfaceIndexCount++;
    return data->interfaceIndex;
}

static const CPIAddress *
mockListener_GetListenAddress(const MetisListenerOps *ops)
{
    MockListenerData *data = ops->context;
    data->getListenAddressCount++;
    return data->listenAddress;
}

static MetisEncapType
mockListener_GetEncapType(const MetisListenerOps *ops)
{
    MockListenerData *data = ops->context;
    data->getEncapTypeCount++;
    return data->encapType;
}

static MetisListenerOps
    mockListenerTemplate = {
    .context           = NULL,
    .destroy           = &mockListener_Destroy,
    .getInterfaceIndex = &mockListener_GetInterfaceIndex,
    .getListenAddress  = &mockListener_GetListenAddress,
    .getEncapType      = &mockListener_GetEncapType
};

MockListenerData *
mockListenData_Create(unsigned interfaceIndex, CPIAddress *listenAddress, MetisEncapType encapType)
{
    MockListenerData *data = parcMemory_AllocateAndClear(sizeof(MockListenerData));
    assertNotNull(data, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MockListenerData));
    memset(data, 0, sizeof(MockListenerData));
    data->encapType = encapType;
    data->interfaceIndex = interfaceIndex;
    data->listenAddress = cpiAddress_Copy(listenAddress);
    return data;
}

MetisListenerOps *
mockListener_Create(MockListenerData *data)
{
    MetisListenerOps *ops = parcMemory_AllocateAndClear(sizeof(MetisListenerOps));
    assertNotNull(ops, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisListenerOps));
    memcpy(ops, &mockListenerTemplate, sizeof(MetisListenerOps));
    ops->context = data;
    return ops;
}

void
mockListenerData_Destroy(MockListenerData **dataPtr)
{
    MockListenerData *data = *dataPtr;
    cpiAddress_Destroy(&data->listenAddress);
    parcMemory_Deallocate((void **) &data);
    *dataPtr = NULL;
}
