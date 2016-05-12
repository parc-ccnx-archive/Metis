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
 * @file metis_FibEntryList.h
 * @brief A typesafe list of MetisFibEntry
 *
 * <#Detailed Description#>
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_FibEntryList_h
#define Metis_metis_FibEntryList_h

#include <ccnx/forwarder/metis/processor/metis_FibEntry.h>

struct metis_fib_entry_list;
typedef struct metis_fib_entry_list MetisFibEntryList;

/**
 * Creates an emtpy FIB entry list
 *
 * Must be destroyed with metisFibEntryList_Destroy.
 *
 * @retval non-null An allocated MetisFibEntryList
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisFibEntryList *metisFibEntryList_Create(void);

/**
 * @function MetisFibEntryList_Detroy
 * @abstract Destroys the list and all entries.
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 */
void metisFibEntryList_Destroy(MetisFibEntryList **listPtr);

/**
 * @function metisFibEntryList_Append
 * @abstract Will store a reference counted copy of the entry.
 * @discussion
 *   Will create and store a reference counted copy.  You keep ownership
 *   of the parameter <code>fibEntry</code>.
 *
 * @param <#param1#>
 * @return <#return#>
 */
void metisFibEntryList_Append(MetisFibEntryList *list, MetisFibEntry *fibEntry);

/**
 * Returns the number of entries in the list
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] list An allocated MetisFibEntryList
 *
 * @retval number The number of entries in the list
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
size_t metisFibEntryList_Length(const MetisFibEntryList *list);

/**
 * @function metisFibEntryList_Get
 * @abstract Gets an element.  This is the internal reference, do not destroy.
 * @discussion
 *   Returns an internal reference from the list.  You must not destroy it.
 *   Will assert if you go off the end of the list.
 *
 * @param <#param1#>
 * @return <#return#>
 */
const MetisFibEntry *metisFibEntryList_Get(const MetisFibEntryList *list, size_t index);
#endif // Metis_metis_FibEntryList_h
