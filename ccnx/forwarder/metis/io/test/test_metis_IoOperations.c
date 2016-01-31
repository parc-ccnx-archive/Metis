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

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../metis_IoOperations.c"
#include <parc/algol/parc_SafeMemory.h>

#include <LongBow/unit-test.h>

#include <ccnx/forwarder/metis/core/test/testrig_MetisIoOperations.h>

// ===========================================

LONGBOW_TEST_RUNNER(metis_IoOperations)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metis_IoOperations)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metis_IoOperations)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, metisIoOperations_GetClosure);
    LONGBOW_RUN_TEST_CASE(Global, metisIoOperations_Send);
    LONGBOW_RUN_TEST_CASE(Global, metisIoOperations_GetRemoteAddress);
    LONGBOW_RUN_TEST_CASE(Global, metisIoOperations_GetAddressPair);
    LONGBOW_RUN_TEST_CASE(Global, metisIoOperations_IsUp);
    LONGBOW_RUN_TEST_CASE(Global, metisIoOperations_IsLocal);
    LONGBOW_RUN_TEST_CASE(Global, metisIoOperations_GetConnectionId);
    LONGBOW_RUN_TEST_CASE(Global, metisIoOperations_Release);
    LONGBOW_RUN_TEST_CASE(Global, metisIoOperations_Class);
    LONGBOW_RUN_TEST_CASE(Global, metisIoOperations_GetConnectionType);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, metisIoOperations_GetClosure)
{
    MetisIoOperations *ops = mockIoOperationsData_CreateSimple(1, 2, 3, true, true, true);
    void *closure = metisIoOperations_GetClosure(ops);
    assertTrue(closure == ops->closure, "Wrong closure, expected %p got %p", ops->closure, closure);
    mockIoOperationsData_Destroy(&ops);
}

LONGBOW_TEST_CASE(Global, metisIoOperations_Send)
{
    MetisIoOperations *ops = mockIoOperationsData_CreateSimple(1, 2, 3, true, true, true);
    MockIoOperationsData *data = metisIoOperations_GetClosure(ops);

    metisIoOperations_Send(ops, NULL, NULL);
    assertTrue(data->sendCount == 1, "Wrong metisIoOperations_Send count expected 1 got %u", data->sendCount);
    mockIoOperationsData_Destroy(&ops);
}

LONGBOW_TEST_CASE(Global, metisIoOperations_GetRemoteAddress)
{
    MetisIoOperations *ops = mockIoOperationsData_CreateSimple(1, 2, 3, true, true, true);
    MockIoOperationsData *data = metisIoOperations_GetClosure(ops);

    metisIoOperations_GetRemoteAddress(ops);
    assertTrue(data->getRemoteAddressCount == 1, "Wrong metisIoOperations_GetRemoteAddress count expected 1 got %u", data->getRemoteAddressCount);
    mockIoOperationsData_Destroy(&ops);
}

LONGBOW_TEST_CASE(Global, metisIoOperations_GetAddressPair)
{
    MetisIoOperations *ops = mockIoOperationsData_CreateSimple(1, 2, 3, true, true, true);
    MockIoOperationsData *data = metisIoOperations_GetClosure(ops);

    metisIoOperations_GetAddressPair(ops);
    assertTrue(data->getAddressPairCount == 1, "Wrong metisIoOperations_GetAddressPairexpected count 1 got %u", data->getAddressPairCount);
    mockIoOperationsData_Destroy(&ops);
}

LONGBOW_TEST_CASE(Global, metisIoOperations_IsUp)
{
    MetisIoOperations *ops = mockIoOperationsData_CreateSimple(1, 2, 3, true, true, true);
    MockIoOperationsData *data = metisIoOperations_GetClosure(ops);

    metisIoOperations_IsUp(ops);
    assertTrue(data->isUpCount == 1, "Wrong metisIoOperations_IsUp count expected 1 got %u", data->isUpCount);
    mockIoOperationsData_Destroy(&ops);
}

LONGBOW_TEST_CASE(Global, metisIoOperations_IsLocal)
{
    MetisIoOperations *ops = mockIoOperationsData_CreateSimple(1, 2, 3, true, true, true);
    MockIoOperationsData *data = metisIoOperations_GetClosure(ops);

    metisIoOperations_IsLocal(ops);
    assertTrue(data->isLocalCount == 1, "Wrong metisIoOperations_IsLocal count expected 1 got %u", data->isLocalCount);
    mockIoOperationsData_Destroy(&ops);
}

LONGBOW_TEST_CASE(Global, metisIoOperations_GetConnectionId)
{
    MetisIoOperations *ops = mockIoOperationsData_CreateSimple(1, 2, 3, true, true, true);
    MockIoOperationsData *data = metisIoOperations_GetClosure(ops);

    metisIoOperations_GetConnectionId(ops);
    assertTrue(data->getConnectionIdCount == 1, "Wrong metisIoOperations_GetConnectionId count expected 1 got %u", data->getConnectionIdCount);
    mockIoOperationsData_Destroy(&ops);
}

LONGBOW_TEST_CASE(Global, metisIoOperations_Release)
{
    MetisIoOperations *ops = mockIoOperationsData_CreateSimple(1, 2, 3, true, true, true);
    MetisIoOperations *copy = ops;
    MockIoOperationsData *data = metisIoOperations_GetClosure(ops);

    metisIoOperations_Release(&ops);
    assertTrue(data->destroyCount == 1, "Wrong metisIoOperations_Release count expected 1 got %u", data->destroyCount);
    mockIoOperationsData_Destroy(&copy);
}

LONGBOW_TEST_CASE(Global, metisIoOperations_Class)
{
    MetisIoOperations *ops = mockIoOperationsData_CreateSimple(1, 2, 3, true, true, true);
    MockIoOperationsData *data = metisIoOperations_GetClosure(ops);

    metisIoOperations_Class(ops);
    assertTrue(data->classCount == 1, "Wrong metisIoOperations_Class count expected 1 got %u", data->classCount);
    mockIoOperationsData_Destroy(&ops);
}

LONGBOW_TEST_CASE(Global, metisIoOperations_GetConnectionType)
{
    MetisIoOperations *ops = mockIoOperationsData_CreateSimple(1, 2, 3, true, true, true);
    MockIoOperationsData *data = metisIoOperations_GetClosure(ops);

    metisIoOperations_GetConnectionType(ops);
    assertTrue(data->getConnectionTypeCount == 1, "Wrong getConnectionTypeCount count expected 1 got %u", data->getConnectionTypeCount);
    mockIoOperationsData_Destroy(&ops);
}



// ===========================================

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metis_IoOperations);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
