/*
 * Copyright (c) 2013-2014, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @file metis_MessengerRecipient.h
 * @brief A recipient represents the entity that will recieve a Missive from the Messenger.
 *
 * A recipient is identified by the pair (contenxt, callback).  The context is the recipients
 * context, such as it's object pointer.  The callback is the function the recipient uses
 * to receive a Missive.
 *
 * If the receiver is going to do a lot of work or potentially send other missives, the receiver
 * should queue the received notifications and process them in its own slice.
 *
 * A recipient will receive a reference counted copy of the missive, so it must call
 * {@link metisMissive_Release} on it.
 *
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2014, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_MessengerRecipient_h
#define Metis_metis_MessengerRecipient_h

struct metis_messenger_recipient;
typedef struct metis_messenger_recipient MetisMessengerRecipient;

/**
 * @typedef MetisMessengerRecipientCallback
 * @abstract A recipient implements a callback to receive Missives.
 * @constant recipient The recipient to recieve the missive
 * @constant missive The missive, recipient must call {@link metisMissive_Release} on it
 * @discussion <#Discussion#>
 */
typedef void (MetisMessengerRecipientCallback)(MetisMessengerRecipient *recipient, MetisMissive *missive);

/**
 * Creates a Recipient, which represents a reciever of missives.
 *
 * Creates a Recipient that can be registerd with the Messenger using {@link metisMessenger_Register}.
 *
 * @param [in] recipientContext This pointer will be passed back to the recipient with each missive, may be NULL
 * @param [in] recipientCallback The function that receives the missive, must be non-NULL.
 *
 * @return non-null A recipient object
 *
 * Example:
 * @code
 * @endcode
 */
MetisMessengerRecipient *metisMessengerRecipient_Create(void *recipientContext, MetisMessengerRecipientCallback *recipientCallback);

/**
 * Destroys a recipient.  You should unregister it first.
 *
 * Destroying a recipient does not unregister it, so be sure to call
 * {@link metisMessenger_Unregister} first.
 *
 * @param [in,out] recipientPtr Double pointer to the recipient to destroy, will be NULL'd.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisMessengerRecipient_Destroy(MetisMessengerRecipient **recipientPtr);

/**
 * Returns the recipient context passed on Create
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] recipient The recipient object
 *
 * @return pointer The context pointer used to create the object, maybe NULL
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void *metisMessengerRecipient_GetRecipientContext(MetisMessengerRecipient *recipient);

/**
 * Delivers a Missive to the recipient
 *
 * Passes the missive to the recipients callback.
 *
 * A recipient will receive a reference counted copy of the missive, so it must call
 * {@link metisMissive_Release} on it.
 *
 * @param [in] recipient The receiver
 * @param [in] missive The message to send
 *
 * @return <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisMessengerRecipient_Deliver(MetisMessengerRecipient *recipient, MetisMissive *missive);
#endif // Metis_metis_MessengerRecipient_h
