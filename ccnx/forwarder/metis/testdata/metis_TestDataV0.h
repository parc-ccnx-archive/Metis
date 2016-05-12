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
 * This test rig provides some hand-made interests and objects for testing.
 * We use __attribute__((unused)) on each of them, because a test might not use all off
 * the messages and it would otherwise generate a warning mesage at compile time.
 */

#ifndef Metis_metis_TestDataV0_h
#define Metis_metis_TestDataV0_h

#include <ccnx/forwarder/metis/tlv/metis_Tlv.h>

uint8_t metisTestDataV0_EncodedInterest[] = {
    0x00, 0x01, 0x00, 0x36, // ver = 0, type = interest, length = 54
    0x00, 0x00, 0x00, 0x15, // reserved = 0, header length = 21
    // ------------------------
    0x00, 0x02, 0x00, 0x01, // HOPLIMIT
    0x20, // 32
    // ------------------------
    0x00, 0x03, 0x00, 0x0C, // Interest Fragment
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, // fragment 0x0102030405060708
    0x05, 0xDC, 0x04, 0x00, // MTU 1500, fragcnt 4, fragnum 0
    // ------------------------
    0x00, 0x01, 0x00, 0x32, // type = interest, length = 50
    // ------------------------
    0x00, 0x00, 0x00, 0x11, // type = name, length = 17
    0x00, 0x02, 0x00, 0x05, // type = binary, length = 5
    'h',  'e',  'l',  'l',
    'o',  // "hello"
    0xF0, 0x00, 0x00, 0x04, // type = app, length = 4
    'o',  'u',  'c',  'h',  // "ouch"
    // ------------------------
    0x00, 0x01, 0x00, 0x04, // type = keyid, length = 4
    0xA0, 0xB0, 0xC0, 0xD0, // 0xA0B0C0D0
    // ------------------------
    0x00, 0x02, 0x00, 0x06, // type = objhash, length = 6
    0x11, 0x22, 0x33, 0x44,
    0x55, 0x66, // 0x112233445566
    // ------------------------
    0x00, 0x03, 0x00, 0x01, // scope, length = 1
    0x02, // value = 2
    // ------------------------
    0x00, 0x05, 0x00, 0x02, // interest lifetime, length = 2
    0x7D, 0x00              // 32000
};

uint8_t metisTestDataV0_EncodedInterest_no_hoplimit[] = {
    0x00, 0x01, 0x00, 0x36,     // ver = 0, type = interest, length = 54
    0x00, 0x00, 0x00, 0x15,     // reserved = 0, header length = 21
    // ------------------------
    0x00, 0xFF, 0x00, 0x01,     // Unknown TLV value
    0x20, // 32
    // ------------------------
    0x00, 0x03, 0x00, 0x0C,     // Interest Fragment
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08,     // fragment 0x0102030405060708
    0x05, 0xDC, 0x04, 0x00,     // MTU 1500, fragcnt 4, fragnum 0
    // ------------------------
    0x00, 0x01, 0x00, 0x32,     // type = interest, length = 50
    // ------------------------
    0x00, 0x00, 0x00, 0x11,     // type = name, length = 17
    0x00, 0x02, 0x00, 0x05,     // type = binary, length = 5
    'h',  'e',  'l',  'l',
    'o',  // "hello"
    0xF0, 0x00, 0x00, 0x04,     // type = app, length = 4
    'o',  'u',  'c',  'h',      // "ouch"
    // ------------------------
    0x00, 0x01, 0x00, 0x04,     // type = keyid, length = 4
    0xA0, 0xB0, 0xC0, 0xD0,     // 0xA0B0C0D0
    // ------------------------
    0x00, 0x02, 0x00, 0x06,     // type = objhash, length = 6
    0x11, 0x22, 0x33, 0x44,
    0x55, 0x66,                 // 0x112233445566
    // ------------------------
    0x00, 0x03, 0x00, 0x01,     // scope, length = 1
    0x02, // value = 2
    // ------------------------
    0x00, 0x05, 0x00, 0x02,     // interest lifetime, length = 2
    0x7D, 0x00                  // 32000
};

uint8_t metisTestDataV0_EncodedInterest_zero_hoplimit[] = {
    0x00, 0x01, 0x00, 0x36,     // ver = 0, type = interest, length = 54
    0x00, 0x00, 0x00, 0x15,     // reserved = 0, header length = 21
    // ------------------------
    0x00, 0x02, 0x00, 0x01,     // HOPLIMIT
    0x00, // 0
    // ------------------------
    0x00, 0x03, 0x00, 0x0C,     // Interest Fragment
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08,     // fragment 0x0102030405060708
    0x05, 0xDC, 0x04, 0x00,     // MTU 1500, fragcnt 4, fragnum 0
    // ------------------------
    0x00, 0x01, 0x00, 0x32,     // type = interest, length = 50
    // ------------------------
    0x00, 0x00, 0x00, 0x11,     // type = name, length = 17
    0x00, 0x02, 0x00, 0x05,     // type = binary, length = 5
    'h',  'e',  'l',  'l',
    'o',  // "hello"
    0xF0, 0x00, 0x00, 0x04,     // type = app, length = 4
    'o',  'u',  'c',  'h',      // "ouch"
    // ------------------------
    0x00, 0x01, 0x00, 0x04,     // type = keyid, length = 4
    0xA0, 0xB0, 0xC0, 0xD0,     // 0xA0B0C0D0
    // ------------------------
    0x00, 0x02, 0x00, 0x06,     // type = objhash, length = 6
    0x11, 0x22, 0x33, 0x44,
    0x55, 0x66, // 0x112233445566
    // ------------------------
    0x00, 0x03, 0x00, 0x01,     // scope, length = 1
    0x02, // value = 2
    // ------------------------
    0x00, 0x05, 0x00, 0x02,     // interest lifetime, length = 2
    0x7D, 0x00                  // 32000
};

uint8_t metisTestDataV0_InterestAllFields[] = {
    0x00, 0x01, 0x00, 0x36, // ver = 0, type = interest, length = 54
    0x00, 0x00, 0x00, 0x15, // reserved = 0, header length = 21
    // ------------------------
    0x00, 0x02, 0x00, 0x01, // HOPLIMIT
    0x20, // 32
    // ------------------------
    0x00, 0x03, 0x00, 0x0C, // Interest Fragment
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, // fragment 0x0102030405060708
    0x05, 0xDC, 0x04, 0x00, // MTU 1500, fragcnt 4, fragnum 0
    // ------------------------
    0x00, 0x01, 0x00, 0x32, // type = interest, length = 50
    // ------------------------
    0x00, 0x00, 0x00, 0x11,    // type = name, length = 17
    0x00, 0x02, 0x00, 0x05,   // type = binary, length = 5
    'h',  'e',  'l',  'l',
    'o',  // "hello"
    0xF0, 0x00, 0x00, 0x04, // type = app, length = 4
    'o',  'u',  'c',  'h',  // "ouch"
    // ------------------------
    0x00, 0x01, 0x00, 0x04, // type = keyid, length = 4
    0xA0, 0xB0, 0xC0, 0xD0, // 0xA0B0C0D0
    // ------------------------
    0x00, 0x02, 0x00, 0x06, // type = objhash, length = 6
    0x11, 0x22, 0x33, 0x44,
    0x55, 0x66, // 0x112233445566
    // ------------------------
    0x00, 0x03, 0x00, 0x01, // scope, length = 1
    0x02, // value = 2
    // ------------------------
    0x00, 0x05, 0x00, 0x02, // interest lifetime, length = 2
    0x7D, 0x00              // 32000
};

uint8_t metisTestDataV0_InterestWithName[] = {
    0x00, 0x01, 0x00, 0x19, // ver = 0, type = interest, length = 25
    0x00, 0x00, 0x00, 0x15, // reserved = 0, header length = 21
    // ------------------------
    0x00, 0x02, 0x00, 0x01, // HOPLIMIT
    0x20, // 32
    // ------------------------
    0x00, 0x03, 0x00, 0x0C, // Interest Fragment
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, // fragment 0x0102030405060708
    0x05, 0xDC, 0x04, 0x00, // MTU 1500, fragcnt 4, fragnum 0
    // ------------------------
    0x00, 0x01, 0x00, 0x15, // type = interest, length = 21
    // ------------------------
    0x00, 0x00, 0x00, 0x11,   // type = name, length = 17
    0x00, 0x02, 0x00, 0x05,  // type = binary, length = 5
    'h',  'e',  'l',  'l',    // "hello"
    'o',
    0xF0, 0x00, 0x00, 0x04, // type = app, length = 4
    'o',  'u',  'c',  'h',  // "ouch"
};

uint8_t metisTestDataV0_InterestWithOtherName[] = {
    0x00, 0x01, 0x00, 0x19, // ver = 0, type = interest, length = 25
    0x00, 0x00, 0x00, 0x15, // reserved = 0, header length = 21
    // ------------------------
    0x00, 0x02, 0x00, 0x01, // HOPLIMIT
    0x20, // 32
    // ------------------------
    0x00, 0x03, 0x00, 0x0C, // Interest Fragment
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, // fragment 0x0102030405060708
    0x05, 0xDC, 0x04, 0x00, // MTU 1500, fragcnt 4, fragnum 0
    // ------------------------
    0x00, 0x01, 0x00, 0x15, // type = interest, length = 21
    // ------------------------
    0x00, 0x00, 0x00, 0x11,  // type = name, length = 17
    0x00, 0x02, 0x00, 0x05,  // type = binary, length = 5
    'p',  'a',  'r',  't',   // "party"
    'y',
    0xF0, 0x00, 0x00, 0x04, // type = app, length = 4
    'o',  'u',  'c',  'h',  // "ouch"
};

uint8_t metisTestDataV0_InterestWithName_keyid[] = {
    0x00, 0x01, 0x00, 0x21, // ver = 0, type = interest, length = 33
    0x00, 0x00, 0x00, 0x15, // reserved = 0, header length = 21
    // ------------------------
    0x00, 0x02, 0x00, 0x01, // HOPLIMIT
    0x20, // 32
    // ------------------------
    0x00, 0x03, 0x00, 0x0C, // Interest Fragment
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, // fragment 0x0102030405060708
    0x05, 0xDC, 0x04, 0x00, // MTU 1500, fragcnt 4, fragnum 0
    // ------------------------
    0x00, 0x01, 0x00, 0x1D, // type = interest, length = 29
    // ------------------------
    0x00, 0x00, 0x00, 0x11,   // type = name, length = 17
    0x00, 0x02, 0x00, 0x05,   // type = binary, length = 5
    'h',  'e',  'l',  'l',    // "hello"
    'o',
    0xF0, 0x00, 0x00, 0x04, // type = app, length = 4
    'o',  'u',  'c',  'h',  // "ouch"
    // ------------------------
    0x00, 0x01, 0x00, 0x04, // type = keyid, length = 4
    0xA0, 0xB0, 0xC0, 0xD0, // 0xA0B0C0D0
};

uint8_t metisTestDataV0_InterestWithName_keyid2[] = {
    0x00, 0x01, 0x00, 0x21, // ver = 0, type = interest, length = 33
    0x00, 0x00, 0x00, 0x15, // reserved = 0, header length = 21
    // ------------------------
    0x00, 0x02, 0x00, 0x01, // HOPLIMIT
    0x20, // 32
    // ------------------------
    0x00, 0x03, 0x00, 0x0C, // Interest Fragment
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, // fragment 0x0102030405060708
    0x05, 0xDC, 0x04, 0x00, // MTU 1500, fragcnt 4, fragnum 0
    // ------------------------
    0x00, 0x01, 0x00, 0x1D, // type = interest, length = 29
    // ------------------------
    0x00, 0x00, 0x00, 0x11,   // type = name, length = 17
    0x00, 0x02, 0x00, 0x05,   // type = binary, length = 5
    'h',  'e',  'l',  'l',    // "hello"
    'o',
    0xF0, 0x00, 0x00, 0x04, // type = app, length = 4
    'o',  'u',  'c',  'h',  // "ouch"
    // ------------------------
    0x00, 0x01, 0x00, 0x04, // type = keyid, length = 4
    0x22, 0x22, 0x22, 0x22, // 0x22222222
};

/**
 * This interest will match "metisTestDataV0_EncodedObject" by name and content object hash.
 * The content object hash was computed out-of-band and manually placed here.
 */
uint8_t metisTestDataV0_InterestWithName_objecthash[] = {
    0x00, 0x01, 0x00, 0x21, // ver = 0, type = interest, length = 33
    0x00, 0x00, 0x00, 0x15, // reserved = 0, header length = 21
    // ------------------------
    0x00, 0x02, 0x00, 0x01, // HOPLIMIT
    0x20, // 32
    // ------------------------
    0x00, 0x03, 0x00, 0x0C,   // Interest Fragment
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08,   // fragment 0x0102030405060708
    0x05, 0xDC, 0x04, 0x00,   // MTU 1500, fragcnt 4, fragnum 0
    // ------------------------
    0x00, 0x01, 0x00, 0x1D,   // type = interest, length = 29
    // ------------------------
    0x00, 0x00, 0x00, 0x11,   // type = name, length = 17
    0x00, 0x02, 0x00, 0x05,   // type = binary, length = 5
    'h',  'e',  'l',  'l',    // "hello"
    'o',
    0xF0, 0x00, 0x00, 0x04, // type = app, length = 4
    'o',  'u',  'c',  'h',  // "ouch"
    // ------------------------
    0x00, 0x02, 0x00, 0x20, // type = objhash, length = 32
    0x9B, 0x86, 0x46, 0x3F, // matches "metisTestDataV0_EncodedObject"
    0x8F, 0xFC, 0x3C, 0x26,
    0x9B, 0xE0, 0x91, 0xDE,
    0xA7, 0xC4, 0x33, 0xDF,
    0xF0, 0xBA, 0xBD, 0x4E,
    0xA7, 0x7E, 0x86, 0xA6,
    0x76, 0x16, 0x5E, 0x40,
    0xF2, 0x73, 0x60, 0xCE
};

uint8_t metisTestDataV0_SecondInterest[] = {
    0x00, 0x01, 0x00, 0x36, // ver = 0, type = interest, length = 54
    0x00, 0x00, 0x00, 0x15, // reserved = 0, header length = 21
    // ------------------------
    0x00, 0x02, 0x00, 0x01, // HOPLIMIT
    0x20, // 32
    // ------------------------
    0x00, 0x03, 0x00, 0x0C, // Interest Fragment
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, // fragment 0x0102030405060708
    0x05, 0xDC, 0x04, 0x00, // MTU 1500, fragcnt 4, fragnum 0
    // ------------------------
    0x00, 0x01, 0x00, 0x32, // type = interest, length = 50
    // ------------------------
    0x00, 0x00, 0x00, 0x11, // type = name, length = 17
    0x00, 0x02, 0x00, 0x05,  // type = binary, length = 5
    'h',  'e',  'l',  'l',
    'o',  // "hello"
    0xF0, 0x00, 0x00, 0x04, // type = app, length = 4
    'o',  'u',  'c',  'h',  // "ouch"
    // ------------------------
    0x00, 0x01, 0x00, 0x04, // type = keyid, length = 4
    0x11, 0x11, 0x11, 0x11, // 0x11111111
    // ------------------------
    0x00, 0x02, 0x00, 0x06, // type = objhash, length = 6
    0x11, 0x22, 0x33, 0x44,
    0x55, 0x66, // 0x112233445566
    // ------------------------
    0x00, 0x03, 0x00, 0x01, // scope, length = 1
    0x02, // value = 2
    // ------------------------
    0x00, 0x05, 0x00, 0x02, // interest lifetime, length = 2
    0x7D, 0x00              // 32000
};

uint8_t metisTestDataV0_EncodedObject[] = {
    0x00, 0x02, 0x00, 0x3A, // ver = 0, type = object, length = 58
    0x00, 0x00, 0x00, 0x1D, // reserved = 0, header length = 29
    // ------------------------
    0x00, 0x02, 0x00, 0x01, // HOPLIMIT
    0x20, // 32
    // ------------------------
    0x00, 0x04, 0x00, 0x14, // object Fragment
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, // fragment 0x0102030405060708
    0x05, 0xDC, 0x04, 0x00, // MTU 1500, fragcnt 4, fragnum 0
    0x12, 0x23, 0x34, 0x45,
    0x56, 0x67, 0x78, 0x89, // interest fragid 0x1223344556677889
    // ------------------------
    0x00, 0x02, 0x00, 0x36, // type = object, length = 54
    // ------------------------
    0x00, 0x00, 0x00, 0x11,   // type = name, length = 17
    0x00, 0x02, 0x00, 0x05,  // type = binary length = 5 (OFFSET is 45 here)
    'h',  'e',  'l',  'l',
    'o',                      // "hello"
    0xF0, 0x00, 0x00, 0x04,   // type = app, length = 4
    'o',  'u',  'c',  'h',
    // ------------------------
    0x00, 0x02, 0x00, 0x08, // nameauth, length = 8
    0x00, 0x01, 0x00, 0x04, // type = keyid, length = 4
    0xA0, 0xB0, 0xC0, 0xD0, // value = 0xA0B0C0D0
    // ------------------------
    0x00, 0x04, 0x00, 0x07, // contents, length = 7
    0xAA, 0xBB, 0xCC, 0xDD,
    0xEE, 0xFF, 0xFF,       // value = 0xAABBCCDDEEFFFF
    // ------------------------
    0x00, 0x05, 0x00, 0x06, // signature block, length = 6
    0x00, 0x0E, 0x00, 0x02, // signature bits, length = 2
    0x00, 0x00              // value = 0x0000
};

const MetisTlvExtent metisTestDataV0_EncodedObject_name = { .offset = 45, .length = 17 };
const MetisTlvExtent metisTestDataV0_EncodedObject_keyid = { .offset = 70, .length = 4 };

uint8_t metisTestDataV0_EncodedObject_no_name[] = {
    0x00, 0x02, 0x00, 0x3A,     // ver = 0, type = object, length = 58
    0x00, 0x00, 0x00, 0x1D,     // reserved = 0, header length = 29
    // ------------------------
    0x00, 0x02, 0x00, 0x01,     // HOPLIMIT
    0x20, // 32
    // ------------------------
    0x00, 0x04, 0x00, 0x14,     // object Fragment
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08,     // fragment 0x0102030405060708
    0x05, 0xDC, 0x04, 0x00,     // MTU 1500, fragcnt 4, fragnum 0
    0x12, 0x23, 0x34, 0x45,
    0x56, 0x67, 0x78, 0x89,     // interest fragid 0x1223344556677889
    // ------------------------
    0x00, 0x02, 0x00, 0x36,     // type = object, length = 54
    // ------------------------
    0xFF, 0xFF, 0x00, 0x11,     // type = Unknown, length = 17
    0x00, 0x02, 0x00, 0x05,    // type = binary, length = 5 OFFSET = 45
    'h',  'e',  'l',  'l',
    'o',                        // "hello"
    0xF0, 0x00, 0x00, 0x04,     // type = app, length = 4
    'o',  'u',  'c',  'h',
    // ------------------------
    0x00, 0x02, 0x00, 0x08,     // nameauth, length = 8
    0x00, 0x01, 0x00, 0x04,     // type = keyid, length = 4
    0xA0, 0xB0, 0xC0, 0xD0,     // value = 0xA0B0C0D0
    // ------------------------
    0x00, 0x04, 0x00, 0x07,     // contents, length = 7
    0xAA, 0xBB, 0xCC, 0xDD,
    0xEE, 0xFF, 0xFF,           // value = 0xAABBCCDDEEFFFF
    // ------------------------
    0x00, 0x05, 0x00, 0x06,      // signature block, length = 6
    0x00, 0x0E, 0x00, 0x02,      // signature bits, length = 2
    0x00, 0x00                   // value = 0x0000
};

/**
 * Same name as metisTestDataV0_EncodedObject, but different keyid and thus different object hash
 */
uint8_t metisTestDataV0_SecondObject[] = {
    0x00, 0x02, 0x00, 0x3E, // ver = 0, type = object, length = 62
    0x00, 0x00, 0x00, 0x1D, // reserved = 0, header length = 29
    // ------------------------
    0x00, 0x02, 0x00, 0x01, // HOPLIMIT
    0x20, // 32
    // ------------------------
    0x00, 0x04, 0x00, 0x14, // object Fragment
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, // fragment 0x0102030405060708
    0x05, 0xDC, 0x04, 0x00, // MTU 1500, fragcnt 4, fragnum 0
    0x12, 0x23, 0x34, 0x45,
    0x56, 0x67, 0x78, 0x89, // interest fragid 0x1223344556677889
    // ------------------------
    0x00, 0x02, 0x00, 0x36, // type = object, length = 54
    // ------------------------
    0x00, 0x00, 0x00, 0x11,    // type = name, length = 17
    0x00, 0x02, 0x00, 0x05,   // type = binary, length = 5 OFFSET = 45
    'h',  'e',  'l',  'l',
    'o',                       // "hello"
    0xF0, 0x00, 0x00, 0x04,    // type = app, length = 4
    'o',  'u',  'c',  'h',
    // ------------------------
    0x00, 0x02, 0x00, 0x08, // nameauth, length = 8
    0x00, 0x01, 0x00, 0x08, // type = keyid, length = 8
    0xA0, 0xB0, 0xC0, 0xD0, // value = 0xA0B0C0D011223344
    0x11, 0x22, 0x33, 0x44,
    // ------------------------
    0x00, 0x04, 0x00, 0x07, // contents, length = 7
    0xAA, 0xBB, 0xCC, 0xDD,
    0xEE, 0xFF, 0xFF, // value = 0xAABBCCDDEEFFFF
    // ------------------------
    0x00, 0x05, 0x00, 0x06, // signature block, length = 6
    0x00, 0x0E, 0x00, 0x02, // signature bits, length = 2
    0x00, 0x00              // value = 0x0000
};

uint8_t metisTestDataV0_object_with_othername[] = {
    0x00, 0x02, 0x00, 0x3E, // ver = 0, type = object, length = 62
    0x00, 0x00, 0x00, 0x1D, // reserved = 0, header length = 29
    // ------------------------
    0x00, 0x02, 0x00, 0x01, // HOPLIMIT
    0x20, // 32
    // ------------------------
    0x00, 0x04, 0x00, 0x14, // object Fragment
    0x01, 0x02, 0x03, 0x04,
    0x05, 0x06, 0x07, 0x08, // fragment 0x0102030405060708
    0x05, 0xDC, 0x04, 0x00, // MTU 1500, fragcnt 4, fragnum 0
    0x12, 0x23, 0x34, 0x45,
    0x56, 0x67, 0x78, 0x89, // interest fragid 0x1223344556677889
    // ------------------------
    0x00, 0x02, 0x00, 0x36, // type = object, length = 54
    // ------------------------
    0x00, 0x00, 0x00, 0x11, // type = name, length = 17
    0x00, 0x02, 0x00, 0x05, // type = binary, length = 5   OFFSET = 45
    'p',  'a',  'r',  't',  // "party"
    'y',
    0xF0, 0x00, 0x00, 0x04, // type = app, length = 4
    'o',  'u',  'c',  'h',
    // ------------------------
    0x00, 0x02, 0x00, 0x08, // nameauth, length = 8
    0x00, 0x01, 0x00, 0x08, // type = keyid, length = 8
    0xA0, 0xB0, 0xC0, 0xD0, // value = 0xA0B0C0D011223344
    0x11, 0x22, 0x33, 0x44,
    // ------------------------
    0x00, 0x04, 0x00, 0x07, // contents, length = 7
    0xAA, 0xBB, 0xCC, 0xDD,
    0xEE, 0xFF, 0xFF, // value = 0xAABBCCDDEEFFFF
    // ------------------------
    0x00, 0x05, 0x00, 0x06, // signature block, length = 6
    0x00, 0x0E, 0x00, 0x02, // signature bits, length = 2
    0x00, 0x00              // value = 0x0000
};


/**
 * An example control message
 */
uint8_t metisTestDataV0_CPIMessage[] = {
    0x00, 0xa4, 0x00, 0x33,// ver = 0, type = control, length = 51
    0x00, 0x00, 0x00, 0x00,
    // ------------------------
    0xbe, 0xef, 0x00, 0x2f,
    0x7b, 0x22, 0x43, 0x50,0x49,  0x5f, 0x52, 0x45,   // {"CPI_RE
    0x51, 0x55, 0x45, 0x53,0x54,  0x22, 0x3a, 0x7b,   // QUEST":{
    0x22, 0x53, 0x45, 0x51,0x55,  0x45, 0x4e, 0x43,   // "SEQUENC
    0x45, 0x22, 0x3a, 0x31,0x2c,  0x22, 0x52, 0x4f,   // E":1,"RO
    0x55, 0x54, 0x45, 0x5f,0x4c,  0x49, 0x53, 0x54,   // UTE_LIST
    0x22, 0x3a, 0x7b, 0x7d,0x7d,  0x7d, 0x00,         // ":{}}}
};
#endif // Metis_metis_TestDataV0_h
