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

//
//  strategy_All.c
//  Metis
//
//  Created by Mosko, Marc <Marc.Mosko@parc.com> on 12/1/13.
//
//

#include <config.h>
#include <stdio.h>
#include <string.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_Memory.h>

#include <ccnx/forwarder/metis/strategies/strategy_All.h>

static void             _strategyAll_ReceiveObject(MetisStrategyImpl *strategy, const MetisMessage *objectMessage, MetisTicks rtt);
static MetisNumberSet *_strategyAll_LookupNexthop(MetisStrategyImpl *strategy, const MetisMessage *interestMessage);
static void             _strategyAll_AddNexthop(MetisStrategyImpl *strategy, CPIRouteEntry *route);
static void             _strategyAll_RemoveNexthop(MetisStrategyImpl *strategy, CPIRouteEntry *route);
static void             _strategyAll_ImplDestroy(MetisStrategyImpl **strategyPtr);

static MetisStrategyImpl _template = {
    .context       = NULL,
    .receiveObject = &_strategyAll_ReceiveObject,
    .lookupNexthop = &_strategyAll_LookupNexthop,
    .addNexthop    = &_strategyAll_AddNexthop,
    .removeNexthop = &_strategyAll_RemoveNexthop,
    .destroy       = &_strategyAll_ImplDestroy,
};

struct strategy_all;
typedef struct strategy_all StrategyAll;

struct strategy_all {
    int x;
};

MetisStrategyImpl *
strategyAll_Create()
{
    StrategyAll *strategy = parcMemory_AllocateAndClear(sizeof(StrategyAll));
    assertNotNull(strategy, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(StrategyAll));

    MetisStrategyImpl *impl = parcMemory_AllocateAndClear(sizeof(MetisStrategyImpl));
    assertNotNull(impl, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisStrategyImpl));
    memcpy(impl, &_template, sizeof(MetisStrategyImpl));
    impl->context = strategy;

    return impl;
}

// =======================================================
// Dispatch API

static void
_strategyAll_ReceiveObject(MetisStrategyImpl *strategy, const MetisMessage *objectMessage, MetisTicks rtt)
{
}

static MetisNumberSet *
_strategyAll_LookupNexthop(MetisStrategyImpl *strategy, const MetisMessage *interestMessage)
{
    return NULL;
}

static void
_strategyAll_AddNexthop(MetisStrategyImpl *strategy, CPIRouteEntry *route)
{
}

static void
_strategyAll_RemoveNexthop(MetisStrategyImpl *strategy, CPIRouteEntry *route)
{
}

static void
_strategyAll_ImplDestroy(MetisStrategyImpl **strategyPtr)
{
    assertNotNull(strategyPtr, "Parameter must be non-null double pointer");
    assertNotNull(*strategyPtr, "Parameter must dereference to non-null pointer");

    MetisStrategyImpl *impl = *strategyPtr;
    StrategyAll *strategy = (StrategyAll *) impl->context;

    parcMemory_Deallocate((void **) &strategy);
    parcMemory_Deallocate((void **) &impl);
    *strategyPtr = NULL;
}
