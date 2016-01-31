/*
 * Copyright (c) 2013, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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

//
//  metis_WebInterface.h
//  Metis
//
//  Created by Mosko, Marc <Marc.Mosko@parc.com> on 12/3/13.
//
//

#ifndef Metis_metis_WebInterface_h
#define Metis_metis_WebInterface_h

#include <ccnx/forwarder/metis/core/metis_Forwarder.h>

struct metis_web;
typedef struct metis_web MetisWeb;

/**
 * Creates a Web on the given port.
 *
 *   A http interface.  The Web interface is created in the STOPPED mode, so
 *   you need to start it for it to be usable.
 *
 *   Create will bind the port, but callbacks in the dispatcher will not be
 *   enabled until it is started.
 *
 * @param port the command port, in host byte order
 * @return NULL if cannot be created on the port
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisWeb *metisWeb_Create(MetisForwarder *metis, uint16_t port);

/**
 * Stops and destroys the web interface.  Existing sessions are destroyed.
 *
 *   <#Discussion#>
 *
 * @param <#param1#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisWeb_Destroy(MetisWeb **WebPtr);

/**
 * Enables the web interface in the event dispatcher.
 *
 *   <#Discussion#>
 *
 * @param <#param1#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisWeb_Start(MetisWeb *web);

/**
 * Disables callback in the event dispatcher.  Existing connections unaffected.
 *
 *   Stopping it only disable accepting new connections.
 *
 * @param <#param1#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisWeb_Stop(MetisWeb *web);
#endif // Metis_metis_WebInterface_h
