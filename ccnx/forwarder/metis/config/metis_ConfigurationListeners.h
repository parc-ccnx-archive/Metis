/*
 * Copyright (c) 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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
 * @file metis_ConfigurationListeners.h
 * @brief Configuration routines related to Listeners
 *
 * Adding and removing listeners.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#ifndef Metis__metis_ConfigurationListeners_h
#define Metis__metis_ConfigurationListeners_h

#include <ccnx/forwarder/metis/core/metis_Forwarder.h>
#include <ccnx/forwarder/metis/config/metis_Configuration.h>

#include <ccnx/api/control/cpi_Address.h>
#include <ccnx/api/control/cpi_ControlMessage.h>

/**
 * Setup udp, tcp, and local listeners
 *
 *   Will bind to all available IP protocols on the given port.
 *   Does not add Ethernet listeners.
 *
 * @param port is the UPD and TCP port to use
 * @param localPath is the AF_UNIX path to use, if NULL no AF_UNIX listener is setup
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisConfigurationListeners_SetupAll(const MetisConfiguration *config, uint16_t port, const char *localPath);

bool metisConfigurationListeners_Add(const MetisConfiguration *config, const CCNxControl *control, unsigned ingressId);
bool metisConfigurationListeners_Remove(const MetisConfiguration *config, const CCNxControl *control, unsigned ingressId);


#endif /* defined(Metis__metis_ConfigurationListeners_h) */
