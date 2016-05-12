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
 * The Forwarding Information Base (FIB) table is a map from a name to a MetisFibEntry.
 *
 * Each MetisFibEntry has a set of nexthops and a MetisStrategy to pick a nexthop.
 *
 * The strategy may be changed.  It will wipe out all the previous state for the last
 * strategy and the new strategy will need to start from scratch.  changing the strategy does
 * not change the nexthops, but it does wipe any stragegy-specific state in each nexthop.
 *
 * So, the FIB table is make up of rows like this:
 *   name -> { strategy, { {nexthop_1, strategyState_1}, {nexthop_2, strategyState_2}, ... } }
 *
 * The "strategy" is a MetisStrategyImpl function structure (see strategies/metis_Strategy.h).
 * Some strategies might allocate an implementation per row, others might use one implementation
 * for the whole table.  Its up to the strategy implementation.
 *
 * NB Right now, there's no strategy in the FIB, its just a list of nexthops (case 192)
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_FIB_h
#define Metis_metis_FIB_h

#include <ccnx/common/ccnx_Name.h>
#include <ccnx/api/control/cpi_RouteEntry.h>

#include <ccnx/forwarder/metis/core/metis_NumberSet.h>
#include <ccnx/forwarder/metis/core/metis_Message.h>
#include <ccnx/forwarder/metis/processor/metis_FibEntryList.h>
#include <ccnx/forwarder/metis/core/metis_Logger.h>

struct metis_fib;
typedef struct metis_fib MetisFIB;

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
 */
MetisFIB *metisFIB_Create(MetisLogger *logger);

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
 */
void metisFIB_Destroy(MetisFIB **fibPtr);

/**
 * @function metisFib_AddOrUpdate
 * @abstract Adds or updates a route
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return true if added/updated, false if a problem.
 */
bool metisFIB_AddOrUpdate(MetisFIB *fib, CPIRouteEntry *route);

/**
 * Removes a route
 *
 * Removes a specific nexthop for a route.  If there are no nexthops left after the
 * removal, the entire route is deleted from the FIB.
 *
 * @param [in] fib The FIB to modify
 * @param [in] route The route to remove
 *
 * @retval true Route completely removed
 * @retval false There are still other nexthops for the route
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisFIB_Remove(MetisFIB *fib, CPIRouteEntry *route);

/**
 * Removes the given connection ID from all routes
 *
 * Removes the given connection ID from all routes.  If that leaves a route
 * with no nexthops, the route remains in the table with an empty nexthop set.
 *
 * decide if this is really the right behavior, should it remove the route? (case 815)
 *
 * @param [in] fib The forwarding table
 * @param [in] connectionId The connection to remove.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisFIB_RemoveConnectionIdFromRoutes(MetisFIB *fib, unsigned connectionId);

/**
 * @function metisFib_Match
 * @abstract Lookup the interest in the FIB, returns set of connection ids to forward over
 * @discussion
 *   This is the internal state of the FIB entry.  If you will store a copy you must acquire a reference.
 *
 * @param <#param1#>
 * @return May be empty, should not be null
 */
const MetisNumberSet *metisFIB_Match(MetisFIB *fib, const MetisMessage *interestMessage);

/**
 * @function metisFib_Length
 * @abstract The number of entries in the forwarding table
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
size_t metisFIB_Length(const MetisFIB *fib);

/**
 * @function metisFib_GetEntries
 * @abstract Returns a list of the current FIB entries.
 * @discussion
 *   Caller must destroy the list
 *
 * @param <#param1#>
 * @return <#return#>
 */
MetisFibEntryList *metisFIB_GetEntries(const MetisFIB *fib);
#endif // Metis_metis_FIB_h
