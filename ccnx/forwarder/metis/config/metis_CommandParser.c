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
 * @author Glenn Scott, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */
#include <config.h>

#include <stdbool.h>
#include <stdint.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>

#include <LongBow/runtime.h>
#include <string.h>

#include <parc/security/parc_Security.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_List.h>
#include <parc/algol/parc_TreeRedBlack.h>
#include <parc/algol/parc_Time.h>

#include <ccnx/common/ccnx_KeystoreUtilities.h>

#include <ccnx/forwarder/metis/config/metis_CommandParser.h>

#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
extern int errno;
#endif

struct metis_command_parser {
    // key = command, value = MetisCommandOps
    PARCTreeRedBlack *commandTree;
    bool debugFlag;
};

static int
_stringCompare(const void *key1, const void *key2)
{
    return strcasecmp((const char *) key1, (const char *) key2);
}

MetisCommandParser *
metisCommandParser_Create(void)
{
    MetisCommandParser *state = parcMemory_AllocateAndClear(sizeof(MetisCommandParser));
    assertNotNull(state, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisCommandParser));
    state->commandTree = parcTreeRedBlack_Create(
        _stringCompare,                                           // key compare
        NULL,                                                     // key free
        NULL,                                                     // key copy
        NULL,                                                     // value equals
        NULL,                                                     // value free
        NULL                                                      // value copy
        );
    state->debugFlag = false;
    return state;
}

void
metisCommandParser_Destroy(MetisCommandParser **parserPtr)
{
    MetisCommandParser *parser = *parserPtr;

    // destroy every element if it has a destroyer
    PARCArrayList *values = parcTreeRedBlack_Values(parser->commandTree);
    if (values) {
        for (int i = 0; i < parcArrayList_Size(values); i++) {
            MetisCommandOps *ops = parcArrayList_Get(values, i);
            parcTreeRedBlack_Remove(parser->commandTree, ops->command);
            if (ops->destroyer) {
                ops->destroyer(&ops);
            }
        }
        parcArrayList_Destroy(&values);
    }

    parcTreeRedBlack_Destroy(&parser->commandTree);

    parcMemory_Deallocate((void **) &parser);
    *parserPtr = NULL;
}

void
metisCommandParser_SetDebug(MetisCommandParser *state, bool debugFlag)
{
    state->debugFlag = debugFlag;
}

bool
metisCommandParser_GetDebug(MetisCommandParser *state)
{
    return state->debugFlag;
}

void
metisCommandParser_RegisterCommand(MetisCommandParser *state, MetisCommandOps *ops)
{
    assertNotNull(state, "Parameter state must be non-null");
    assertNotNull(ops, "Parameter ops must be non-null");
    assertNotNull(ops->command, "Operation command string must be non-null");

    void *exists = parcTreeRedBlack_Get(state->commandTree, ops->command);
    assertNull(exists, "Command '%s' already exists in the tree %p\n", ops->command, (void *) exists);

    parcTreeRedBlack_Insert(state->commandTree, (void *) ops->command, (void *) ops);

    // if the command being registered asked for an init function to be called, call it
    if (ops->init != NULL) {
        ops->init(state, ops);
    }
}

static PARCList *
parseStringIntoTokens(const char *originalString)
{
    PARCList *list = parcList(parcArrayList_Create(parcArrayList_StdlibFreeFunction), PARCArrayListAsPARCList);

    char *token;

    char *tofree = parcMemory_StringDuplicate(originalString, strlen(originalString) + 1);
    char *string = tofree;

    while ((token = strsep(&string, " \t\n")) != NULL) {
        if (strlen(token) > 0) {
            parcList_Add(list, strdup(token));
        }
    }

    parcMemory_Deallocate((void **) &tofree);

    return list;
}

/**
 * Matches the user arguments to available commands, returning the command or NULL if not found
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
static MetisCommandOps *
metisCommandParser_MatchCommand(MetisCommandParser *state, PARCList *args)
{
    // Find the longest matching prefix command.
    // Pretty wildly inefficient

    size_t longest_token_count = 0;
    char *longest_command = NULL;

    PARCArrayList *commands = parcTreeRedBlack_Keys(state->commandTree);
    for (int i = 0; i < parcArrayList_Size(commands); i++) {
        char *command = parcArrayList_Get(commands, i);
        PARCList *command_tokens = parseStringIntoTokens(command);

        // is it a prefix match?
        if (parcList_Size(args) >= parcList_Size(command_tokens)) {
            bool possible_match = true;
            for (int i = 0; i < parcList_Size(command_tokens) && possible_match; i++) {
                const char *a = parcList_GetAtIndex(command_tokens, i);
                const char *b = parcList_GetAtIndex(args, i);
                if (strncasecmp(a, b, strlen(a) + 1) != 0) {
                    possible_match = false;
                }
            }

            if (possible_match && parcList_Size(command_tokens) > longest_token_count) {
                longest_token_count = parcList_Size(command_tokens);
                longest_command = command;
            }
        }

        parcList_Release(&command_tokens);
    }

    parcArrayList_Destroy(&commands);

    if (longest_token_count == 0) {
        return NULL;
    } else {
        MetisCommandOps *ops = parcTreeRedBlack_Get(state->commandTree, longest_command);
        assertNotNull(ops, "Got null operations for command '%s'\n", longest_command);
        return ops;
    }
}

MetisCommandReturn
metisCommandParser_DispatchCommand(MetisCommandParser *state, PARCList *args)
{
    MetisCommandOps *ops = metisCommandParser_MatchCommand(state, args);

    if (ops == NULL) {
        printf("Command not found.\n");
        return MetisCommandReturn_Failure;
    } else {
        return ops->execute(state, ops, args);
    }
}

bool
metisCommandParser_ContainsCommand(MetisCommandParser *parser, const char *command)
{
    MetisCommandOps *ops = parcTreeRedBlack_Get(parser->commandTree, command);
    return (ops != NULL);
}
