/*
 * Copyright (c) 2013, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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
//  metis_Missive.c
//  Libccnx
//
//  Created by Mosko, Marc <Marc.Mosko@parc.com> on 11/21/13.

#include <config.h>
#include <stdio.h>
#include <LongBow/runtime.h>
#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_Memory.h>

#include <ccnx/forwarder/metis/messenger/metis_Missive.h>

struct metis_missive {
    MetisMissiveType missiveType;
    unsigned connectionid;
};

parcObject_Override(MetisMissive, PARCObject,
                    .isLockable = false);

MetisMissive *
metisMissive_Create(MetisMissiveType missiveType, unsigned connectionid)
{
    MetisMissive *missive = parcObject_CreateInstance(MetisMissive);
    missive->missiveType = missiveType;
    missive->connectionid = connectionid;
    return missive;
}

MetisMissive *
metisMissive_Acquire(const MetisMissive *missive)
{
    return parcObject_Acquire(missive);
}

void
metisMissive_Release(MetisMissive **missivePtr)
{
    parcObject_Release((void **) missivePtr);
}

MetisMissiveType
metisMissive_GetType(const MetisMissive *missive)
{
    assertNotNull(missive, "Parameter missive must be non-null");
    return missive->missiveType;
}

unsigned
metisMissive_GetConnectionId(const MetisMissive *missive)
{
    assertNotNull(missive, "Parameter missive must be non-null");
    return missive->connectionid;
}
