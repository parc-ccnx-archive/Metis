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
//  testrig_MockTap.h
//  Metis
//
//  Created by Mosko, Marc <Marc.Mosko@parc.com> on 12/5/13.
//
//

#ifndef Metis_testrig_MockTap_h
#define Metis_testrig_MockTap_h

// =========================================================================
// Mock for tap testing
// Allows the test to set the IsTapOnX return values.
// Counts the number of calls to each TapOnX.
// Records the last message pointer
// The user sets and examines values in the static "testTap" variable and
// passes "testTapTemplate" to the tap setup.

static bool testTap_IsTapOnReceive(const MetisTap *tap);
static bool testTap_IsTapOnSend(const MetisTap *tap);
static bool testTap_IsTapOnDrop(const MetisTap *tap);
static void testTap_TapOnReceive(MetisTap *tap, const MetisMessage *message);
static void testTap_TapOnSend(MetisTap *tap, const MetisMessage *message);
static void testTap_TapOnDrop(MetisTap *tap, const MetisMessage *message);

// this test variable is zeroed in each FIXTURE_SETUP.
// To test tap functionality, set the various callOnX flags, run your test,
// then check the onXCounts to make sure they are right.
struct testTap_s {
    bool callOnReceive;
    bool callOnSend;
    bool callOnDrop;
    unsigned onReceiveCount;
    unsigned onSendCount;
    unsigned onDropCount;

    const MetisMessage *lastMessage;
} testTap;

// you should not need tochange this template
MetisTap testTapTemplate = {
    .context        = &testTap,
    .isTapOnReceive = &testTap_IsTapOnReceive,
    .isTapOnSend    = &testTap_IsTapOnSend,
    .isTapOnDrop    = &testTap_IsTapOnDrop,
    .tapOnReceive   = &testTap_TapOnReceive,
    .tapOnSend      = &testTap_TapOnSend,
    .tapOnDrop      = &testTap_TapOnDrop
};

static bool
testTap_IsTapOnReceive(const MetisTap *tap)
{
    struct testTap_s *mytap = (struct testTap_s *) tap->context;
    return mytap->callOnReceive;
}

static bool
testTap_IsTapOnSend(const MetisTap *tap)
{
    struct testTap_s *mytap = (struct testTap_s *) tap->context;
    return mytap->callOnSend;
}

static bool
testTap_IsTapOnDrop(const MetisTap *tap)
{
    struct testTap_s *mytap = (struct testTap_s *) tap->context;
    return mytap->callOnDrop;
}

static void
testTap_TapOnReceive(MetisTap *tap, const MetisMessage *message)
{
    struct testTap_s *mytap = (struct testTap_s *) tap->context;
    mytap->onReceiveCount++;
    mytap->lastMessage = message;
}

static void
testTap_TapOnSend(MetisTap *tap, const MetisMessage *message)
{
    struct testTap_s *mytap = (struct testTap_s *) tap->context;
    mytap->onSendCount++;
    mytap->lastMessage = message;
}

static void
testTap_TapOnDrop(MetisTap *tap, const MetisMessage *message)
{
    struct testTap_s *mytap = (struct testTap_s *) tap->context;
    mytap->onDropCount++;
    mytap->lastMessage = message;
}
#endif // Metis_testrig_MockTap_h
