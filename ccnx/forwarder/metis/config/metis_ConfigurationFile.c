/*
 * Copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include <config.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include <LongBow/longBow_Runtime.h>
#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_ArrayList.h>
#include <parc/algol/parc_List.h>
#include <ccnx/forwarder/metis/config/metis_ConfigurationFile.h>
#include <ccnx/forwarder/metis/config/metis_Configuration.h>
#include <ccnx/forwarder/metis/config/metis_ControlState.h>
#include <ccnx/forwarder/metis/config/metisControl_Root.h>

struct metis_configuration_file {
    MetisForwarder *metis;
    const char *filename;
    FILE *fh;

    size_t linesRead;

    // our custom state machine.
    MetisControlState *controlState;
};


/**
 * Called by the command parser for each command.
 *
 * The command parser will make a CCNxControl message inside the CCNxMetaMessage and send it here.
 * This function must return a ACK or NACK in a CCNxControl in a CCNxMetaMessage.
 *
 * @param [in] userdata A void * to MetisConfigurationFile
 * @param [in] msg The CCNxControl message to process
 *
 * @retval CCNxMetaMessage A CPI ACK or NACK in a CCNxControl in a CCNxMetaMessage
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
static CCNxMetaMessage *
_writeRead(void *userdata, CCNxMetaMessage *msgin)
{
    MetisConfigurationFile *configFile = (MetisConfigurationFile *) userdata;

    CCNxControl *request = ccnxMetaMessage_GetControl(msgin);
    CCNxControl *response = metisConfiguration_ReceiveControl(metisForwarder_GetConfiguration(configFile->metis), request, 0);

    CCNxMetaMessage *msgout = ccnxMetaMessage_CreateFromControl(response);
    ccnxControl_Release(&response);

    return msgout;
}

/**
 * Removes leading whitespace (space + tab).
 *
 * If the string is all whitespace, the return value will point to the terminating '\0'.
 *
 * @param [in] str A null-terminated c-string
 *
 * @retval non-null A pointer in to string of the first non-whitespace
 *
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
static char *
_stripLeadingWhitespace(char *str)
{
    while (isspace(*str)) {
        str++;
    }
    return str;
}

/**
 * Removes trailing whitespace
 *
 * Inserts a NULL after the last non-whitespace character, modiyfing the input string.
 *
 * @param [in] str A null-terminated c-string
 *
 * @return non-null A pointer to the input string
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 */
static char *
_stripTrailingWhitespace(char *str)
{
    char *p = str + strlen(str) - 1;
    while (p > str && isspace(*p)) {
        p--;
    }

    // cap it.  If no whitespace, p+1 == str + strlen(str), so will overwrite the
    // current null.  If all whitespace p+1 == str+1.  For an empty string, p+1 = str.
    *(p + 1) = 0;

    // this does not catch the case where the entire string is whitespace
    if (p == str && isspace(*p)) {
        *p = 0;
    }

    return str;
}

/**
 * Removed leading and trailing whitespace
 *
 * Modifies the input string (may add a NULL at the end).  Will return
 * a pointer to the first non-whitespace character or the terminating NULL.
 *
 * @param [in] str A null-terminated c-string
 *
 * @return non-null A pointer in to the input string
 *
 * Example:
 * @code
 * {
 *     <#example#>
 * }
 * @endcode
 */
static char *
_trim(char *str)
{
    return _stripTrailingWhitespace(_stripLeadingWhitespace(str));
}

/**
 * Parse a string in to a PARCList with one word per element
 *
 * The string passed will be modified by inserting NULLs after each token.
 *
 * @param [in] str A c-string (will be modified)
 *
 * @retval non-null A PARCList where each item is a single word
 *
 * Example:
 * @code
 * <#example#>
 * @endcode
 */
static PARCList *
_parseArgs(char *str)
{
    PARCList *list = parcList(parcArrayList_Create(NULL), PARCArrayListAsPARCList);

    const char delimiters[] = " \t";

    char *token;
    while ((token = strsep(&str, delimiters)) != NULL) {
        parcList_Add(list, token);
    }

    return list;
}

// =============================================================

static void
_destroy(MetisConfigurationFile **configFilePtr)
{
    MetisConfigurationFile *configFile = *configFilePtr;
    parcMemory_Deallocate((void **) &configFile->filename);

    if (configFile->fh != NULL) {
        fclose(configFile->fh);
    }

    metisControlState_Destroy(&configFile->controlState);
}

parcObject_ExtendPARCObject(MetisConfigurationFile, _destroy, NULL, NULL, NULL, NULL, NULL, NULL);

parcObject_ImplementRelease(metisConfigurationFile, MetisConfigurationFile);

MetisConfigurationFile *
metisConfigurationFile_Create(MetisForwarder *metis, const char *filename)
{
    assertNotNull(metis, "Parameter metis must be non-null");
    assertNotNull(filename, "Parameter filename must be non-null");

    MetisConfigurationFile *configFile = parcObject_CreateInstance(MetisConfigurationFile);

    if (configFile) {
        configFile->linesRead = 0;
        configFile->metis = metis;
        configFile->filename = parcMemory_StringDuplicate(filename, strlen(filename));
        assertNotNull(configFile->filename, "Could not copy string '%s'", filename);

        // setup the control state for the command parser
        configFile->controlState = metisControlState_Create(configFile, _writeRead);

        // we do not register Help commands
        metisControlState_RegisterCommand(configFile->controlState, metisControlRoot_Create(configFile->controlState));

        // open the file and make sure we can read it
        configFile->fh = fopen(configFile->filename, "r");

        if (configFile->fh) {
            if (metisLogger_IsLoggable(metisForwarder_GetLogger(metis), MetisLoggerFacility_Config, PARCLogLevel_Debug)) {
                metisLogger_Log(metisForwarder_GetLogger(metis), MetisLoggerFacility_Config, PARCLogLevel_Debug, __func__,
                                "Open config file %s",
                                configFile->filename);
            }
        } else {
            if (metisLogger_IsLoggable(metisForwarder_GetLogger(metis), MetisLoggerFacility_Config, PARCLogLevel_Error)) {
                metisLogger_Log(metisForwarder_GetLogger(metis), MetisLoggerFacility_Config, PARCLogLevel_Error, __func__,
                                "Could not open config file %s: (%d) %s",
                                configFile->filename,
                                errno,
                                strerror(errno));
            }

            // failure cleanup the object -- this nulls it so final return null be NULL
            metisConfigurationFile_Release(&configFile);
        }
    }
    return configFile;
}

bool
metisConfigurationFile_Process(MetisConfigurationFile *configFile)
{
    assertNotNull(configFile, "Parameter configFile must be non-null");

    // default to a "true" return value and only set to false if we encounter an error.
    bool success = true;

    #define BUFFERLEN 2048
    char buffer[BUFFERLEN];

    configFile->linesRead = 0;

    // always clear errors and fseek to start of file in case we get called multiple times.
    clearerr(configFile->fh);
    rewind(configFile->fh);

    while (success && fgets(buffer, BUFFERLEN, configFile->fh) != NULL) {
        configFile->linesRead++;

        char *stripedBuffer = _trim(buffer);
        if (strlen(stripedBuffer) > 0) {
            if (stripedBuffer[0] != '#') {
                // not empty and not a comment

                // _parseArgs will modify the string
                char *copy = parcMemory_StringDuplicate(stripedBuffer, strlen(stripedBuffer));
                PARCList *args = _parseArgs(copy);
                MetisCommandReturn result = metisControlState_DispatchCommand(configFile->controlState, args);

                // we ignore EXIT from the configuration file
                if (result == MetisCommandReturn_Failure) {
                    if (metisLogger_IsLoggable(metisForwarder_GetLogger(configFile->metis), MetisLoggerFacility_Config, PARCLogLevel_Error)) {
                        metisLogger_Log(metisForwarder_GetLogger(configFile->metis), MetisLoggerFacility_Config, PARCLogLevel_Error, __func__,
                                        "Error on input file %s line %d: %s",
                                        configFile->filename,
                                        configFile->linesRead,
                                        stripedBuffer);
                    }
                    success = false;
                }
                parcList_Release(&args);
                parcMemory_Deallocate((void **) &copy);
            }
        }
    }

    if (ferror(configFile->fh)) {
        if (metisLogger_IsLoggable(metisForwarder_GetLogger(configFile->metis), MetisLoggerFacility_Config, PARCLogLevel_Error)) {
            metisLogger_Log(metisForwarder_GetLogger(configFile->metis), MetisLoggerFacility_Config, PARCLogLevel_Error, __func__,
                            "Error on input file %s line %d: (%d) %s",
                            configFile->filename,
                            configFile->linesRead,
                            errno,
                            strerror(errno));
        }
        success = false;
    }

    return success;
}

