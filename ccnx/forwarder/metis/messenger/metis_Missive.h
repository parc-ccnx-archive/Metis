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
 * @file metis_Missive.h
 * @brief A Missive is a status message sent over a broadcast channel inside Metis
 *
 * Recipients use {@link metisMessenger_Register} to receive missives.  They are
 * broadcast to all recipients.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#ifndef Metis_metis_missive_h
#define Metis_metis_missive_h

#include <ccnx/forwarder/metis/messenger/metis_MissiveType.h>

struct metis_missive;
typedef struct metis_missive MetisMissive;

/**
 * Creates a Missive and sets the reference count to 1
 *
 * A Missive may be sent to listeners of the MetisMessenger to inform them of events on a connection id.
 *
 * @param [in] MetisMissiveType The event type
 * @param [in] connectionid The relevant conneciton id
 *
 * @return non-null A message
 * @retrun null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisMissive *metisMissive_Create(MetisMissiveType missiveType, unsigned connectionid);

/**
 * Acquire a reference counted copy
 *
 * Increases the reference count by 1 and returns the original object.
 *
 * @param [in] missive An allocated missive
 *
 * @return non-null The original missive with increased reference count
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisMissive *metisMissive_Acquire(const MetisMissive *missive);

/**
 * Releases a reference counted copy.
 *
 *  If it is the last reference, the missive is freed.
 *
 * @param [in,out] missivePtr Double pointer to a missive, will be nulled.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisMissive_Release(MetisMissive **missivePtr);

/**
 * Returns the type of the missive
 *
 * Returns the type of event the missive represents
 *
 * @param [in] missive An allocated missive
 *
 * @return MetisMissiveType The event type
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisMissiveType metisMissive_GetType(const MetisMissive *missive);

/**
 * Returns the connection ID of the missive
 *
 * An event is usually associated with a connection id (i.e. the I/O channel
 * that originaged the event).
 *
 * @param [in] missive An allocated missive
 *
 * @return number The relevant connection id.
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
unsigned metisMissive_GetConnectionId(const MetisMissive *missive);
#endif // Metis_metis_missive_h
