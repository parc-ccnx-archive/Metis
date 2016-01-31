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

/**
 * These are tests for error conditions, mostly in packet formats.
 */

#include <config.h>
#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <net/ethernet.h>

#include <ccnx/api/control/cpi_RouteEntry.h>
#include <ccnx/forwarder/metis/core/metis_Forwarder.h>
#include <ccnx/forwarder/metis/io/metis_UdpListener.h>
#include <ccnx/forwarder/metis/io/metis_TcpListener.h>
#include <ccnx/forwarder/metis/io/metis_EtherListener.h>
#include <ccnx/forwarder/metis/io/metis_EtherConnection.h>

#include <ccnx/forwarder/metis/testdata/metis_TestDataV0.h>
#include <ccnx/forwarder/metis/testdata/metis_TestDataV1.h>

#include <ccnx/forwarder/metis/io/test/testrig_GenericEther.c>

LONGBOW_TEST_RUNNER(sys_Errors)
{
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(sys_Errors)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(sys_Errors)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// ---- Used to monitor Missive messages so we know when
//      a connection is setup
static struct test_notifier_data {
    MetisMissiveType type;
    unsigned connectionid;
} testNotifierData;

static void
_testNotifier(MetisMessengerRecipient *recipient, MetisMissive *missive)
{
    struct test_notifier_data *data = metisMessengerRecipient_GetRecipientContext(recipient);
    data->type = metisMissive_GetType(missive);
    data->connectionid = metisMissive_GetConnectionId(missive);
    metisMissive_Release(&missive);
}

// ---- Utility functions to setup endpoints

static void
_setupListener(MetisForwarder *metis, uint16_t port, MetisEncapType type)
{
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr));

    MetisListenerSet *listeners = metisForwarder_GetListenerSet(metis);
    MetisListenerOps *ops = NULL;
    
    switch (type) {
        case METIS_ENCAP_UDP:
            ops = metisUdpListener_CreateInet(metis, addr);
            break;
            
        case METIS_ENCAP_TCP:
            ops = metisTcpListener_CreateInet(metis, addr);
            break;
            
        case METIS_ENCAP_ETHER:
            ops = metisEtherListener_Create(metis, "fake0", port);
            break;

        default:
            trapUnexpectedState("Unsupported ");
    }

    assertNotNull(ops, "Got null io operations");
    
    metisListenerSet_Add(listeners, ops);

    // crank the handle once
    metisDispatcher_RunDuration(metisForwarder_GetDispatcher(metis), &((struct timeval) {0, 10000}));
}

static int
_setupInetClient(MetisForwarder *metis, uint16_t port, int type)
{
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &(addr.sin_addr));

    int fd = socket(PF_INET, type, 0);
    assertFalse(fd < 0, "Error on socket: (%d) %s", errno, strerror(errno));

    int failure = connect(fd, (struct sockaddr *) &addr, sizeof(addr));
    assertFalse(failure, "Error on connect: (%d) %s", errno, strerror(errno));

    // crank the handle once
    metisDispatcher_RunDuration(metisForwarder_GetDispatcher(metis), &((struct timeval) {0, 10000}));
    return fd;
}

// ---- Global state used by tests

typedef struct test_data {
    MetisForwarder *metis;
    MetisMessengerRecipient *recipient;

    int fd_sender;
} TestData;

static void
_commonSetup(const LongBowTestCase *testCase)
{
    TestData *data = parcMemory_Allocate(sizeof(TestData));
    
    data->metis = metisForwarder_Create(NULL);
    metisLogger_SetLogLevel(metisForwarder_GetLogger(data->metis), MetisLoggerFacility_IO, PARCLogLevel_Debug);
    metisLogger_SetLogLevel(metisForwarder_GetLogger(data->metis), MetisLoggerFacility_Message, PARCLogLevel_Debug);
    metisLogger_SetLogLevel(metisForwarder_GetLogger(data->metis), MetisLoggerFacility_Core, PARCLogLevel_Debug);
    metisLogger_SetLogLevel(metisForwarder_GetLogger(data->metis), MetisLoggerFacility_Processor, PARCLogLevel_Debug);
    
    data->recipient = metisMessengerRecipient_Create(&testNotifierData, _testNotifier);
    
    // register a messenger callback so we know when the connections get setup
    MetisMessenger *messenger = metisForwarder_GetMessenger(data->metis);
    metisMessenger_Register(messenger, data->recipient);
    
    longBowTestCase_SetClipBoardData(testCase, data);
}

static void
_setupInetEncap(const LongBowTestCase *testCase, uint16_t port, MetisEncapType encap)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    
    _setupListener(data->metis, port, encap);

    // create a client
    switch (encap) {
        case METIS_ENCAP_UDP:
            data->fd_sender = _setupInetClient(data->metis, port, SOCK_DGRAM);
            break;
            
        case METIS_ENCAP_TCP:
            data->fd_sender = _setupInetClient(data->metis, port, SOCK_STREAM);
            break;

        default:
            trapUnexpectedState("Unsupported encap type: %d", encap);
    }
    
    metisDispatcher_RunDuration(metisForwarder_GetDispatcher(data->metis), &((struct timeval) {0, 1000}));


    // send something to actually connect it, this is a good packet
    ssize_t nwritten = write(data->fd_sender, metisTestDataV1_Interest_NameA_Crc32c, sizeof(metisTestDataV1_Interest_NameA_Crc32c));
    assertTrue(nwritten == sizeof(metisTestDataV1_Interest_NameA_Crc32c), "Short write, expected %zu got %zd", sizeof(metisTestDataV1_Interest_NameA_Crc32c), nwritten);
    
    metisDispatcher_RunDuration(metisForwarder_GetDispatcher(data->metis), &((struct timeval) {0, 1000}));

    printf("sender port %u connection id = %u\n", port, testNotifierData.connectionid);
}

static void
_commonTeardown(const LongBowTestCase *testCase)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    
    metisForwarder_Destroy(&data->metis);
    metisMessengerRecipient_Destroy(&data->recipient);

    close(data->fd_sender);
    parcMemory_Deallocate((void **) &data);
}

// ==========================================================

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, udp);
    LONGBOW_RUN_TEST_CASE(Global, ether);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    _commonSetup(testCase);
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    _commonTeardown(testCase);
    if (parcSafeMemory_ReportAllocation(STDOUT_FILENO) != 0) {
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, udp)
{
    _setupInetEncap(testCase, 44999, METIS_ENCAP_UDP);
    
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    
    for (int i = 0; metisTestDataV1_ErrorFrames[i].frame != NULL; i++) {
        const uint8_t *frame = metisTestDataV1_ErrorFrames[i].frame;
        const size_t length = metisTestDataV1_ErrorFrames[i].length;
        
        printf("Writing frame %d length %zu\n", i, length);
        
        const ssize_t nwritten = write(data->fd_sender, frame, length);
        assertTrue(nwritten == length, "Short write, expected %zu got %zd", length, nwritten);
        metisDispatcher_RunDuration(metisForwarder_GetDispatcher(data->metis), &((struct timeval) {0, 1000}));
    }
}

LONGBOW_TEST_CASE(Global, ether)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    
    _setupListener(data->metis, 0x0801, METIS_ENCAP_ETHER);
    
    // there's only 1 listener, so it's at index 0
    MetisListenerOps *listener = metisListenerSet_Get(metisForwarder_GetListenerSet(data->metis), 0);
    MetisGenericEther *ether = metisEtherListener_GetGenericEtherFromListener(listener);
    PARCBuffer *localAddressBuffer = metisGenericEther_GetMacAddress(ether);
    uint8_t *dmac = parcBuffer_Overlay(localAddressBuffer, 0);
    uint8_t smac[ETHER_ADDR_LEN] = { 1, 2, 3, 4, 5, 6 };
    
    // we're now ready to start receiving data.  We "send" data to the mock ethernet
    // by using the API to the mock GenericEther.
    
    for (int i = 0; metisTestDataV1_ErrorFrames[i].frame != NULL; i++) {
        const uint8_t *frame = metisTestDataV1_ErrorFrames[i].frame;
        const size_t length = metisTestDataV1_ErrorFrames[i].length;
        
        printf("Writing frame %d length %zu\n", i, length);
        
        PARCBuffer *buffer = mockGenericEther_createFrame(length, frame, dmac, smac, 0x0801);
        mockGenericEther_QueueFrame(ether, buffer);
        mockGenericEther_Notify(ether);
        parcBuffer_Release(&buffer);
        
        metisDispatcher_RunDuration(metisForwarder_GetDispatcher(data->metis), &((struct timeval) {0, 1000}));
    }    
}


// =================================================

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(sys_Errors);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
