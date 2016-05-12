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
#include <ccnx/forwarder/metis/processor/metis_PitEntry.h>
#include <ccnx/forwarder/metis/core/metis_NumberSet.h>

#include <LongBow/runtime.h>

struct metis_pit_entry {
    MetisMessage *message;
    MetisNumberSet *ingressIdSet;
    MetisNumberSet *egressIdSet;

    MetisTicks expiryTime;

    unsigned refcount;
};

MetisPitEntry *
metisPitEntry_Create(MetisMessage *message, MetisTicks expiryTime)
{
    MetisPitEntry *pitEntry = parcMemory_AllocateAndClear(sizeof(MetisPitEntry));
    assertNotNull(pitEntry, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisPitEntry));
    pitEntry->message = message;
    pitEntry->ingressIdSet = metisNumberSet_Create();
    pitEntry->egressIdSet = metisNumberSet_Create();
    pitEntry->refcount = 1;

    // add the message to the reverse path set
    metisNumberSet_Add(pitEntry->ingressIdSet, metisMessage_GetIngressConnectionId(message));

    // hack in a 4-second timeout
    pitEntry->expiryTime = expiryTime;
    return pitEntry;
}

void
metisPitEntry_Release(MetisPitEntry **pitEntryPtr)
{
    assertNotNull(pitEntryPtr, "Parameter must be non-null double pointer");
    assertNotNull(*pitEntryPtr, "Parameter must dereference to non-null pointer");

    MetisPitEntry *pitEntry = *pitEntryPtr;
    trapIllegalValueIf(pitEntry->refcount == 0, "Illegal state: has refcount of 0");

    pitEntry->refcount--;
    if (pitEntry->refcount == 0) {
        metisNumberSet_Release(&pitEntry->ingressIdSet);
        metisNumberSet_Release(&pitEntry->egressIdSet);
        metisMessage_Release(&pitEntry->message);
        parcMemory_Deallocate((void **) &pitEntry);
    }
    *pitEntryPtr = NULL;
}

MetisPitEntry *
metisPitEntry_Acquire(MetisPitEntry *original)
{
    assertNotNull(original, "Parameter original must be non-null");
    original->refcount++;
    return original;
}

void
metisPitEntry_AddIngressId(MetisPitEntry *pitEntry, unsigned ingressId)
{
    assertNotNull(pitEntry, "Parameter pitEntry must be non-null");
    metisNumberSet_Add(pitEntry->ingressIdSet, ingressId);
}

void
metisPitEntry_AddEgressId(MetisPitEntry *pitEntry, unsigned egressId)
{
    assertNotNull(pitEntry, "Parameter pitEntry must be non-null");
    metisNumberSet_Add(pitEntry->egressIdSet, egressId);
}

MetisTicks
metisPitEntry_GetExpiryTime(const MetisPitEntry *pitEntry)
{
    assertNotNull(pitEntry, "Parameter pitEntry must be non-null");
    return pitEntry->expiryTime;
}

void
metisPitEntry_SetExpiryTime(MetisPitEntry *pitEntry, MetisTicks expiryTime)
{
    assertNotNull(pitEntry, "Parameter pitEntry must be non-null");
    pitEntry->expiryTime = expiryTime;
}


const MetisNumberSet *
metisPitEntry_GetIngressSet(const MetisPitEntry *pitEntry)
{
    assertNotNull(pitEntry, "Parameter pitEntry must be non-null");
    return pitEntry->ingressIdSet;
}

const MetisNumberSet *
metisPitEntry_GetEgressSet(const MetisPitEntry *pitEntry)
{
    assertNotNull(pitEntry, "Parameter pitEntry must be non-null");
    return pitEntry->egressIdSet;
}

MetisMessage *
metisPitEntry_GetMessage(const MetisPitEntry *pitEntry)
{
    assertNotNull(pitEntry, "Parameter pitEntry must be non-null");
    return metisMessage_Acquire(pitEntry->message);
}
