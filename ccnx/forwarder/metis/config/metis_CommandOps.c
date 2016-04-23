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
 * @author Glenn Scott, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */
#include <config.h>

#include <stdbool.h>
#include <stdint.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>

#include <LongBow/runtime.h>
#include <string.h>

#ifndef _ANDROID_
#ifdef HAVE_ERRNO_H
#include <errno.h>
#else
extern int errno;
#endif
#endif

#include <parc/algol/parc_Memory.h>

#include <ccnx/forwarder/metis/config/metis_CommandOps.h>
#include <ccnx/forwarder/metis/config/metis_CommandParser.h>

MetisCommandOps *
metisCommandOps_Create(void *closure, const char *command, void (*init)(MetisCommandParser *parser, MetisCommandOps *ops),
                       MetisCommandReturn (*execute)(MetisCommandParser *parser, MetisCommandOps *ops, PARCList *args),
                       void (*destroyer)(MetisCommandOps **opsPtr))
{
    assertNotNull(command, "Parameter command must be non-null");
    assertNotNull(execute, "Parameter execute must be non-null");
    MetisCommandOps *ops = parcMemory_AllocateAndClear(sizeof(MetisCommandOps));
    assertNotNull(ops, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(MetisCommandOps));
    ops->closure = closure;
    ops->command = parcMemory_StringDuplicate(command, strlen(command) + 1);
    ops->init = init;
    ops->execute = execute;
    ops->destroyer = destroyer;
    return ops;
}

void
metisCommandOps_Destroy(MetisCommandOps **opsPtr)
{
    assertNotNull(opsPtr, "Parameter opsPtr must be non-null");
    assertNotNull(*opsPtr, "Parameter opsPtr must dereference to non-null pointer");

    MetisCommandOps *ops = *opsPtr;
    parcMemory_Deallocate((void **) &(ops->command));
    // DO NOT call ops->destroyer, we are one!
    parcMemory_Deallocate((void **) &ops);

    *opsPtr = NULL;
}
