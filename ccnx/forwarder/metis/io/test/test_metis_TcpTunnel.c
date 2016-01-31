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
#include "../metis_TcpTunnel.c"
#include <LongBow/unit-test.h>

#include <parc/algol/parc_SafeMemory.h>

#include <ccnx/forwarder/metis/testdata/metis_TestDataV0.h>

// so we can see packet events
#include "../../processor/test/testrig_MockTap.h"

// inet_pton
#include <arpa/inet.h>

#include <fcntl.h>

#ifndef INPORT_ANY
#define INPORT_ANY 0
#endif

LONGBOW_TEST_RUNNER(metis_TcpTunnel)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metis_TcpTunnel)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metis_TcpTunnel)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

typedef struct test_data {
    MetisForwarder *metis;
    MetisDispatcher *dispatcher;

    int serverSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in localAddr;

    CPIAddress *localCpiAddress;
    CPIAddress *remoteCpiAddress;

    MetisIoOperations *tunnelOps;
} TestData;

static void
listenToInet(TestData *data)
{
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    assertFalse(fd < 0, "error on bind: (%d) %s", errno, strerror(errno));

    // Set non-blocking flag
    int flags = fcntl(fd, F_GETFL, NULL);
    assertTrue(flags != -1, "fcntl failed to obtain file descriptor flags (%d)\n", errno);
    int failure = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    assertFalse(failure, "fcntl failed to set file descriptor flags (%d)\n", errno);

    failure = bind(fd, (struct sockaddr *) &data->serverAddr, sizeof(struct sockaddr_in));
    assertFalse(failure, "error on bind: (%d) %s", errno, strerror(errno));

    failure = listen(fd, 16);
    assertFalse(failure, "error on listen: (%d) %s", errno, strerror(errno));

    data->serverSocket = fd;
    socklen_t x = sizeof(data->serverAddr);
    getsockname(fd, (struct sockaddr *) &data->serverAddr, &x);
}


LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, metisTcpTunnel_Create);
    LONGBOW_RUN_TEST_CASE(Global, metisTcpTunnel_Create_ConnectionStartsDown);
    LONGBOW_RUN_TEST_CASE(Global, metisTcpTunnel_Create_UpStateAfterAccept);
    LONGBOW_RUN_TEST_CASE(Global, metisTcpTunnel_Create_Ping);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    memset(&testTap, 0, sizeof(testTap));

    TestData *data = malloc(sizeof(TestData));
    memset(data, 0, sizeof(TestData));

    data->metis = metisForwarder_Create(NULL);
    data->serverAddr.sin_family = PF_INET;
    data->serverAddr.sin_port = INPORT_ANY;
    inet_pton(AF_INET, "127.0.0.1", &(data->serverAddr.sin_addr));

    data->localAddr.sin_family = PF_INET;
    data->localAddr.sin_addr.s_addr = INADDR_ANY;
    data->localAddr.sin_port = INPORT_ANY;

    listenToInet(data);

    data->localCpiAddress = cpiAddress_CreateFromInet(&data->localAddr);
    data->remoteCpiAddress = cpiAddress_CreateFromInet(&data->serverAddr);

    data->dispatcher = metisForwarder_GetDispatcher(data->metis);

    longBowTestCase_SetClipBoardData(testCase, data);

    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    cpiAddress_Destroy(&data->localCpiAddress);
    cpiAddress_Destroy(&data->remoteCpiAddress);

    close(data->serverSocket);
    metisForwarder_Destroy(&data->metis);
    free(data);

    if (parcSafeMemory_ReportAllocation(STDOUT_FILENO) != 0) {
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, metisTcpTunnel_Create)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    data->tunnelOps = metisTcpTunnel_Create(data->metis, data->localCpiAddress, data->remoteCpiAddress);
    assertNotNull(data->tunnelOps, "Got null IO operations for the tunnel");
    data->tunnelOps->destroy(&data->tunnelOps);
}


LONGBOW_TEST_CASE(Global, metisTcpTunnel_Create_ConnectionStartsDown)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    data->tunnelOps = metisTcpTunnel_Create(data->metis, data->localCpiAddress, data->remoteCpiAddress);
    assertFalse(data->tunnelOps->isUp(data->tunnelOps), "Connection is not down on start");
    data->tunnelOps->destroy(&data->tunnelOps);
}

LONGBOW_TEST_CASE(Global, metisTcpTunnel_Create_UpStateAfterAccept)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    data->tunnelOps = metisTcpTunnel_Create(data->metis, data->localCpiAddress, data->remoteCpiAddress);

    // run for a milli second
    metisDispatcher_RunDuration(data->dispatcher, &((struct timeval) { 0, 1000 }));

    // we should be able to accept
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLength = sizeof(clientAddr);

    int clientSocket = accept(data->serverSocket, (struct sockaddr *) &clientAddr, &clientAddrLength);
    assertFalse(clientSocket < 0, "error on accept: (%d) %s", errno, strerror(errno));

    // run for a milli second
    metisDispatcher_RunDuration(data->dispatcher, &((struct timeval) { 0, 1000 }));

    assertTrue(data->tunnelOps->isUp(data->tunnelOps), "Connection is not up after accept");
    data->tunnelOps->destroy(&data->tunnelOps);
}

/**
 * send a packet from client to server
 */
LONGBOW_TEST_CASE(Global, metisTcpTunnel_Create_Ping)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);

    // setup our message tap so we can see packet counts
    testTap.callOnReceive = true;
    metisForwarder_AddTap(data->metis, &testTapTemplate);

    data->tunnelOps = metisTcpTunnel_Create(data->metis, data->localCpiAddress, data->remoteCpiAddress);

    // run for a milli second
    metisDispatcher_RunDuration(data->dispatcher, &((struct timeval) { 0, 1000 }));

    // we should be able to accept
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLength = sizeof(clientAddr);

    int clientSocket = accept(data->serverSocket, (struct sockaddr *) &clientAddr, &clientAddrLength);
    assertFalse(clientSocket < 0, "error on accept: (%d) %s", errno, strerror(errno));

    // run for a milli second
    metisDispatcher_RunDuration(data->dispatcher, &((struct timeval) { 0, 1000 }));

    size_t beforeOnReceiveCount = testTap.onReceiveCount;
    ssize_t write_length = write(clientSocket, metisTestDataV0_EncodedInterest, sizeof(metisTestDataV0_EncodedInterest));
    assertTrue(write_length == sizeof(metisTestDataV0_EncodedInterest), "wrong write length, expected %zd got %zd", sizeof(metisTestDataV0_EncodedInterest), write_length);
    metisDispatcher_RunDuration(data->dispatcher, &((struct timeval) { 0, 1000 }));
    size_t afterOnReceiveCount = testTap.onReceiveCount;

    assertTrue(afterOnReceiveCount == beforeOnReceiveCount + 1, "Did not see packet, expected %zu got %zu", beforeOnReceiveCount + 1, afterOnReceiveCount);

    assertTrue(data->tunnelOps->isUp(data->tunnelOps), "Connection is not up after accept");
    data->tunnelOps->destroy(&data->tunnelOps);
}

LONGBOW_TEST_FIXTURE(Local)
{
}

LONGBOW_TEST_FIXTURE_SETUP(Local)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Local)
{
    if (parcSafeMemory_ReportAllocation(STDOUT_FILENO) != 0) {
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metis_TcpTunnel);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
