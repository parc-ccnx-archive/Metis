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

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Time.h>

#include <ccnx/forwarder/metis/config/metisControl_ListRoutes.h>

#include <ccnx/api/control/cpi_ManageLinks.h>
#include <ccnx/api/control/cpi_Forwarding.h>

static MetisCommandReturn _metisControlListRoutes_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args);
static MetisCommandReturn _metisControlListRoutes_HelpExecute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args);

static const char *_commandListRoutes = "list routes";
static const char *_commandListRoutesHelp = "help list routes";

// ====================================================

MetisCommandOps *
metisControlListRoutes_Create(MetisControlState *state)
{
    return metisCommandOps_Create(state, _commandListRoutes, NULL, _metisControlListRoutes_Execute, metisCommandOps_Destroy);
}

MetisCommandOps *
metisControlListRoutes_HelpCreate(MetisControlState *state)
{
    return metisCommandOps_Create(state, _commandListRoutesHelp, NULL, _metisControlListRoutes_HelpExecute, metisCommandOps_Destroy);
}

// ====================================================

static MetisCommandReturn
_metisControlListRoutes_HelpExecute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
{
    printf("command: list routes\n");
    printf("\n");
    printf("This command will fetch the prefix routing table.  For each route, it will list:\n");
    printf("   iface:    interface\n");
    printf("   protocol: the routing protocol, such as STATIC, CONNECTED, etc.\n");
    printf("   type:     LMP or EXACT (longest matching prefix or exact match)\n");
    printf("   cost:     The route cost, lower being preferred\n");
    printf("   next:     List of next hops by interface id\n");
    printf("   prefix:   The CCNx name prefix\n");
    printf("\n");
    return MetisCommandReturn_Success;
}

static MetisCommandReturn
_metisControlListRoutes_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
{
    if (parcList_Size(args) != 2) {
        _metisControlListRoutes_HelpExecute(parser, ops, args);
        return MetisCommandReturn_Failure;
    }

    MetisControlState *state = ops->closure;

    CCNxControl *routeListRequest = ccnxControl_CreateRouteListRequest();

    CCNxMetaMessage *message = ccnxMetaMessage_CreateFromControl(routeListRequest);

    CCNxMetaMessage *rawResponse = metisControlState_WriteRead(state, message);

    CCNxControl *response = ccnxMetaMessage_GetControl(rawResponse);

    if (metisControlState_GetDebug(state)) {
        char *str = parcJSON_ToString(ccnxControl_GetJson(response));
        printf("reponse:\n%s\n", str);
        parcMemory_Deallocate((void **) &str);
    }

    CPIRouteEntryList *list = cpiForwarding_RouteListFromControlMessage(response);

    printf("%6.6s %9.9s %7.7s %8.8s %20.20s %s\n", "iface", "protocol", "route", "cost", "next", "prefix");

    for (size_t i = 0; i < cpiRouteEntryList_Length(list); i++) {
        CPIRouteEntry *route = cpiRouteEntryList_Get(list, i);

        PARCBufferComposer *composer = parcBufferComposer_Create();

        parcBufferComposer_Format(composer, "%6d %9.9s %7.7s %8u ",
                                  cpiRouteEntry_GetInterfaceIndex(route),
                                  cpiNameRouteProtocolType_ToString(cpiRouteEntry_GetRouteProtocolType(route)),
                                  cpiNameRouteType_ToString(cpiRouteEntry_GetRouteType(route)),
                                  cpiRouteEntry_GetCost(route));

        if (cpiRouteEntry_GetNexthop(route) != NULL) {
            cpiAddress_BuildString(cpiRouteEntry_GetNexthop(route), composer);
        } else {
            parcBufferComposer_PutString(composer, "---.---.---.---/....");
        }

        if (cpiRouteEntry_HasLifetime(route)) {
            char *timeString = parcTime_TimevalAsString(cpiRouteEntry_GetLifetime(route));
            parcBufferComposer_PutString(composer, timeString);
            parcMemory_Deallocate((void **) &timeString);
        } else {
            parcBufferComposer_PutString(composer, " ");
        }

        char *ccnxName = ccnxName_ToString(cpiRouteEntry_GetPrefix(route));
        parcBufferComposer_PutString(composer, ccnxName);
        parcMemory_Deallocate((void **) &ccnxName);

        PARCBuffer *tempBuffer = parcBufferComposer_ProduceBuffer(composer);
        char *result = parcBuffer_ToString(tempBuffer);
        parcBuffer_Release(&tempBuffer);

        puts(result);
        parcMemory_Deallocate((void **) &result);
        parcBufferComposer_Release(&composer);
        cpiRouteEntry_Destroy(&route);
    }

    cpiRouteEntryList_Destroy(&list);
    ccnxMetaMessage_Release(&rawResponse);
    ccnxControl_Release(&routeListRequest);

    printf("Done\n\n");

    return MetisCommandReturn_Success;
}
