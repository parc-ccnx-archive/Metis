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
 * @file metis_FibEntry.h
 * @brief A forwarding entry in the FIB table
 *
 * A Forwarding Information Base (FIB) entry (MetisFibEntry) is a
 * set of nexthops for a name.  It also indicates the forwarding strategy.
 *
 * Each nexthop contains the ConnectionId assocaited with it.  This could be
 * something specific like a MAC address or point-to-point tunnel.  Or, it
 * could be something general like a MAC group address or ip multicast overlay.
 *
 * See metis/strategies/metis_Strategy.h for a description of forwarding strategies.
 * In short, a strategy is the algorithm used to select one or more nexthops from
 * the set of available nexthops.
 *
 * Each nexthop also contains a void* to a forwarding strategy data container.
 * This allows a strategy to keep proprietary information about each nexthop.
 *
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#ifndef Metis_metis_FibEntry_h
#define Metis_metis_FibEntry_h

#include <ccnx/forwarder/metis/tlv/metis_TlvName.h>
#include <ccnx/forwarder/metis/strategies/metis_StrategyImpl.h>

struct metis_fib_entry;
typedef struct metis_fib_entry MetisFibEntry;

MetisFibEntry *metisFibEntry_Create(MetisTlvName *name);

/**
 * Decrements the reference count by one, and destroys the memory after last release
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in,out] fibEntryPtr A pointer to a MetisFibEntry, will be NULL'd
 *
 * Example:
 * @code
 * {
 *     MetisFibEntry *fibEntry = metisFibEntry(name);
 *     metisFibEntry_Release(&fibEntry);
 * }
 * @endcode
 */
void metisFibEntry_Release(MetisFibEntry **fibEntryPtr);

/**
 * Returns a reference counted copy of the fib entry
 *
 * The reference count is increased by one.  The returned value must be
 * released via metisFibEnty_Release().
 *
 * @param [in] fibEntry An allocated MetisFibEntry
 *
 * @return non-null A reference counted copy of the fibEntry
 *
 * Example:
 * @code
 * {
 *     MetisFibEntry *fibEntry = metisFibEntry(name);
 *     MetisFibEntry *copy = metisFibEntry_Acquire(fibEntry);
 *     metisFibEntry_Release(&copy);
 *     metisFibEntry_Release(&fibEntry);
 * }
 * @endcode
 */
MetisFibEntry *metisFibEntry_Acquire(const MetisFibEntry *fibEntry);

void metisFibEntry_SetStrategy(MetisFibEntry *fibEntry, MetisStrategyImpl *strategyImpl);
void metisFibEntry_AddNexthop(MetisFibEntry *fibEntry, unsigned connectionId);
void metisFibEntry_RemoveNexthop(MetisFibEntry *fibEntry, unsigned connectionId);

size_t metisFibEntry_NexthopCount(const MetisFibEntry *fibEntry);

/**
 * @function metisFibEntry_GetNexthops
 * @abstract Returns the nexthop set of the FIB entry.  You must Acquire if it will be saved.
 * @discussion
 *   Returns the next hop set for the FIB entry.
 *
 * @param <#param1#>
 * @return <#return#>
 */
const MetisNumberSet *metisFibEntry_GetNexthops(const MetisFibEntry *fibEntry);

/**
 * @function metisFibEntry_GetPrefix
 * @abstract Returns a copy of the prefix.
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return A reference counted copy that you must destroy
 */
MetisTlvName *metisFibEntry_GetPrefix(const MetisFibEntry *fibEntry);
#endif // Metis_metis_FibEntry_h
