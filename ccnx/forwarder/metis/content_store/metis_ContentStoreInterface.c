/*
 * Copyright (c) 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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
 * @author Alan Walendowski, Palo Alto Research Center (Xerox PARC)
 * @copyright 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>

#include <ccnx/forwarder/metis/content_store/metis_ContentStoreInterface.h>

void
metisContentStoreInterface_Release(MetisContentStoreInterface **storeImplPtr)
{
    (*storeImplPtr)->release(storeImplPtr);
}

bool
metisContentStoreInterface_PutContent(MetisContentStoreInterface *storeImpl, MetisMessage *content, uint64_t currentTimeTicks)
{
    return storeImpl->putContent(storeImpl, content, currentTimeTicks);
}

bool
metisContentStoreInterface_RemoveContent(MetisContentStoreInterface *storeImpl, MetisMessage *content)
{
    return storeImpl->removeContent(storeImpl, content);
}

MetisMessage *
metisContentStoreInterface_MatchInterest(MetisContentStoreInterface *storeImpl, MetisMessage *interest)
{
    return storeImpl->matchInterest(storeImpl, interest);
}

size_t
metisContentStoreInterface_GetObjectCapacity(MetisContentStoreInterface *storeImpl)
{
    return storeImpl->getObjectCapacity(storeImpl);
}

size_t
metisContentStoreInterface_GetObjectCount(MetisContentStoreInterface *storeImpl)
{
    return storeImpl->getObjectCount(storeImpl);
}

void
metisContentStoreInterface_Log(MetisContentStoreInterface *storeImpl)
{
    storeImpl->log(storeImpl);
}

void *
metisContentStoreInterface_GetPrivateData(MetisContentStoreInterface *storeImpl)
{
    return storeImpl->_privateData;
}
