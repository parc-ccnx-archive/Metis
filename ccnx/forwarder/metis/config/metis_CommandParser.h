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
 * @file metis_CommandParser.h
 * @brief Creates a dictionary of commands and parses a command-line to match against them
 *
 * A user creates individual CommandParserEntry that map a command-line to a function
 * to execute.  The CommandParser then does a longest-matching prefix match of a command-line
 * to the dictionary of commands and executes the appropriate command.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_command_parser_h
#define Metis_metis_command_parser_h

#include <ccnx/transport/common/transport_MetaMessage.h>

#include <ccnx/forwarder/metis/config/metis_CommandReturn.h>
#include <ccnx/forwarder/metis/config/metis_CommandOps.h>

struct metis_command_parser;
typedef struct metis_command_parser MetisCommandParser;

/**
 * metisControlState_Create
 *
 * Creates the global state for the MetisControl program
 *
 * @return non-null A command parser
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisCommandParser *metisCommandParser_Create(void);

/**
 * Destroys the control state, closing all network connections
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
void metisCommandParser_Destroy(MetisCommandParser **statePtr);

/**
 * Registers a MetisCommandOps with the system.
 *
 * Each command has its complete command prefix in the "command" field.  RegisterCommand
 * will put these command prefixes in to a tree and then match what a user types against
 * the longest-matching prefix in the tree.  If there's a match, it will call the "execute"
 * function.
 *
 * When the parser is destroyed, each command's destroyer function will be called.
 *
 * @param [in] state An allocated MetisControlState
 * @param [in] command The command to register with the system
 *
 * Example:
 * @code
 *      static MetisControlReturn
 *      metisControl_Root_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
 *      {
 *          printf("Root Command\n");
 *          return MetisCommandReturn_Success;
 *      }
 *
 *      static MetisControlReturn
 *      metisControl_FooBar_Execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
 *      {
 *          printf("Foo Bar Command\n");
 *          return MetisCommandReturn_Success;
 *      }
 *
 *      const MetisCommandOps metisControl_Root = {
 *      .closure = NULL,
 *      .command = "", // empty string for root
 *      .init    = NULL,
 *      .execute = metisControl_Root_Execute
 *      };
 *
 *      const MetisCommandOps metisControl_FooBar = {
 *      .closure = NULL,
 *      .command = "foo bar", // empty string for root
 *      .init    = NULL,
 *      .execute = metisControl_FooBar_Execute
 *      };
 *
 *   void startup(void)
 *   {
 *      MetisControlState *state = metisControlState_Create("happy", "day");
 *      metisControlState_RegisterCommand(state, metisControl_FooBar);
 *      metisControlState_RegisterCommand(state, metisControl_Root);
 *
 *      // this executes "root"
 *      metisControlState_DispatchCommand(state, "foo");
 *      metisControlState_Destroy(&state);
 *  }
 * @endcode
 */
void metisCommandParser_RegisterCommand(MetisCommandParser *state, MetisCommandOps *command);

/**
 * Performs a longest-matching prefix of the args to the command tree
 *
 * The command tree is created with metisControlState_RegisterCommand.
 *
 * @param [in] state The allocated MetisControlState
 * @param [in] args  Each command-line word parsed to the ordered list
 *
 * @return MetisCommandReturn_Success the command was successful
 * @return MetisCommandReturn_Failure the command failed or was not found
 * @return MetisCommandReturn_Exit the command indicates that the interactive mode should exit
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisCommandReturn metisCommandParser_DispatchCommand(MetisCommandParser *state, PARCList *args);

/**
 * Sets the Debug mode, which will print out much more information.
 *
 * Prints out much more diagnostic information about what metis-control is doing.
 * yes, you would make a MetisCommandOps to set and unset this :)
 *
 * @param [in] debugFlag true means to print debug info, false means to turn it off
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisCommandParser_SetDebug(MetisCommandParser *state, bool debugFlag);

/**
 * Returns the debug state of MetisControlState
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
bool metisCommandParser_GetDebug(MetisCommandParser *state);

/**
 * Checks if the command is registered
 *
 * Checks if the exact command given is registered.  This is not a prefix match.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @return true The command is registered
 * @return false The command is not registered
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisCommandParser_ContainsCommand(MetisCommandParser *parser, const char *command);
#endif // Metis_metis_command_parser_h
