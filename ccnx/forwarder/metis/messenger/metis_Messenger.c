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
 *
 * The messenger is contructued with a reference to the forwarder's dispatcher so it can
 * schedule future events.  When someone calls metisMessenger_Send(...), it will put the
 * message on a queue.  If the queue was empty, it will scheudle itself to be run.
 * By running the queue in a future dispatcher slice, it guarantees that there will be
 * no re-entrant behavior between callers and message listeners.
 *
 * A recipient will receive a reference counted copy of the missive, so it must call
 * {@link metisMissive_Release} on it.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>
#include <LongBow/runtime.h>
#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_ArrayList.h>
#include <parc/algol/parc_EventScheduler.h>
#include <parc/algol/parc_Event.h>

#include <ccnx/forwarder/metis/messenger/metis_Messenger.h>
#include <ccnx/forwarder/metis/messenger/metis_MissiveDeque.h>

struct metis_messenger {
    PARCArrayList *callbacklist;
    MetisDispatcher *dispatcher;
    MetisMissiveDeque *eventQueue;

    PARCEventTimer *timerEvent;
};

static void metisMessenger_Dequeue(int fd, PARCEventType which_event, void *messengerVoidPtr);

// =========================================
// Public API

MetisMessenger *
metisMessenger_Create(MetisDispatcher *dispatcher)
{
    MetisMessenger *messenger = parcMemory_AllocateAndClear(sizeof(MetisMessenger));
    assertNotNull(messenger, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisMessenger));

    // NULL destroyer because we're storing structures owned by the caller
    messenger->dispatcher = dispatcher;
    messenger->callbacklist = parcArrayList_Create(NULL);
    messenger->eventQueue = metisMissiveDeque_Create();

    // creates the timer, but does not start it
    messenger->timerEvent = metisDispatcher_CreateTimer(dispatcher, false, metisMessenger_Dequeue, messenger);

    return messenger;
}

void
metisMessenger_Destroy(MetisMessenger **messengerPtr)
{
    assertNotNull(messengerPtr, "Parameter must be non-null double pointer");
    assertNotNull(*messengerPtr, "Parameter must dereference to non-null pointer");

    MetisMessenger *messenger = *messengerPtr;
    parcArrayList_Destroy(&messenger->callbacklist);
    metisMissiveDeque_Release(&messenger->eventQueue);
    metisDispatcher_DestroyTimerEvent(messenger->dispatcher, &messenger->timerEvent);
    parcMemory_Deallocate((void **) &messenger);
    *messengerPtr = NULL;
}

void
metisMessenger_Send(MetisMessenger *messenger, MetisMissive *missive)
{
    assertNotNull(messenger, "Parameter messenger must be non-null");
    assertNotNull(missive, "Parameter event must be non-null");

    metisMissiveDeque_Append(messenger->eventQueue, missive);
    if (metisMissiveDeque_Size(messenger->eventQueue) == 1) {
        // We need to scheudle ourself when an event is added to an empty queue

        // precondition: timer should not be running.
        struct timeval immediateTimeout = { 0, 0 };
        metisDispatcher_StartTimer(messenger->dispatcher, messenger->timerEvent, &immediateTimeout);
    }
}

static void
removeRecipient(MetisMessenger *messenger, const MetisMessengerRecipient *recipient)
{
    // don't increment i in the loop
    for (size_t i = 0; i < parcArrayList_Size(messenger->callbacklist); ) {
        const void *p = parcArrayList_Get(messenger->callbacklist, i);
        if (p == recipient) {
            // removing will compact the list, so next element will also be at i.
            parcArrayList_RemoveAndDestroyAtIndex(messenger->callbacklist, i);
        } else {
            i++;
        }
    }
}

/**
 * @function metisEventMessenger_Register
 * @abstract Receive all event messages
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
void
metisMessenger_Register(MetisMessenger *messenger, const MetisMessengerRecipient *recipient)
{
    assertNotNull(messenger, "Parameter messenger must be non-null");
    assertNotNull(recipient, "Parameter recipient must be non-null");

    // do not allow duplicates
    removeRecipient(messenger, recipient);

    parcArrayList_Add(messenger->callbacklist, recipient);
}

/**
 * @function metisEventMessenger_Unregister
 * @abstract Stop receiving event messages
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
void
metisMessenger_Unregister(MetisMessenger *messenger, const MetisMessengerRecipient *recipient)
{
    assertNotNull(messenger, "Parameter messenger must be non-null");
    assertNotNull(recipient, "Parameter recipient must be non-null");

    removeRecipient(messenger, recipient);
}

/**
 * Called by event scheduler to give us a slice in which to dequeue events
 *
 * Called inside an event callback, so we now have exclusive access to the system.
 * Dequeues all pending events and calls all the listeners for each one.
 *
 * @param [in] fd unused, required for compliance with function prototype
 * @param [in] which_event unused, required for compliance with function prototype
 * @param [in] messengerVoidPtr A void* to MetisMessenger
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
static void
metisMessenger_Dequeue(int fd, PARCEventType which_event, void *messengerVoidPtr)
{
    MetisMessenger *messenger = (MetisMessenger *) messengerVoidPtr;
    assertNotNull(messenger, "Called with null messenger pointer");

    MetisMissive *missive;
    while ((missive = metisMissiveDeque_RemoveFirst(messenger->eventQueue)) != NULL) {
        for (size_t i = 0; i < parcArrayList_Size(messenger->callbacklist); i++) {
            MetisMessengerRecipient *recipient = parcArrayList_Get(messenger->callbacklist, i);
            assertTrue(recipient, "Recipient is null at index %zu", i);

            metisMessengerRecipient_Deliver(recipient, metisMissive_Acquire(missive));
        }

        // now let go of our reference to the missive
        metisMissive_Release(&missive);
    }
}
