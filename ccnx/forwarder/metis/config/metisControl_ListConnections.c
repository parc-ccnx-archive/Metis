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

#include <ccnx/forwarder/metis/config/metisControl_ListConnections.h>

#include <ccnx/api/control/cpi_ManageLinks.h>
#include <ccnx/api/control/cpi_Forwarding.h>

static MetisCommandReturn _metisControlListConnections_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args);
static MetisCommandReturn _metisControlListConnections_HelpExecute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args);

static const char *_commandListConnections = "list connections";
static const char *_commandListConnectionsHelp = "help list connections";

MetisCommandOps *
metisControlListConnections_Create(MetisControlState *state)
{
    return metisCommandOps_Create(state, _commandListConnections, NULL, _metisControlListConnections_Execute, metisCommandOps_Destroy);
}

MetisCommandOps *
metisControlListConnections_HelpCreate(MetisControlState *state)
{
    return metisCommandOps_Create(state, _commandListConnectionsHelp, NULL, _metisControlListConnections_HelpExecute, metisCommandOps_Destroy);
}

// ====================================================

static MetisCommandReturn
_metisControlListConnections_HelpExecute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
{
    printf("list connections: displays a 1-line summary of each connection\n");
    printf("\n");
    printf("The columns are:\n");
    printf("   connection id : an integer index for the connection\n");
    printf("   state         : UP or DOWN\n");
    printf("   local address : the local network address associated with the connection\n");
    printf("   remote address: the remote network address associated with the connection\n");
    printf("   protocol      : the network protocol (tcp, udp, gre, mcast, ether)\n");
    printf("\n");
    return MetisCommandReturn_Success;
}

static MetisCommandReturn
_metisControlListConnections_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
{
    if (parcList_Size(args) != 2) {
        _metisControlListConnections_HelpExecute(parser, ops, args);
        return MetisCommandReturn_Failure;
    }

    MetisControlState *state = ops->closure;

    CCNxControl *connectionListRequest = ccnxControl_CreateConnectionListRequest();

    CCNxMetaMessage *message = ccnxMetaMessage_CreateFromControl(connectionListRequest);
    CCNxMetaMessage *rawResponse = metisControlState_WriteRead(state, message);
    ccnxMetaMessage_Release(&message);

    CCNxControl *response = ccnxMetaMessage_GetControl(rawResponse);

    if (metisControlState_GetDebug(state)) {
        char *str = parcJSON_ToString(ccnxControl_GetJson(response));
        printf("reponse:\n%s\n", str);
        parcMemory_Deallocate((void **) &str);
    }

    CPIConnectionList *list = cpiLinks_ConnectionListFromControlMessage(response);
    //
    //    //"%3u %10s %1s%1s %8u "
    //    //    printf("%3.3s %10.10s %1.1s%1.1s %8.8s \n", "interface", "name", "loopback", "multicast", "MTU");
    for (size_t i = 0; i < cpiConnectionList_Length(list); i++) {
        CPIConnection *connection = cpiConnectionList_Get(list, i);
        char *string = cpiConnection_ToString(connection);
        puts(string);
        parcMemory_Deallocate((void **) &string);
        cpiConnection_Release(&connection);
    }
    cpiConnectionList_Destroy(&list);
    ccnxMetaMessage_Release(&rawResponse);

    return MetisCommandReturn_Success;
}
