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
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdio.h>
#include <LongBow/runtime.h>
#include <parc/algol/parc_Memory.h>

#include <ccnx/forwarder/metis/messenger/metis_Messenger.h>
#include <ccnx/forwarder/metis/messenger/metis_MessengerRecipient.h>

struct metis_messenger_recipient {
    void *context;
    MetisMessengerRecipientCallback *notify;
};

MetisMessengerRecipient *
metisMessengerRecipient_Create(void *recipientContext, MetisMessengerRecipientCallback *recipientCallback)
{
    assertNotNull(recipientCallback, "Parameter recipientCallback must be non-null");

    MetisMessengerRecipient *recipient = parcMemory_AllocateAndClear(sizeof(MetisMessengerRecipient));
    assertNotNull(recipient, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisMessengerRecipient));
    recipient->context = recipientContext;
    recipient->notify = recipientCallback;
    return recipient;
}

void
metisMessengerRecipient_Destroy(MetisMessengerRecipient **recipientPtr)
{
    assertNotNull(recipientPtr, "Parameter must be non-null double pointer");
    assertNotNull(*recipientPtr, "Parameter must dereference to non-null pointer");

    parcMemory_Deallocate((void **) recipientPtr);
    *recipientPtr = NULL;
}

void *
metisMessengerRecipient_GetRecipientContext(MetisMessengerRecipient *recipient)
{
    assertNotNull(recipient, "Parameter must be non-null");

    return recipient->context;
}

void
metisMessengerRecipient_Deliver(MetisMessengerRecipient *recipient, MetisMissive *missive)
{
    assertNotNull(recipient, "Parameter must be non-null");
    recipient->notify(recipient, missive);
}
