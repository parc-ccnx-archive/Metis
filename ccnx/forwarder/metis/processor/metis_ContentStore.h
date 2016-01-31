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

//
//  content_store.h
//  tlvrtr
//
//  Created by Mosko, Marc <Marc.Mosko@parc.com> on 7/26/13.
//  Copyright (c) 2013 Marc Mosko. All rights reserved.

#ifndef Metis_metis_ContentStore_h
#define Metis_metis_ContentStore_h

#include <ccnx/forwarder/metis/core/metis_Message.h>
#include <ccnx/forwarder/metis/core/metis_Logger.h>

struct metis_contentstore;
typedef struct metis_contentstore MetisContentStore;

MetisContentStore *metisContentStore_Create(size_t objectCapacity, MetisLogger *logger);
void metisContentStore_Destroy(MetisContentStore **storePtr);

/**
 * @function metisContentStore_Save
 * @abstract Saves content object in the store
 * @discussion
 *   Will make a reference counted copy of the message, so caller retains ownership of original message.
 *
 * @param <#param1#>
 * @return True if saved, false othewise
 */
bool metisContentStore_Save(MetisContentStore *store, MetisMessage *objectMessage);

/**
 * @function metisContentStore_Fetch
 * @abstract Fetch a content object from the store that matches the interest message
 * @discussion
 *   Returns a reference counted copy, caller must Destroy it.
 *
 * @param <#param1#>
 * @return May be NULL if no match
 */
MetisMessage *metisContentStore_Fetch(MetisContentStore *store, MetisMessage *interestMessage);

#endif // Metis_metis_ContentStore_h
