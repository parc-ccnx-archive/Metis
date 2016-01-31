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

#ifndef Metis_metis_ConnectionTable_h
#define Metis_metis_ConnectionTable_h

#include <ccnx/forwarder/metis/core/metis_Connection.h>
#include <ccnx/forwarder/metis/core/metis_ConnectionList.h>
#include <ccnx/forwarder/metis/io/metis_IoOperations.h>
#include <ccnx/forwarder/metis/io/metis_AddressPair.h>

struct metis_connection_table;
typedef struct metis_connection_table MetisConnectionTable;

/**
 * Creates an empty connection table
 *
 * <#Paragraphs Of Explanation#>
 *
 * @retval <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisConnectionTable *metisConnectionTable_Create(void);

/**
 * Destroys the connection table
 *
 * This will release the reference to all connections stored in the connection table.
 *
 * @param [in,out] conntablePtr Pointer to the allocated connection table, will be NULL'd
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisConnectionTable_Destroy(MetisConnectionTable **conntablePtr);

/**
 * @function metisConnectionTable_Add
 * @abstract Add a connection, takes ownership of memory
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 */
void metisConnectionTable_Add(MetisConnectionTable *table, MetisConnection *connection);

/**
 * @function metisConnectionTable_Remove
 * @abstract Removes the connection, calling Destroy on our copy
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 */
void metisConnectionTable_Remove(MetisConnectionTable *table, const MetisConnection *connection);

/**
 * Removes a connection from the connection table
 *
 * Looks up a connection by its connection ID and removes it from the connection table.
 * Removing the connection will call metisConnection_Release() on the connection object.
 *
 * @param [in] table The allocated connection table
 * @param [in] id The connection ID
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisConnectionTable_RemoveById(MetisConnectionTable *table, unsigned id);

/**
 * Lookup a connection by the (local, remote) addres pair
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] table The allocated connection table
 * @param [in] pair The address pair to match, based on the inner values of the local and remote addresses
 *
 * @retval non-null The matched conneciton
 * @retval null No match found or error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
const MetisConnection *metisConnectionTable_FindByAddressPair(MetisConnectionTable *table, const MetisAddressPair *pair);

/**
 * @function metisConnectionTable_FindById
 * @abstract Find a connection by its numeric id.
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return NULL if not found
 */
const MetisConnection *metisConnectionTable_FindById(MetisConnectionTable *table, unsigned id);

/**
 * @function metisConnectionTable_GetEntries
 * @abstract Returns a list of connections.  They are reference counted copies from the table.
 * @discussion
 *   An allocated list of connections in the table.  Each list entry is a reference counted
 *   copy of the connection in the table, thus they are "live" objects.
 *
 * @param <#param1#>
 * @return An allocated list, which you must destroy
 */
MetisConnectionList *metisConnectionTable_GetEntries(const MetisConnectionTable *table);
#endif // Metis_metis_ConnectionTable_h
