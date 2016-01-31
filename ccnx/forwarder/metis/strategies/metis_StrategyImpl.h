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
 * @file metis_StrategyImpl.h
 * @brief Defines the function structure for a Strategy implementation
 *
 * <#Detailed Description#>
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

/**
 * A dispatch structure for a concrete implementation of a forwarding strategy.
 */

#ifndef Metis_metis_StrategyImpl_h
#define Metis_metis_StrategyImpl_h

#include <ccnx/forwarder/metis/core/metis_NumberSet.h>
#include <ccnx/forwarder/metis/core/metis_Message.h>
#include <ccnx/api/control/cpi_RouteEntry.h>

struct metis_strategy_impl;
typedef struct metis_strategy_impl MetisStrategyImpl;

/**
 * @typedef MetisStrategyImpl
 * @abstract Forwarding strategy implementation
 * @constant receiveObject is called when we receive an object and have a measured round trip time.  This
 *           allows a strategy to update its performance data.
 * @constant lookupNexthop Find the set of nexthops to use for the Interest.
 *           May be empty, should not be NULL.  Must be destroyed.
 * @constant addNexthop Add a nexthop to the list of available nexthops with a routing protocol-specific cost.
 * @constant destroy cleans up the strategy, freeing all memory and state.  A strategy is reference counted,
 *           so the final destruction only happens after the last reference is released.
 * @discussion <#Discussion#>
 */
struct metis_strategy_impl {
    void *context;
    void (*receiveObject)(MetisStrategyImpl *strategy, const MetisMessage *objectMessage, MetisTicks rtt);
    MetisNumberSet * (*lookupNexthop)(MetisStrategyImpl *strategy, const MetisMessage *interestMessage);
    void (*addNexthop)(MetisStrategyImpl *strategy, CPIRouteEntry *route);
    void (*removeNexthop)(MetisStrategyImpl *strategy, CPIRouteEntry *route);
    void (*destroy)(MetisStrategyImpl **strategyPtr);
};
#endif // Metis_metis_StrategyImpl_h
