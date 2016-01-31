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
//  strategy_CNF.h
//  Metis
//
//  Created by Mosko, Marc <Marc.Mosko@parc.com> on 12/1/13.

/**
 * Conjunctive Normal Form forwarding.  Nexthops are a conjunctive set of
 * disjunctive sets of interface IDs.  That is, at the first conjunctive
 * level, we have:
 *   A_1 and A_2 and A_3 and ... and A_m
 *
 * Each set A_i is made up of a disjunctive set:
 *   B_(i,1) or B_(i,2) or ... or B_(i,n)
 *
 * An interest is forwarded to every conjunctive set A_i.  If a set A_i has
 * more than one B_(i,j), we pick a B_(i,j) using a weighted round robin, but only
 * one B_(i,j) is used.
 *
 * The final set of egress interfaces is the union of B_(i,j) over all i.
 */

#ifndef Metis_strategy_CNF_h
#define Metis_strategy_CNF_h

#include <ccnx/forwarder/metis/strategy/metis_Strategy.h>
#endif // Metis_strategy_CNF_h
