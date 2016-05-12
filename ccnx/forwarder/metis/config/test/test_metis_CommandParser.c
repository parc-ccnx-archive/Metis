/*
 * Copyright (c) 2014-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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

// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Framework.
#include "../metis_CommandParser.c"
#include <parc/algol/parc_SafeMemory.h>
#include <LongBow/unit-test.h>

LONGBOW_TEST_RUNNER(metis_CommandParser)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Local);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(metis_CommandParser)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(metis_CommandParser)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, metisCommandParser_Create_Destroy);

    LONGBOW_RUN_TEST_CASE(Global, metisCommandParser_DispatchCommand_Exact);
    LONGBOW_RUN_TEST_CASE(Global, metisCommandParser_DispatchCommand_Longer);
    LONGBOW_RUN_TEST_CASE(Global, metisCommandParser_DispatchCommand_Shorter);
    LONGBOW_RUN_TEST_CASE(Global, metisCommandParser_DispatchCommand_Sibling);
    LONGBOW_RUN_TEST_CASE(Global, metisCommandParser_GetDebug);
    LONGBOW_RUN_TEST_CASE(Global, metisCommandParser_Interactive);
    LONGBOW_RUN_TEST_CASE(Global, metisCommandParser_RegisterCommand_NullInit);
    LONGBOW_RUN_TEST_CASE(Global, metisCommandParser_RegisterCommand_WithInit);
    LONGBOW_RUN_TEST_CASE(Global, metisCommandParser_SetDebug);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, metisCommandParser_Create_Destroy)
{
    MetisCommandParser *parser = metisCommandParser_Create();
    assertNotNull(parser, "Got null parser from metisCommandParser_Create");
    metisCommandParser_Destroy(&parser);
    assertTrue(parcSafeMemory_ReportAllocation(STDOUT_FILENO) == 0, "Memory imbalance!");
    assertNull(parser, "metisCommandParser_Destroy did not null pointer");
}

static MetisCommandReturn
test_execute(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args)
{
    bool *execute_called_ptr = (bool *) ops->closure;
    *execute_called_ptr = true;
    return MetisCommandReturn_Success;
}

/**
 * argc = the exact number of args, don't include the command name
 * example: argc = 2, argv = {"Hello", "World"}
 *
 * expectedResult true means the execute function is called
 */
static void
dispatchCommand(const char *command_string, int argc, char **argv, bool expectedResult)
{
    MetisCommandParser *parser = metisCommandParser_Create();

    bool execute_called = false;

    MetisCommandOps *ops = metisCommandOps_Create(&execute_called, command_string, NULL, test_execute, metisCommandOps_Destroy);

    PARCList *args = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);
    parcList_AddAll(args, argc, (void **) &argv[0]);

    execute_called = false;
    metisCommandParser_RegisterCommand(parser, ops);
    metisCommandParser_DispatchCommand(parser, args);
    if (expectedResult) {
        assertTrue(execute_called, "Did not call the execute function");
    } else {
        assertFalse(execute_called, "The execute function should not have been called but was");
    }

    metisCommandParser_Destroy(&parser);
    parcList_Release(&args);
}

LONGBOW_TEST_CASE(Global, metisCommandParser_DispatchCommand_Exact)
{
    // note that it is not case sensitive
    dispatchCommand("hello world", 2, (char *[]) { "Hello", "World" }, true);
}

LONGBOW_TEST_CASE(Global, metisCommandParser_DispatchCommand_Sibling)
{
    // note that it is not case sensitive
    dispatchCommand("hello world", 2, (char *[]) { "Hello", "Universe" }, false);
}


LONGBOW_TEST_CASE(Global, metisCommandParser_DispatchCommand_Longer)
{
    // note that it is not case sensitive
    dispatchCommand("hello world", 3, (char *[]) { "Hello", "World", "Again" }, true);
}

LONGBOW_TEST_CASE(Global, metisCommandParser_DispatchCommand_Shorter)
{
    // note that it is not case sensitive
    dispatchCommand("hello world", 1, (char *[]) { "Hello" }, false);
}

LONGBOW_TEST_CASE(Global, metisCommandParser_GetDebug)
{
    MetisCommandParser *parser = metisCommandParser_Create();
    bool test = metisCommandParser_GetDebug(parser);
    assertTrue(test == parser->debugFlag, "Got %d expected %d", test, parser->debugFlag);
    metisCommandParser_Destroy(&parser);
}

LONGBOW_TEST_CASE(Global, metisCommandParser_Interactive)
{
    testUnimplemented("");
}

static bool called_init = false;
static void
test_init_command(MetisCommandParser *parser, MetisCommandOps *ops)
{
    called_init = true;
}

LONGBOW_TEST_CASE(Global, metisCommandParser_RegisterCommand_WithInit)
{
    MetisCommandParser *parser = metisCommandParser_Create();

    MetisCommandOps *ops = metisCommandOps_Create(NULL, "hello world", test_init_command, test_execute, metisCommandOps_Destroy);

    called_init = false;
    metisCommandParser_RegisterCommand(parser, ops);

    MetisCommandOps *test = parcTreeRedBlack_Get(parser->commandTree, ops->command);
    assertNotNull(test, "Got null looking up command in tree");
    assertTrue(test == ops, "Wrong pointer, got %p expected %p", (void *) test, (void *) ops);
    assertTrue(called_init, "Did not call the init function");

    metisCommandParser_Destroy(&parser);
}

LONGBOW_TEST_CASE(Global, metisCommandParser_RegisterCommand_NullInit)
{
    MetisCommandParser *parser = metisCommandParser_Create();

    MetisCommandOps command = {
        .command = "hello world",
        .init    = NULL,
        .execute = NULL
    };

    called_init = false;
    metisCommandParser_RegisterCommand(parser, &command);

    MetisCommandOps *test = parcTreeRedBlack_Get(parser->commandTree, command.command);
    assertNotNull(test, "Got null looking up command in tree");
    assertTrue(test == &command, "Wrong pointer, got %p expected %p", (void *) test, (void *) &command);
    assertFalse(called_init, "Somehow called the init function");

    metisCommandParser_Destroy(&parser);
}

LONGBOW_TEST_CASE(Global, metisCommandParser_SetDebug)
{
    MetisCommandParser *parser = metisCommandParser_Create();
    // flip the setting
    bool truth = ~parser->debugFlag;
    metisCommandParser_SetDebug(parser, truth);
    assertTrue(truth == parser->debugFlag, "Got %d expected %d", parser->debugFlag, truth);
    metisCommandParser_Destroy(&parser);
}

LONGBOW_TEST_FIXTURE(Local)
{
    LONGBOW_RUN_TEST_CASE(Local, metisCommandParser_MatchCommand);
    LONGBOW_RUN_TEST_CASE(Local, parseStringIntoTokens);
    LONGBOW_RUN_TEST_CASE(Local, stringCompare);
}

LONGBOW_TEST_FIXTURE_SETUP(Local)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Local)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Local, metisCommandParser_MatchCommand)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Local, parseStringIntoTokens)
{
    testUnimplemented("");
}

LONGBOW_TEST_CASE(Local, stringCompare)
{
    testUnimplemented("");
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(metis_CommandParser);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
