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
/**
 * Currently uses an unsorted array of numbers.
 *
 * NB: Should implement over a sorted list to improve efficiency of set membership tests (case 816)
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>
#include <parc/algol/parc_Memory.h>
#include <ccnx/forwarder/metis/core/metis_NumberSet.h>
#include <parc/algol/parc_ArrayList.h>

#include <LongBow/runtime.h>

struct metis_number_set {
    MetisNumber *arrayOfNumbers;
    size_t length;
    size_t limit;
    unsigned refcount;
};

static void metisNumberSet_Expand(MetisNumberSet *set);

MetisNumberSet *
metisNumberSet_Create()
{
    MetisNumberSet *set = parcMemory_AllocateAndClear(sizeof(MetisNumberSet));
    assertNotNull(set, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisNumberSet));
    set->arrayOfNumbers = parcMemory_AllocateAndClear(sizeof(MetisNumber) * 16);
    assertNotNull((set->arrayOfNumbers), "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisNumber) * 16);
    set->length = 0;
    set->limit = 16;
    set->refcount = 1;
    return set;
}

MetisNumberSet *
metisNumberSet_Acquire(const MetisNumberSet *original)
{
    assertNotNull(original, "Parameter original must be non-null");
    MetisNumberSet *copy = (MetisNumberSet *) original;
    copy->refcount++;
    return copy;
}

void
metisNumberSet_Release(MetisNumberSet **setPtr)
{
    assertNotNull(setPtr, "Parameter must be non-null double pointer");
    assertNotNull(*setPtr, "Parameter must dereference to non-null pointer");

    MetisNumberSet *set = *setPtr;
    assertTrue(set->refcount > 0, "Invalid state: calling destroy on an object with 0 reference count");
    set->refcount--;

    if (set->refcount == 0) {
        parcMemory_Deallocate((void **) &(set->arrayOfNumbers));
        parcMemory_Deallocate((void **) &set);
        *setPtr = NULL;
    }
}

/**
 * @function metisNumberSet_AddNoChecks
 * @abstract Add a number we know is not already in the set
 * @discussion
 *   Used by other functions that already know the number is unique in the set,
 *   Does not do the expensive Contains check.
 *
 * @param <#param1#>
 */
static void
metisNumberSet_AddNoChecks(MetisNumberSet *set, MetisNumber number)
{
    if (set->length == set->limit) {
        metisNumberSet_Expand(set);
    }

    set->arrayOfNumbers[ set->length ] = number;
    set->length++;
}

bool
metisNumberSet_Add(MetisNumberSet *set, MetisNumber number)
{
    assertNotNull(set, "Parameter set must be non-null");
    if (metisNumberSet_Contains(set, number)) {
        return false;
    }

    metisNumberSet_AddNoChecks(set, number);
    return true;
}

size_t
metisNumberSet_Length(const MetisNumberSet *set)
{
    assertNotNull(set, "Parameter set must be non-null");
    return set->length;
}

MetisNumber
metisNumberSet_GetItem(const MetisNumberSet *set, size_t ordinalIndex)
{
    assertNotNull(set, "Parameter set must be non-null");
    assertTrue(ordinalIndex < set->length, "Limit beyond end of set, length %zu got %zu", set->length, ordinalIndex);

    return set->arrayOfNumbers[ordinalIndex];
}

bool
metisNumberSet_Contains(const MetisNumberSet *set, MetisNumber number)
{
    assertNotNull(set, "Parameter set must be non-null");
    for (size_t i = 0; i < set->length; i++) {
        if (set->arrayOfNumbers[i] == number) {
            return true;
        }
    }
    return false;
}

void
metisNumberSet_AddSet(MetisNumberSet *destinationSet, const MetisNumberSet *setToAdd)
{
    assertNotNull(destinationSet, "Parameter destinationSet must be non-null");
    assertNotNull(setToAdd, "Parameter setToAdd must be non-null");

    for (size_t i = 0; i < setToAdd->length; i++) {
        metisNumberSet_Add(destinationSet, setToAdd->arrayOfNumbers[i]);
    }
}

MetisNumberSet *
metisNumberSet_Subtract(const MetisNumberSet *minuend, const MetisNumberSet *subtrahend)
{
    // because the underlying ADT is not sorted, this is pretty ineffient, could be O(n^2).

    MetisNumberSet *difference = metisNumberSet_Create();

    for (size_t i = 0; i < minuend->length; i++) {
        bool unique = true;
        for (size_t j = 0; j < subtrahend->length && unique; j++) {
            if (minuend->arrayOfNumbers[i] == subtrahend->arrayOfNumbers[j]) {
                unique = false;
            }
        }

        if (unique) {
            metisNumberSet_AddNoChecks(difference, minuend->arrayOfNumbers[i]);
        }
    }
    return difference;
}

bool
metisNumberSet_Equals(const MetisNumberSet *a, const MetisNumberSet *b)
{
    if (a == NULL && b == NULL) {
        return true;
    }

    if (a == NULL || b == NULL) {
        return false;
    }

    if (a->length == b->length) {
        for (size_t i = 0; i < a->length; i++) {
            bool found = false;
            for (size_t j = 0; j < b->length && !found; j++) {
                if (a->arrayOfNumbers[i] == b->arrayOfNumbers[j]) {
                    found = true;
                }
            }
            if (!found) {
                return false;
            }
        }
        return true;
    }

    return false;
}

void
metisNumberSet_Remove(MetisNumberSet *set, MetisNumber number)
{
    assertNotNull(set, "Parameter set must be non-null");
    for (size_t i = 0; i < set->length; i++) {
        if (set->arrayOfNumbers[i] == number) {
            set->length--;
            if (set->length > 0) {
                // move the last element to the removed element to keep the array packed.
                set->arrayOfNumbers[i] = set->arrayOfNumbers[set->length];
            }
            return;
        }
    }
}

// =====================================================

static void
metisNumberSet_Expand(MetisNumberSet *set)
{
    size_t newlimit = set->limit * 2;
    size_t newbytes = newlimit * sizeof(MetisNumber);

    set->arrayOfNumbers = parcMemory_Reallocate(set->arrayOfNumbers, newbytes);
    set->limit = newlimit;
}
