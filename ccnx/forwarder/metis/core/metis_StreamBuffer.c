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
//  metis_StreamBuffer.c
//  Metis
//
//  Created by Mosko, Marc <Marc.Mosko@parc.com> on 11/22/13.

#include <config.h>
#include <stdio.h>

#include <LongBow/runtime.h>

#include <ccnx/forwarder/metis/core/metis_StreamBuffer.h>

void
metisStreamBuffer_Destroy(PARCEventQueue **bufferPtr)
{
    assertNotNull(bufferPtr, "Parameter must be non-null double pointer");
    assertNotNull(*bufferPtr, "Parameter must dereference to non-null pointer");
    parcEventQueue_Destroy(bufferPtr);
    *bufferPtr = NULL;
}

void
metisStreamBuffer_SetWatermark(PARCEventQueue *buffer, bool setRead, bool setWrite, size_t low, size_t high)
{
    assertNotNull(buffer, "Parameter buffer must be non-null");

    short flags = 0;
    if (setRead) {
        flags |= PARCEventType_Read;
    }

    if (setWrite) {
        flags |= PARCEventType_Write;
    }

    parcEventQueue_SetWatermark(buffer, flags, low, high);
}

int
metisStreamBuffer_Flush(PARCEventQueue *buffer, bool flushRead, bool flushWrite)
{
    assertNotNull(buffer, "Parameter buffer must be non-null");

    short flags = 0;
    if (flushRead) {
        flags |= PARCEventType_Read;
    }

    if (flushWrite) {
        flags |= PARCEventType_Write;
    }

    return parcEventQueue_Flush(buffer, flags);
}

// NOT USED!!
int
metisStreamBuffer_FlushCheckpoint(PARCEventQueue *buffer, bool flushRead, bool flushWrite)
{
    assertNotNull(buffer, "Parameter buffer must be non-null");

    short flags = 0;
    if (flushRead) {
        flags |= PARCEventType_Read;
    }

    if (flushWrite) {
        flags |= PARCEventType_Write;
    }

    return parcEventQueue_Flush(buffer, flags);
}

// NOT USED!!
int
metisStreamBuffer_FlushFinished(PARCEventQueue *buffer, bool flushRead, bool flushWrite)
{
    assertNotNull(buffer, "Parameter buffer must be non-null");

    short flags = 0;
    if (flushRead) {
        flags |= PARCEventType_Read;
    }

    if (flushWrite) {
        flags |= PARCEventType_Write;
    }

    return parcEventQueue_Flush(buffer, flags);
}

void
metisStreamBuffer_SetCallbacks(PARCEventQueue *buffer,
                               PARCEventQueue_Callback *readCallback,
                               PARCEventQueue_Callback *writeCallback,
                               PARCEventQueue_EventCallback *eventCallback,
                               void *user_data)
{
    assertNotNull(buffer, "Parameter buffer must be non-null");

    parcEventQueue_SetCallbacks(buffer, readCallback, writeCallback, eventCallback, user_data);
}

void
metisStreamBuffer_EnableCallbacks(PARCEventQueue *buffer, bool enableRead, bool enableWrite)
{
    assertNotNull(buffer, "Parameter buffer must be non-null");
    short flags = 0;
    if (enableRead) {
        flags |= PARCEventType_Read;
    }
    if (enableWrite) {
        flags |= PARCEventType_Write;
    }

    parcEventQueue_Enable(buffer, flags);
}

/**
 * @function MetisStreamBuffer_DisableCallbacks
 * @abstract Disables specified callbacks.  Does not affect others.
 * @discussion
 *   Disables enabled callbacks.  If a callback is already disabled, has no effect.
 *   A "false" value does not enable it.
 *
 * @param <#param1#>
 * @return <#return#>
 */
void
metisStreamBuffer_DisableCallbacks(PARCEventQueue *buffer, bool disableRead, bool disableWrite)
{
    assertNotNull(buffer, "Parameter buffer must be non-null");
    short flags = 0;
    if (disableRead) {
        flags |= PARCEventType_Read;
    }
    if (disableWrite) {
        flags |= PARCEventType_Write;
    }

    parcEventQueue_Disable(buffer, flags);
}
