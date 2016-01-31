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

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../metis_ThreadedForwarder.c"

#include <LongBow/unit-test.h>

LONGBOW_TEST_RUNNER(metis_ThreadedForwarder)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metis_ThreadedForwarder)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metis_ThreadedForwarder)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, metisThreadedForwarder_AddCLI);
    LONGBOW_RUN_TEST_CASE(Global, metisThreadedForwarder_Create);
    LONGBOW_RUN_TEST_CASE(Global, metisThreadedForwarder_Destroy);
    LONGBOW_RUN_TEST_CASE(Global, metisThreadedForwarder_SetupAllListeners);
    LONGBOW_RUN_TEST_CASE(Global, metisThreadedForwarder_Start);
    LONGBOW_RUN_TEST_CASE(Global, metisThreadedForwarder_Stop);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, metisThreadedForwarder_AddCLI)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Global, metisThreadedForwarder_Create)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Global, metisThreadedForwarder_Destroy)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Global, metisThreadedForwarder_SetupAllListeners)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Global, metisThreadedForwarder_Start)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Global, metisThreadedForwarder_Stop)
{
    testUnimplemented("");
}

LONGBOW_TEST_FIXTURE(Local)
{
    LONGBOW_RUN_TEST_CASE(Local, metisThreadedForwarder_BroadcastStatus);
    LONGBOW_RUN_TEST_CASE(Local, metisThreadedForwarder_LockState);
    LONGBOW_RUN_TEST_CASE(Local, metisThreadedForwarder_Run);
    LONGBOW_RUN_TEST_CASE(Local, metisThreadedForwarder_UnlockState);
    LONGBOW_RUN_TEST_CASE(Local, metisThreadedForwarder_WaitStatus);
}

LONGBOW_TEST_FIXTURE_SETUP(Local)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Local)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Local, metisThreadedForwarder_BroadcastStatus)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Local, metisThreadedForwarder_LockState)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Local, metisThreadedForwarder_Run)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Local, metisThreadedForwarder_UnlockState)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Local, metisThreadedForwarder_WaitStatus)
{
    testUnimplemented("");
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metis_ThreadedForwarder);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
