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
 * @file metis_TlvSchemaV0.h
 * @brief Define the TLV Operations for the V0 schema
 *
 * Defines the operations for parsing a V0 schema name
 *
 * THIS IS A DEPRECATED CLASS. V0 IS NO LONGER IN USE.
 * See BugzId: 3921.
 *
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#ifndef Metis_metis_TlvSchemaV0
#define Metis_metis_TlvSchemaV0

#include <ccnx/forwarder/metis/tlv/metis_TlvOps.h>

/**
 * Defines the TLV Operations for the V0 Schema
 *
 * Example:
 * @code
 * {
 *    uint8_t *packet = // read a packet from the network
 *    MetisTlvSkeleton skeleton;
 *    bool success = MetisTlvSchemaV0_Ops.parseSkeleton(packet, &skeleton);
 *    if (success) {
 *       if (MetisTlvSchemaV0_Ops.isPacketTypeInterest(packet)) {
 *       // parse interest
 *    }
 * }
 * @endcode
 */
extern const MetisTlvOps MetisTlvSchemaV0_Ops;

#endif // Metis_metis_TlvSchemaV0

