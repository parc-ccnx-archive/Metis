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

#include <config.h>

#include "../metis_LruList.c"
#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>

LONGBOW_TEST_RUNNER(metis_LruList)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metis_LruList)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metis_LruList)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// ============================================================================

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, metisLruList_Create_Destroy);

    LONGBOW_RUN_TEST_CASE(Global, MetisLruListEntry_Destroy);
    LONGBOW_RUN_TEST_CASE(Global, metisLruEntry_GetData);
    LONGBOW_RUN_TEST_CASE(Global, metisLruEntry_MoveToHead);

    LONGBOW_RUN_TEST_CASE(Global, metisLruList_NewHeadEntry);
    LONGBOW_RUN_TEST_CASE(Global, metisLruList_PopTail);
    LONGBOW_RUN_TEST_CASE(Global, MetisLruList_Length);
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

LONGBOW_TEST_CASE(Global, MetisLruListEntry_Destroy)
{
    MetisLruList *lru = metisLruList_Create();

    size_t beforeMemory = parcMemory_Outstanding();
    MetisLruListEntry *entry = (MetisLruListEntry *) metisLruList_NewHeadEntry(lru, (void *) 0x02);
    metisLruList_PopTail(lru);
    metisLruList_EntryDestroy(&entry);
    size_t afterMemory = parcMemory_Outstanding();

    metisLruList_Destroy(&lru);

    assertTrue(afterMemory == beforeMemory, "Memory imbalance for LruEntry_Destroy, expected %zu got %zu", beforeMemory, afterMemory);
}

LONGBOW_TEST_CASE(Global, metisLruEntry_GetData)
{
    void *data = (void *) 99;
    MetisLruList *lru = metisLruList_Create();
    MetisLruListEntry *entry = (MetisLruListEntry *) metisLruList_NewHeadEntry(lru, data);
    void *p = metisLruList_EntryGetData(entry);

    assertTrue(p == data, "Data did not match, expected %p, got %p", data, p);
    metisLruList_Destroy(&lru);
}

LONGBOW_TEST_CASE(Global, metisLruEntry_MoveToHead)
{
    int loops = 10;

    MetisLruList *lru = metisLruList_Create();

    for (size_t i = 1; i <= loops; i++) {
        metisLruList_NewHeadEntry(lru, (void *) i);
    }

    MetisLruListEntry *tail = metisLruList_PopTail(lru);
    metisLruList_EntryMoveToHead(tail);

    MetisLruListEntry *test = TAILQ_FIRST(&lru->head);
    assertTrue(test == tail, "Head element not moved, expected %p got %p", (void *) tail, (void *) test);

    metisLruList_Destroy(&lru);
}

LONGBOW_TEST_CASE(Global, metisLruList_Create_Destroy)
{
    size_t baselineMemory = parcMemory_Outstanding();

    MetisLruList *lru = metisLruList_Create();
    metisLruList_Destroy(&lru);

    assertTrue(parcMemory_Outstanding() == baselineMemory, "Memory imbalance on create/destroy: %u", parcMemory_Outstanding());
}

LONGBOW_TEST_CASE(Global, metisLruList_Length)
{
    int loops = 10;

    MetisLruList *lru = metisLruList_Create();

    for (size_t i = 1; i <= loops; i++) {
        MetisLruListEntry *entry = (MetisLruListEntry *) metisLruList_NewHeadEntry(lru, (void *) i);
        assertTrue(lru->itemsInList == i, "Incorrect list length element %zu, expected %zu got %zu", i, i, lru->itemsInList);
        assertTrue(metisLruList_Length(lru) == i, "Incorrect length encountered");

        MetisLruListEntry *test = TAILQ_FIRST(&lru->head);
        assertTrue(test == entry, "Head element not new entry, expected %p got %p", (void *) entry, (void *) test);
    }
    metisLruList_Destroy(&lru);
}

LONGBOW_TEST_CASE(Global, metisLruList_NewHeadEntry)
{
    int loops = 10;

    MetisLruList *lru = metisLruList_Create();

    for (size_t i = 1; i <= loops; i++) {
        MetisLruListEntry *entry = (MetisLruListEntry *) metisLruList_NewHeadEntry(lru, (void *) i);
        assertTrue(lru->itemsInList == i, "Incorrect list length element %zu, expected %zu got %zu", i, i, lru->itemsInList);

        MetisLruListEntry *test = TAILQ_FIRST(&lru->head);
        assertTrue(test == entry, "Head element not new entry, expected %p got %p", (void *) entry, (void *) test);
    }
    metisLruList_Destroy(&lru);
}

LONGBOW_TEST_CASE(Global, metisLruList_PopTail)
{
    int loops = 10;

    MetisLruList *lru = metisLruList_Create();

    for (size_t i = 1; i <= loops; i++) {
        metisLruList_NewHeadEntry(lru, (void *) i);
    }

    for (size_t i = 1; i <= loops; i++) {
        MetisLruListEntry *entry = metisLruList_PopTail(lru);
        void *data = metisLruList_EntryGetData(entry);
        assertTrue(data == (void *) i, "Got wrong data, expected %p got %p", (void *) i, (void *) data);

        metisLruList_EntryDestroy(&entry);
    }

    metisLruList_Destroy(&lru);
}


LONGBOW_TEST_CASE(Global, MetisLruList_Length)
{
    int loops = 10;

    MetisLruList *lru = metisLruList_Create();

    for (size_t i = 1; i <= loops; i++) {
        metisLruList_NewHeadEntry(lru, (void *) i);
        assertTrue(metisLruList_Length(lru) == i, "Unexpected LruList length");
    }

    metisLruList_Destroy(&lru);
}

// ============================================================================

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metis_LruList);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
