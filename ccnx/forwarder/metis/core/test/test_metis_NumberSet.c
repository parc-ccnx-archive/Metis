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
#include "../metis_NumberSet.c"
#include <LongBow/unit-test.h>

#include <parc/algol/parc_SafeMemory.h>

LONGBOW_TEST_RUNNER(metis_NumberSet)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metis_NumberSet)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metis_NumberSet)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Append_NoExpand);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Append_Expand);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Append_Duplicate);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Contains);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Copy);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Create_Destroy);

    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Equals_IsEqual);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Equals_BothEmpty);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Equals_BothNull);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Equals_OneNull);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Equals_DifferentLengths);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Equals_IsNotEqual);

    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_GetItem);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Length);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Subtract_Disjoint);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Subtract_Equivalent);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Subtract_Overlap);

    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Remove_LastElement);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Remove_AllElements);
    LONGBOW_RUN_TEST_CASE(Global, metisNumberSet_Remove_FirstElement);
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

LONGBOW_TEST_CASE(Global, metisNumberSet_Append_NoExpand)
{
    MetisNumberSet *set = metisNumberSet_Create();

    for (int i = 1; i <= set->limit; i++) {
        bool result = metisNumberSet_Add(set, i);
        assertTrue(result, "Got failure on append, i = %d", i);
        assertTrue(set->length == i, "Set length wrong, expected %d got %zu", i, set->length);
    }

    metisNumberSet_Release(&set);
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Append_Expand)
{
    MetisNumberSet *set = metisNumberSet_Create();

    size_t limit = set->limit;
    for (int i = 1; i <= limit + 5; i++) {
        bool result = metisNumberSet_Add(set, i);
        assertTrue(result, "Got failure on append, i = %d", i);
        assertTrue(set->length == i, "Set length wrong, expected %d got %zu", i, set->length);
    }

    metisNumberSet_Release(&set);
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Append_Duplicate)
{
    MetisNumberSet *set = metisNumberSet_Create();

    for (int i = 1; i <= set->limit; i++) {
        bool result = metisNumberSet_Add(set, i);
        assertTrue(result, "Got failure on append, i = %d", i);
        assertTrue(set->length == i, "Set length wrong, expected %d got %zu", i, set->length);
    }

    for (int i = 1; i <= set->limit; i++) {
        bool result = metisNumberSet_Add(set, i);
        assertFalse(result, "Got success on duplicate append, i = %d", i);
        assertTrue(set->length == set->limit, "Set length wrong, expected %zu got %zu", set->limit, set->length);
    }

    metisNumberSet_Release(&set);
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Contains)
{
    MetisNumberSet *set = metisNumberSet_Create();

    int limit = 10;
    for (int i = 1; i <= limit; i++) {
        bool result = metisNumberSet_Add(set, i);
        assertTrue(result, "Got failure on append, i = %d", i);
        assertTrue(set->length == i, "Set length wrong, expected %d got %zu", i, set->length);
    }

    for (int i = 1; i <= limit; i++) {
        bool result = metisNumberSet_Contains(set, i);
        assertTrue(result, "Got missing member, i = %d", i);
    }

    for (int i = limit + 1; i <= 2 * limit; i++) {
        bool result = metisNumberSet_Contains(set, i);
        assertFalse(result, "Got contains returned true for missing element, i = %d", i);
    }

    metisNumberSet_Release(&set);
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Copy)
{
    MetisNumberSet *set = metisNumberSet_Create();

    int limit = 10;
    for (int i = 1; i <= limit; i++) {
        bool result = metisNumberSet_Add(set, i);
        assertTrue(result, "Got failure on append, i = %d", i);
        assertTrue(set->length == i, "Set length wrong, expected %d got %zu", i, set->length);
    }

    MetisNumberSet *copy = metisNumberSet_Acquire(set);
    assertTrue(set->refcount == 2, "Set refcount not 2: %u", set->refcount);

    metisNumberSet_Release(&copy);
    assertTrue(set->refcount == 1, "Set refcount not 1: %u", set->refcount);

    metisNumberSet_Release(&set);
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Create_Destroy)
{
    MetisNumberSet *set = metisNumberSet_Create();
    assertTrue(set->length == 0, "Set not 0 length on create: %zu", set->length);
    assertTrue(set->refcount == 1, "Set refcount not 1: %u", set->refcount);
    metisNumberSet_Release(&set);

    assertTrue(parcSafeMemory_ReportAllocation(STDOUT_FILENO) == 0, "Memory imbalance on create/destroy: %u", parcMemory_Outstanding());
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Equals_IsEqual)
{
    // 0 is the terminator
    unsigned a_set[] = { 1, 2, 3, 4, 5, 6, 7, 0 };
    unsigned b_set[] = { 1, 2, 3, 4, 5, 6, 7, 0 };

    MetisNumberSet *a = metisNumberSet_Create();
    MetisNumberSet *b = metisNumberSet_Create();

    for (int i = 0; a_set[i] != 0; i++) {
        metisNumberSet_Add(a, a_set[i]);
    }

    for (int i = 0; b_set[i] != 0; i++) {
        metisNumberSet_Add(b, b_set[i]);
    }

    bool equal = metisNumberSet_Equals(a, b);

    metisNumberSet_Release(&a);
    metisNumberSet_Release(&b);

    assertTrue(equal, "Equal sets did not compare as equal");
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Equals_BothEmpty)
{
    MetisNumberSet *a = metisNumberSet_Create();
    MetisNumberSet *b = metisNumberSet_Create();

    bool equal = metisNumberSet_Equals(a, b);

    metisNumberSet_Release(&a);
    metisNumberSet_Release(&b);

    assertTrue(equal, "Two empty sets did not compare as equal");
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Equals_BothNull)
{
    MetisNumberSet *a = NULL;
    MetisNumberSet *b = NULL;

    bool equal = metisNumberSet_Equals(a, b);

    assertTrue(equal, "Two NULL sets did not compare as equal");
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Equals_OneNull)
{
    MetisNumberSet *a = NULL;
    MetisNumberSet *b = metisNumberSet_Create();

    bool equal = metisNumberSet_Equals(a, b);

    assertFalse(equal, "One null one allocated sets did compared as equal");
    metisNumberSet_Release(&b);
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Equals_DifferentLengths)
{
    // 0 is the terminator
    unsigned a_set[] = { 1, 2, 3, 4, 5, 6, 7, 0 };
    unsigned b_set[] = { 1, 2, 3, 4, 5, 6, 0 };

    MetisNumberSet *a = metisNumberSet_Create();
    MetisNumberSet *b = metisNumberSet_Create();

    for (int i = 0; a_set[i] != 0; i++) {
        metisNumberSet_Add(a, a_set[i]);
    }

    for (int i = 0; b_set[i] != 0; i++) {
        metisNumberSet_Add(b, b_set[i]);
    }

    bool equal = metisNumberSet_Equals(a, b);

    metisNumberSet_Release(&a);
    metisNumberSet_Release(&b);

    assertFalse(equal, "Sets of different lengths compared as equal");
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Equals_IsNotEqual)
{
    // 0 is the terminator
    unsigned a_set[] = { 1, 2, 3, 4, 5, 6, 7, 0 };
    unsigned b_set[] = { 1, 2, 3, 4, 5, 6, 8, 0 };

    MetisNumberSet *a = metisNumberSet_Create();
    MetisNumberSet *b = metisNumberSet_Create();

    for (int i = 0; a_set[i] != 0; i++) {
        metisNumberSet_Add(a, a_set[i]);
    }

    for (int i = 0; b_set[i] != 0; i++) {
        metisNumberSet_Add(b, b_set[i]);
    }

    bool equal = metisNumberSet_Equals(a, b);

    metisNumberSet_Release(&a);
    metisNumberSet_Release(&b);

    assertFalse(equal, "Same length but unequal sets compared as equal");
}

LONGBOW_TEST_CASE(Global, metisNumberSet_GetItem)
{
    MetisNumberSet *set = metisNumberSet_Create();

    int limit = 10;
    for (int i = 1; i <= limit; i++) {
        bool result = metisNumberSet_Add(set, i);
        assertTrue(result, "Got failure on append, i = %d", i);
        assertTrue(set->length == i, "Set length wrong, expected %d got %zu", i, set->length);
    }

    for (int i = 0; i < limit; i++) {
        MetisNumber n = metisNumberSet_GetItem(set, i);
        assertTrue(n == i + 1, "Got wrong number, i = %d, n = %u", i, n);
    }

    metisNumberSet_Release(&set);
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Length)
{
    MetisNumberSet *set = metisNumberSet_Create();

    int limit = 10;
    for (int i = 1; i <= limit; i++) {
        bool result = metisNumberSet_Add(set, i);
        assertTrue(result, "Got failure on append, i = %d", i);
        assertTrue(metisNumberSet_Length(set) == i, "Set length wrong, expected %d got %zu", i, metisNumberSet_Length(set));
    }

    metisNumberSet_Release(&set);
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Subtract_Disjoint)
{
    // 0 is the terminator
    unsigned a_set[] = { 1, 2, 3, 4, 5, 6, 7, 0 };
    unsigned b_set[] = { 11, 12, 13, 14, 15, 0 };
    unsigned truth_set[] = { 1, 2, 3, 4, 5, 6, 7, 0 };

    MetisNumberSet *a = metisNumberSet_Create();
    MetisNumberSet *b = metisNumberSet_Create();
    MetisNumberSet *truth = metisNumberSet_Create();

    for (int i = 0; a_set[i] != 0; i++) {
        metisNumberSet_Add(a, a_set[i]);
    }

    for (int i = 0; b_set[i] != 0; i++) {
        metisNumberSet_Add(b, b_set[i]);
    }

    for (int i = 0; truth_set[i] != 0; i++) {
        metisNumberSet_Add(truth, truth_set[i]);
    }

    MetisNumberSet *test = metisNumberSet_Subtract(a, b);

    bool equal = metisNumberSet_Equals(truth, test);

    metisNumberSet_Release(&a);
    metisNumberSet_Release(&b);
    metisNumberSet_Release(&truth);
    metisNumberSet_Release(&test);

    assertTrue(equal, "subtraction result incorrect for disjoint sets");
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Subtract_Equivalent)
{
    // 0 is the terminator
    unsigned a_set[] = { 1, 2, 3, 4, 5, 6, 7, 0 };
    unsigned b_set[] = { 1, 2, 3, 4, 5, 6, 7, 0 };
    unsigned truth_set[] = { 0 };

    MetisNumberSet *a = metisNumberSet_Create();
    MetisNumberSet *b = metisNumberSet_Create();
    MetisNumberSet *truth = metisNumberSet_Create();

    for (int i = 0; a_set[i] != 0; i++) {
        metisNumberSet_Add(a, a_set[i]);
    }

    for (int i = 0; b_set[i] != 0; i++) {
        metisNumberSet_Add(b, b_set[i]);
    }

    for (int i = 0; truth_set[i] != 0; i++) {
        metisNumberSet_Add(truth, truth_set[i]);
    }

    MetisNumberSet *test = metisNumberSet_Subtract(a, b);

    bool equal = metisNumberSet_Equals(truth, test);

    metisNumberSet_Release(&a);
    metisNumberSet_Release(&b);
    metisNumberSet_Release(&truth);
    metisNumberSet_Release(&test);

    assertTrue(equal, "subtraction result incorrect for disjoint sets");
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Subtract_Overlap)
{
    // 0 is the terminator
    unsigned a_set[] = { 1, 2, 3, 4, 5, 6, 7, 0 };
    unsigned b_set[] = { 1, 2, 3, 4, 5, 0 };
    unsigned truth_set[] = { 6, 7, 0 };

    MetisNumberSet *a = metisNumberSet_Create();
    MetisNumberSet *b = metisNumberSet_Create();
    MetisNumberSet *truth = metisNumberSet_Create();

    for (int i = 0; a_set[i] != 0; i++) {
        metisNumberSet_Add(a, a_set[i]);
    }

    for (int i = 0; b_set[i] != 0; i++) {
        metisNumberSet_Add(b, b_set[i]);
    }

    for (int i = 0; truth_set[i] != 0; i++) {
        metisNumberSet_Add(truth, truth_set[i]);
    }

    MetisNumberSet *test = metisNumberSet_Subtract(a, b);

    bool equal = metisNumberSet_Equals(truth, test);

    metisNumberSet_Release(&a);
    metisNumberSet_Release(&b);
    metisNumberSet_Release(&truth);
    metisNumberSet_Release(&test);

    assertTrue(equal, "subtraction result incorrect for disjoint sets");
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Remove_LastElement)
{
    // 0 is the terminator
    unsigned a_set[] = { 1, 2, 3, 4, 5, 6, 7, 0 };
    unsigned to_remove = 7;
    unsigned truth_set[] = { 1, 2, 3, 4, 5, 6, 0 };

    MetisNumberSet *a = metisNumberSet_Create();
    MetisNumberSet *truth = metisNumberSet_Create();

    for (int i = 0; a_set[i] != 0; i++) {
        metisNumberSet_Add(a, a_set[i]);
    }

    for (int i = 0; truth_set[i] != 0; i++) {
        metisNumberSet_Add(truth, truth_set[i]);
    }

    metisNumberSet_Remove(a, to_remove);

    bool equal = metisNumberSet_Equals(truth, a);

    metisNumberSet_Release(&a);
    metisNumberSet_Release(&truth);

    assertTrue(equal, "Removing element gives incorrect set");
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Remove_AllElements)
{
    // 0 is the terminator
    unsigned a_set[] = { 1, 0 };
    unsigned to_remove = 1;
    unsigned truth_set[] = { 0 };

    MetisNumberSet *a = metisNumberSet_Create();
    MetisNumberSet *truth = metisNumberSet_Create();

    for (int i = 0; a_set[i] != 0; i++) {
        metisNumberSet_Add(a, a_set[i]);
    }

    for (int i = 0; truth_set[i] != 0; i++) {
        metisNumberSet_Add(truth, truth_set[i]);
    }

    metisNumberSet_Remove(a, to_remove);

    bool equal = metisNumberSet_Equals(truth, a);

    metisNumberSet_Release(&a);
    metisNumberSet_Release(&truth);

    assertTrue(equal, "Removing element gives incorrect set");
}

LONGBOW_TEST_CASE(Global, metisNumberSet_Remove_FirstElement)
{
    // 0 is the terminator
    unsigned a_set[] = { 1, 2, 3, 4, 5, 6, 7, 0 };
    unsigned to_remove = 1;
    unsigned truth_set[] = { 2, 3, 4, 5, 6, 7, 0 };

    MetisNumberSet *a = metisNumberSet_Create();
    MetisNumberSet *truth = metisNumberSet_Create();

    for (int i = 0; a_set[i] != 0; i++) {
        metisNumberSet_Add(a, a_set[i]);
    }

    for (int i = 0; truth_set[i] != 0; i++) {
        metisNumberSet_Add(truth, truth_set[i]);
    }

    metisNumberSet_Remove(a, to_remove);

    bool equal = metisNumberSet_Equals(truth, a);

    metisNumberSet_Release(&a);
    metisNumberSet_Release(&truth);

    assertTrue(equal, "Removing element gives incorrect set");
}

// ======================================================================================

LONGBOW_TEST_FIXTURE(Local)
{
    LONGBOW_RUN_TEST_CASE(Local, metisNumberSet_Expand);
    LONGBOW_RUN_TEST_CASE(Local, metisNumberSet_AddNoChecks);
}

LONGBOW_TEST_FIXTURE_SETUP(Local)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Local)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Local, metisNumberSet_Expand)
{
    testUnimplemented("This test is unimplemented");
}

LONGBOW_TEST_CASE(Local, metisNumberSet_AddNoChecks)
{
    testUnimplemented("This test is unimplemented");
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metis_NumberSet);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
