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

#include <parc/security/parc_Security.h>

#include <parc/algol/parc_Memory.h>

#include <ccnx/forwarder/metis/config/metisControl_List.h>
#include <ccnx/forwarder/metis/config/metisControl_ListConnections.h>
#include <ccnx/forwarder/metis/config/metisControl_ListInterfaces.h>
#include <ccnx/forwarder/metis/config/metisControl_ListRoutes.h>

static void _metisControlList_Init(MetisCommandParser *parser, MetisCommandOps *ops);
static MetisCommandReturn _metisControlList_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args);
static MetisCommandReturn _metisControlList_HelpExecute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args);

static const char *_commandList = "list";
static const char *_commandListHelp = "help list";

MetisCommandOps *
metisControlList_Create(MetisControlState *state)
{
    return metisCommandOps_Create(state, _commandList, _metisControlList_Init, _metisControlList_Execute, metisCommandOps_Destroy);
}

MetisCommandOps *
metisControlList_HelpCreate(MetisControlState *state)
{
    return metisCommandOps_Create(state, _commandListHelp, NULL, _metisControlList_HelpExecute, metisCommandOps_Destroy);
}

// =====================================================

static MetisCommandReturn
_metisControlList_HelpExecute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
{
    MetisCommandOps *ops_list_connections = metisControlListConnections_HelpCreate(NULL);
    MetisCommandOps *ops_list_interfaces = metisControlListInterfaces_HelpCreate(NULL);
    MetisCommandOps *ops_list_routes = metisControlListRoutes_HelpCreate(NULL);

    printf("Available commands:\n");
    printf("   %s\n", ops_list_connections->command);
    printf("   %s\n", ops_list_interfaces->command);
    printf("   %s\n", ops_list_routes->command);
    printf("\n");

    metisCommandOps_Destroy(&ops_list_connections);
    metisCommandOps_Destroy(&ops_list_interfaces);
    metisCommandOps_Destroy(&ops_list_routes);

    return MetisCommandReturn_Success;
}

static void
_metisControlList_Init(MetisCommandParser *parser, MetisCommandOps *ops)
{
    MetisControlState *state = ops->closure;
    metisControlState_RegisterCommand(state, metisControlListConnections_HelpCreate(state));
    metisControlState_RegisterCommand(state, metisControlListInterfaces_HelpCreate(state));
    metisControlState_RegisterCommand(state, metisControlListRoutes_HelpCreate(state));
    metisControlState_RegisterCommand(state, metisControlListConnections_Create(state));
    metisControlState_RegisterCommand(state, metisControlListInterfaces_Create(state));
    metisControlState_RegisterCommand(state, metisControlListRoutes_Create(state));
}

static MetisCommandReturn
_metisControlList_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
{
    return _metisControlList_HelpExecute(parser, ops, args);
}

// ======================================================================
