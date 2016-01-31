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

/*
 * The methods in this header are for the non-threaded forwarder.  They should only be called
 * within the forwarders thread of execution.
 */
#ifndef Metis_metis_Forwarder_h
#define Metis_metis_Forwarder_h

#include <sys/time.h>
#include <stdlib.h>

#include <ccnx/api/control/cpi_RouteEntry.h>

#include <ccnx/forwarder/metis/messenger/metis_Messenger.h>
#include <ccnx/forwarder/metis/core/metis_Dispatcher.h>
#include <ccnx/forwarder/metis/core/metis_ConnectionTable.h>

#include <ccnx/forwarder/metis/core/metis_Message.h>
#include <ccnx/forwarder/metis/core/metis_Ticks.h>
#include <ccnx/forwarder/metis/core/metis_Logger.h>
#include <ccnx/forwarder/metis/io/metis_ListenerSet.h>

#include <ccnx/forwarder/metis/processor/metis_FibEntryList.h>

#include <parc/algol/parc_Clock.h>


// hard coded port number for default listeners (case 868)
#define PORT_NUMBER 9695
#define PORT_NUMBER_AS_STRING "9695"

// ==============================================

struct metis_forwarder;
typedef struct metis_forwarder MetisForwarder;

// needs to be after the definition of MetisForwarder
#include <ccnx/forwarder/metis/config/metis_Configuration.h>

/**
 * @function metisForwarder_Create
 * @abstract Create the forwarder and use the provided logger for diagnostic output
 * @discussion
 *   If the logger is null, Metis will create a STDOUT logger.
 *
 * @param logger may be NULL
 * @return <#return#>
 */
MetisForwarder *metisForwarder_Create(MetisLogger *logger);

/**
 * @function metisForwarder_Destroy
 * @abstract Destroys the forwarder, stopping all traffic and freeing all memory
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisForwarder_Destroy(MetisForwarder **metisPtr);

/**
 * @function metisForwarder_SetupAllListeners
 * @abstract Setup all listeners (tcp, udp, local, ether, ip multicast) on all interfaces
 * @discussion
 *   Sets up all listeners on all running interfaces.  This provides a quick and easy
 *   startup, rather than providing a configuration file or programmatic commands.
 *
 * @param port is used by TCP and UDP listeners, in host byte order
 * @param localPath is the AF_UNIX path to use, if NULL no AF_UNIX listener is setup
 */
void metisForwarder_SetupAllListeners(MetisForwarder *forwarder, uint16_t port, const char *localPath);

/**
 * Configure Metis via a configuration file
 *
 * The configuration file is a set of lines, just like used in metis_control.
 * You need to have "add listener" lines in the file to receive connections.  No default
 * listeners are configured.
 *
 * @param [in] forwarder An alloated MetisForwarder
 * @param [in] filename The path to the configuration file
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisForwarder_SetupFromConfigFile(MetisForwarder *forwarder, const char *filename);

/**
 * Returns the logger used by this forwarder
 *
 * If you will store the logger, you should acquire a reference to it.
 *
 * @param [in] metis An allocated Metis forwarder
 *
 * @retval non-null The logger used by Metis
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisLogger *metisForwarder_GetLogger(const MetisForwarder *metis);

/**
 * @function metisForwarder_SetLogLevel
 * @abstract Sets the minimum level to log
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 */
void metisForwarder_SetLogLevel(MetisForwarder *metis, PARCLogLevel level);

/**
 * @function metisForwarder_GetNextConnectionId
 * @abstract Get the next identifier for a new connection
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
unsigned metisForwarder_GetNextConnectionId(MetisForwarder *metis);

MetisMessenger *metisForwarder_GetMessenger(MetisForwarder *metis);

MetisDispatcher *metisForwarder_GetDispatcher(MetisForwarder *metis);

/**
 * Returns the set of currently active listeners
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] metis An allocated Metis forwarder
 *
 * @retval non-null The set of active listeners
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisListenerSet *metisForwarder_GetListenerSet(MetisForwarder *metis);

/**
 * Returns the forwrder's connection table
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] metis An allocated Metis forwarder
 *
 * @retval non-null The connection tabler
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisConnectionTable *metisForwarder_GetConnectionTable(MetisForwarder *metis);

/**
 * Returns a Tick-based clock
 *
 * Runs at approximately 1 msec per tick (see METISHZ in metis_Forwarder.c).
 * Do not Release this clock.  If you save a copy of it, create your own
 * reference to it with parcClock_Acquire().
 *
 * @param [in] metis An allocated Metis forwarder
 *
 * @retval non-null An allocated Metis Clock based on the Tick counter
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
PARCClock *metisForwarder_GetClock(const MetisForwarder *metis);

/**
 * Direct call to get the Tick clock
 *
 * Runs at approximately 1 msec per tick (see METISHZ in metis_Forwarder.c)
 *
 * @param [in] metis An allocated Metis forwarder
 *
 * @retval <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisTicks metisForwarder_GetTicks(const MetisForwarder *metis);

/**
 * Convert nano seconds to Ticks
 *
 * Converts nano seconds to Ticks, based on METISHZ (in metis_Forwarder.c)
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
MetisTicks metisForwarder_NanosToTicks(uint64_t nanos);


uint64_t metisForwarder_TicksToNanos(MetisTicks ticks);

void metisForwarder_Receive(MetisForwarder *metis, MetisMessage *mesage);

/**
 * @function metisForwarder_AddOrUpdateRoute
 * @abstract Adds or updates a route on all the message processors
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
bool metisForwarder_AddOrUpdateRoute(MetisForwarder *metis, CPIRouteEntry *route);

/**
 * @function metisForwarder_RemoveRoute
 * @abstract Removes a route from all the message processors
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
bool metisForwarder_RemoveRoute(MetisForwarder *metis, CPIRouteEntry *route);

/**
 * Removes a connection id from all routes
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
void metisForwarder_RemoveConnectionIdFromRoutes(MetisForwarder *metis, unsigned connectionId);

/**
 * @function metisForwarder_GetConfiguration
 * @abstract The configuration object
 * @discussion
 *   The configuration contains all user-issued commands.  It does not include dynamic state.
 *
 * @param <#param1#>
 * @return <#return#>
 */
MetisConfiguration *metisForwarder_GetConfiguration(MetisForwarder *metis);

MetisFibEntryList *metisForwarder_GetFibEntries(MetisForwarder *metis);

/**
 * Sets the maximum number of content objects in the content store
 *
 * Implementation dependent - may wipe the cache.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisForwarder_SetContentObjectStoreSize(MetisForwarder *metis, size_t maximumContentStoreSize);

// ========================
// Functions to manipulate the event dispatcher

#include <ccnx/forwarder/metis/processor/metis_Tap.h>

/**
 * @function metisForwarder_AddTap
 * @abstract Add a diagnostic tap to see message events.
 * @discussion
 *   There can only be one tap at a time.  The most recent add wins.
 *
 * @param <#param1#>
 */
void metisForwarder_AddTap(MetisForwarder *metis, MetisTap *tap);

/**
 * @function metisForwarder_RemoveTap
 * @abstract Removes a message tap, no effect if it was not in effect
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 */
void metisForwarder_RemoveTap(MetisForwarder *metis, MetisTap *tap);
#endif // Metis_metis_Forwarder_h
