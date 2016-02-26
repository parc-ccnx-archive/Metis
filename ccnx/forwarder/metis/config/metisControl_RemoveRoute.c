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
#include <ctype.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_List.h>

#include <ccnx/forwarder/metis/config/metisControl_RemoveRoute.h>

static MetisCommandReturn _metisControlRemoveRoute_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args);
static MetisCommandReturn _metisControlRemoveRoute_HelpExecute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args);

static const char *_commandRemoveRoute = "remove route";
static const char *_commandRemoveRouteHelp = "help remove route";

// ====================================================

MetisCommandOps *
metisControlRemoveRoute_Create(MetisControlState *state)
{
    return metisCommandOps_Create(state, _commandRemoveRoute, NULL, _metisControlRemoveRoute_Execute, metisCommandOps_Destroy);
}

MetisCommandOps *
metisControlRemoveRoute_HelpCreate(MetisControlState *state)
{
    return metisCommandOps_Create(state, _commandRemoveRouteHelp, NULL, _metisControlRemoveRoute_HelpExecute, metisCommandOps_Destroy);
}

/**
 * Return true if string is purely an integer
 */
static bool
_isNumber(const char *string)
{
    size_t len = strlen(string);
    for (size_t i = 0; i < len; i++) {
        if (!isdigit(string[i])) {
            return false;
        }
    }
    return true;
}

/**
 * A symbolic name must be at least 1 character and must begin with an alpha.
 * The remainder must be an alphanum.
 */
static bool
_validateSymbolicName(const char *symbolic)
{
    bool success = false;
    size_t len = strlen(symbolic);
    if (len > 0) {
        if (isalpha(symbolic[0])) {
            success = true;
            for (size_t i = 1; i < len; i++) {
                if (!isalnum(symbolic[i])) {
                    success = false;
                    break;
                }
            }
        }
    }
    return success;
}

// ====================================================

static MetisCommandReturn
_metisControlRemoveRoute_HelpExecute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
{
    printf("commands:\n");
    printf("   remove route <symbolic | connid> <prefix>\n");
    printf("\n");
    printf("   symbolic:  The optional symbolic name for an exgress\n");
    printf("   connid:    The optional egress connection id (see 'help list connections')\n");
    printf("   prefix:    The CCNx name as a URI (e.g. lci:/foo/bar)\n");
    printf("\n");
    printf("Examples:\n");
    printf("   remove route 25 lci:/foo/bar\n");
    printf("      removes route to prefix '/foo/bar' on egress connection index 25\n");
    printf("   remove route tun3 lci:/foo/bar\n");
    printf("      removes route to prefix '/foo/bar' on egress connection 'tun3'\n");
    printf("\n");
    
    return MetisCommandReturn_Success;
}

static MetisCommandReturn
_metisControlRemoveRoute_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
{
    MetisControlState *state = ops->closure;

    if (parcList_Size(args) != 4) {
        _metisControlRemoveRoute_HelpExecute(parser, ops, args);
        return MetisCommandReturn_Failure;
    }

    const char *symbolicOrConnid = parcList_GetAtIndex(args, 2);

    if (_validateSymbolicName(symbolicOrConnid) || _isNumber(symbolicOrConnid)) {

        const char *prefixString = parcList_GetAtIndex(args, 3);
        CCNxName *prefix = ccnxName_CreateFromURI(prefixString);
        if (prefix == NULL) {
            printf("ERROR: could not parse prefix '%s'\n", prefixString);
            return MetisCommandReturn_Failure;
        }

        char *protocolTypeAsString = "static";

        CPINameRouteProtocolType protocolType = cpiNameRouteProtocolType_FromString(protocolTypeAsString);
        CPINameRouteType routeType = cpiNameRouteType_LONGEST_MATCH;
        CPIAddress *nexthop = NULL;

        struct timeval *lifetime = NULL;

        CPIRouteEntry *route = NULL;

        if (_isNumber(symbolicOrConnid)) {
            unsigned connid = (unsigned) strtold(symbolicOrConnid, NULL);
            route = cpiRouteEntry_Create(prefix, connid, nexthop, protocolType, routeType, lifetime, 0);
        } else {
            route = cpiRouteEntry_CreateSymbolic(prefix, symbolicOrConnid, protocolType, routeType, lifetime, 0);
        }

        CCNxControl *addRouteRequest = ccnxControl_CreateRemoveRouteRequest(route);

        cpiRouteEntry_Destroy(&route);

        if (metisControlState_GetDebug(state)) {
            char *str = parcJSON_ToString(ccnxControl_GetJson(addRouteRequest));
            printf("request: %s\n", str);
            parcMemory_Deallocate((void **) &str);
        }

        CCNxMetaMessage *message = ccnxMetaMessage_CreateFromControl(addRouteRequest);
        CCNxMetaMessage *rawResponse = metisControlState_WriteRead(state, message);
        ccnxMetaMessage_Release(&message);

        ccnxControl_Release(&addRouteRequest);

        CCNxControl *response = ccnxMetaMessage_GetControl(rawResponse);

        if (metisControlState_GetDebug(state)) {
            char *str = parcJSON_ToString(ccnxControl_GetJson(response));
            printf("response: %s\n", str);
            parcMemory_Deallocate((void **) &str);
        }

        ccnxMetaMessage_Release(&rawResponse);

        return MetisCommandReturn_Success;
    }
    else {
        printf("ERROR: Invalid symbolic or connid.  Symbolic name must begin with an alpha followed by alphanum.  connid must be an integer\n");
        return MetisCommandReturn_Failure;
    }

}
