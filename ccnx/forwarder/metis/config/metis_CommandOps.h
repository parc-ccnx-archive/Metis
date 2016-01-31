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
 * @file metis_CommandOps.h
 * @brief The function structure defining a CLI command
 *
 * The function structure that defines a CLI command.  Each command will return one
 * of these which defines how to run the command.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#ifndef Metis_metis_CommandOps_h
#define Metis_metis_CommandOps_h

#include <parc/algol/parc_List.h>

#include <ccnx/forwarder/metis/config/metis_CommandReturn.h>

// forward reference
struct metis_command_parser;

struct metis_command_ops;
typedef struct metis_command_ops MetisCommandOps;

/**
 * @typedef MetisCommandOps
 * @abstract Each command implements a MetisCommandOps
 * @constant closure is a user-specified pointer for any state the user needs
 * @constant command The text string of the command, must be the spelled out string, e.g. "help list routes"
 * @constant init A function to call to initialize the command at program startup
 * @constant execute A function to call to execute the command
 * @constant destroyer A function to call to release the command
 * @discussion
 *     Typically, the root of the thee has an Init function that then initilizes the
 *     rest of the tree.  For example:
 *
 * @code
 *    const MetisCommandOps metisControl_Root = {
 *      .closure = NULL,
 *      .command = "", // empty string for root
 *      .init    = metisControl_Root_Init,
 *      .execute = metisControl_Root_Execute
 *      .destroyer = NULL
 *    };
 * @endcode
 *
 * The metisControl_Root_Init function will then begin adding the subtree under root.  For example:
 *
 * @code
 *  const MetisCommandOps metisControl_Add = {
 *      .closure = NULL,
 *      .command = "add",
 *      .init    = metisControl_Add_Init,
 *      .execute = metisControl_Add_Execute,
 *      .destroyer = NULL
 *  };
 *
 *  static void
 *  metisControl_Root_Init(MetisControlState *state, MetisCommandOps *ops)
 *  {
 *      metisControlState_RegisterCommand(state, &metisControl_Add);
 *  }
 * @endcode
 */
struct metis_command_ops {
    void *closure;
    char *command;
    void (*init)(struct metis_command_parser *parser, MetisCommandOps *ops);
    MetisCommandReturn (*execute)(struct metis_command_parser *parser, MetisCommandOps *ops, PARCList *args);
    void (*destroyer)(MetisCommandOps **opsPtr);
};

/**
 * A helper function to create the pubically defined MetisCommandOps.
 *
 * Retruns allocated memory of the command
 *
 * @param [in] command The string is copied
 *
 * @retval <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisCommandOps *metisCommandOps_Create(void *closure,
                                        const char *command,
                                        void (*init)(struct metis_command_parser *parser, MetisCommandOps *ops),
                                        MetisCommandReturn (*execute)(struct metis_command_parser *parser, MetisCommandOps *ops, PARCList *args),
                                        void (*destroyer)(MetisCommandOps **opsPtr));

/**
 * De-allocates the memory of the MetisCommandOps and the copied command string
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @retval <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisCommandOps_Destroy(MetisCommandOps **opsPtr);
#endif // Metis_metis_CommandOps_h
