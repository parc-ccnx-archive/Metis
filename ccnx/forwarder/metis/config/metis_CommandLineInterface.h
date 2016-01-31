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
 * @file metis_CommandLineInterface.h
 * @brief A telnet-like server for management interface
 *
 *
 * We do not start the CLI until metisCommandLineInterface_Start() is called.  This allows us to always create it in
 * metisForwarder_Create(), but not bind the port until needed.  Binding the port in metisForwarder_Create()
 * causes severe issues in rapid execution of unit tests.
 *
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#ifndef Metis_metis_CommandLineInterface_h
#define Metis_metis_CommandLineInterface_h

#include <ccnx/forwarder/metis/core/metis_Forwarder.h>

struct metis_command_line_interface;
typedef struct metis_command_line_interface MetisCommandLineInterface;

/**
 * Creates a CLI on the given port.
 *
 *   A telnet-style interface.  Creating it will not bind the port or start
 *   the service.  You need to call <code>metisCli_Start()</code>
 *
 * @param port the command port, in host byte order
 *
 * @return NULL if cannot be created on the port
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisCommandLineInterface *metisCommandLineInterface_Create(MetisForwarder *metis, uint16_t port);

/**
 * Stops and destroys the CLI.  Existing sessions are destroyed.
 *
 *   <#Discussion#>
 *
 * @param cliPtr
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisCommandLineInterface_Destroy(MetisCommandLineInterface **cliPtr);

/*
 * Binds the port and starts the CLI service
 *
 *   <#Discussion#>
 *
 * @param cli
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisCommandLineInterface_Start(MetisCommandLineInterface *cli);
#endif // Metis_metis_CommandLineInterface_h
