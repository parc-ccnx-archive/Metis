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
 * @file metis_PitEntry.h
 * @brief The embodiment of a PIT entry
 *
 * Embodies a PIT entry
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_PitEntry_h
#define Metis_metis_PitEntry_h

#include <ccnx/forwarder/metis/core/metis_Ticks.h>
#include <ccnx/forwarder/metis/core/metis_Message.h>
#include <ccnx/forwarder/metis/core/metis_NumberSet.h>

struct metis_pit_entry;
typedef struct metis_pit_entry MetisPitEntry;

/**
 * @function metisPitEntry_Create
 * @abstract Takes ownership of the message inside the PitEntry
 * @discussion
 *   When the PIT entry is destroyed, will call <code>metisMessage_Release()</code> on the message.
 *
 * @param <#param1#>
 * @return <#return#>
 */
MetisPitEntry *metisPitEntry_Create(MetisMessage *message, MetisTicks expiryTime);

/**
 * Release a previously acquired reference to the specified instance,
 * decrementing the reference count for the instance.
 *
 * The pointer to the instance is set to NULL as a side-effect of this function.
 *
 * If the invocation causes the last reference to the instance to be released,
 * the instance is deallocated and the instance's implementation will perform
 * additional cleanup and release other privately held references.
 *
 * @param [in,out] pitEntryPtr A pointer to a MetisPitEntry instance pointer, which will be set to zero on return.
 *
 * Example:
 * @code
 * {
 * }
 * @endcode
 */
void metisPitEntry_Release(MetisPitEntry **pitEntryPtr);

/**
 * @function metisPitEntry_Acquire
 * @abstract Returns a reference counted copy
 * @discussion
 *   A reference counted copy that shares the same state as the original.
 *   Caller must use <code>metisPitEntry_Release()</code> on it when done.
 *
 * @return A reference counted copy, use Destroy on it.
 */
MetisPitEntry *metisPitEntry_Acquire(MetisPitEntry *original);

/**
 * @function metisPitEntry_AddIngressId
 * @abstract Add an ingress connection id to the list of reverse paths
 * @discussion
 *   A PitEntry has two NumberSets.  The first is the set of ingress ports, which
 *   make up the reverse path.  The second is the set of egress ports, which make up
 *   its forward path.
 *
 *   This function tracks which reverse paths have sent us the interest.
 *
 * @param ingressId the reverse path
 */
void  metisPitEntry_AddIngressId(MetisPitEntry *pitEntry, unsigned ingressId);

/**
 * @function metisPitEntry_AddEgressId
 * @abstract Add an egress connection id to the list of attempted paths
 * @discussion
 *   A PitEntry has two NumberSets.  The first is the set of ingress ports, which
 *   make up the reverse path.  The second is the set of egress ports, which make up
 *   its forward path.
 *
 *   This function tracks which forward paths we've tried for the interest.
 *
 * @param egressId the forwarded path
 */
void  metisPitEntry_AddEgressId(MetisPitEntry *pitEntry, unsigned egressId);

/**
 * @function metisPitEntry_GetIngressSet
 * @abstract The Ingress connection id set
 * @discussion
 *   You must acquire a copy of the number set if you will store the result.  This is
 *   the internal reference.
 *
 * @param <#param1#>
 * @return May be empty, will not be null.  Must be destroyed.
 */
const MetisNumberSet *metisPitEntry_GetIngressSet(const MetisPitEntry *pitEntry);

/**
 * @function metisPitEntry_GetEgressSet
 * @abstract The Egress connection id set
 * @discussion
 *   You must acquire a copy of the number set if you will store the result.  This is
 *   the internal reference.
 *
 * @param <#param1#>
 * @return May be empty, will not be null.  Must be destroyed.
 */
const MetisNumberSet *metisPitEntry_GetEgressSet(const MetisPitEntry *pitEntry);

/**
 * @function metisPitEntry_GetMessage
 * @abstract Gets the interest underpinning the PIT entry
 * @discussion
 *   A reference counted copy, call <code>MetisMessage_Release()</code> on it.
 *
 * @param <#param1#>
 * @return A reference counted copy, call <code>MetisMessage_Release()</code> on it.
 */
MetisMessage *metisPitEntry_GetMessage(const MetisPitEntry *pitEntry);

/**
 * Returns the time (in ticks) at which the PIT entry is no longer valid
 *
 * The ExpiryTime is computed when the PIT entry is added (or via metisPitEntry_SetExpiryTime).
 * It is the aboslute time (in Ticks) at which the Pit entry is no longer valid.
 *
 * @param [in] MetisPitEntry An allocated PIT entry
 *
 * @retval number The abosolute time (in Ticks) of the Expiry
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisTicks metisPitEntry_GetExpiryTime(const MetisPitEntry *pitEntry);

/**
 * Sets the ExpriyTime of the PIT entry to the given value
 *
 * It is probalby an error to set the expiryTime to a smaller value than currently set to, but
 * this is not enforced.  PIT entries use lazy delete.
 *
 * @param [in] pitEntry The allocated PIT entry to modify
 * @param [in] expiryTime The new expiryTime (UTC in forwarder Ticks)
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisPitEntry_SetExpiryTime(MetisPitEntry *pitEntry, MetisTicks expiryTime);

#endif // Metis_metis_PitEntry_h
