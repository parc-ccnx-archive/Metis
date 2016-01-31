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
 * @author Marc Mosko, Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#ifndef Metis_metis_LRUContentStore_h
#define Metis_metis_LRUContentStore_h

#include <stdio.h>
#include <ccnx/forwarder/metis/content_store/metis_ContentStoreInterface.h>
#include <ccnx/forwarder/metis/core/metis_Logger.h>

/**
 * Create and Initialize an instance of MetisLRUContentStore. A newly allocated {@link MetisContentStoreInterface}
 * object is initialized and returned. It must eventually be released by calling {@link metisContentStoreInterface_Release}.
 *
 *
 * @param config An instance of `MetisContentStoreConfig`, specifying options to be applied
 *               by the underlying MetisLRUContentStore instance.
 * @param logger An instance of a {@link MetisLogger} to use for logging content store events.
 *
 * @return a newly created MetisLRUContentStore instance.
 *
 * Example:
 * @code
 * {
 *     MetisContentStoreConfig config = {
 *         .objectCapacity = 10
 *     };
 *
 *     MetisContentStoreInterface *store = metisLRUContentStore_Create(&config, logger);
 *     assertTrue(status, "Expected to init a content store");
 *
 *     store->Display(&store);
 *     metisContentStoreInterface_Release(&store);
 * }
 * @endcode
 * @see MetisContentStoreInterface
 * @see metisContentStoreInterface_Release
 */
MetisContentStoreInterface *metisLRUContentStore_Create(MetisContentStoreConfig *config, MetisLogger *logger);
#endif // Metis_metis_LRUContentStore_h
