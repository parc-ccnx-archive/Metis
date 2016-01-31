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
/**
 * @header Metis Threaded Forwarder
 * @abstract This is a wrapper around metis_Forwarder to run it as a thread
 * @discussion
 *     <#Discussion#>
 *
 * @author Marc Mosko on 12/17/13
 * @copyright 2013, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#ifndef Metis_metis_ThreadedForwarder_h
#define Metis_metis_ThreadedForwarder_h

#include <ccnx/forwarder/metis/core/metis_Forwarder.h>

struct metis_threaded_forwarder;
typedef struct metis_threaded_forwarder MetisThreadedForwarder;

/**
 * @function metisThreadedForwarder_Create
 * @abstract Creates a threaded forwarder in the stopped state
 * @discussion
 *   IMPORTANT: The logger is called from the Metis thread, so it is up to
 *   the user to implement any necessary thread saftey in the logger.  There
 *   is only a single metis thread, so it does not need to be re-enterent.
 *
 * @param <#param1#>
 * @return <#return#>
 */
MetisThreadedForwarder *metisThreadedForwarder_Create(MetisLogger *logger);

/**
 * @function metisThreadedForwarder_AddCLI
 * @abstract Add a command line interface (CLI) on the given port
 * @discussion
 *   MUST BE DONE PRIOR TO START.  This function will add a CLI to the forwarder
 *   prior to starting it.  Once started, will assert if you try to do this.
 *
 * @param <#param1#>
 */
void metisThreadedForwarder_AddCLI(MetisThreadedForwarder *metis, uint16_t port);

/**
 * @function metisThreadedForwarder_AddTcpListener
 * @abstract Adds a TCP listenener
 * @discussion
 *   MUST BE DONE PRIOR TO START.
 *   May be IPv4 or IPv6
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisThreadedForwarder_AddTcpListener(MetisThreadedForwarder *metis, struct sockaddr *address);

/**
 * @function metisThreadedForwarder_SetupAllListeners
 * @abstract Setup all tcp/udp ipv4/ipv6 listeners on the given port
 * @discussion
 *   MUST BE DONE PRIOR TO START.
 *
 * @param port is the UDP and TCP port
 * @param localPath is the AF_UNIX path, may be NULL for no AF_UNIX socket.
 * @return <#return#>
 */
void metisThreadedForwarder_SetupAllListeners(MetisThreadedForwarder *metis, uint16_t port, const char *localPath);

/**
 * @function metisThreadedForwarder_Start
 * @abstract Blocks until started
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 */
void metisThreadedForwarder_Start(MetisThreadedForwarder *metis);

/**
 * @function metisThreadedForwarder_Stop
 * @abstract Blocks until stopped
 * @discussion
 *   Currently we do not support re-starting a thread after it is stopped.
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisThreadedForwarder_Stop(MetisThreadedForwarder *metis);

/**
 * @function metisThreadedForwarder_Destroy
 * @abstract Blocks until stopped and destoryed
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisThreadedForwarder_Destroy(MetisThreadedForwarder **metisPtr);
#endif // Metis_metis_ThreadedForwarder_h
