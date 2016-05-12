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
 * Common operations for the metisControl tests.  This C module
 * is intended to be #include'd in to each test.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <LongBow/unit-test.h>

#include "../metis_ControlState.c"
#include <parc/algol/parc_SafeMemory.h>
#include <ccnx/forwarder/metis/config/metis_CommandParser.h>
#include <ccnx/api/control/controlPlaneInterface.h>

typedef struct test_data {
    MetisControlState *state;
    unsigned writeread_count;

    // If the user specifies this, it will be used as the reply to all test_WriteRead calls
    CCNxControl * (*customWriteReadReply)(void *userdata, CCNxMetaMessage * messageToWrite);
} TestData;

/**
 * As part of the testrig, we simply create a CPIAck of the request message.
 * We also increment the call count in TestData.
 *
 * If the user specified a customWriteReadReply function, we will call that to get
 * the specific response to send.
 */
static CCNxMetaMessage *
test_WriteRead(void *userdata, CCNxMetaMessage *messageToWrite)
{
    TestData *data = (TestData *) userdata;
    data->writeread_count++;

    assertTrue(ccnxMetaMessage_IsControl(messageToWrite), "messageToWrite is not a control message");

    CCNxControl *response;
    CCNxMetaMessage *result;

    if (data->customWriteReadReply == NULL) {
        CCNxControl *request = ccnxMetaMessage_GetControl(messageToWrite);
        PARCJSON *json = ccnxControl_GetJson(request);
        PARCJSON *jsonAck = cpiAcks_CreateAck(json);

        response = ccnxControl_CreateCPIRequest(jsonAck);
        result = ccnxMetaMessage_CreateFromControl(response);

        parcJSON_Release(&jsonAck);
        ccnxControl_Release(&response);
    } else {
        response = data->customWriteReadReply(userdata, messageToWrite);
        assertTrue(ccnxMetaMessage_IsControl(response), "response is not a control message");
        result = ccnxMetaMessage_CreateFromControl(response);
        ccnxControl_Release(&response);
    }

    return result;
}

static void
testrigMetisControl_commonSetup(const LongBowTestCase *testCase)
{
    TestData *data = parcMemory_AllocateAndClear(sizeof(TestData));
    assertNotNull(data, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(TestData));
    memset(data, 0, sizeof(TestData));

    data->state = metisControlState_Create(data, test_WriteRead);
    longBowTestCase_SetClipBoardData(testCase, data);
}

static void
testrigMetisControl_CommonTeardown(const LongBowTestCase *testCase)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    metisControlState_Destroy(&data->state);
    parcMemory_Deallocate((void **) &data);
}

/**
 * Verify that a Command Create operated correctly
 *
 * We verify the basic properties of what a Create returns.  Will assert if a failure.
 *
 * @param [in] testCase The LongBow test case (used for the clipboard)
 * @param [in] create The command create function pointer to test
 * @param [in] title The descriptive title to display in case of error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void
testCommandCreate(const LongBowTestCase *testCase, MetisCommandOps * (*create)(MetisControlState * state), const char *title)
{
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = create(data->state);
    assertNotNull(ops, "%s: Got null ops", title);
    assertNotNull(ops->execute, "%s: Ops execute must not be null", title);
    assertNotNull(ops->command, "%s: Ops command must not be null", title);
    assertTrue(ops->closure == data->state, "%s: ops closure should be data->state, got wrong pointer", title);

    metisCommandOps_Destroy(&ops);
    assertNull(ops, "Ops not nulled by Destroy");
}

/**
 * Test a Help command's execution.
 *
 * A Help execution will display text (which we don't test).  We make sure there
 * is no memory leak and that it returns successfully.  We will call the passed create method
 * to create the Help command then execute its execute.
 *
 * @param [in] testCase The LongBow test case (used for the clipboard)
 * @param [in] create The command create function pointer to test
 * @param [in] title The descriptive title to display in case of error
 * @param [in] expected A MetisCommandReturn to use as the expected result
 *
 * Example:
 * @code
 * {
 *    // expectes MetisCommandReturn_Success
 *    testHelpExecute(testCase, metisControl_Add_Create, __func__, MetisCommandReturn_Success);
 *
 *    // expectes MetisCommandReturn_Exit
 *    testHelpExecute(testCase, metisControl_Quit_Create, __func__, MetisCommandReturn_Exit);
 * }
 * @endcode
 */
void
testHelpExecute(const LongBowTestCase *testCase, MetisCommandOps * (*create)(MetisControlState * state), const char *title, MetisCommandReturn expected)
{
    uint32_t beforeMemory = parcMemory_Outstanding();
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = create(data->state);
    MetisCommandReturn result = ops->execute(NULL, ops, NULL);
    assertTrue(result == expected, "Wrong return, got %d expected %d", result, expected);
    metisCommandOps_Destroy(&ops);
    uint32_t afterMemory = parcMemory_Outstanding();

    assertTrue(beforeMemory == afterMemory, "Memory leak by %d\n", (int) (afterMemory - beforeMemory));
}

/**
 * Verify that a list of commands is added by the Init function
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] testCase The LongBow test case (used for the clipboard)
 * @param [in] create We will create one of these and call it's init() function
 * @param [in] title The descriptive title to display in case of error
 * @param [in] commandList Null terminated list of commands
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void
testInit(const LongBowTestCase *testCase, MetisCommandOps * (*create)(MetisControlState * state), const char *title, const char **commandList)
{
    // this will register 8 commands, so check they exist
    TestData *data = longBowTestCase_GetClipBoardData(testCase);
    MetisCommandOps *ops = create(data->state);
    assertNotNull(ops, "%s got null ops from the create function", title);
    assertNotNull(ops->init, "%s got null ops->init from the create function", title);

    ops->init(data->state->parser, ops);

    for (int i = 0; commandList[i] != NULL; i++) {
        bool success = metisCommandParser_ContainsCommand(data->state->parser, commandList[i]);
        assertTrue(success, "%s: Missing: %s", title, commandList[i]);
    }

    metisCommandOps_Destroy(&ops);
}
