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
#include "../metis_TlvNameCodec.c"
#include <parc/algol/parc_SafeMemory.h>
#include <LongBow/unit-test.h>

LONGBOW_TEST_RUNNER(tlv_NameCodec)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(tlv_NameCodec)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(tlv_NameCodec)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, tlvName_Decode_0_Length_Name);
    LONGBOW_RUN_TEST_CASE(Global, tlvName_Decode_0_Length_Segment);
    LONGBOW_RUN_TEST_CASE(Global, tlvName_Decode_Good);
    LONGBOW_RUN_TEST_CASE(Global, tlvName_Decode_Overflow);
    LONGBOW_RUN_TEST_CASE(Global, tlvName_Decode_UnderRun);
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

/**
 * Buffer is 1 .. 3 bytes
 */
LONGBOW_TEST_CASE_EXPECTS(Global, tlvName_Decode_UnderRun, .event = &LongBowTrapIllegalValue)
{
    // offset               0     1     2     3     4     5     6     7     8   9   10   11   12     13
    //                        |-- type --|-- length --||
    uint8_t buffer[] = { 0xFF, 0x00, 0x00, 0x00, 0x04, 0xFF };

    // This will assert
    // CCNxName *name =
    metisTlvNameCodec_Decode(buffer, 5, 6);
}

/**
 * Buffer exactly 0 bytes
 */
LONGBOW_TEST_CASE(Global, tlvName_Decode_0_Length_Name)
{
    // offset               0     1     2     3     4     5     6     7     8   9   10   11   12     13
    //                        |-- type --|-- length --||
    uint8_t buffer[] = { 0xFF, 0x00, 0x00, 0x00, 0x04, 0xFF };

    // skip the two 0xFF bytes
    // name = "lci:/%02=abcd"
    CCNxName *test = metisTlvNameCodec_Decode(buffer, 5, 5);
    CCNxName *truth = ccnxName_Create();
    char *nameString = ccnxName_ToString(test);

    assertTrue(ccnxName_Equals(truth, test), "Names not equal, got %s", nameString);

    parcMemory_Deallocate((void **) &nameString);
    ccnxName_Release(&truth);
    ccnxName_Release(&test);
}

/**
 * Buffer exactly 4 bytes
 */
LONGBOW_TEST_CASE(Global, tlvName_Decode_0_Length_Segment)
{
    // offset               0     1     2     3     4     5     6     7     8   9   10   11   12     13
    //                        |-- type --|-- length --|-- type --|-- length --||
    uint8_t buffer[] = { 0xFF, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00, 0x00, 0xFF };

    // skip the two 0xFF bytes
    // name = "lci:/%02=abcd"
    CCNxName *test = metisTlvNameCodec_Decode(buffer, 5, 9);
//    CCNxName *truth = ccnxName_CreateFromCString("lci:/%02=");
    CCNxName *truth = ccnxName_CreateFromCString("lci:/2=");
    char *nameString = ccnxName_ToString(test);

    assertTrue(ccnxName_Equals(truth, test), "Names not equal, got %s", nameString);

    parcMemory_Deallocate((void **) &nameString);
    ccnxName_Release(&truth);
    ccnxName_Release(&test);
}

/**
 * A good, normal name
 */
LONGBOW_TEST_CASE(Global, tlvName_Decode_Good)
{
    // offset       0     1     2     3     4     5     6     7     8   9   10   11   12     13
    //                |-- type --|-- length --|-- type --|-- length --| ----- value -----|
    uint8_t buffer[] = { 0xFF, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, 0x00, 0x04, 'a', 'b', 'c', 'd', 0xFF };

    // skip the two 0xFF bytes
    // name = "lci:/%02=abcd"
    CCNxName *test = metisTlvNameCodec_Decode(buffer, 5, 13);

//    CCNxName *truth = ccnxName_CreateFromCString("lci:/%02=abcd");
    CCNxName *truth = ccnxName_CreateFromCString("lci:/2=abcd");
    char *nameString = ccnxName_ToString(test);

    assertTrue(ccnxName_Equals(truth, test), "Names not equal, got %s", nameString);

    parcMemory_Deallocate((void **) &nameString);
    ccnxName_Release(&truth);
    ccnxName_Release(&test);
}

/**
 * The name component length shoots beyond the end of the buffer.  Byte 8 is "5" instead of "4".
 */
LONGBOW_TEST_CASE_EXPECTS(Global, tlvName_Decode_Overflow, .event = &LongBowTrapIllegalValue)
{
    // offset               0     1     2     3     4     5     6     7     8   9   10   11   12     13
    //                        |-- type --|-- length --|-- type --|-- length --| ----- value -----|
    uint8_t buffer[] = { 0xFF, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, 0x00, 0x05, 'a', 'b', 'c', 'd', 0xFF };

    // This will trap because the length 5 will go beyond 12
    metisTlvNameCodec_Decode(buffer, 5, 13);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(tlv_NameCodec);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
