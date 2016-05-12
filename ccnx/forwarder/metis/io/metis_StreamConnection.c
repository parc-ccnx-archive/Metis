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
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

/**
 * Common activity for STREAM based listeners.
 */

#include <config.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <ccnx/forwarder/metis/io/metis_StreamConnection.h>
#include <ccnx/forwarder/metis/core/metis_Forwarder.h>
#include <ccnx/forwarder/metis/core/metis_Connection.h>
#include <ccnx/forwarder/metis/core/metis_Message.h>
#include <parc/algol/parc_Hash.h>

#include <LongBow/runtime.h>
#include <parc/algol/parc_Memory.h>
#include <ccnx/forwarder/metis/tlv/metis_Tlv.h>

// 128 KB output queue
#define OUTPUT_QUEUE_BYTES  (128 * 1024)

static void
_conn_readcb(PARCEventQueue *bufferEventVector, PARCEventType type, void *ioOpsVoid);

static void
_conn_eventcb(PARCEventQueue *bufferEventVector, PARCEventQueueEventType events, void *ioOpsVoid);

typedef struct metis_stream_state {
    MetisForwarder *metis;
    MetisLogger *logger;

    int fd;

    MetisAddressPair *addressPair;
    PARCEventQueue *bufferEventVector;

    bool isLocal;
    bool isUp;
    bool isClosed;
    unsigned id;

    size_t nextMessageLength;
} _MetisStreamState;

// Prototypes
static bool                     _metisStreamConnection_Send(MetisIoOperations *ops, const CPIAddress *nexthop, MetisMessage *message);
static const CPIAddress        *_metisStreamConnection_GetRemoteAddress(const MetisIoOperations *ops);
static const MetisAddressPair  *_metisStreamConnection_GetAddressPair(const MetisIoOperations *ops);
static unsigned                 _metisStreamConnection_GetConnectionId(const MetisIoOperations *ops);
static bool                     _metisStreamConnection_IsUp(const MetisIoOperations *ops);
static bool                     _metisStreamConnection_IsLocal(const MetisIoOperations *ops);
static void                     _metisStreamConnection_DestroyOperations(MetisIoOperations **opsPtr);

static void                     _setConnectionState(_MetisStreamState *stream, bool isUp);
static CPIConnectionType        _metisStreamConnection_GetConnectionType(const MetisIoOperations *ops);

/*
 * This assigns a unique pointer to the void * which we use
 * as a GUID for this class.
 */
static const void *_metisIoOperationsGuid = __FILE__;

/*
 * Return our GUID
 */
static const void *
_metisStreamConnection_Class(const MetisIoOperations *ops)
{
    return _metisIoOperationsGuid;
}

static MetisIoOperations _template = {
    .closure           = NULL,
    .send              = &_metisStreamConnection_Send,
    .getRemoteAddress  = &_metisStreamConnection_GetRemoteAddress,
    .getAddressPair    = &_metisStreamConnection_GetAddressPair,
    .getConnectionId   = &_metisStreamConnection_GetConnectionId,
    .isUp              = &_metisStreamConnection_IsUp,
    .isLocal           = &_metisStreamConnection_IsLocal,
    .destroy           = &_metisStreamConnection_DestroyOperations,
    .class             = &_metisStreamConnection_Class,
    .getConnectionType = &_metisStreamConnection_GetConnectionType
};

MetisIoOperations *
metisStreamConnection_AcceptConnection(MetisForwarder *metis, int fd, MetisAddressPair *pair, bool isLocal)
{
    _MetisStreamState *stream = parcMemory_AllocateAndClear(sizeof(_MetisStreamState));
    assertNotNull(stream, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(_MetisStreamState));

    MetisDispatcher *dispatcher = metisForwarder_GetDispatcher(metis);
    PARCEventScheduler *eventBase = metisDispatcher_GetEventScheduler(dispatcher);
    stream->bufferEventVector = parcEventQueue_Create(eventBase, fd, PARCEventQueueOption_CloseOnFree | PARCEventQueueOption_DeferCallbacks);

    stream->metis = metis;
    stream->logger = metisLogger_Acquire(metisForwarder_GetLogger(metis));
    stream->fd = fd;
    stream->id = metisForwarder_GetNextConnectionId(metis);
    stream->addressPair = pair;
    stream->isClosed = false;

    // allocate a connection
    MetisIoOperations *io_ops = parcMemory_AllocateAndClear(sizeof(MetisIoOperations));
    assertNotNull(io_ops, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisIoOperations));
    memcpy(io_ops, &_template, sizeof(MetisIoOperations));
    io_ops->closure = stream;

    parcEventQueue_SetCallbacks(stream->bufferEventVector, _conn_readcb, NULL, _conn_eventcb, (void *) io_ops);
    parcEventQueue_Enable(stream->bufferEventVector, PARCEventType_Read);

    metisMessenger_Send(metisForwarder_GetMessenger(stream->metis), metisMissive_Create(MetisMissiveType_ConnectionCreate, stream->id));

    // As we are acceting a connection, we begin in the UP state
    _setConnectionState(stream, true);

    if (metisLogger_IsLoggable(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug)) {
        char *pair_str = metisAddressPair_ToString(pair);
        metisLogger_Log(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug, __func__,
                        "StreamConnection %p accept for address pair %s", (void *) stream, pair_str);
        free(pair_str);
    }

    return io_ops;
}

MetisIoOperations *
metisStreamConnection_OpenConnection(MetisForwarder *metis, MetisAddressPair *pair, bool isLocal)
{
    assertNotNull(metis, "Parameter metis must be non-null");
    assertNotNull(pair, "Parameter pair must be non-null");

    // if there's an error on the bind or connect, will return NULL
    PARCEventQueue *bufferEventVector = metisDispatcher_StreamBufferConnect(metisForwarder_GetDispatcher(metis), pair);
    if (bufferEventVector == NULL) {
        // error opening connection
        return NULL;
    }

    _MetisStreamState *stream = parcMemory_AllocateAndClear(sizeof(_MetisStreamState));
    assertNotNull(stream, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(_MetisStreamState));

    stream->metis = metis;
    stream->logger = metisLogger_Acquire(metisForwarder_GetLogger(metis));
    stream->fd = parcEventQueue_GetFileDescriptor(bufferEventVector);
    stream->bufferEventVector = bufferEventVector;
    stream->id = metisForwarder_GetNextConnectionId(metis);
    stream->addressPair = pair;
    stream->isClosed = false;

    // allocate a connection
    MetisIoOperations *io_ops = parcMemory_AllocateAndClear(sizeof(MetisIoOperations));
    assertNotNull(io_ops, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisIoOperations));
    memcpy(io_ops, &_template, sizeof(MetisIoOperations));
    io_ops->closure = stream;

    parcEventQueue_SetCallbacks(stream->bufferEventVector, _conn_readcb, NULL, _conn_eventcb, (void *) io_ops);
    parcEventQueue_Enable(stream->bufferEventVector, PARCEventType_Read);

    // we start in DOWN state, until remote side answers
    metisMessenger_Send(metisForwarder_GetMessenger(stream->metis), metisMissive_Create(MetisMissiveType_ConnectionCreate, stream->id));
    _setConnectionState(stream, false);

    if (metisLogger_IsLoggable(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Info)) {
        char *pair_str = metisAddressPair_ToString(pair);
        metisLogger_Log(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Info, __func__,
                        "StreamConnection %p connect for address pair %s", (void *) stream, pair_str);
        free(pair_str);
    }

    return io_ops;
}

static void
_metisStreamConnection_DestroyOperations(MetisIoOperations **opsPtr)
{
    assertNotNull(opsPtr, "Parameter opsPtr must be non-null double pointer");
    assertNotNull(*opsPtr, "Parameter opsPtr must dereference to non-null pointer");

    MetisIoOperations *ops = *opsPtr;
    assertNotNull(metisIoOperations_GetClosure(ops), "ops->context must not be null");

    _MetisStreamState *stream = (_MetisStreamState *) metisIoOperations_GetClosure(ops);

    parcEventQueue_Destroy(&stream->bufferEventVector);

    metisAddressPair_Release(&stream->addressPair);

    if (!stream->isClosed) {
        stream->isClosed = true;
        metisMessenger_Send(metisForwarder_GetMessenger(stream->metis), metisMissive_Create(MetisMissiveType_ConnectionClosed, stream->id));
    }

    metisMessenger_Send(metisForwarder_GetMessenger(stream->metis), metisMissive_Create(MetisMissiveType_ConnectionDestroyed, stream->id));

    if (metisLogger_IsLoggable(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Info)) {
        metisLogger_Log(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Info, __func__,
                        "StreamConnection %p destroyed", (void *) stream);
    }

    metisLogger_Release(&stream->logger);
    parcMemory_Deallocate((void **) &stream);
    parcMemory_Deallocate((void **) &ops);

    *opsPtr = NULL;
}

static bool
_metisStreamConnection_IsUp(const MetisIoOperations *ops)
{
    assertNotNull(ops, "Parameter must be non-null");
    const _MetisStreamState *stream = (const _MetisStreamState *) metisIoOperations_GetClosure(ops);
    return stream->isUp;
}

static bool
_metisStreamConnection_IsLocal(const MetisIoOperations *ops)
{
    assertNotNull(ops, "Parameter must be non-null");
    const _MetisStreamState *stream = (const _MetisStreamState *) metisIoOperations_GetClosure(ops);
    return stream->isLocal;
}

static const CPIAddress *
_metisStreamConnection_GetRemoteAddress(const MetisIoOperations *ops)
{
    assertNotNull(ops, "Parameter must be non-null");
    const _MetisStreamState *stream = (const _MetisStreamState *) metisIoOperations_GetClosure(ops);
    return metisAddressPair_GetRemote(stream->addressPair);
}

static const MetisAddressPair *
_metisStreamConnection_GetAddressPair(const MetisIoOperations *ops)
{
    assertNotNull(ops, "Parameter must be non-null");
    const _MetisStreamState *stream = (const _MetisStreamState *) metisIoOperations_GetClosure(ops);
    return stream->addressPair;
}

static unsigned
_metisStreamConnection_GetConnectionId(const MetisIoOperations *ops)
{
    assertNotNull(ops, "Parameter must be non-null");
    const _MetisStreamState *stream = (const _MetisStreamState *) metisIoOperations_GetClosure(ops);
    return stream->id;
}

/**
 * @function metisStreamConnection_Send
 * @abstract Non-destructive send of the message.
 * @discussion
 *   Send uses metisMessage_CopyToStreamBuffer, which is a non-destructive write.
 *   The send may fail if there's no buffer space in the output queue.
 *
 * @param dummy is ignored.  A stream has only one peer.
 * @return <#return#>
 */
static bool
_metisStreamConnection_Send(MetisIoOperations *ops, const CPIAddress *dummy, MetisMessage *message)
{
    assertNotNull(ops, "Parameter ops must be non-null");
    assertNotNull(message, "Parameter message must be non-null");
    _MetisStreamState *stream = (_MetisStreamState *) metisIoOperations_GetClosure(ops);

    bool success = false;
    if (stream->isUp) {
        PARCEventBuffer *buffer = parcEventBuffer_GetQueueBufferOutput(stream->bufferEventVector);
        size_t buffer_backlog = parcEventBuffer_GetLength(buffer);
        parcEventBuffer_Destroy(&buffer);

        if (buffer_backlog < OUTPUT_QUEUE_BYTES) {
            if (metisLogger_IsLoggable(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug)) {
                metisLogger_Log(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug, __func__,
                                "connid %u Writing %zu bytes to buffer with backlog %zu bytes",
                                stream->id,
                                metisMessage_Length(message),
                                buffer_backlog);
            }

            int failure = metisMessage_Write(stream->bufferEventVector, message);
            if (failure == 0) {
                success = true;
            }
        } else {
            if (metisLogger_IsLoggable(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Warning)) {
                metisLogger_Log(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Warning, __func__,
                                "connid %u Writing to buffer backlog %zu bytes DROP MESSAGE",
                                stream->id,
                                buffer_backlog);
            }
        }
    } else {
        if (metisLogger_IsLoggable(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Error)) {
            metisLogger_Log(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Error, __func__,
                            "connid %u tried to send to down connection (isUp %d isClosed %d)",
                            stream->id,
                            stream->isUp,
                            stream->isClosed);
        }
    }

    return success;
}

static CPIConnectionType
_metisStreamConnection_GetConnectionType(const MetisIoOperations *ops)
{
    return cpiConnection_TCP;
}

// =================================================================
// the actual I/O functions

/**
 * @function startNewMessage
 * @abstract Peek at the fixed header and set the stream->nextMessageLength
 * @discussion
 *   This function manipulates the stream->nextMessageLength. After reading a FixedHeader, set nextMessageLength
 *   to the total length of the message.
 *
 *   PRECONDITION: stream->nextMessageLength == 0
 *   PRECONDITION: inputBytesAvailable >= FIXED_HEADER_LEN
 *
 * @param stream is the stream begin parsed.
 * @param input is the input PARCEventBuffer (corresponds to the buffer event's input)
 * @param inputBytesAvailable is the number of bytes available in the input PARCEventBuffer.
 * @return <#return#>
 */
static void
_startNewMessage(_MetisStreamState *stream, PARCEventBuffer *input, size_t inputBytesAvailable)
{
    assertTrue(stream->nextMessageLength == 0, "Invalid state, nextMessageLength not zero: %zu", stream->nextMessageLength);
    assertTrue(inputBytesAvailable >= metisTlv_FixedHeaderLength(), "read_length not a whole fixed header!: %zd", inputBytesAvailable);

    // this linearizes the first FIXED_HEADER_LEN bytes of the input buffer's iovecs and
    // returns a pointer to it.
    uint8_t *fh = parcEventBuffer_Pullup(input, metisTlv_FixedHeaderLength());

    // Calculate the total message size based on the fixed header
    stream->nextMessageLength = metisTlv_TotalPacketLength(fh);
}

/**
 * @function readMessage
 * @abstract Read the complete message from the input
 * @discussion
 *   Called to read a complete message from the input and return a MetisMessage
 *
 *   PRECONDITION: There are at least <code>stream->nextMessageLength</code>
 *   bytes available on the input PARCEventBuffer.
 *
 * @param stream is the stream being parsed
 * @param time is the current forwarder time (metisForwarder_GetTicks(stream->metis))
 * @param input is the input PARCEventBuffer to readessage bytes.
 * @return <#return#>
 */
static MetisMessage *
_readMessage(_MetisStreamState *stream, MetisTicks time, PARCEventBuffer *input)
{
    MetisMessage *message = metisMessage_ReadFromBuffer(stream->id, time, input, stream->nextMessageLength, stream->logger);

    return message;
}

/**
 * @function single_read
 * @abstract Read at most 1 message from the network
 * @discussion
 *   If a complete message is ready on the input buffer, will allocate and return it.
 *
 *   This function manipulates the stream->nextMessageLength.  (1) Initializes with nextMessageLength = 0,
 *   which means we have not started parsing a packet.  (2) After reading a FixedHeader, set nextMessageLength
 *   to the total length of the message.  (3) After reading nextMessageLength bytes, return the outputBuffer
 *   and reset nextMessageLength to 0.
 *
 * @param input is the PARCEventBuffer to read
 * @param stream is the related stream state for the input
 * @return true if there's more to read after this message.
 */
static MetisMessage *
_single_read(PARCEventBuffer *input, _MetisStreamState *stream)
{
    size_t bytesAvailable = parcEventBuffer_GetLength(input);

    assertTrue(bytesAvailable >= metisTlv_FixedHeaderLength(), "Called with too short an input: %zu", bytesAvailable);

    if (metisLogger_IsLoggable(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug)) {
        metisLogger_Log(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug, __func__,
                        "connid %u read %zu bytes",
                        stream->id, bytesAvailable);
    }

    if (stream->nextMessageLength == 0) {
        _startNewMessage(stream, input, bytesAvailable);
    }

    // This is not an ELSE statement.  We can both start a new message then
    // check if there's enough bytes to read the whole thing.

    if (bytesAvailable >= stream->nextMessageLength) {
        MetisMessage *message = _readMessage(stream, metisForwarder_GetTicks(stream->metis), input);

        if (metisLogger_IsLoggable(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug)) {
            metisLogger_Log(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Debug, __func__,
                            "connid %u msg_length %zu read_length %zu, resetting parser",
                            stream->id,
                            stream->nextMessageLength,
                            bytesAvailable);
        }

        // now reset message length for next packet
        stream->nextMessageLength = 0;

        return message;
    }

    return NULL;
}

/**
 * @function conn_readcb
 * @abstract Event callback for reads
 * @discussion
 *   Will read messages off the input.  Continues reading as long as we
 *   can get a header to determine the next message length or as long as we
 *   can read a complete message.
 *
 *   This function manipulates the read low water mark.  (1) read a fixed header plus complete message,
 *   then set the low water mark to FIXED_HEADER_LEN.  (2) read a fixed header, but not a complete
 *   message, then set low water mark to the total mesage length.  Using the low water mark like this
 *   means the buffer event will only trigger on meaningful byte boundaries when we can get actual
 *   work done.
 *
 * @param <#param1#>
 * @return <#return#>
 */
static void
_conn_readcb(PARCEventQueue *event, PARCEventType type, void *ioOpsVoid)
{
    MetisIoOperations *ops = (MetisIoOperations *) ioOpsVoid;
    _MetisStreamState *stream = (_MetisStreamState *) metisIoOperations_GetClosure(ops);

    PARCEventBuffer *input = parcEventBuffer_GetQueueBufferInput(event);

    // drain the input buffer
    while (parcEventBuffer_GetLength(input) >= metisTlv_FixedHeaderLength() && parcEventBuffer_GetLength(input) >= stream->nextMessageLength) {
        // this may set the stream->nextMessageLength
        MetisMessage *message = _single_read(input, stream);

        if (message) {
            metisForwarder_Receive(stream->metis, message);
        }
    }

    if (stream->nextMessageLength == 0) {
        // we don't have the next header, so set it to the header length
        metisStreamBuffer_SetWatermark(event, true, false, metisTlv_FixedHeaderLength(), 0);
    } else {
        // set it to the packet length
        metisStreamBuffer_SetWatermark(event, true, false, stream->nextMessageLength, 0);
    }
    parcEventBuffer_Destroy(&input);
}

static void
_setConnectionState(_MetisStreamState *stream, bool isUp)
{
    assertNotNull(stream, "Parameter stream must be non-null");

    MetisMessenger *messenger = metisForwarder_GetMessenger(stream->metis);

    bool oldStateIsUp = stream->isUp;
    stream->isUp = isUp;

    if (oldStateIsUp && !isUp) {
        // bring connection DOWN
        MetisMissive *missive = metisMissive_Create(MetisMissiveType_ConnectionDown, stream->id);
        metisMessenger_Send(messenger, missive);
        return;
    }


    if (!oldStateIsUp && isUp) {
        // bring connection UP
        MetisMissive *missive = metisMissive_Create(MetisMissiveType_ConnectionUp, stream->id);
        metisMessenger_Send(messenger, missive);
        return;
    }
}

static void
_conn_eventcb(PARCEventQueue *event, PARCEventQueueEventType events, void *ioOpsVoid)
{
    MetisIoOperations *ops = (MetisIoOperations *) ioOpsVoid;
    _MetisStreamState *stream = (_MetisStreamState *) metisIoOperations_GetClosure(ops);

    if (events & PARCEventQueueEventType_Connected) {
        if (metisLogger_IsLoggable(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Info)) {
            metisLogger_Log(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Info, __func__,
                            "Connection %u is connected", stream->id);
        }

        // if the stream was closed, do not transition to an UP state
        if (!stream->isClosed) {
            _setConnectionState(stream, true);
        }
    } else
    if (events & PARCEventQueueEventType_EOF) {
        if (metisLogger_IsLoggable(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Info)) {
            metisLogger_Log(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Info, __func__,
                            "connid %u closed.",
                            stream->id);
        }

        parcEventQueue_Disable(stream->bufferEventVector, PARCEventType_Read);

        _setConnectionState(stream, false);

        if (!stream->isClosed) {
            stream->isClosed = true;
            // this will cause the connection manager to destroy the connection later
            metisMessenger_Send(metisForwarder_GetMessenger(stream->metis), metisMissive_Create(MetisMissiveType_ConnectionClosed, stream->id));
        }
    } else
    if (events & PARCEventQueueEventType_Error) {
        if (metisLogger_IsLoggable(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Error)) {
            metisLogger_Log(stream->logger, MetisLoggerFacility_IO, PARCLogLevel_Error, __func__,
                            "Got an error on the connection %u: %s", stream->id, strerror(errno));
        }

        parcEventQueue_Disable(stream->bufferEventVector, PARCEventType_Read | PARCEventType_Write);

        _setConnectionState(stream, false);

        if (!stream->isClosed) {
            stream->isClosed = true;
            // this will cause the connection manager to destroy the connection later
            metisMessenger_Send(metisForwarder_GetMessenger(stream->metis), metisMissive_Create(MetisMissiveType_ConnectionClosed, stream->id));
        }
    }
    /* None of the other events can happen here, since we haven't enabled
     * timeouts */
}
