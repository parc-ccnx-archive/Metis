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
 * @file metis_MessageProcessor.h
 * @brief Executes the set of rules dictated by the PacketType
 *
 * This is a "run-to-completion" handling of a message based on the PacketType.
 *
 * The MessageProcessor also owns the PIT and FIB tables.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_MessageProcessor_h
#define Metis_metis_MessageProcessor_h

#include <ccnx/api/control/cpi_RouteEntry.h>
#include <ccnx/forwarder/metis/core/metis_Forwarder.h>
#include <ccnx/forwarder/metis/core/metis_Message.h>
#include <ccnx/forwarder/metis/processor/metis_Tap.h>
#include <ccnx/forwarder/metis/content_store/metis_ContentStoreInterface.h>

struct metis_message_processor;
typedef struct metis_message_processor MetisMessageProcessor;

/**
 * Allocates a MessageProcessor along with PIT, FIB and ContentStore tables
 *
 * The metis pointer is primarily used for logging (metisForwarder_Log), getting the
 * configuration, and accessing the connection table.
 *
 * @param [in] metis Pointer to owning Metis process
 *
 * @retval non-null An allocated message processor
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisMessageProcessor *metisMessageProcessor_Create(MetisForwarder *metis);

/**
 * Deallocates a message processor an all internal tables
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in,out] processorPtr Pointer to message processor to de-allocate, will be NULL'd.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisMessageProcessor_Destroy(MetisMessageProcessor **processorPtr);

/**
 * @function metisMessageProcessor_Receive
 * @abstract Process the message, takes ownership of the memory.
 * @discussion
 *   Will call destroy on the memory when done with it, so if the caller wants to
 *   keep it, make a reference counted copy.
 *
 *   Receive may modify some fields in the message, such as the HopLimit field.
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisMessageProcessor_Receive(MetisMessageProcessor *procesor, MetisMessage *message);

/**
 * @function metisMessageProcessor_AddTap
 * @abstract Add a tap to see messages.  Only one allowed. caller must remove and free it.
 * @discussion
 *   The tap will see messages on Receive, Drop, or Send, based on the properties of the Tap.
 *   The caller owns the memory and must remove and free it.
 *
 *   Currently only supports one tap.  If one is already set, its replaced.
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisMessageProcessor_AddTap(MetisMessageProcessor *procesor, MetisTap *tap);

/**
 * @function metisMessageProcessor_RemoveTap
 * @abstract Removes the tap from the message path.
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisMessageProcessor_RemoveTap(MetisMessageProcessor *procesor, const MetisTap *tap);

/**
 * Adds or updates a route in the FIB
 *
 * If the route already exists, it is replaced
 *
 * @param [in] procesor An allocated message processor
 * @param [in] route The route to update
 *
 * @retval true added or updated
 * @retval false An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisMessageProcessor_AddOrUpdateRoute(MetisMessageProcessor *procesor, CPIRouteEntry *route);

/**
 * Removes a route from the FIB
 *
 * Removes a specific nexthop for a route.  If there are no nexthops left after the
 * removal, the entire route is deleted from the FIB.
 *
 * @param [in] procesor An allocated message processor
 * @param [in] route The route to remove
 *
 * @retval true Route completely removed
 * @retval false There is still a nexthop for the route
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisMessageProcessor_RemoveRoute(MetisMessageProcessor *procesor, CPIRouteEntry *route);

/**
 * Removes a given connection id from all FIB entries
 *
 * Iterates the FIB and removes the given connection ID from every route.
 * If a route is left with no nexthops, it stays in the FIB, but packets that match it will
 * not be forwarded.  IS THIS THE RIGHT BEHAVIOR?
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisMessageProcessor_RemoveConnectionIdFromRoutes(MetisMessageProcessor *processor, unsigned connectionId);

/**
 * Returns a list of all FIB entries
 *
 * You must destroy the list.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @retval non-null The list of FIB entries
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisFibEntryList *metisMessageProcessor_GetFibEntries(MetisMessageProcessor *processor);

/**
 * Adjusts the ContentStore to the given size.
 *
 * This will destroy and re-create the content store, so any cached objects will be lost.
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisMessageProcessor_SetContentObjectStoreSize(MetisMessageProcessor *processor, size_t maximumContentStoreSize);

/**
 * Return the interface to the currently instantiated ContentStore, if any.
 *
 * @param [in] processor the `MetisMessageProcessor` from which to return the ContentStoreInterface.
 *
 * Example:
 * @code
 * {
 *     MetisContentStoreInterface *storeImpl = metisMessageProcessor_GetContentObjectStore(processor);
 *     size_t capacity = metisContentStoreInterface_GetObjectCapacity(storeImpl);
 * }
 * @endcode
 */
MetisContentStoreInterface *metisMessageProcessor_GetContentObjectStore(const MetisMessageProcessor *processor);
#endif // Metis_metis_MessageProcessor_h
