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
 * @file metis_SymbolicNameTable.h
 * @brief The symbolic name table maps a string name to a connection id
 *
 * When configuring tunnels/connections, the user provides a string name (symbolic name) that they
 * will use to refer to that connection.  The symblic name table translates that symbolic name
 * to a connection id.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef __Metis__metis_SymbolicNameTable__
#define __Metis__metis_SymbolicNameTable__

struct metis_symblic_name_table;
typedef struct metis_symblic_name_table MetisSymbolicNameTable;

#include <stdbool.h>

/**
 * Creates a symbolic name table
 *
 * Allocates a MetisSymbolicNameTable, which will store the symbolic names
 * in a hash table.
 *
 * @retval non-null An allocated MetisSymbolicNameTable
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisSymbolicNameTable *metisSymbolicNameTable_Create(void);

/**
 * Destroys a name table
 *
 * All keys and data are released.
 *
 * @param [in,out] tablePtr A pointer to a MetisSymbolicNameTable, which will be NULL'd
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisSymbolicNameTable_Destroy(MetisSymbolicNameTable **tablePtr);

/**
 * Tests if the name (case insensitive) is in the table
 *
 * Does a case-insensitive match to see if the name is in the table
 *
 * @param [in] table An allocated MetisSymbolicNameTable
 * @param [in] symbolicName The name to check for
 *
 * @retval true The name is in the table
 * @retval false The name is not in the talbe
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisSymbolicNameTable_Exists(MetisSymbolicNameTable *table, const char *symbolicName);

/**
 * Adds a (name, connid) pair to the table.
 *
 * The name is stored case insensitive.  The value UINT_MAX is used to indicate a
 * non-existent key, so it should not be stored as a value in the table.
 *
 * @param [in] table An allocated MetisSymbolicNameTable
 * @param [in] symbolicName The name to save (will make a copy)
 * @param [in] connid The connection id to associate with the name
 *
 * @retval true The pair was added
 * @retval false The pair was not added (likely duplicate key)
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisSymbolicNameTable_Add(MetisSymbolicNameTable *table, const char *symbolicName, unsigned connid);

/**
 * Returns the connection id associated with the symbolic name
 *
 * This function will look for the given name (case insensitive) and return the
 * corresponding connid.  If the name is not in the table, the function will return UINT_MAX.
 *
 * @param [in] table An allocated MetisSymbolicNameTable
 * @param [in] symbolicName The name to retrieve
 *
 * @retval UINT_MAX symbolicName not found
 * @retval number the corresponding connid.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
unsigned metisSymbolicNameTable_Get(MetisSymbolicNameTable *table, const char *symbolicName);

#endif /* defined(__Metis__metis_SymbolicNameTable__) */
