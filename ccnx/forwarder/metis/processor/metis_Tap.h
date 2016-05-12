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
 * The MetisForwarder supports a Tap that will inspect all messages passing through the
 * forwarder.  See metisForwarder_AddTap() and metisForwarder_RemoveTap().
 *
 *
 * Example:
 * @code
 * {
 *     struct testTap_s {
 *        bool callOnReceive;
 *        unsigned onReceiveCount;
 *     } testTap;
 *
 *     static bool
 *     testTap_IsTapOnReceive(const MetisTap *tap)
 *     {
 *        struct testTap_s *mytap = (struct testTap_s *) tap->context;
 *        return mytap->callOnReceive;
 *     }
 *
 *     static void
 *     testTap_TapOnReceive(MetisTap *tap, const MetisMessage *message)
 *     {
 *        struct testTap_s *mytap = (struct testTap_s *) tap->context;
 *        mytap->onReceiveCount++;
 *        mytap->lastMessage = message;
 *     }
 *
 *     MetisTap testTapTemplate = {
 *        .context        = &testTap,
 *        .isTapOnReceive = &testTap_IsTapOnReceive,
 *        .isTapOnSend    = NULL,
 *        .isTapOnDrop    = NULL,
 *        .tapOnReceive   = &testTap_TapOnReceive,
 *        .tapOnSend      = NULL,
 *        .tapOnDrop      = NULL
 *     };
 *
 * }
 * @endcode
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#ifndef Metis_metis_Tap_h
#define Metis_metis_Tap_h

struct metis_tap;


typedef struct metis_tap MetisTap;

/**
 * Defines callbacks for message taps
 *
 * Each of the taps (tapOnReceive, tapOnSend, tapOnDrop) may be NULL.
 *   if a tap is not null, then the correspnoding isX function must be non-null.  The isX functions
 *   allow turning on/off particular calls depending on user preference.
 */
struct metis_tap {

    /**
     * A user-defined parameter
     */
    void *context;

    /**
     * Determines if the tapOnReceive() function should be called
     *
     * If *tapOnReceive is non-null, this function must be defined too.
     *
     * @param [in] MetisTap The tap structure
     *
     * @return true call the tap function
     * @return false Do not call the tap function.
     */
    bool (*isTapOnReceive)(const MetisTap *tap);

    /**
     * Determines if the tapOnSend() function should be called
     *
     * If *tapOnSend is non-null, this function must be defined too.
     *
     * @param [in] MetisTap The tap structure
     *
     * @return true call the tap function
     * @return false Do not call the tap function.
     */
    bool (*isTapOnSend)(const MetisTap *tap);

    /**
     * Determines if the tapOnDrop() function should be called
     *
     * If *tapOnDrop is non-null, this function must be defined too.
     *
     * @param [in] MetisTap The tap structure
     *
     * @return true call the tap function
     * @return false Do not call the tap function.
     */
    bool (*isTapOnDrop)(const MetisTap *tap);

    /**
     * Called for each message entering the message processor.  May be NULL.
     *
     * @param [in] MetisTap The tap structure
     */
    void (*tapOnReceive)(MetisTap *tap, const MetisMessage *message);

    /**
     * Called for each message forwarded by the message processor.  May be NULL.
     *
     * @param [in] MetisTap The tap structure
     */
    void (*tapOnSend)(MetisTap *tap, const MetisMessage *message);

    /**
     * Called for each message dropped by the message processor.  May be NULL.
     *
     * @param [in] MetisTap The tap structure
     */
    void (*tapOnDrop)(MetisTap *tap, const MetisMessage *message);
};


#endif // Metis_metis_Tap_h
