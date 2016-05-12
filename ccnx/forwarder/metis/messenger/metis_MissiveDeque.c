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
 * A type-safe wrapper for Missives around a {@link PARCDeque}.  We only implement
 * the subset of functions used.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>
#include <LongBow/runtime.h>
#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Deque.h>

#include <ccnx/forwarder/metis/messenger/metis_Missive.h>
#include <ccnx/forwarder/metis/messenger/metis_MissiveDeque.h>

struct metis_missive_deque {
    PARCDeque *queue;
};

/**
 * Create a `PARCDeque` instance with the default element equals function.
 *
 * The queue is created with no elements.
 *
 * The default element equals function is used by the `parcDeque_Equals` function and
 * simply compares the values using the `==` operator.
 * Users that need more sophisticated comparisons of the elements need to supply their own
 * function via the `parcDeque_CreateCustom` function.
 *
 * @return non-NULL A pointer to a PARCDeque instance.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
MetisMissiveDeque *
metisMissiveDeque_Create(void)
{
    MetisMissiveDeque *missiveDeque = parcMemory_AllocateAndClear(sizeof(MetisMissiveDeque));
    assertNotNull(missiveDeque, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisMissiveDeque));
    missiveDeque->queue = parcDeque_Create();
    return missiveDeque;
}

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
void
metisMissiveDeque_Release(MetisMissiveDeque **dequePtr)
{
    assertNotNull(dequePtr, "Double pointer must be non-null");
    assertNotNull(*dequePtr, "Double pointer must dereference to non-null");
    MetisMissiveDeque *missiveDeque = *dequePtr;

    // flush the queue
    while (!parcDeque_IsEmpty(missiveDeque->queue)) {
        MetisMissive *missive = metisMissiveDeque_RemoveFirst(missiveDeque);
        metisMissive_Release(&missive);
    }

    parcDeque_Release(&missiveDeque->queue);
    parcMemory_Deallocate((void **) &missiveDeque);
    *dequePtr = NULL;
}

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
MetisMissiveDeque *
metisMissiveDeque_Append(MetisMissiveDeque *deque, MetisMissive *missive)
{
    assertNotNull(deque, "Parameter deque must be non-null");
    assertNotNull(missive, "Parameter missive must be non-null");

    parcDeque_Append(deque->queue, missive);
    return deque;
}

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
MetisMissive *
metisMissiveDeque_RemoveFirst(MetisMissiveDeque *deque)
{
    assertNotNull(deque, "Parameter deque must be non-null");
    return (MetisMissive *) parcDeque_RemoveFirst(deque->queue);
}

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 *
 * @see <#references#>
 */
size_t
metisMissiveDeque_Size(const MetisMissiveDeque *deque)
{
    assertNotNull(deque, "Parameter deque must be non-null");
    return parcDeque_Size(deque->queue);
}
