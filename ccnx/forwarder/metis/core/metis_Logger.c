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

#include <config.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include <LongBow/runtime.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Object.h>

#include <parc/logging/parc_Log.h>

#include <ccnx/forwarder/metis/core/metis_Logger.h>
#include <ccnx/forwarder/metis/core/metis_Forwarder.h>

struct metis_logger {
    PARCClock *clock;

    PARCLogReporter *reporter;
    PARCLog *loggerArray[MetisLoggerFacility_END];
};

static const struct facility_to_string {
    MetisLoggerFacility facility;
    const char *string;
} _facilityToString[] = {
    { .facility = MetisLoggerFacility_Config,    .string = "Config"    },
    { .facility = MetisLoggerFacility_Core,      .string = "Core"      },
    { .facility = MetisLoggerFacility_IO,        .string = "IO"        },
    { .facility = MetisLoggerFacility_Message,   .string = "Message"   },
    { .facility = MetisLoggerFacility_Processor, .string = "Processor" },
    { .facility = 0,                             .string = NULL        }
};

const char *
metisLogger_FacilityString(MetisLoggerFacility facility)
{
    for (int i = 0; _facilityToString[i].string != NULL; i++) {
        if (_facilityToString[i].facility == facility) {
            return _facilityToString[i].string;
        }
    }
    return "Unknown";
}

static void
_allocateLoggers(MetisLogger *logger, PARCLogReporter *reporter)
{
    trapUnexpectedStateIf(logger->reporter != NULL, "Trying to allocate a reporter when the previous one is not null");
    logger->reporter = parcLogReporter_Acquire(reporter);

    char hostname[255];
    int gotHostName = gethostname(hostname, 255);
    if (gotHostName < 0) {
        snprintf(hostname, 255, "unknown");
    }

    for (int i = 0; i < MetisLoggerFacility_END; i++) {
        logger->loggerArray[i] = parcLog_Create(hostname, metisLogger_FacilityString(i), "metis", logger->reporter);
        parcLog_SetLevel(logger->loggerArray[i], PARCLogLevel_Error);
    }
}

static void
_releaseLoggers(MetisLogger *logger)
{
    for (int i = 0; i < MetisLoggerFacility_END; i++) {
        parcLog_Release(&logger->loggerArray[i]);
    }
    parcLogReporter_Release(&logger->reporter);
}

static void
_destroyer(MetisLogger **loggerPtr)
{
    MetisLogger *logger = *loggerPtr;
    _releaseLoggers(logger);
    parcClock_Release(&(*loggerPtr)->clock);
}

parcObject_ExtendPARCObject(MetisLogger, _destroyer, NULL, NULL, NULL, NULL, NULL, NULL);

parcObject_ImplementAcquire(metisLogger, MetisLogger);

parcObject_ImplementRelease(metisLogger, MetisLogger);

MetisLogger *
metisLogger_Create(PARCLogReporter *reporter, const PARCClock *clock)
{
    assertNotNull(reporter, "Parameter reporter must be non-null");
    assertNotNull(clock, "Parameter clock must be non-null");

    MetisLogger *logger = parcObject_CreateAndClearInstance(MetisLogger);
    if (logger) {
        logger->clock = parcClock_Acquire(clock);
        _allocateLoggers(logger, reporter);
    }

    return logger;
}

void
metisLogger_SetReporter(MetisLogger *logger, PARCLogReporter *reporter)
{
    assertNotNull(logger, "Parameter logger must be non-null");

    // save the log level state
    PARCLogLevel savedLevels[MetisLoggerFacility_END];
    for (int i = 0; i < MetisLoggerFacility_END; i++) {
        savedLevels[i] = parcLog_GetLevel(logger->loggerArray[i]);
    }

    _releaseLoggers(logger);

    _allocateLoggers(logger, reporter);

    // restore log level state
    for (int i = 0; i < MetisLoggerFacility_END; i++) {
        parcLog_SetLevel(logger->loggerArray[i], savedLevels[i]);
    }
}

void
metisLogger_SetClock(MetisLogger *logger, PARCClock *clock)
{
    assertNotNull(logger, "Parameter logger must be non-null");
    parcClock_Release(&logger->clock);
    logger->clock = parcClock_Acquire(clock);
}

static void
_assertInvariants(const MetisLogger *logger, MetisLoggerFacility facility)
{
    assertNotNull(logger, "Parameter logger must be non-null");
    trapOutOfBoundsIf(facility >= MetisLoggerFacility_END, "Invalid facility %d", facility);
}

void
metisLogger_SetLogLevel(MetisLogger *logger, MetisLoggerFacility facility, PARCLogLevel minimumLevel)
{
    _assertInvariants(logger, facility);
    PARCLog *log = logger->loggerArray[facility];
    parcLog_SetLevel(log, minimumLevel);
}

bool
metisLogger_IsLoggable(const MetisLogger *logger, MetisLoggerFacility facility, PARCLogLevel level)
{
    _assertInvariants(logger, facility);
    PARCLog *log = logger->loggerArray[facility];
    return parcLog_IsLoggable(log, level);
}

void
metisLogger_Log(MetisLogger *logger, MetisLoggerFacility facility, PARCLogLevel level, const char *module, const char *format, ...)
{
    if (metisLogger_IsLoggable(logger, facility, level)) {
        // this is logged as the messageid
        uint64_t logtime = parcClock_GetTime(logger->clock);

        // metisLogger_IsLoggable asserted invariants so we know facility is in bounds
        PARCLog *log = logger->loggerArray[facility];

        va_list va;
        va_start(va, format);

        parcLog_MessageVaList(log, level, logtime, format, va);

        va_end(va);
    }
}

