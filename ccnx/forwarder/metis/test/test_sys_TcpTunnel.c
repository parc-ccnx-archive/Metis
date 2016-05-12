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
 * Run two instances of metis.
 * Client_1 - Metis_A - Metis_B - Client_2
 *
 * Steps
 * 1) run two instances of Metis
 * 2) Create TCP listeners on 127.0.0.1:10001 and 127.0.0.1:10002
 * 3) create a tunnel from A->B.
 * 4) setup route to /foo from a to b
 * 5) Connect client 1 to A
 * 6) Connect client 2 to B
 * 7) Setup route to /foo from metis B to client 2.
 * 8) Sent interest from #1 to #2
 * 9) Send object back from #2 to #1
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>

#include <ccnx/api/control/cpi_RouteEntry.h>
#include <ccnx/forwarder/metis/core/metis_Forwarder.h>
#include <ccnx/forwarder/metis/io/metis_TcpTunnel.h>
#include <ccnx/forwarder/metis/config/metis_Configuration.h>
#include <ccnx/forwarder/metis/config/metis_ConfigurationListeners.h>

#include <ccnx/forwarder/metis/testdata/metis_TestDataV0.h>

LONGBOW_TEST_RUNNER(test_sys_TcpTunnel)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(test_sys_TcpTunnel)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(test_sys_TcpTunnel)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, tcpTunnel);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    if (parcSafeMemory_ReportAllocation(STDOUT_FILENO) != 0) {
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

typedef struct notify_receiver {
    MetisMissive *lastMessage;
} NotifyReceiver;

static void
missiveNotify(MetisMessengerRecipient *recipient, MetisMissive *missive)
{
    NotifyReceiver *receiver = (NotifyReceiver *) metisMessengerRecipient_GetRecipientContext(recipient);
    if (receiver->lastMessage != NULL) {
        metisMissive_Release(&receiver->lastMessage);
    }
    receiver->lastMessage = missive;
}

LONGBOW_TEST_CASE(Global, tcpTunnel)
{
    uint16_t metisA_port = 10001;
    uint16_t metisB_port = 10002;

    // these will get filled in with the most recent message
    NotifyReceiver receiver_a = { NULL };
    NotifyReceiver receiver_b = { NULL };

    MetisMessengerRecipient *recipient_a = metisMessengerRecipient_Create(&receiver_a, missiveNotify);
    MetisMessengerRecipient *recipient_b = metisMessengerRecipient_Create(&receiver_b, missiveNotify);

    // in between each step, run the dispatchers for 1 msec to let things settle.

    // ===============================================
    /* 1) run two instances of Metis */
    MetisForwarder *metis_a = metisForwarder_Create(NULL);
    MetisForwarder *metis_b = metisForwarder_Create(NULL);

    MetisDispatcher *dispatcher_a = metisForwarder_GetDispatcher(metis_a);
    MetisDispatcher *dispatcher_b = metisForwarder_GetDispatcher(metis_b);

    // register to receive notifications
    metisMessenger_Register(metisForwarder_GetMessenger(metis_a), recipient_a);
    metisMessenger_Register(metisForwarder_GetMessenger(metis_b), recipient_b);

    // ===============================================
    /* 2) Create TCP listeners on 127.0.0.1:10001 and 10002 */

    metisConfigurationListeners_SetupAll(metisForwarder_GetConfiguration(metis_a), metisA_port, NULL);
    metisConfigurationListeners_SetupAll(metisForwarder_GetConfiguration(metis_b), metisB_port, NULL);

    // ---- run
    metisDispatcher_RunDuration(dispatcher_a, &((struct timeval) { 0, 1000 }));
    metisDispatcher_RunDuration(dispatcher_b, &((struct timeval) { 0, 1000 }));
    // ----

    // ===============================================
    /* 3) create a tunnel from A->B. */

    // connect from any address
    struct sockaddr_in metisA_AnyIpAddress;
    memset(&metisA_AnyIpAddress, 0, sizeof(metisA_AnyIpAddress));
    metisA_AnyIpAddress.sin_family = PF_INET;
    metisA_AnyIpAddress.sin_addr.s_addr = INADDR_ANY;

    // connect to 127.0.0.1:10002
    struct sockaddr_in metisB_LoopbackAddress;
    memset(&metisB_LoopbackAddress, 0, sizeof(metisB_LoopbackAddress));
    metisB_LoopbackAddress.sin_family = PF_INET;
    metisB_LoopbackAddress.sin_port = htons(metisB_port);
    inet_pton(AF_INET, "127.0.0.1", &(metisB_LoopbackAddress.sin_addr));

    CPIAddress *metisA_localCpiAddress = cpiAddress_CreateFromInet(&metisA_AnyIpAddress);
    CPIAddress *metisA_remoteCpiAddress = cpiAddress_CreateFromInet(&metisB_LoopbackAddress);

    MetisIoOperations *ops = metisTcpTunnel_Create(metis_a, metisA_localCpiAddress, metisA_remoteCpiAddress);
    MetisConnection *conn = metisConnection_Create(ops);
    metisConnectionTable_Add(metisForwarder_GetConnectionTable(metis_a), conn);

    cpiAddress_Destroy(&metisA_localCpiAddress);
    cpiAddress_Destroy(&metisA_remoteCpiAddress);

    // ---- run
    metisDispatcher_RunDuration(dispatcher_a, &((struct timeval) { 0, 1000 }));
    metisDispatcher_RunDuration(dispatcher_b, &((struct timeval) { 0, 1000 }));
    // ----

    // ===============================================
    /* 4) setup route to /foo from a to b */

    CCNxName *ccnxName = ccnxName_CreateFromCString("lci:/2=hello");
    CPIRouteEntry *route = cpiRouteEntry_Create(ccnxName, ops->getConnectionId(ops), NULL, cpiNameRouteProtocolType_STATIC, cpiNameRouteType_LONGEST_MATCH, NULL, 1);
    bool success = metisForwarder_AddOrUpdateRoute(metis_a, route);
    cpiRouteEntry_Destroy(&route);
    assertTrue(success, "error adding route from A to B");

    // ---- run
    metisDispatcher_RunDuration(dispatcher_a, &((struct timeval) { 0, 1000 }));
    metisDispatcher_RunDuration(dispatcher_b, &((struct timeval) { 0, 1000 }));
    // ----

    // ===============================================
    /* 5) Connect client 1 to A */

    struct sockaddr_in metisA_LoopbackAddress;
    memset(&metisA_LoopbackAddress, 0, sizeof(metisA_LoopbackAddress));
    metisA_LoopbackAddress.sin_family = PF_INET;
    metisA_LoopbackAddress.sin_port = htons(metisA_port);
    inet_pton(AF_INET, "127.0.0.1", &(metisA_LoopbackAddress.sin_addr));

    int client1_Socket = socket(PF_INET, SOCK_STREAM, 0);
    assertFalse(client1_Socket < 0, "Error creating socket: (%d) %s", errno, strerror(errno));

    int failure = connect(client1_Socket, (struct sockaddr *) &metisA_LoopbackAddress, sizeof(metisA_LoopbackAddress));
    assertFalse(failure, "Error connect: (%d) %s", errno, strerror(errno));

    // ---- run
    metisDispatcher_RunDuration(dispatcher_a, &((struct timeval) { 0, 1000 }));
    metisDispatcher_RunDuration(dispatcher_b, &((struct timeval) { 0, 1000 }));
    // ----

    // ===============================================
    /* 6) Connect client 2 to B */

    // We need to sniff connections on metis b to learn the connection ID of the client

    int client2_Socket = socket(PF_INET, SOCK_STREAM, 0);
    assertFalse(client2_Socket < 0, "Error creating socket: (%d) %s", errno, strerror(errno));

    failure = connect(client2_Socket, (struct sockaddr *) &metisB_LoopbackAddress, sizeof(metisB_LoopbackAddress));
    assertFalse(failure, "Error connect: (%d) %s", errno, strerror(errno));

    // ---- run
    metisDispatcher_RunDuration(dispatcher_a, &((struct timeval) { 0, 1000 }));
    metisDispatcher_RunDuration(dispatcher_b, &((struct timeval) { 0, 1000 }));
    // ----

    unsigned client2_ConnectionId = metisMissive_GetConnectionId(receiver_b.lastMessage);
    printf("client 2 connection id is %u\n", client2_ConnectionId);

    // ===============================================
    /* 7) Setup route to /foo from metis B to client 2. */

    ccnxName = ccnxName_CreateFromCString("lci:/2=hello");
    route = cpiRouteEntry_Create(ccnxName, client2_ConnectionId, NULL, cpiNameRouteProtocolType_STATIC, cpiNameRouteType_LONGEST_MATCH, NULL, 1);
    success = metisForwarder_AddOrUpdateRoute(metis_b, route);
    cpiRouteEntry_Destroy(&route);
    assertTrue(success, "error adding route from B to #2");

    // ---- run
    metisDispatcher_RunDuration(dispatcher_a, &((struct timeval) { 0, 1000 }));
    metisDispatcher_RunDuration(dispatcher_b, &((struct timeval) { 0, 1000 }));
    // ----

    // ===============================================
    /* 8) Sent interest from #1 to #2 */

    ssize_t interest_write_length = write(client1_Socket, metisTestDataV0_InterestWithName, sizeof(metisTestDataV0_InterestWithName));
    assertTrue(interest_write_length == sizeof(metisTestDataV0_InterestWithName),
               "Wrong write length, expected %zu got %zu",
               sizeof(metisTestDataV0_EncodedInterest),
               interest_write_length);

    metisDispatcher_RunDuration(dispatcher_a, &((struct timeval) { 0, 1000 }));
    metisDispatcher_RunDuration(dispatcher_b, &((struct timeval) { 0, 1000 }));

    // wait to receive it
    uint8_t readBuffer[1024];
    ssize_t interest_read_length = read(client2_Socket, readBuffer, 1024);
    assertTrue(interest_read_length == sizeof(metisTestDataV0_InterestWithName),
               "Wrong write length, expected %zu got %zu",
               sizeof(metisTestDataV0_InterestWithName),
               interest_read_length);

    metisDispatcher_RunDuration(dispatcher_a, &((struct timeval) { 0, 1000 }));
    metisDispatcher_RunDuration(dispatcher_b, &((struct timeval) { 0, 1000 }));

    // ===============================================
    /* 9) Send object back from #2 to #1 */

    ssize_t object_write_length = write(client2_Socket, metisTestDataV0_EncodedObject, sizeof(metisTestDataV0_EncodedObject));
    assertTrue(object_write_length == sizeof(metisTestDataV0_EncodedObject),
               "Wrong write length, expected %zu got %zu",
               sizeof(metisTestDataV0_EncodedInterest),
               object_write_length);

    // very important: run b first, then a
    metisDispatcher_RunDuration(dispatcher_b, &((struct timeval) { 0, 1000 }));
    metisDispatcher_RunDuration(dispatcher_a, &((struct timeval) { 0, 1000 }));

    // wait to receive it
    ssize_t object_read_length = read(client1_Socket, readBuffer, 1024);
    assertTrue(object_read_length == sizeof(metisTestDataV0_EncodedObject),
               "Wrong write length, expected %zu got %zu",
               sizeof(metisTestDataV0_EncodedObject),
               object_read_length);


    // ===============================================
    // cleanup
    metisMissive_Release(&receiver_a.lastMessage);
    metisMissive_Release(&receiver_b.lastMessage);
    metisMessengerRecipient_Destroy(&recipient_a);
    metisMessengerRecipient_Destroy(&recipient_b);
    metisForwarder_Destroy(&metis_b);
    metisForwarder_Destroy(&metis_a);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(test_sys_TcpTunnel);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
