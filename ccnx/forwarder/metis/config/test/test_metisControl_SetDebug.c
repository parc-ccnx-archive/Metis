/*
 * Copyright (c) 2014-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../metisControl_SetDebug.c"
#include "testrig_MetisControl.c"
#include <parc/algol/parc_SafeMemory.h>
#include <LongBow/unit-test.h>

LONGBOW_TEST_RUNNER(metisControl_SetDebug)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metisControl_SetDebug)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metisControl_SetDebug)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, metisControlSetDebug_HelpCreate);
    LONGBOW_RUN_TEST_CASE(Global, metisControlSetDebug_Create);
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

LONGBOW_TEST_CASE(Global, metisControlSetDebug_HelpCreate)
{
    testCommandCreate(testCase, &metisControlSetDebug_HelpCreate, __func__);
}

LONGBOW_TEST_CASE(Global, metisControlSetDebug_Create)
{
    testCommandCreate(testCase, &metisControlSetDebug_Create, __func__);
}

LONGBOW_TEST_FIXTURE(Local)
{
    LONGBOW_RUN_TEST_CASE(Local, metisControl_Help_SetDebug_Execute);
    LONGBOW_RUN_TEST_CASE(Local, metisControl_SetDebug_Execute_WrongArgCount);
    LONGBOW_RUN_TEST_CASE(Local, metisControl_SetDebug_Execute_Good);
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

LONGBOW_TEST_CASE(Local, metisControl_Help_SetDebug_Execute)
{
    testHelpExecute(testCase, &metisControlSetDebug_HelpCreate, __func__, MetisCommandReturn_Success);
}

static MetisCommandReturn
testDebug(const LongBowTestCase *testCase, MetisCommandOps * (*create)(MetisControlState * state), int argc, bool initialDebugSetting, bool expectedDebugSetting)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    const char *argv[] = { "blah", "blah" };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, argc, (void **) &argv[0]);

    metisControlState_SetDebug(data->state, initialDebugSetting);
    MetisCommandOps *ops = create(data->state);
    MetisCommandReturn result = ops->execute(data->state->parser, ops, args);
    if (result == MetisCommandReturn_Success) {
        assertTrue(data->state->debugFlag == expectedDebugSetting,
                   "Debug flag wrong, expected %d got %d",
                   expectedDebugSetting,
                   data->state->debugFlag);
    }

    metisCommandOps_Destroy(&ops);
    parcList_Release(&args);
    return result;
}

LONGBOW_TEST_CASE(Local, metisControl_SetDebug_Execute_WrongArgCount)
{
    MetisCommandReturn result = testDebug(testCase, metisControlSetDebug_Create, 3, false, true);
    assertTrue(result == MetisCommandReturn_Failure,
               "metisControl_SetDebug_Execute should return %d, got %d", MetisCommandReturn_Failure, result);
}


LONGBOW_TEST_CASE(Local, metisControl_SetDebug_Execute_Good)
{
    MetisCommandReturn result = testDebug(testCase, metisControlSetDebug_Create, 2, false, true);
    assertTrue(result == MetisCommandReturn_Success,
               "metisControl_SetDebug_Execute should return %d, got %d", MetisCommandReturn_Success, result);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metisControl_SetDebug);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
