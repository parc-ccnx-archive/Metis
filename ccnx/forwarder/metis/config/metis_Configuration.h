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
 * @file metis_Configuration.h
 * @brief Metis configuration, such as in-band commands or CLI
 *
 * Manages all user configuration of the system, such as from the CLI or web interface
 * It remembers the user commands and will be able to write out a config file.
 *
 * Caveats:
 * - Does not remember user input, so no scroll back (case 811)
 * - Does not implement configuration file (case 167)
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_Configuration_h
#define Metis_metis_Configuration_h

#include <ccnx/forwarder/metis/core/metis_Forwarder.h>

struct metis_configuration;
typedef struct metis_configuration MetisConfiguration;

/**
 * <#One Line Description#>
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
MetisConfiguration *metisConfiguration_Create(MetisForwarder *metis);

/**
 * <#One Line Description#>
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
void metisConfiguration_Destroy(MetisConfiguration **configPtr);

void metisConfiguration_SetupAllListeners(MetisConfiguration *config, uint16_t port, const char *localPath);

/**
 * Receive a CPI control message from the user encapsulated in a MetisMessage
 *
 *   Takes ownership of the message, and will destroy it as needed.
 *
 * @param message is of type CCNX_MSG_CPI.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisConfiguration_Receive(MetisConfiguration *config, MetisMessage *message);

/**
 * Receives a CPI control message from the user
 *
 * Processes the message and generates the CPI control response.  The response should always
 * be non-null and must be released by the caller.
 *
 * @param [in] config Allocated MetisConfiguration
 * @param [in] request The CPI Request to process
 * @param [in] ingressId The ingress connection ID, used to track in logging messages
 *
 * @retval CCNxControl The response control message (an ACK, NACK, or Response).
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
CCNxControl *metisConfiguration_ReceiveControl(MetisConfiguration *config, CCNxControl *request, unsigned ingressId);

/**
 * <#One Line Description#>
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
PARCJSON *metisConfiguration_GetVersion(MetisConfiguration *config);

/**
 * <#One Line Description#>
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
void metisConfiguration_StartCLI(MetisConfiguration *config, uint16_t port);

/**
 * Returns the configured size of the content store
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
size_t metisConfiguration_GetObjectStoreSize(MetisConfiguration *config);

/**
 * Sets the size of the content store (in objects, not bytes)
 *
 * Must be set before starting the forwarder
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void   metisConfiguration_SetObjectStoreSize(MetisConfiguration *config, size_t maximumContentObjectCount);

/**
 * Returns the MetisForwarder that owns the MetisConfiguration
 *
 * Returns the Metis Forwarder.  Used primarily by associated classes in the
 * configuration group.
 *
 * @param [in] config An allocated MetisConfiguration
 *
 * @return non-null The owning MetisForwarder
 * @return null An error
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 */
MetisForwarder *metisConfiguration_GetForwarder(const MetisConfiguration *config);

/**
 * Returns the logger used by the Configuration subsystem
 *
 * Returns the logger specified when the MetisConfiguration was created.
 *
 * @param [in] config An allocated MetisConfiguration
 *
 * @retval non-null The logger
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisLogger *metisConfiguration_GetLogger(const MetisConfiguration *config);
#endif // Metis_metis_Configuration_h
