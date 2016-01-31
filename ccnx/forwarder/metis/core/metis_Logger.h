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
 * @file metis_Logger.h
 * @brief Logger for the Metis forwarder
 *
 * A facility based logger to allow selective logging from different parts of Metis
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#ifndef Metis_metis_Logger_h
#define Metis_metis_Logger_h

#include <sys/time.h>
#include <stdarg.h>
#include <parc/algol/parc_Buffer.h>
#include <parc/logging/parc_LogLevel.h>
#include <parc/logging/parc_LogReporter.h>

#include <parc/algol/parc_Clock.h>

struct metis_logger;
typedef struct metis_logger MetisLogger;

/**
 * CONFIG faciilty concerns anything in the /config directory
 * CORE concerns anything in the /core directory
 * IO concerns anything in the /io directory (listeners, connectors, tcp, ethernet, etc.)
 * PROCESSOR concerns FIB, PIT, CS
 * MESSAGE concerns message events, like parsing
 */
typedef enum {
    MetisLoggerFacility_Config,
    MetisLoggerFacility_Core,
    MetisLoggerFacility_IO,
    MetisLoggerFacility_Processor,
    MetisLoggerFacility_Message,
    MetisLoggerFacility_END          // sentinel value
} MetisLoggerFacility;

/**
 * Returns a string representation of a facility
 *
 * Do not free the returned value.
 *
 * @param [in] facility The facility to change to a string
 *
 * @retval string A string representation of the facility
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
const char * metisLogger_FacilityString(MetisLoggerFacility facility);

/**
 * Returns a string representation of a log level
 *
 * Do not free the returned value.
 *
 * @param [in] level The level to change to a string
 *
 * @retval string A string representation of the level
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
const char * metisLogger_LevelString(PARCLogLevel level);

/**
 * Create a logger that uses a given writer and clock
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [in] writer The output writer
 * @param [in] clock The clock to use for log messages
 *
 * @retval non-null An allocated logger
 * @retval null An error
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisLogger * metisLogger_Create(PARCLogReporter *reporter, const PARCClock *clock);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @retval <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisLogger_Release(MetisLogger **loggerPtr);

/**
 * <#One Line Description#>
 *
 * <#Paragraphs Of Explanation#>
 *
 * @param [<#in out in,out#>] <#name#> <#description#>
 *
 * @retval <#value#> <#explanation#>
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
MetisLogger *metisLogger_Acquire(const MetisLogger *logger);

/**
 * Sets the minimum log level for a facility
 *
 * The default log level is ERROR.  For a message to be logged, it must be of equal
 * or higher log level.
 *
 * @param [in] logger An allocated logger
 * @param [in] facility The facility to set the log level for
 * @param [in] The minimum level to log
 *
 * @retval <#value#> <#explanation#>
 *
 * Example:
 * @code
 * {
 *    PARCLogReporter *reporter = parcLogReporterTextStdout_Create();
 *    MetisLogger *logger = metisLogger_Create(reporter, parcClock_Wallclock());
 *    parcLogReporter_Release(&reporter);
 *    metisLogger_SetLogLevel(logger, MetisLoggerFacility_IO, PARCLogLevel_Warning);
 * }
 * @endcode
 */
void metisLogger_SetLogLevel(MetisLogger *logger, MetisLoggerFacility facility, PARCLogLevel minimumLevel);

/**
 * Tests if the log level would be logged
 *
 * If the facility would log the given level, returns true.  May be used as a
 * guard around expensive logging functions.
 *
 * @param [in] logger An allocated logger
 * @param [in] facility The facility to test
 * @param [in] The level to test
 *
 * @retval true The given facility would log the given level
 * @retval false A message of the given level would not be logged
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
bool metisLogger_IsLoggable(const MetisLogger *logger, MetisLoggerFacility facility, PARCLogLevel level);

/**
 * Log a message
 *
 * The message will only be logged if it is loggable (metisLogger_IsLoggable returns true).
 *
 * @param [in] logger An allocated MetisLogger
 * @param [in] facility The facility to log under
 * @param [in] level The log level of the message
 * @param [in] module The specific module logging the message
 * @param [in] format The message with varargs
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisLogger_Log(MetisLogger *logger, MetisLoggerFacility facility, PARCLogLevel level, const char *module, const char *format, ...);

/**
 * Switch the logger to a new reporter
 *
 * Will close the old reporter and re-setup the internal loggers to use the new reporter.
 * All current log level settings are preserved.
 *
 * @param [in] logger An allocated MetisLogger
 * @param [in] reporter An allocated PARCLogReporter
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisLogger_SetReporter(MetisLogger *logger, PARCLogReporter *reporter);

/**
 * Set a new clock to use with the logger
 *
 * The logger will start getting the time (logged as the messageid) from the specified clock
 *
 * @param [in] logger An allocated MetisLogger
 * @param [in] clock An allocated PARCClock
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
void metisLogger_SetClock(MetisLogger *logger, PARCClock *clock);
#endif // Metis_metis_Logger_h
