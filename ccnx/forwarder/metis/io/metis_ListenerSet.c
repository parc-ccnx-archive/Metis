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

#include <config.h>
#include <stdio.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_ArrayList.h>

#include <ccnx/forwarder/metis/io/metis_ListenerSet.h>

#include <LongBow/runtime.h>

struct metis_listener_set {
    PARCArrayList *listOfListeners;
};

static void
metisListenerSet_DestroyListenerOps(void **opsPtr)
{
    MetisListenerOps *ops = *((MetisListenerOps **) opsPtr);
    ops->destroy(&ops);
}

MetisListenerSet *
metisListenerSet_Create()
{
    MetisListenerSet *set = parcMemory_AllocateAndClear(sizeof(MetisListenerSet));
    assertNotNull(set, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisListenerSet));
    set->listOfListeners = parcArrayList_Create(metisListenerSet_DestroyListenerOps);

    return set;
}

void
metisListenerSet_Destroy(MetisListenerSet **setPtr)
{
    assertNotNull(setPtr, "Parameter must be non-null double pointer");
    assertNotNull(*setPtr, "Parameter must dereference to non-null pointer");

    MetisListenerSet *set = *setPtr;
    parcArrayList_Destroy(&set->listOfListeners);
    parcMemory_Deallocate((void **) &set);
    *setPtr = NULL;
}

/**
 * @function metisListenerSet_Add
 * @abstract Adds the listener to the set
 * @discussion
 *     Unique set based on pair (MetisEncapType, localAddress)
 *
 * @param <#param1#>
 * @return <#return#>
 */
bool
metisListenerSet_Add(MetisListenerSet *set, MetisListenerOps *ops)
{
    assertNotNull(set, "Parameter set must be non-null");
    assertNotNull(ops, "Parameter ops must be non-null");

    int opsEncap = ops->getEncapType(ops);
    const CPIAddress *opsAddress = ops->getListenAddress(ops);

    // make sure its not in the set
    size_t length = parcArrayList_Size(set->listOfListeners);
    for (size_t i = 0; i < length; i++) {
        MetisListenerOps *entry = parcArrayList_Get(set->listOfListeners, i);

        int entryEncap = entry->getEncapType(entry);
        const CPIAddress *entryAddress = entry->getListenAddress(entry);

        if (opsEncap == entryEncap && cpiAddress_Equals(opsAddress, entryAddress)) {
            // duplicate
            return false;
        }
    }

    parcArrayList_Add(set->listOfListeners, ops);
    return true;
}

size_t
metisListenerSet_Length(const MetisListenerSet *set)
{
    assertNotNull(set, "Parameter set must be non-null");
    return parcArrayList_Size(set->listOfListeners);
}

/**
 * Returns the listener at the given index
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] set An allocated listener set
 * @param [in] index The index position (0 <= index < metisListenerSet_Count)
 *
 * @retval non-null The listener at index
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisListenerOps *
metisListenerSet_Get(const MetisListenerSet *set, size_t index)
{
    assertNotNull(set, "Parameter set must be non-null");
    return parcArrayList_Get(set->listOfListeners, index);
}

MetisListenerOps *
metisListenerSet_Find(const MetisListenerSet *set, MetisEncapType encapType, const CPIAddress *localAddress)
{
    assertNotNull(set, "Parameter set must be non-null");
    assertNotNull(localAddress, "Parameter localAddress must be non-null");

    MetisListenerOps *match = NULL;

    for (size_t i = 0; i < parcArrayList_Size(set->listOfListeners) && !match; i++) {
        MetisListenerOps *ops = parcArrayList_Get(set->listOfListeners, i);
        assertNotNull(ops, "Got null listener ops at index %zu", i);

        if (ops->getEncapType(ops) == encapType) {
            if (cpiAddress_Equals(localAddress, ops->getListenAddress(ops))) {
                match = ops;
            }
        }
    }

    return match;
}
