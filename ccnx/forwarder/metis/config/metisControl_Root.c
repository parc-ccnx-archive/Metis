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

#include <stdbool.h>
#include <stdint.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>

#include <LongBow/runtime.h>

#include <ccnx/forwarder/metis/config/metisControl_Root.h>
#include <ccnx/forwarder/metis/config/metisControl_Add.h>
#include <ccnx/forwarder/metis/config/metisControl_List.h>
#include <ccnx/forwarder/metis/config/metisControl_Quit.h>
#include <ccnx/forwarder/metis/config/metisControl_Remove.h>
#include <ccnx/forwarder/metis/config/metisControl_Set.h>
#include <ccnx/forwarder/metis/config/metisControl_Unset.h>

static void _metisControlRoot_Init(MetisCommandParser *parser, MetisCommandOps *ops);
static MetisCommandReturn _metisControlRoot_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args);
static MetisCommandReturn _metisControlRoot_HelpExecute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args);

static const char *_commandRoot = "";
static const char *_commandRootHelp = "help";

// ====================================================

MetisCommandOps *
metisControlRoot_Create(MetisControlState *state)
{
    return metisCommandOps_Create(state, _commandRoot, _metisControlRoot_Init, _metisControlRoot_Execute, metisCommandOps_Destroy);
}

MetisCommandOps *
metisControlRoot_HelpCreate(MetisControlState *state)
{
    return metisCommandOps_Create(state, _commandRootHelp, NULL, _metisControlRoot_HelpExecute, metisCommandOps_Destroy);
}

// ===================================================

static MetisCommandReturn
_metisControlRoot_HelpExecute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
{
    printf("Command-line execution:\n");
    printf("   metis_control [--keystore <keystorepath>] [--password <password>] command\n");
    printf("\n");
    printf("Interactive execution:\n");
    printf("   metis_control [--keystore <keystorepath>] [--password <password>]\n");
    printf("\n");
    printf("If the keystore is not specified, the default path is used. Keystore must exist prior to running program.\n");
    printf("If the password is not specified, the user will be prompted.\n");
    printf("\n");

    MetisCommandOps *ops_help_add = metisControlAdd_CreateHelp(NULL);
    MetisCommandOps *ops_help_list = metisControlList_HelpCreate(NULL);
    MetisCommandOps *ops_help_quit = metisControlQuit_HelpCreate(NULL);
    MetisCommandOps *ops_help_remove = metisControlRemove_HelpCreate(NULL);
    MetisCommandOps *ops_help_set = metisControlSet_HelpCreate(NULL);
    MetisCommandOps *ops_help_unset = metisControlUnset_HelpCreate(NULL);

    printf("Available commands:\n");
    printf("   %s\n", ops_help_add->command);
    printf("   %s\n", ops_help_list->command);
    printf("   %s\n", ops_help_quit->command);
    printf("   %s\n", ops_help_remove->command);
    printf("   %s\n", ops_help_set->command);
    printf("   %s\n", ops_help_unset->command);
    printf("\n");

    metisCommandOps_Destroy(&ops_help_add);
    metisCommandOps_Destroy(&ops_help_list);
    metisCommandOps_Destroy(&ops_help_quit);
    metisCommandOps_Destroy(&ops_help_remove);
    metisCommandOps_Destroy(&ops_help_set);
    metisCommandOps_Destroy(&ops_help_unset);

    return MetisCommandReturn_Success;
}

static void
_metisControlRoot_Init(MetisCommandParser *parser, MetisCommandOps *ops)
{
    MetisControlState *state = ops->closure;

    metisControlState_RegisterCommand(state, metisControlAdd_CreateHelp(state));
    metisControlState_RegisterCommand(state, metisControlList_HelpCreate(state));
    metisControlState_RegisterCommand(state, metisControlQuit_HelpCreate(state));
    metisControlState_RegisterCommand(state, metisControlRemove_HelpCreate(state));
    metisControlState_RegisterCommand(state, metisControlSet_HelpCreate(state));
    metisControlState_RegisterCommand(state, metisControlUnset_HelpCreate(state));

    metisControlState_RegisterCommand(state, metisControlAdd_Create(state));
    metisControlState_RegisterCommand(state, metisControlList_Create(state));
    metisControlState_RegisterCommand(state, metisControlQuit_Create(state));
    metisControlState_RegisterCommand(state, metisControlRemove_Create(state));
    metisControlState_RegisterCommand(state, metisControlSet_Create(state));
    metisControlState_RegisterCommand(state, metisControlUnset_Create(state));
}

static MetisCommandReturn
_metisControlRoot_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
{
    return MetisCommandReturn_Success;
}

// ======================================================================
