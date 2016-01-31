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
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#include <config.h>

#include <stdbool.h>
#include <stdint.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_Memory.h>

#include <ccnx/forwarder/metis/config/metisControl_Add.h>
#include <ccnx/forwarder/metis/config/metisControl_AddConnection.h>
#include <ccnx/forwarder/metis/config/metisControl_AddRoute.h>
#include <ccnx/forwarder/metis/config/metisControl_AddListener.h>

// ===================================================

static void _metisControlAdd_Init(MetisCommandParser *parser, MetisCommandOps *ops);
static MetisCommandReturn _metisControlAdd_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args);
static MetisCommandReturn _metisControlAdd_HelpExecute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args);

// ===================================================

static const char *command_add = "add";
static const char *help_command_add = "help add";

MetisCommandOps *
metisControlAdd_Create(MetisControlState *state)
{
    return metisCommandOps_Create(state, command_add, _metisControlAdd_Init, _metisControlAdd_Execute, metisCommandOps_Destroy);
}

MetisCommandOps *
metisControlAdd_CreateHelp(MetisControlState *state)
{
    return metisCommandOps_Create(state, help_command_add, NULL, _metisControlAdd_HelpExecute, metisCommandOps_Destroy);
}

// ===================================================

static MetisCommandReturn
_metisControlAdd_HelpExecute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
{
    printf("Available commands:\n");
    printf("   %s\n", command_add);
    printf("   %s\n", help_command_add);
    printf("\n");
    return MetisCommandReturn_Success;
}

static void
_metisControlAdd_Init(MetisCommandParser *parser, MetisCommandOps *ops)
{
    MetisControlState *state = ops->closure;
    metisControlState_RegisterCommand(state, metisControlAddListener_HelpCreate(state));
    metisControlState_RegisterCommand(state, metisControlAddListener_Create(state));
    metisControlState_RegisterCommand(state, metisControlAddConnection_HelpCreate(state));
    metisControlState_RegisterCommand(state, metisControlAddRoute_HelpCreate(state));
    metisControlState_RegisterCommand(state, metisControlAddConnection_Create(state));
    metisControlState_RegisterCommand(state, metisControlAddRoute_Create(state));
}

static MetisCommandReturn
_metisControlAdd_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
{
    return _metisControlAdd_HelpExecute(parser, ops, args);
}
