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
#include "../metisControl_AddConnection.c"
#include "testrig_MetisControl.c"

LONGBOW_TEST_RUNNER(metisControl_AddConnection)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metisControl_AddConnection)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metisControl_AddConnection)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, metisControlAddConnection_Create);
    LONGBOW_RUN_TEST_CASE(Global, metisControlAddConnection_HelpCreate);
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

LONGBOW_TEST_CASE(Global, metisControlAddConnection_Create)
{
    testCommandCreate(testCase, &metisControlAddConnection_Create, __func__);
}

LONGBOW_TEST_CASE(Global, metisControlAddConnection_HelpCreate)
{
    testCommandCreate(testCase, &metisControlAddConnection_HelpCreate, __func__);
}

LONGBOW_TEST_FIXTURE(Local)
{
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_EtherCreate);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_EtherExecute);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_McastCreate);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_McastExecute);

    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_TcpCreate);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_TcpExecute);

    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_UdpCreate);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_UdpExecute);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_Execute);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_Init);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_ConvertStringsToCpiAddress);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_CreateTunnel);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_EtherHelpCreate);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_EtherHelpExecute);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_McastHelpCreate);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_McastHelpExecute);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_TcpHelpCreate);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_TcpHelpExecute);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_UdpHelpCreate);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_UdpHelpExecute);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_HelpExecute);
    LONGBOW_RUN_TEST_CASE(Local, _metisControlAddConnection_IpHelp);

    LONGBOW_RUN_TEST_CASE(Local, metisControl_ParseIPCommandLine_TooFewArgs);
    LONGBOW_RUN_TEST_CASE(Local, metisControl_ParseIPCommandLine_TooManyArgs);
    LONGBOW_RUN_TEST_CASE(Local, metisControl_ParseIPCommandLine_BadRemoteIp);
    LONGBOW_RUN_TEST_CASE(Local, metisControl_ParseIPCommandLine_GoodRemoteIp);
    LONGBOW_RUN_TEST_CASE(Local, metisControl_ParseIPCommandLine_WithLocalIp);
    LONGBOW_RUN_TEST_CASE(Local, metisControl_ParseIPCommandLine_WithLocalIpAndPort);
    LONGBOW_RUN_TEST_CASE(Local, metisControl_ParseIPCommandLine_BadLocalIp);
    LONGBOW_RUN_TEST_CASE(Local, metisControl_ParseIPCommandLine_MismatchLocalAndRemote);
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

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_EtherCreate)
{
    testCommandCreate(testCase, &_metisControlAddConnection_EtherCreate, __func__);
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_EtherExecute)
{
    const char *argv[] = { "add", "connection", "ether", "conn3", "e8-06-88-cd-28-de", "em3" };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 6, (void **) &argv[0]);


    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = _metisControlAddConnection_EtherCreate(data->state);
    MetisCommandReturn result = ops->execute(data->state->parser, ops, args);
    metisCommandOps_Destroy(&ops);
    parcList_Release(&args);
    assertTrue(result == MetisCommandReturn_Success, "Valid command line should succeed");
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_McastCreate)
{
    testCommandCreate(testCase, &_metisControlAddConnection_McastCreate, __func__);
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_McastExecute)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = _metisControlAddConnection_McastCreate(data->state);
    MetisCommandReturn result = ops->execute(data->state->parser, ops, NULL);
    metisCommandOps_Destroy(&ops);
    assertTrue(result == MetisCommandReturn_Failure, "Unimplemented execute should have failed");
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_TcpCreate)
{
    testCommandCreate(testCase, &_metisControlAddConnection_TcpCreate, __func__);
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_TcpExecute)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    const char *argv[] = { "add", "connection", "tcp", "conn3", "1.2.3.4", "123" };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 6, (void **) &argv[0]);

    MetisCommandOps *ops = _metisControlAddConnection_TcpCreate(data->state);
    MetisCommandReturn result = ops->execute(data->state->parser, ops, args);

    metisCommandOps_Destroy(&ops);
    parcList_Release(&args);

    assertTrue(result == MetisCommandReturn_Success, "Unimplemented execute should have failed");
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_UdpCreate)
{
    testCommandCreate(testCase, &_metisControlAddConnection_UdpCreate, __func__);
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_UdpExecute)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    const char *argv[] = { "add", "connection", "tcp", "conn3", "1.2.3.4", "123" };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 6, (void **) &argv[0]);

    MetisCommandOps *ops = _metisControlAddConnection_UdpCreate(data->state);
    MetisCommandReturn result = ops->execute(data->state->parser, ops, args);

    metisCommandOps_Destroy(&ops);
    parcList_Release(&args);

    assertTrue(result == MetisCommandReturn_Success, "Unimplemented execute should have failed");
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_Execute)
{
    // this just prints a Help message
    testHelpExecute(testCase, metisControlAddConnection_Create, __func__, MetisCommandReturn_Success);
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_Init)
{
    testInit(testCase, metisControlAddConnection_Create, __func__,
             (const char *[]) {
        _commandAddConnectionTcp, _commandAddConnectionUdp, _commandAddConnectionEther, _commandAddConnectionMcast,
        _commandAddConnectionTcpHelp, _commandAddConnectionUdpHelp, _commandAddConnectionEtherHelp, _commandAddConnectionMcastHelp,
        NULL
    });
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_ConvertStringsToCpiAddress)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_CreateTunnel)
{
    // this is actully testred in the Tcp_Execute and Udp_Execute
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_EtherHelpCreate)
{
    testCommandCreate(testCase, &_metisControlAddConnection_EtherHelpCreate, __func__);
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_EtherHelpExecute)
{
    testHelpExecute(testCase, _metisControlAddConnection_EtherHelpCreate, __func__, MetisCommandReturn_Success);
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_McastHelpCreate)
{
    testCommandCreate(testCase, &_metisControlAddConnection_McastHelpCreate, __func__);
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_McastHelpExecute)
{
    testHelpExecute(testCase, _metisControlAddConnection_McastHelpCreate, __func__, MetisCommandReturn_Success);
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_TcpHelpCreate)
{
    testCommandCreate(testCase, &_metisControlAddConnection_TcpHelpCreate, __func__);
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_TcpHelpExecute)
{
    testHelpExecute(testCase, _metisControlAddConnection_TcpHelpCreate, __func__, MetisCommandReturn_Success);
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_UdpHelpCreate)
{
    testCommandCreate(testCase, &_metisControlAddConnection_UdpHelpCreate, __func__);
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_UdpHelpExecute)
{
    testHelpExecute(testCase, _metisControlAddConnection_UdpHelpCreate, __func__, MetisCommandReturn_Success);
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_HelpExecute)
{
    testHelpExecute(testCase, metisControlAddConnection_HelpCreate, __func__, MetisCommandReturn_Success);
}

/**
 * Expectes 5 to 7 options
 */
LONGBOW_TEST_CASE(Local, metisControl_ParseIPCommandLine_TooFewArgs)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    const char *argv[] = { "a", "b", "c" };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 3, (void **) &argv[0]);

    MetisCommandOps *ops = _metisControlAddConnection_TcpCreate(data->state);

    CPIAddress *remote;
    CPIAddress *local;
    char *symbolic = NULL;

    MetisCommandReturn result = _metisControlAddConnection_ParseIPCommandLine(data->state->parser, ops, args, &remote, &local, &symbolic);
    metisCommandOps_Destroy(&ops);
    parcList_Release(&args);

    assertTrue(result == MetisCommandReturn_Failure, "ParseIPCommandLine with 3 args should have returned %d, got %d", MetisCommandReturn_Failure, result);
}

/**
 * Expects 5 to 7 options
 */
LONGBOW_TEST_CASE(Local, metisControl_ParseIPCommandLine_TooManyArgs)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    const char *argv[] = { "a", "b", "c", "d", "e", "f", "g", "h", "i" };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 9, (void **) &argv[0]);

    MetisCommandOps *ops = _metisControlAddConnection_TcpCreate(data->state);

    CPIAddress *remote;
    CPIAddress *local;
    char *symbolic = NULL;

    MetisCommandReturn result = _metisControlAddConnection_ParseIPCommandLine(data->state->parser, ops, args, &remote, &local, &symbolic);
    metisCommandOps_Destroy(&ops);
    parcList_Release(&args);

    assertTrue(result == MetisCommandReturn_Failure, "ParseIPCommandLine with 3 args should have returned %d, got %d", MetisCommandReturn_Failure, result);
}

LONGBOW_TEST_CASE(Local, metisControl_ParseIPCommandLine_BadRemoteIp)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    const char *argv[] = { "a", "b", "c", "tun0", "555.555.555.555", "123", };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 6, (void **) &argv[0]);

    MetisCommandOps *ops = _metisControlAddConnection_TcpCreate(data->state);

    CPIAddress *remote;
    CPIAddress *local;
    char *symbolic = NULL;

    MetisCommandReturn result = _metisControlAddConnection_ParseIPCommandLine(data->state->parser, ops, args, &remote, &local, &symbolic);
    metisCommandOps_Destroy(&ops);
    parcList_Release(&args);

    assertTrue(result == MetisCommandReturn_Failure, "ParseIPCommandLine with invalid IP address should have returned %d, got %d", MetisCommandReturn_Failure, result);
}

/**
 * Pass a set of args to metisControl_ParseIPCommandLine, then verify:
 * Successful
 * remote_ip is what we gave it
 * remote_port is what we gave it
 * local_ip is 0.0.0.0 or what we gave it
 * local_pot is 0 or what we gave it.
 */
static void
verifyParseIpWithGoodAddress(TestData *data, int argc, const char *remote_ip, const char *remote_port, const char *local_ip, const char *local_port)
{
    const char *argv[] = { "a", "b", "c", "tun0", remote_ip, remote_port, local_ip, local_port };

    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, argc, (void **) &argv[0]);

    MetisCommandOps *ops = _metisControlAddConnection_TcpCreate(data->state);

    CPIAddress *remote;
    CPIAddress *local;
    char *symbolic = NULL;

    MetisCommandReturn result = _metisControlAddConnection_ParseIPCommandLine(data->state->parser, ops, args, &remote, &local, &symbolic);
    metisCommandOps_Destroy(&ops);
    parcList_Release(&args);

    assertTrue(result == MetisCommandReturn_Success, "ParseIPCommandLine with invalid IP address should have returned %d, got %d", MetisCommandReturn_Failure, result);

    struct sockaddr *sockaddr_remote = parcNetwork_SockAddress(remote_ip, atoi(remote_port));
    struct sockaddr *sockaddr_local = parcNetwork_SockAddress(local_ip, atoi(local_port));
    CPIAddress *truth_remote = cpiAddress_CreateFromInet((struct sockaddr_in *) sockaddr_remote);
    CPIAddress *truth_local = cpiAddress_CreateFromInet((struct sockaddr_in *) sockaddr_local);
    parcMemory_Deallocate((void **) &sockaddr_local);
    parcMemory_Deallocate((void **) &sockaddr_remote);

    assertTrue(cpiAddress_Equals(truth_remote, remote), "Got wrong remote address");
    assertTrue(cpiAddress_Equals(truth_local, local), "Got wrong local address");
    cpiAddress_Destroy(&truth_remote);
    cpiAddress_Destroy(&truth_local);
    cpiAddress_Destroy(&remote);
    cpiAddress_Destroy(&local);
}

LONGBOW_TEST_CASE(Local, metisControl_ParseIPCommandLine_GoodRemoteIp)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    verifyParseIpWithGoodAddress(data, 6, "1.2.3.4", "123", "0.0.0.0", "0");
}

LONGBOW_TEST_CASE(Local, metisControl_ParseIPCommandLine_WithLocalIp)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    verifyParseIpWithGoodAddress(data, 7, "1.2.3.4", "123", "10.11.12.13", "0");
}

LONGBOW_TEST_CASE(Local, metisControl_ParseIPCommandLine_WithLocalIpAndPort)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    verifyParseIpWithGoodAddress(data, 8, "1.2.3.4", "123", "10.11.12.13", "456");
}

LONGBOW_TEST_CASE(Local, metisControl_ParseIPCommandLine_BadLocalIp)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    const char *argv[] = { "a", "b", "c", "tun0", "1.2.3.4", "123", "666.666.666.666", "123", };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 8, (void **) &argv[0]);

    MetisCommandOps *ops = _metisControlAddConnection_TcpCreate(data->state);

    CPIAddress *remote;
    CPIAddress *local;
    char *symbolic = NULL;

    MetisCommandReturn result = _metisControlAddConnection_ParseIPCommandLine(data->state->parser, ops, args, &remote, &local, &symbolic);
    metisCommandOps_Destroy(&ops);
    parcList_Release(&args);

    assertTrue(result == MetisCommandReturn_Failure, "ParseIPCommandLine with invalid local IP address should have returned %d, got %d", MetisCommandReturn_Failure, result);
}

/**
 * One's an IPv4 and one's an IPv6.
 */
LONGBOW_TEST_CASE(Local, metisControl_ParseIPCommandLine_MismatchLocalAndRemote)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    const char *argv[] = { "a", "b", "c", "tun0", "1.2.3.4", "123", "2001:720:1500:1::a100", "123", };
    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, 8, (void **) &argv[0]);

    MetisCommandOps *ops = _metisControlAddConnection_TcpCreate(data->state);

    CPIAddress *remote;
    CPIAddress *local;
    char *symbolic = NULL;

    MetisCommandReturn result = _metisControlAddConnection_ParseIPCommandLine(data->state->parser, ops, args, &remote, &local, &symbolic);
    metisCommandOps_Destroy(&ops);
    parcList_Release(&args);

    assertTrue(result == MetisCommandReturn_Failure, "ParseIPCommandLine with invalid local IP address should have returned %d, got %d", MetisCommandReturn_Failure, result);
}

LONGBOW_TEST_CASE(Local, _metisControlAddConnection_IpHelp)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = _metisControlAddConnection_McastHelpCreate(data->state);
    MetisCommandReturn result = _metisControlAddConnection_IpHelp(NULL, ops, NULL, "WIZARD");
    assertTrue(result == MetisCommandReturn_Success, "Wrong return, got %d expected %d", result, MetisCommandReturn_Success);
    metisCommandOps_Destroy(&ops);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metisControl_AddConnection);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
