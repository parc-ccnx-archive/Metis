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
#include "../metis_ConnectionTable.c"
#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>

#include "testrig_MetisIoOperations.h"

static struct test_set_s {
    unsigned localAddr;
    unsigned remoteAddr;
    unsigned id;
    MetisIoOperations *ops;
    MetisConnection *conn;
} test_set [] = {
    { .localAddr = 1,  .remoteAddr = 2, .id = 3,          .ops = NULL },
    { .localAddr = 2,  .remoteAddr = 1, .id = 4,          .ops = NULL },
    { .localAddr = 7,  .remoteAddr = 2, .id = 22,         .ops = NULL },
    { .localAddr = 13, .remoteAddr = 2, .id = 102332,     .ops = NULL },
    { .localAddr = 99, .remoteAddr = 2, .id = 99,         .ops = NULL },
    { .localAddr = 3,  .remoteAddr = 5, .id = 0xFFFFFFFF, .ops = NULL },
    { .localAddr = 0,  .remoteAddr = 0, .id = 0,          .ops = NULL }
};

// ============================================

LONGBOW_TEST_RUNNER(metis_ConnectionTable)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(CreateDestroy);
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metis_ConnectionTable)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metis_ConnectionTable)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// ============================================

LONGBOW_TEST_FIXTURE(CreateDestroy)
{
    LONGBOW_RUN_TEST_CASE(CreateDestroy, metisConnectionTable_Create_Destroy);
    LONGBOW_RUN_TEST_CASE(CreateDestroy, metisConnectionTable_Add);
}

LONGBOW_TEST_FIXTURE_SETUP(CreateDestroy)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(CreateDestroy)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(CreateDestroy, metisConnectionTable_Create_Destroy)
{
    MetisConnectionTable *table = metisConnectionTable_Create();
    metisConnectionTable_Destroy(&table);

    assertTrue(parcMemory_Outstanding() == 0, "Got memory imbalance on create/destroy: %u", parcMemory_Outstanding());
}

LONGBOW_TEST_CASE(CreateDestroy, metisConnectionTable_Add)
{
    MetisConnectionTable *table = metisConnectionTable_Create();
    MetisIoOperations *ops = mockIoOperationsData_CreateSimple(1, 2, 3, true, true, true);
    MockIoOperationsData *data = metisIoOperations_GetClosure(ops);

    MetisConnection *conn = metisConnection_Create(ops);

    assertTrue(parcHashCodeTable_Length(table->storageTableById) == 0,
               "storageTableById not empty at start of test, length %zu",
               parcHashCodeTable_Length(table->storageTableById));

    assertTrue(parcHashCodeTable_Length(table->indexByAddressPair) == 0,
               "indexByAddressPair not empty at start of test, length %zu",
               parcHashCodeTable_Length(table->indexByAddressPair));

    metisConnectionTable_Add(table, conn);

    assertTrue(parcHashCodeTable_Length(table->storageTableById) == 1,
               "Incorrect storage table size, expected %u got %zu",
               1,
               parcHashCodeTable_Length(table->storageTableById));

    assertTrue(parcHashCodeTable_Length(table->indexByAddressPair) == 1,
               "Incorrect storage table size, expected %u got %zu",
               1,
               parcHashCodeTable_Length(table->indexByAddressPair));
    metisConnectionTable_Destroy(&table);

    assertTrue(data->destroyCount == 1, "metisConnectionTable_Destroy did not call entry's destroyer");

    mockIoOperationsData_Destroy(&ops);
    assertTrue(parcMemory_Outstanding() == 0, "Got memory imbalance on create/destroy: %u", parcMemory_Outstanding());
}

// ============================================

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, metisConnectionTable_FindByAddressPair);
    LONGBOW_RUN_TEST_CASE(Global, metisConnectionTable_FindById);
    LONGBOW_RUN_TEST_CASE(Global, metisConnectionTable_Remove);
    LONGBOW_RUN_TEST_CASE(Global, metisConnectionTable_RemoveById);

//    LONGBOW_RUN_TEST_CASE(Global, metisConnectionTable_GetEntries);
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

LONGBOW_TEST_CASE(Global, metisConnectionTable_FindByAddressPair)
{
    MetisConnectionTable *table = metisConnectionTable_Create();

    // add the test set to the table
    for (int i = 0; test_set[i].localAddr != 0; i++) {
        test_set[i].ops = mockIoOperationsData_CreateSimple(test_set[i].localAddr, test_set[i].remoteAddr, test_set[i].id, true, true, true);
        assertNotNull(test_set[i].ops, "Got null from testdata_CreateSimple index %d", i);
        test_set[i].conn = metisConnection_Create(test_set[i].ops);
        assertNotNull(test_set[i].conn, "Got null from metisConnection_Create index %d", i);
        metisConnectionTable_Add(table, test_set[i].conn);
    }


    // now make sure we can find them all by their address pair
    for (int i = 0; test_set[i].localAddr != 0; i++) {
        const MetisAddressPair *pair = test_set[i].ops->getAddressPair(test_set[i].ops);
        const MetisConnection *conn = metisConnectionTable_FindByAddressPair(table, pair);
        assertTrue(conn == test_set[i].conn,
                   "id %u returned wrong pointer, expected %p got %p",
                   test_set[i].id,
                   (void *) test_set[i].conn,
                   (void *) conn);
    }

    // cleanup and verify destructions
    metisConnectionTable_Destroy(&table);

    for (int i = 0; test_set[i].localAddr != 0; i++) {
        MockIoOperationsData *data = metisIoOperations_GetClosure(test_set[i].ops);
        assertTrue(data->destroyCount == 1, "Did not destroy data element %d, count %u", i, data->destroyCount);
        mockIoOperationsData_Destroy(&test_set[i].ops);
    }
}

LONGBOW_TEST_CASE(Global, metisConnectionTable_FindById)
{
    MetisConnectionTable *table = metisConnectionTable_Create();

    // add the test set to the table
    for (int i = 0; test_set[i].localAddr != 0; i++) {
        test_set[i].ops = mockIoOperationsData_CreateSimple(test_set[i].localAddr, test_set[i].remoteAddr, test_set[i].id, true, true, true);
        assertNotNull(test_set[i].ops, "Got null from testdata_CreateSimple index %d", i);
        test_set[i].conn = metisConnection_Create(test_set[i].ops);
        assertNotNull(test_set[i].conn, "Got null from metisConnection_Create index %d", i);
        metisConnectionTable_Add(table, test_set[i].conn);
    }


    // now make sure we can find them all by their id
    for (int i = 0; test_set[i].localAddr != 0; i++) {
        const MetisConnection *conn = metisConnectionTable_FindById(table, test_set[i].id);
        assertTrue(conn == test_set[i].conn,
                   "id %u returned wrong pointer, expected %p got %p",
                   test_set[i].id,
                   (void *) test_set[i].conn,
                   (void *) conn);
    }

    // cleanup and verify destructions
    metisConnectionTable_Destroy(&table);

    for (int i = 0; test_set[i].localAddr != 0; i++) {
        MockIoOperationsData *data = metisIoOperations_GetClosure(test_set[i].ops);
        assertTrue(data->destroyCount == 1, "Did not destroy data element %d, count %u", i, data->destroyCount);
        mockIoOperationsData_Destroy(&test_set[i].ops);
    }
}

LONGBOW_TEST_CASE(Global, metisConnectionTable_Remove)
{
    MetisConnectionTable *table = metisConnectionTable_Create();

    MetisIoOperations *ops = mockIoOperationsData_CreateSimple(1, 2, 3, true, true, true);
    MetisConnection *conn = metisConnection_Create(ops);
    metisConnectionTable_Add(table, conn);


    // Check preconditions
    assertTrue(parcHashCodeTable_Length(table->storageTableById) == 1,
               "storageTableById wrong size, expected %u got %zu",
               1,
               parcHashCodeTable_Length(table->storageTableById));

    assertTrue(parcHashCodeTable_Length(table->indexByAddressPair) == 1,
               "indexByAddressPair wrong size, expected %u got %zu",
               1,
               parcHashCodeTable_Length(table->indexByAddressPair));

    // test the operation
    metisConnectionTable_Remove(table, conn);

    // check post conditions
    assertTrue(parcHashCodeTable_Length(table->storageTableById) == 0,
               "storageTableById wrong size, expected %u got %zu",
               0,
               parcHashCodeTable_Length(table->storageTableById));

    assertTrue(parcHashCodeTable_Length(table->indexByAddressPair) == 0,
               "indexByAddressPair wrong size, expected %u got %zu",
               0,
               parcHashCodeTable_Length(table->indexByAddressPair));

    // cleanup
    MockIoOperationsData *data = metisIoOperations_GetClosure(ops);
    assertTrue(data->destroyCount == 1, "Remove did not destroy data, count %u", data->destroyCount);
    mockIoOperationsData_Destroy(&ops);
    metisConnectionTable_Destroy(&table);
}

LONGBOW_TEST_CASE(Global, metisConnectionTable_RemoveById)
{
    MetisConnectionTable *table = metisConnectionTable_Create();

    unsigned connid = 3;

    MetisIoOperations *ops = mockIoOperationsData_CreateSimple(1, 2, connid, true, true, true);
    MetisConnection *conn = metisConnection_Create(ops);
    metisConnectionTable_Add(table, conn);

    // Check preconditions
    assertTrue(parcHashCodeTable_Length(table->storageTableById) == 1,
               "storageTableById wrong size, expected %u got %zu",
               1,
               parcHashCodeTable_Length(table->storageTableById));

    assertTrue(parcHashCodeTable_Length(table->indexByAddressPair) == 1,
               "indexByAddressPair wrong size, expected %u got %zu",
               1,
               parcHashCodeTable_Length(table->indexByAddressPair));

    // test the operation
    metisConnectionTable_RemoveById(table, connid);


    // check post conditions
    assertTrue(parcHashCodeTable_Length(table->storageTableById) == 0,
               "storageTableById wrong size, expected %u got %zu",
               0,
               parcHashCodeTable_Length(table->storageTableById));

    assertTrue(parcHashCodeTable_Length(table->indexByAddressPair) == 0,
               "indexByAddressPair wrong size, expected %u got %zu",
               0,
               parcHashCodeTable_Length(table->indexByAddressPair));

    // cleanup
    MockIoOperationsData *data = metisIoOperations_GetClosure(ops);
    assertTrue(data->destroyCount == 1, "Remove did not destroy data, count %u", data->destroyCount);
    mockIoOperationsData_Destroy(&ops);
    metisConnectionTable_Destroy(&table);
}

LONGBOW_TEST_CASE(Global, metisConnectionTable_GetEntries)
{
    MetisConnectionTable *table = metisConnectionTable_Create();

    size_t count = 0;
    // add the test set to the table
    for (int i = 0; test_set[i].localAddr != 0; i++) {
        test_set[i].ops = mockIoOperationsData_CreateSimple(test_set[i].localAddr, test_set[i].remoteAddr, test_set[i].id, true, true, true);
        assertNotNull(test_set[i].ops, "Got null from testdata_CreateSimple index %d", i);
        test_set[i].conn = metisConnection_Create(test_set[i].ops);
        assertNotNull(test_set[i].conn, "Got null from metisConnection_Create index %d", i);
        metisConnectionTable_Add(table, test_set[i].conn);
        count++;
    }

    MetisConnectionList *list = metisConnectionTable_GetEntries(table);
    assertTrue(metisConnectionList_Length(list) == count, "List wrong size, expected %zu got %zu", count, metisConnectionList_Length(list));

    // now verify each entry.  The entries are not necessarily in the same order
    for (int i = 0; i < count; i++) {
        MetisConnection *test = metisConnectionList_Get(list, i);
        const MetisAddressPair *test_pair = metisConnection_GetAddressPair(test);
        const MetisAddressPair *truth_pair = test_set[i].ops->getAddressPair(test_set[i].ops);
        assertTrue(metisAddressPair_Equals(test_pair, truth_pair), "Address pairs not equal, index %d", i);
    }

    metisConnectionList_Destroy(&list);
    for (int i = 0; test_set[i].localAddr != 0; i++) {
        MockIoOperationsData *data = metisIoOperations_GetClosure(test_set[i].ops);
        assertTrue(data->destroyCount == 1, "Did not destroy data element %d, count %u", i, data->destroyCount);
        mockIoOperationsData_Destroy(&test_set[i].ops);
    }
}

// ==============================================================================

LONGBOW_TEST_FIXTURE(Local)
{
    LONGBOW_RUN_TEST_CASE(Local, metisConnectionTable_AddressPairEquals_IsEqual);
    LONGBOW_RUN_TEST_CASE(Local, metisConnectionTable_AddressPairEquals_IsNotEqual);
    LONGBOW_RUN_TEST_CASE(Local, metisConnectionTable_AddressPairHashCode);
    LONGBOW_RUN_TEST_CASE(Local, metisConnectionTable_ConnectionDestroyer);
    LONGBOW_RUN_TEST_CASE(Local, metisConnectionTable_ConnectionIdDestroyer);
    LONGBOW_RUN_TEST_CASE(Local, metisConnectionTable_ConnectionIdEquals_IsEqual);
    LONGBOW_RUN_TEST_CASE(Local, metisConnectionTable_ConnectionIdEquals_IsNotEqual);
    LONGBOW_RUN_TEST_CASE(Local, metisConnectionTable_ConnectionIdHashCode);
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


LONGBOW_TEST_CASE(Local, metisConnectionTable_AddressPairEquals_IsEqual)
{
    CPIAddress *a1 = cpiAddress_CreateFromInterface(1);
    CPIAddress *a2 = cpiAddress_CreateFromInterface(2);
    MetisAddressPair *pair_a = metisAddressPair_Create(a1, a2);

    CPIAddress *b1 = cpiAddress_CreateFromInterface(1);
    CPIAddress *b2 = cpiAddress_CreateFromInterface(2);
    MetisAddressPair *pair_b = metisAddressPair_Create(b1, b2);

    bool success = metisConnectionTable_AddressPairEquals((void *) pair_a, (void *) pair_b);
    assertTrue(success, "Equal address pairs do not compare");

    metisAddressPair_Release(&pair_a);
    metisAddressPair_Release(&pair_b);
    cpiAddress_Destroy(&a1);
    cpiAddress_Destroy(&a2);
    cpiAddress_Destroy(&b1);
    cpiAddress_Destroy(&b2);
}

LONGBOW_TEST_CASE(Local, metisConnectionTable_AddressPairEquals_IsNotEqual)
{
    CPIAddress *a1 = cpiAddress_CreateFromInterface(1);
    CPIAddress *a2 = cpiAddress_CreateFromInterface(2);
    MetisAddressPair *pair_a = metisAddressPair_Create(a1, a2);

    CPIAddress *b1 = cpiAddress_CreateFromInterface(1);
    CPIAddress *b2 = cpiAddress_CreateFromInterface(2);
    MetisAddressPair *pair_b = metisAddressPair_Create(b2, b1);

    bool success = metisConnectionTable_AddressPairEquals((void *) pair_a, (void *) pair_b);
    assertFalse(success, "Unequal address pairs compare as equal");

    metisAddressPair_Release(&pair_a);
    metisAddressPair_Release(&pair_b);
    cpiAddress_Destroy(&a1);
    cpiAddress_Destroy(&a2);
    cpiAddress_Destroy(&b1);
    cpiAddress_Destroy(&b2);
}

LONGBOW_TEST_CASE(Local, metisConnectionTable_AddressPairHashCode)
{
    CPIAddress *a1 = cpiAddress_CreateFromInterface(1);
    CPIAddress *a2 = cpiAddress_CreateFromInterface(2);
    MetisAddressPair *pair_a = metisAddressPair_Create(a1, a2);

    HashCodeType truth = metisAddressPair_HashCode(pair_a);
    HashCodeType hash = metisConnectionTable_AddressPairHashCode((void *) pair_a);

    assertTrue(truth == hash, "Incorrect hash code, expected %04"PRIX64 "got %04"PRIX64, truth, hash);

    metisAddressPair_Release(&pair_a);
    cpiAddress_Destroy(&a1);
    cpiAddress_Destroy(&a2);
}

LONGBOW_TEST_CASE(Local, metisConnectionTable_ConnectionDestroyer)
{
    MetisIoOperations *ops = mockIoOperationsData_CreateSimple(1, 2, 3, true, true, true);
    MetisConnection *conn = metisConnection_Create(ops);
    MockIoOperationsData *data = metisIoOperations_GetClosure(ops);

    assertTrue(data->destroyCount == 0, "testdata_CreateSimple did not zero destroyCount");

    metisConnectionTable_ConnectionDestroyer((void **) &conn);

    assertTrue(data->destroyCount == 1, "metisConnectionTable_ConnectionDestroyer did not call destroy on MetisIoOperations");
    mockIoOperationsData_Destroy(&ops);

    assertTrue(parcMemory_Outstanding() == 0, "Memory imbalance, expected 0 got %u", parcMemory_Outstanding());
}

LONGBOW_TEST_CASE(Local, metisConnectionTable_ConnectionIdDestroyer)
{
    unsigned *aptr = parcMemory_Allocate(sizeof(unsigned));
    assertNotNull(aptr, "parcMemory_Allocate(%zu) returned NULL", sizeof(unsigned));
    metisConnectionTable_ConnectionIdDestroyer((void **) &aptr);
    assertNull(aptr, "destroyer did not null pointer");
    assertTrue(parcMemory_Outstanding() == 0, "Memory imbalance, expected 0 got %u", parcMemory_Outstanding());
}

LONGBOW_TEST_CASE(Local, metisConnectionTable_ConnectionIdEquals_IsEqual)
{
    unsigned a = 0x01020304;
    unsigned b = 0x01020304;

    bool success = metisConnectionTable_ConnectionIdEquals(&a, &b);
    assertTrue(success, "equal unsigned pointers do not compare");
}

LONGBOW_TEST_CASE(Local, metisConnectionTable_ConnectionIdEquals_IsNotEqual)
{
    unsigned a = 0x01020304;
    unsigned b = 0x01020305;

    bool success = metisConnectionTable_ConnectionIdEquals(&a, &b);
    assertFalse(success, "Unequal unsigned pointers compare as equal");
}

LONGBOW_TEST_CASE(Local, metisConnectionTable_ConnectionIdHashCode)
{
    unsigned a = 0x01020304;

    HashCodeType truth = parcHash32_Int32(a);
    HashCodeType hash = metisConnectionTable_ConnectionIdHashCode(&a);

    assertTrue(truth == hash, "Incorrect hash code, expected %04"PRIX64 "got %04"PRIX64, truth, hash);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metis_ConnectionTable);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
