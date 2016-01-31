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

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../metisControl_ListInterfaces.c"
#include "testrig_MetisControl.c"
#include <parc/algol/parc_SafeMemory.h>
#include <LongBow/unit-test.h>

LONGBOW_TEST_RUNNER(metisControl_ListInterfaces)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metisControl_ListInterfaces)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);

    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metisControl_ListInterfaces)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, metisControlListInterfaces_HelpCreate);
    LONGBOW_RUN_TEST_CASE(Global, metisControlListInterfaces_Create);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    testrigMetisControl_commonSetup(testCase);
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    testrigMetisControl_CommonTeardown(testCase);
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, metisControlListInterfaces_HelpCreate)
{
    testCommandCreate(testCase, &metisControlListInterfaces_HelpCreate, __func__);
}

LONGBOW_TEST_CASE(Global, metisControlListInterfaces_Create)
{
    testCommandCreate(testCase, &metisControlListInterfaces_Create, __func__);
}

LONGBOW_TEST_FIXTURE(Local)
{
    LONGBOW_RUN_TEST_CASE(Local, metisControl_Help_ListInterfaces_Execute);
    LONGBOW_RUN_TEST_CASE(Local, metisControl_ListInterfaces_Execute_WrongArgCount);
    LONGBOW_RUN_TEST_CASE(Local, metisControl_ListInterfaces_Execute_Good);
}

LONGBOW_TEST_FIXTURE_SETUP(Local)
{
    testrigMetisControl_commonSetup(testCase);
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Local)
{
    testrigMetisControl_CommonTeardown(testCase);
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Local, metisControl_Help_ListInterfaces_Execute)
{
    testHelpExecute(testCase, &metisControlListInterfaces_HelpCreate, __func__, MetisCommandReturn_Success);
}

LONGBOW_TEST_CASE(Local, metisControl_ListInterfaces_Execute)
{
    testUnimplemented("");
}

static CCNxControl *
customWriteReadResponse(void *userdata, CCNxMetaMessage *messageToWrite)
{
    CPIInterfaceSet *set = cpiInterfaceSet_Create();
    cpiInterfaceSet_Add(set, cpiInterface_Create("abc0", 1, false, true, 1500));
    cpiInterfaceSet_Add(set, cpiInterface_Create("abc1", 2, false, true, 1500));
    PARCJSON *setJson = cpiInterfaceSet_ToJson(set);

    CCNxControl *inboundControlMessage = ccnxMetaMessage_GetControl(messageToWrite);

    // Create a response to the inbound Control message.
    CCNxControl *outboundControlMessage = cpi_CreateResponse(inboundControlMessage, setJson);
    parcJSON_Release(&setJson);

    ccnxControl_Release(&inboundControlMessage);
    cpiInterfaceSet_Destroy(&set);

    return outboundControlMessage;
}

static MetisCommandReturn
testListInterfaces(const LongBowTestCase *testCase, int argc)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    metisControlState_SetDebug(data->state, true);
    data->customWriteReadReply = &customWriteReadResponse;

    const char *argv[] = { "list", "connections" };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, argc, (void **) &argv[0]);

    MetisCommandOps *ops = metisControlListInterfaces_Create(data->state);

    MetisCommandReturn result = ops->execute(data->state->parser, ops, args);
    metisCommandOps_Destroy(&ops);
    parcList_Release(&args);
    return result;
}

LONGBOW_TEST_CASE(Local, metisControl_ListInterfaces_Execute_WrongArgCount)
{
    // argc is wrong, needs to be 2.
    MetisCommandReturn result = testListInterfaces(testCase, 3);

    assertTrue(result == MetisCommandReturn_Failure,
               "metisControl_ListInterfaces with wrong argc should return %d, got %d", MetisCommandReturn_Failure, result);
}

LONGBOW_TEST_CASE(Local, metisControl_ListInterfaces_Execute_Good)
{
    MetisCommandReturn result = testListInterfaces(testCase, 2);

    assertTrue(result == MetisCommandReturn_Success,
               "metisControl_ListInterfaces should return %d, got %d", MetisCommandReturn_Success, result);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metisControl_ListInterfaces);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
