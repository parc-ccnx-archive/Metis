/*
 * Copyright (c) 2013-2014, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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
 * @file metis_MissiveType
 * @brief Defines what a Missive represents
 *
 * Currently, missives only carry information about the state of a connection
 * (created, up, down, closed, destroyed).
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2014, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#ifndef Metis_metis_MissiveType_h
#define Metis_metis_MissiveType_h

/**
 * @typedef Represents the state of a connection
 * @abstract CREATE is the initial state.  UP & DOWN are recurrent states.  CLOSED is transient.  DESTROYED is the terminal state.
 * @constant MetisMissiveType_ConnectionCreate    Connection created (new)
 * @constant MetisMissiveType_ConnectionUp        Connection is active and passing data
 * @constant MetisMissiveType_ConnectionDown      Connection is inactive and cannot pass data
 * @constant MetisMissiveType_ConnectionClosed    Connection closed and will be destroyed
 * @constant MetisMissiveType_ConnectionDestroyed Connection destroyed
 * @discussion State transitions:
 *                initial   -> CREATE
 *                CREATE    -> (UP | DOWN)
 *                UP        -> (DOWN | DESTROYED)
 *                DOWN      -> (UP | CLOSED | DESTROYED)
 *                CLOSED    -> DESTROYED
 *                DESTROYED -> terminal
 */
typedef enum {
    MetisMissiveType_ConnectionCreate,
    MetisMissiveType_ConnectionUp,
    MetisMissiveType_ConnectionDown,
    MetisMissiveType_ConnectionClosed,
    MetisMissiveType_ConnectionDestroyed
} MetisMissiveType;
#endif // Metis_metis_MissiveType_h
