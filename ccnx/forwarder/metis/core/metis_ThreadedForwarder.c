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
 * @header Metis Threaded Forwarder
 * @abstract A thread wrapper around metis_Forwarder.
 * @discussion
 *     Cannot restart a thread after its stopped.  I think this should be ok, but
 *     have not had time to test it yet, so dont support it.
 *
 *     This wrapper does not expose any of the metis_Forwarder calls, as those
 *     are all non-threaded calls.  You can only create, start, stop, and destroy
 *     the forwarder.  All configuration needs to be via the CLI or via CPI control messages.
 *
 *     You may run multiple Metis forwarders as long as they are on different ports.
 *
 * @author Marc Mosko on 12/17/13
 * @copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <parc/algol/parc_Memory.h>
#include <LongBow/runtime.h>

#include <ccnx/forwarder/metis/core/metis_Forwarder.h>
#include <ccnx/forwarder/metis/core/metis_ThreadedForwarder.h>
#include <ccnx/forwarder/metis/core/metis_Dispatcher.h>
#include <ccnx/forwarder/metis/config/metis_Configuration.h>
#include <ccnx/forwarder/metis/config/metis_CommandLineInterface.h>

struct metis_threaded_forwarder {
    pthread_t thread;
    pthread_mutex_t state_mutex;
    pthread_cond_t state_cond;

    // indicates that the Start function was called
    bool started;

    // indicates that the thread has entered the Run function and is running
    bool running;

    MetisForwarder *forwarder;
    MetisLogger *logger;
    MetisCommandLineInterface *cli;
};

static void
metisThreadedForwarder_LockState(MetisThreadedForwarder *threadedMetis)
{
    int res = pthread_mutex_lock(&threadedMetis->state_mutex);
    assertTrue(res == 0, "error from pthread_mutex_lock: %d", res);
}

static void
metisThreadedForwarder_UnlockState(MetisThreadedForwarder *threadedMetis)
{
    int res = pthread_mutex_unlock(&threadedMetis->state_mutex);
    assertTrue(res == 0, "error from pthread_mutex_unlock: %d", res);
}

static void
metisThreadedForwarder_WaitStatus(MetisThreadedForwarder *threadedMetis)
{
    int res = pthread_cond_wait(&threadedMetis->state_cond, &threadedMetis->state_mutex);
    assertTrue(res == 0, "error from pthread_mutex_unlock: %d", res);
}

static void
metisThreadedForwarder_BroadcastStatus(MetisThreadedForwarder *threadedMetis)
{
    int res = pthread_cond_broadcast(&threadedMetis->state_cond);
    assertTrue(res == 0, "error from pthread_mutex_unlock: %d", res);
}

static void *
metisThreadedForwarder_Run(void *arg)
{
    MetisThreadedForwarder *threadedMetis = (MetisThreadedForwarder *) arg;

    metisThreadedForwarder_LockState(threadedMetis);
    assertFalse(threadedMetis->running, "Invalid State: forwarder already in running state");
    threadedMetis->running = true;
    metisThreadedForwarder_BroadcastStatus(threadedMetis);
    metisThreadedForwarder_UnlockState(threadedMetis);

    // --------
    // Block in the dispatch loop
    MetisDispatcher *dispatcher = metisForwarder_GetDispatcher(threadedMetis->forwarder);
    metisDispatcher_Run(dispatcher);
    // --------

    metisThreadedForwarder_LockState(threadedMetis);
    assertTrue(threadedMetis->running, "Invalid State: forwarder indicates its not running!");
    threadedMetis->running = false;
    metisThreadedForwarder_BroadcastStatus(threadedMetis);
    metisThreadedForwarder_UnlockState(threadedMetis);

    pthread_exit(NULL);
}

// ===========================

MetisThreadedForwarder *
metisThreadedForwarder_Create(MetisLogger *logger)
{
    struct sigaction ignore_action;
    ignore_action.sa_handler = SIG_IGN;
    sigemptyset(&ignore_action.sa_mask);
    ignore_action.sa_flags = 0;
    //    sigaction(SIGPIPE, NULL, &save_sigpipe);
    sigaction(SIGPIPE, &ignore_action, NULL);


    MetisThreadedForwarder *threadedMetis = parcMemory_AllocateAndClear(sizeof(MetisThreadedForwarder));
    assertNotNull(threadedMetis, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisThreadedForwarder));
    threadedMetis->logger = metisLogger_Acquire(logger);
    threadedMetis->forwarder = metisForwarder_Create(logger);

    pthread_mutex_init(&threadedMetis->state_mutex, NULL);
    pthread_cond_init(&threadedMetis->state_cond, NULL);

    threadedMetis->thread = (pthread_t) { 0 };
    threadedMetis->cli = NULL;
    threadedMetis->running = false;
    return threadedMetis;
}

void
metisThreadedForwarder_AddCLI(MetisThreadedForwarder *threadedMetis, uint16_t port)
{
    assertNotNull(threadedMetis, "Parameter must be non-null");
    assertFalse(threadedMetis->started, "Must be done prior to starting!");
    assertNull(threadedMetis->cli, "Can only define one CLI");

    threadedMetis->cli = metisCommandLineInterface_Create(threadedMetis->forwarder, port);

    // this sets up all the network events in the dispatcher so when the thread is
    // started, the CLI will be ready to go.
    metisCommandLineInterface_Start(threadedMetis->cli);
}

void
metisThreadedForwarder_SetupAllListeners(MetisThreadedForwarder *threadedMetis, uint16_t port, const char *localPath)
{
    assertNotNull(threadedMetis, "Parameter must be non-null");
    assertFalse(threadedMetis->started, "Must be done prior to starting!");

    metisForwarder_SetupAllListeners(threadedMetis->forwarder, port, localPath);
}

void
metisThreadedForwarder_Start(MetisThreadedForwarder *threadedMetis)
{
    assertNotNull(threadedMetis, "Parameter must be non-null");
    assertFalse(threadedMetis->started, "Must be done prior to starting!");

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int failure = pthread_create(&threadedMetis->thread, &attr, metisThreadedForwarder_Run, threadedMetis);
    assertFalse(failure, "Eror creating thread: %d", failure);

    // block until running
    metisThreadedForwarder_LockState(threadedMetis);
    while (!threadedMetis->running) {
        metisThreadedForwarder_WaitStatus(threadedMetis);
    }
    metisThreadedForwarder_UnlockState(threadedMetis);
}

/**
 * @function metisThreadedForwarder_Stop
 * @abstract Blocks until stopped
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
void
metisThreadedForwarder_Stop(MetisThreadedForwarder *threadedMetis)
{
    assertNotNull(threadedMetis, "Parameter must be non-null");

    // These are explicitly thread-safe operations inside Metis
    MetisDispatcher *dispatcher = metisForwarder_GetDispatcher(threadedMetis->forwarder);
    metisDispatcher_Stop(dispatcher);

    // Equivalently, we could block until joined

    // block until stopped
    metisThreadedForwarder_LockState(threadedMetis);
    while (threadedMetis->running) {
        metisThreadedForwarder_WaitStatus(threadedMetis);
    }
    metisThreadedForwarder_UnlockState(threadedMetis);
}

/**
 * @function metisThreadedForwarder_Destroy
 * @abstract Blocks until stopped and destoryed
 * @discussion
 *   <#Discussion#>
 *
 * @param <#param1#>
 * @return <#return#>
 */
void
metisThreadedForwarder_Destroy(MetisThreadedForwarder **threadedMetisPtr)
{
    assertNotNull(threadedMetisPtr, "Parameter must be non-null double pointer");
    assertNotNull(*threadedMetisPtr, "Parameter must dereference to non-null pointer");

    MetisThreadedForwarder *threadedMetis = *threadedMetisPtr;
    metisThreadedForwarder_Stop(threadedMetis);

    pthread_mutex_destroy(&threadedMetis->state_mutex);
    pthread_cond_destroy(&threadedMetis->state_cond);

    metisLogger_Release(&threadedMetis->logger);
    metisForwarder_Destroy(&threadedMetis->forwarder);
    parcMemory_Deallocate((void **) &threadedMetis);
    *threadedMetisPtr = NULL;
}
