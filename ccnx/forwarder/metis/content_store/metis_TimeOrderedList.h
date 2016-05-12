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
 * @author Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#ifndef __Metis__metis_TimeOrderedList__
#define __Metis__metis_TimeOrderedList__

#include <stdio.h>
#include <ccnx/forwarder/metis/core/metis_Message.h>
#include <ccnx/forwarder/metis/content_store/metis_ContentStoreEntry.h>
#include <parc/algol/parc_TreeRedBlack.h>

struct metis_timeordered_list;
typedef struct metis_timeordered_list MetisTimeOrderedList;

/**
 * A signum function that takes two instances of MetisContentStoreEntrys and
 * returns a value based on their relative values.
 */
typedef PARCTreeRedBlack_KeyCompare MetisTimeOrderList_KeyCompare;

/**
 * Create a new instance of `MetisTimeOrderedList` that will maintain the order of its
 * list items using the supplied `keyCompareFunction`.
 *
 * The newly created `MetisTimeOrderedList` must eventually be released by calling
 * {@link metisTimeOrderedList_Release}.
 *
 * @param keyCompareFunction the signum comparison function to use to sort stored items.
 * @return a new instance of `MetisTimeOrderList`.
 * @return NULL if the new instance couldn't be created.
 *
 * Example:
 * @code
 * {
 *     MetisTimeOrderedList *list =
 *         metisTimeOrderedList_Create((MetisTimeOrderList_KeyCompare *) metisContentStoreEntry_CompareExpiryTime);
 *
 *     ...
 *
 *     metisTimeOrderedList_Release(&list);
 * }
 * @endcode
 *
 * @see metisTimeOrderedList_Release
 * @see metisContentStoreEntry_CompareExpiryTime
 * @see metisContentStoreEntry_CompareRecommendedCacheTime
 */
MetisTimeOrderedList *metisTimeOrderedList_Create(MetisTimeOrderList_KeyCompare *keyCompareFunction);

/**
 * Release a previously acquired reference to the specified instance,
 * decrementing the reference count for the instance.
 *
 * The pointer to the instance is set to NULL as a side-effect of this function.
 *
 * If the invocation causes the last reference to the instance to be released,
 * the instance is deallocated and the instance's implementation will perform
 * additional cleanup and release other privately held references.
 *
 * @param [in,out] listP A pointer to a pointer to the instance to release.
 * Example:
 * @code
 * {
 *     MetisTimeOrderedList *list =
 *         metisTimeOrderedList_Create((MetisTimeOrderList_KeyCompare *) metisContentStoreEntry_CompareExpiryTime);
 *
 *     ...
 *
 *     metisTimeOrderedList_Release(&list);
 * }
 * @endcode
 */
void metisTimeOrderedList_Release(MetisTimeOrderedList **listP);

/**
 * Add a {@link MetisContentStoreEntry} instance to the specified list. Note that a new refernece to
 * the specified `storeEntry` is not acquired.
 *
 * @param list the list instance into which to add the specified storeEntry.
 * @param storeEntry the storeEntry instance to add.
 *
 * Example:
 * @code
 * {
 *     MetisLogger *logger = metisLogger_Create(...);
 *     MetisTimeOrderedList *list =
 *         metisTimeOrderedList_Create((MetisTimeOrderList_KeyCompare *) metisContentStoreEntry_CompareExpiryTime);
 *     MetisLruList *lruList = metisLruList_Create();
 *     MetisMessage *message = metisMessage_CreateFromArray((uint8_t *) "\x00" "ehlo", 5, 111, 2, logger);
 *
 *     MetisContentStoreEntry *entry = metisContentStoreEntry_Create(message, lruList, 2l, 1l);
 *
 *     metisTimeOrderedList_Add(list, entry);
 *
 *     metisTimeOrderedList_Release(&list);
 *
 *     metisContentStoreEntry_Release(&entry);
 *     metisMessage_Release(&message);
 *     metisLruList_Destroy(&lruList);
 *     metisLogger_Release(&logger);
 * }
 * @endcode
 * @see metisTimeOrderedList_Remove
 */
void metisTimeOrderedList_Add(MetisTimeOrderedList *list, MetisContentStoreEntry *storeEntry);

/**
 * Remove a {@link MetisContentStoreEntry} instance from the specified list.
 *
 * @param list the list instance from which to remove the specified storeEntry.
 * @param storeEntry the storeEntry instance to remove.
 * @return true if the removal was succesful.
 * @return false if the removal was not succesful.
 *
 * Example:
 * @code
 * {
 *     MetisLogger *logger = metisLogger_Create(...);
 *     MetisTimeOrderedList *list =
 *         metisTimeOrderedList_Create((MetisTimeOrderList_KeyCompare *) metisContentStoreEntry_CompareExpiryTime);
 *     MetisMessage *message = metisMessage_CreateFromArray((uint8_t *) "\x00" "ehlo", 5, 111, 2, logger);
 *
 *     MetisContentStoreEntry *entry = metisContentStoreEntry_Create(message, NULL, 2l, 1l);
 *
 *     metisTimeOrderedList_Add(list, entry);
 *
 *        ...
 *
 *     metisTimeOrderedList_Remove(list, entry);
 *     metisTimeOrderedList_Release(&list);
 *
 *     metisContentStoreEntry_Release(&entry);
 *     metisMessage_Release(&message);
 *     metisLogger_Release(&logger);
 * }
 * @endcode
 * @see metisTimeOrderedList_Add
 */
bool metisTimeOrderedList_Remove(MetisTimeOrderedList *list, MetisContentStoreEntry *storeEntry);

/**
 * Return the oldest {@link MetisContentStoreEntry} instance in this list. That is, the one with the smallest
 * time value.
 *
 * @param list the list instance from which to retrieve the oldest storeEntry.
 * @param the oldest `MetisContentStoreEntry` in the list
 * @param NULL if no `MetisContentStoreEntry` was available.
 *
 * Example:
 * @code
 * {
 *     MetisLogger *logger = metisLogger_Create(...);
 *     MetisTimeOrderedList *list =
 *         metisTimeOrderedList_Create((MetisTimeOrderList_KeyCompare *) metisContentStoreEntry_CompareExpiryTime);
 *     MetisLruList *lruList = metisLruList_Create();
 *     MetisMessage *message = metisMessage_CreateFromArray((uint8_t *) "\x00" "ehlo", 5, 111, 2, logger);
 *
 *     MetisContentStoreEntry *entry = metisContentStoreEntry_Create(message, lruList, 2l, 1l);
 *
 *     metisTimeOrderedList_Add(list, entry);
 *
 *     MetisContentStoreEntry *oldest = metisTimeOrderedList_GetOldest(list);
 *
 *     ...
 *
 *     metisTimeOrderedList_Release(&list);
 *
 *     metisContentStoreEntry_Release(&entry);
 *     metisMessage_Release(&message);
 *     metisLruList_Destroy(&lruList);
 *     metisLogger_Release(&logger);
 * }
 * @endcode
 * @see metisTimeOrderedList_Remove
 */
MetisContentStoreEntry *metisTimeOrderedList_GetOldest(MetisTimeOrderedList *list);

/**
 * Return the number of items currently stored in the list.
 *
 * @param list the `MetisTimeOrderedList` instance from which to retrieve the count.
 * @return the number of items in the list.
 *
 * Example:
 * @code
 * {
 *     MetisLogger *logger = metisLogger_Create(...);
 *     MetisTimeOrderedList *list =
 *         metisTimeOrderedList_Create((MetisTimeOrderList_KeyCompare *) metisContentStoreEntry_CompareExpiryTime);
 *     MetisLruList *lruList = metisLruList_Create();
 *     MetisMessage *message = metisMessage_CreateFromArray((uint8_t *) "\x00" "ehlo", 5, 111, 2, logger);
 *
 *     MetisContentStoreEntry *entry = metisContentStoreEntry_Create(message, lruList, 2l, 1l);
 *
 *     metisTimeOrderedList_Add(list, entry);
 *
 *     ...
 *     size_t numItemsInList = metisTimeOrderedList_Length(list);
 *     ...
 *
 *     metisTimeOrderedList_Release(&list);
 *
 *     metisContentStoreEntry_Release(&entry);
 *     metisMessage_Release(&message);
 *     metisLruList_Destroy(&lruList);
 *     metisLogger_Release(&logger);
 * }
 * @endcode
 */
size_t metisTimeOrderedList_Length(MetisTimeOrderedList *list);
#endif /* defined(__Metis__metis_TimeOrderedList__) */
