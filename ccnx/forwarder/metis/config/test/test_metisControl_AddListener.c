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

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../metisControl_AddListener.c"
#include "testrig_MetisControl.c"

LONGBOW_TEST_RUNNER(metisControl_AddListener)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metisControl_AddListener)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metisControl_AddListener)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, metisControlAddListener_Create);
    LONGBOW_RUN_TEST_CASE(Global, metisControlAddListener_HelpCreate);
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

LONGBOW_TEST_CASE(Global, metisControlAddListener_Create)
{
    testCommandCreate(testCase, &metisControlAddListener_Create, __func__);
}

LONGBOW_TEST_CASE(Global, metisControlAddListener_HelpCreate)
{
    testCommandCreate(testCase, &metisControlAddListener_HelpCreate, __func__);
}

// ===========================================================

LONGBOW_TEST_FIXTURE(Local)
{
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddListener_Execute_WrongArgCount);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddListener_Execute_Tcp);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddListener_Execute_Udp);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddListener_Execute_Udp6);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddListener_Execute_Ether);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddListener_Execute_UnknownProtocol);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddListener_Execute_BadSymbolic);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddListener_Execute_BadSymbolic_NotAlphaNum);

    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddListener_HelpExecute);
    LONGBOW_RUN_TEST_CASE(Local, _createTcpListener);
    LONGBOW_RUN_TEST_CASE(Local, _createUdpListener);
    LONGBOW_RUN_TEST_CASE(Local, _createEtherListener);
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

LONGBOW_TEST_CASE(Local, _createTcpListener)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = metisControlAddListener_Create(data->state);

    const char *argv[] = { "add", "listener", "tcp", "public0", "13.14.15.16", "9596", };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 6, (void **) &argv[0]);

    MetisCommandReturn test = _createTcpListener(data->state->parser, ops, args);

    assertTrue(test == MetisCommandReturn_Success, "Command did not return success: %d", test);
    assertTrue(data->writeread_count == 1, "Wrong write/read count, expected %d got %u", 1, data->writeread_count);

    parcList_Release(&args);
    ops->destroyer(&ops);
}

LONGBOW_TEST_CASE(Local, _createUdpListener)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = metisControlAddListener_Create(data->state);

    const char *argv[] = { "add", "listener", "udp", "public0", "13.14.15.16", "9596", };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 6, (void **) &argv[0]);

    MetisCommandReturn test = _createUdpListener(data->state->parser, ops, args);

    assertTrue(test == MetisCommandReturn_Success, "Command did not return success: %d", test);
    assertTrue(data->writeread_count == 1, "Wrong write/read count, expected %d got %u", 1, data->writeread_count);

    parcList_Release(&args);
    ops->destroyer(&ops);
}

LONGBOW_TEST_CASE(Local, _createEtherListener)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = metisControlAddListener_Create(data->state);

    const char *argv[] = { "add", "listener", "ether", "nic3", "eth3", "0x0801", };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 6, (void **) &argv[0]);

    MetisCommandReturn test = _createEtherListener(data->state->parser, ops, args);

    assertTrue(test == MetisCommandReturn_Success, "Command did not return success: %d", test);
    assertTrue(data->writeread_count == 1, "Wrong write/read count, expected %d got %u", 1, data->writeread_count);

    parcList_Release(&args);
    ops->destroyer(&ops);
}

LONGBOW_TEST_CASE(Local, _metisControlAddListener_Execute_WrongArgCount)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = metisControlAddListener_Create(data->state);

    const char *argv[] = { "add", "listener", "ether" "nic3", "eth3", "0x0801", "foobar" };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 7, (void **) &argv[0]);

    MetisCommandReturn test = _metisControlAddListener_Execute(data->state->parser, ops, args);

    assertTrue(test == MetisCommandReturn_Failure, "Command did not return failure: %d", test);
    assertTrue(data->writeread_count == 0, "Wrong write/read count, expected %d got %u", 0, data->writeread_count);

    parcList_Release(&args);
    ops->destroyer(&ops);
}

LONGBOW_TEST_CASE(Local, _metisControlAddListener_Execute_Tcp)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = metisControlAddListener_Create(data->state);
    metisControlState_SetDebug(data->state, true);

    const char *argv[] = { "add", "listener", "tcp", "public0", "13.14.15.16", "9596", };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 6, (void **) &argv[0]);

    MetisCommandReturn test = _metisControlAddListener_Execute(data->state->parser, ops, args);

    assertTrue(test == MetisCommandReturn_Success, "Command did not return success: %d", test);
    assertTrue(data->writeread_count == 1, "Wrong write/read count, expected %d got %u", 1, data->writeread_count);

    parcList_Release(&args);
    ops->destroyer(&ops);
}

LONGBOW_TEST_CASE(Local, _metisControlAddListener_Execute_Udp)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = metisControlAddListener_Create(data->state);
    metisControlState_SetDebug(data->state, true);

    const char *argv[] = { "add", "listener", "udp", "public0", "13.14.15.16", "9596", };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 6, (void **) &argv[0]);

    MetisCommandReturn test = _metisControlAddListener_Execute(data->state->parser, ops, args);

    assertTrue(test == MetisCommandReturn_Success, "Command did not return success: %d", test);
    assertTrue(data->writeread_count == 1, "Wrong write/read count, expected %d got %u", 1, data->writeread_count);

    parcList_Release(&args);
    ops->destroyer(&ops);
}

LONGBOW_TEST_CASE(Local, _metisControlAddListener_Execute_Udp6)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = metisControlAddListener_Create(data->state);
    metisControlState_SetDebug(data->state, true);

    // INET6 address
    const char *argv[] = { "add", "listener", "udp", "public0", "::1", "9596", };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 6, (void **) &argv[0]);

    MetisCommandReturn test = _metisControlAddListener_Execute(data->state->parser, ops, args);

    assertTrue(test == MetisCommandReturn_Success, "Command did not return success: %d", test);
    assertTrue(data->writeread_count == 1, "Wrong write/read count, expected %d got %u", 1, data->writeread_count);

    parcList_Release(&args);
    ops->destroyer(&ops);
}

LONGBOW_TEST_CASE(Local, _metisControlAddListener_Execute_Ether)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = metisControlAddListener_Create(data->state);

    const char *argv[] = { "add", "listener", "ether", "nic3", "eth3", "0x0801", };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 6, (void **) &argv[0]);

    MetisCommandReturn test = _metisControlAddListener_Execute(data->state->parser, ops, args);

    assertTrue(test == MetisCommandReturn_Success, "Command did not return success: %d", test);
    assertTrue(data->writeread_count == 1, "Wrong write/read count, expected %d got %u", 1, data->writeread_count);

    parcList_Release(&args);
    ops->destroyer(&ops);
}

LONGBOW_TEST_CASE(Local, _metisControlAddListener_Execute_UnknownProtocol)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = metisControlAddListener_Create(data->state);

    const char *argv[] = { "add", "listener", "pup", "nic3", "eth3", "0x0801" };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 6, (void **) &argv[0]);

    MetisCommandReturn test = _metisControlAddListener_Execute(data->state->parser, ops, args);

    assertTrue(test == MetisCommandReturn_Failure, "Command did not return failure: %d", test);
    assertTrue(data->writeread_count == 0, "Wrong write/read count, expected %d got %u", 0, data->writeread_count);

    parcList_Release(&args);
    ops->destroyer(&ops);
}

LONGBOW_TEST_CASE(Local, _metisControlAddListener_Execute_BadSymbolic)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = metisControlAddListener_Create(data->state);

    const char *argv[] = { "add", "listener", "ether" "111", "eth3", "0x0801" };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 6, (void **) &argv[0]);

    MetisCommandReturn test = _metisControlAddListener_Execute(data->state->parser, ops, args);

    assertTrue(test == MetisCommandReturn_Failure, "Command did not return failure: %d", test);
    assertTrue(data->writeread_count == 0, "Wrong write/read count, expected %d got %u", 0, data->writeread_count);

    parcList_Release(&args);
    ops->destroyer(&ops);
}


LONGBOW_TEST_CASE(Local, _metisControlAddListener_Execute_BadSymbolic_NotAlphaNum)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = metisControlAddListener_Create(data->state);

    const char *argv[] = { "add", "listener", "ether", "n()t", "eth3", "0x0801" };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 6, (void **) &argv[0]);

    MetisCommandReturn test = _metisControlAddListener_Execute(data->state->parser, ops, args);

    assertTrue(test == MetisCommandReturn_Failure, "Command did not return failure: %d", test);
    assertTrue(data->writeread_count == 0, "Wrong write/read count, expected %d got %u", 0, data->writeread_count);

    parcList_Release(&args);
    ops->destroyer(&ops);
}

LONGBOW_TEST_CASE(Local, _metisControlAddListener_HelpExecute)
{
    _metisControlAddListener_HelpExecute(NULL, NULL, NULL);
}

// ===========================================================

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metisControl_AddListener);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
