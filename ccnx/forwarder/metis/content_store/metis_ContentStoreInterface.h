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

#ifndef Metis_metis_ContentStoreInterface_h
#define Metis_metis_ContentStoreInterface_h

#include <stdio.h>

#include <ccnx/forwarder/metis/core/metis_Message.h>

typedef struct metis_contentstore_config {
    size_t objectCapacity;
} MetisContentStoreConfig;

typedef struct metis_contentstore_interface MetisContentStoreInterface;

struct metis_contentstore_interface {
    /**
     * Place a MetisMessage representing a ContentObject into the ContentStore. If necessary to make room,
     * remove expired content or content that has exceeded the Recommended Cache Time.
     *
     * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
     * @param content - a pointer to a `MetisMessage` to place in the store.
     * @param currentTimeTicks - the current time, in metis ticks, since the UTC epoch.
     */
    bool (*putContent)(MetisContentStoreInterface *storeImpl, MetisMessage *content, uint64_t currentTimeTicks);

    /**
     * The function to call to remove content from the ContentStore.
     * It will Release any references that were created when the content was placed into the ContentStore.
     *
     * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
     * @param content - a pointer to a `MetisMessage` to remove from the store.
     */
    bool (*removeContent)(MetisContentStoreInterface *storeImpl, MetisMessage *content);

    /**
     * Given a MetisMessage that represents and Interest, try to find a matching ContentObject. Matching is
     * done on a most-restrictive basis.
     *
     *  a) If the interest has a ContentObjectHash restriction, it will match on the Name and the Object Hash.
     *  b) If it has a KeyId, it will match on the Name and the KeyId
     *  c) otherwise, it matches by Name
     *
     * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
     * @param interest - a pointer to a `MetisMessage` representing the Interest to match.
     *
     * @return a pointer to a MetisMessage containing the matching ContentObject
     * @return NULL if no matching ContentObject was found
     */
    MetisMessage * (*matchInterest)(MetisContentStoreInterface*storeImpl, MetisMessage *interest);

    /**
     * Return the maximum number of ContentObjects that can be stored in this ContentStore. This is a raw
     * count, not based on memory size.
     *
     * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
     *
     * @return the maximum number of ContentObjects that can be stored
     */
    size_t (*getObjectCapacity)(MetisContentStoreInterface *storeImpl);

    /**
     * Return the number of ContentObjects currently stored in the ContentStore.
     *
     * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
     *
     * @return the current number of ContentObjects in the ContentStore
     */
    size_t (*getObjectCount)(MetisContentStoreInterface *storeImpl);

    /**
     * Loga ContentStore implementation specific version of store-related information.
     *
     * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
     */
    void (*log)(MetisContentStoreInterface *storeImpl);

    /**
     * Acquire a new reference to the specified ContentStore instance. This reference will eventually need
     * to be released by calling {@link metisContentStoreInterface_Release}.
     *
     * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
     */
    MetisContentStoreInterface *(*acquire)(const MetisContentStoreInterface *storeImpl);

    /**
     * Release the ContentStore, which will also Release any references held by it.
     *
     * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
     */
    void (*release)(MetisContentStoreInterface **storeImpl);

    /**
     * A pointer to opaque private data used by the ContentStore instance represented by this instance of
     * MetisContentStoreInterface.
     */
    void *_privateData;
};

/**
 * Place a MetisMessage representing a ContentObject into the ContentStore. If necessary to make room,
 * remove expired content or content that has exceeded the Recommended Cache Time.
 *
 * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
 * @param content - a pointer to a `MetisMessage` to place in the store.
 *
 * @param currentTimeTicks - the current time, in metis ticks, since the UTC epoch.
 */
bool metisContentStoreInterface_PutContent(MetisContentStoreInterface *storeImpl, MetisMessage *content, uint64_t currentTimeTicks);

/**
 * The function to call to remove content from the ContentStore.
 * It will Release any references that were created when the content was placed into the ContentStore.
 *
 * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
 * @param content - a pointer to a `MetisMessage` to remove from the store.
 */
bool metisContentStoreInterface_RemoveContent(MetisContentStoreInterface *storeImpl, MetisMessage *content);

/**
 * Given a MetisMessage that represents and Interest, try to find a matching ContentObject. Matching is
 * done on a most-restrictive basis.
 *
 *  a) If the interest has a ContentObjectHash restriction, it will match on the Name and the Object Hash.
 *  b) If it has a KeyId, it will match on the Name and the KeyId
 *  c) otherwise, it matches by Name
 *
 * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
 * @param interest - a pointer to a `MetisMessage` representing the Interest to match.
 *
 * @return a pointer to a MetisMessage containing the matching ContentObject
 * @return NULL if no matching ContentObject was found
 */
MetisMessage *metisContentStoreInterface_MatchInterest(MetisContentStoreInterface*storeImpl, MetisMessage *interest);

/**
 * Return the maximum number of ContentObjects that can be stored in this ContentStore. This is a raw
 * count, not based on memory size.
 *
 * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
 *
 * @return the maximum number of ContentObjects that can be stored
 */
size_t metisContentStoreInterface_GetObjectCapacity(MetisContentStoreInterface *storeImpl);

/**
 * Return the number of ContentObjects currently stored in the ContentStore.
 *
 * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
 *
 * @return the current number of ContentObjects in the ContentStore
 */
size_t metisContentStoreInterface_GetObjectCount(MetisContentStoreInterface *storeImpl);

/**
 * Loga ContentStore implementation specific version of store-related information.
 *
 * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
 */
void metisContentStoreInterface_Log(MetisContentStoreInterface *storeImpl);

/**
 * Acquire a new reference to the specified ContentStore instance. This reference will eventually need
 * to be released by calling {@link metisContentStoreInterface_Release}.
 *
 * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
 */
MetisContentStoreInterface *metisContentStoreInterface_Aquire(const MetisContentStoreInterface *storeImpl);

/**
 * Release the ContentStore, which will also Release any references held by it.
 *
 * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
 */
void metisContentStoreInterface_Release(MetisContentStoreInterface **storeImplPtr);

/**
 * Return a pointer to the data private to this implementation of the ContentStore interface.
 *
 * @param storeImpl - a pointer to this MetisContentStoreInterface instance.
 */
void *metisContentStoreInterface_GetPrivateData(MetisContentStoreInterface *storeImpl);
#endif // Metis_metis_ContentStoreInterface_h
