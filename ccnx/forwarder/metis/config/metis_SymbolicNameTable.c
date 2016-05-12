/*
 * Copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <ctype.h>

#include <LongBow/runtime.h>
#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_HashCodeTable.h>
#include <parc/algol/parc_Hash.h>

#include <ccnx/forwarder/metis/config/metis_SymbolicNameTable.h>

struct metis_symblic_name_table {
    PARCHashCodeTable *symbolicNameTable;
};

// ========================================================================================
// symbolic name table functions

static bool
_symbolicNameEquals(const void *keyA, const void *keyB)
{
    return (strcasecmp((const char *) keyA, (const char *) keyB) == 0);
}

static HashCodeType
_symbolicNameHash(const void *keyA)
{
    const char *str = (const char *) keyA;
    size_t length = strlen(str);
    return parcHash32_Data(str, length);
}

// ========================================================================================

MetisSymbolicNameTable *
metisSymbolicNameTable_Create(void)
{
    MetisSymbolicNameTable *table = parcMemory_Allocate(sizeof(MetisSymbolicNameTable));

    if (table) {
        // key = char *
        // value = uint32_t *
        table->symbolicNameTable = parcHashCodeTable_Create(_symbolicNameEquals, _symbolicNameHash, parcMemory_DeallocateImpl, parcMemory_DeallocateImpl);
    }

    return table;
}

void
metisSymbolicNameTable_Destroy(MetisSymbolicNameTable **tablePtr)
{
    MetisSymbolicNameTable *table = *tablePtr;
    parcHashCodeTable_Destroy(&table->symbolicNameTable);
    parcMemory_Deallocate((void **) &table);
    *tablePtr = NULL;
}

static char *
_createKey(const char *symbolicName)
{
    char *key = parcMemory_StringDuplicate(symbolicName, strlen(symbolicName));

    // convert key to upper case
    char *p = key;

    // keeps looping until the first null
    while ((*p = toupper(*p))) {
        p++;
    }
    return key;
}

bool
metisSymbolicNameTable_Exists(MetisSymbolicNameTable *table, const char *symbolicName)
{
    assertNotNull(table, "Parameter table must be non-null");
    assertNotNull(symbolicName, "Parameter symbolicName must be non-null");

    char *key = _createKey(symbolicName);
    bool found = (parcHashCodeTable_Get(table->symbolicNameTable, key) != NULL);
    parcMemory_Deallocate((void **) &key);
    return found;
}

bool
metisSymbolicNameTable_Add(MetisSymbolicNameTable *table, const char *symbolicName, unsigned connid)
{
    assertNotNull(table, "Parameter table must be non-null");
    assertNotNull(symbolicName, "Parameter symbolicName must be non-null");
    assertTrue(connid < UINT32_MAX, "Parameter connid must be less than %u", UINT32_MAX);

    char *key = _createKey(symbolicName);

    uint32_t *value = parcMemory_Allocate(sizeof(uint32_t));
    *value = connid;

    bool success = parcHashCodeTable_Add(table->symbolicNameTable, key, value);
    if (!success) {
        parcMemory_Deallocate((void **) &key);
        parcMemory_Deallocate((void **) &value);
    }

    return success;
}

unsigned
metisSymbolicNameTable_Get(MetisSymbolicNameTable *table, const char *symbolicName)
{
    assertNotNull(table, "Parameter table must be non-null");
    assertNotNull(symbolicName, "Parameter symbolicName must be non-null");

    unsigned connid = UINT32_MAX;

    char *key = _createKey(symbolicName);

    uint32_t *value = parcHashCodeTable_Get(table->symbolicNameTable, key);
    if (value) {
        connid = *value;
    }

    parcMemory_Deallocate((void **) &key);
    return connid;
}

