/*
 * Copyright (c) 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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
 * @file metis_HopByHopFragmenterer.h
 * @brief Implements a fragmenter for a hop-by-hop protocol
 *
 * The hop-by-hop fragmenter can be though of as a bi-directional queue.
 *
 * The receive process takes fragmented packets from the wire and reassembles them.  Once a packet is
 * reassembed, it is put on end of the receive queue.  The user can then pop a message of the top of
 * the receive queue and process it normally.
 *
 * The send process takes a potentially large input packet and fragments it in to pieces that are places
 * in order on the send queue.  The caller can then pop messages of the head of the send queue and
 * put them on the wire.
 *
 * In the next update, we probalby want to make the send queue and receive queue external so I/O and
 * message processing threads can access them directly.
 *
 * @author Marc Mosko, Palo Alto Research Center (Xerox PARC)
 * @copyright 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
 */

#ifndef __Metis__metis_HopByHopFragmenter__
#define __Metis__metis_HopByHopFragmenter__

#include <stdbool.h>
#include <ccnx/forwarder/metis/core/metis_Logger.h>
#include <ccnx/forwarder/metis/core/metis_Message.h>

struct metis_hopbyhop_fragment;
typedef struct metis_hopbyhop_fragment MetisHopByHopFragmenter;

/**
 * Creates a fragmentation class for a specific session
 *
 * The fragmenter is specific to a given flow (i.e. source-destination-ethertype tuple).
 * It is the responsibility of the caller to create the appropriate number of fragmenters
 * and classify packets in to the right fragmenter.
 *
 * @param [in] logger The logger to use for output
 * @param [in] mtu The MTU to use for send operations
 *
 * @return non-null An allocted MetisHopByHopFragmenter
 * @return null An error
 *
 * Example:
 * @code
 * {
 *    MetisHopByHopFragmenter *fragmenter = metisHopByHopFragmenter_Create(logger, mtu);
 *    metisHopByHopFragmenter_Release(&fragmenter);
 * }
 * @endcode
 */
MetisHopByHopFragmenter *metisHopByHopFragmenter_Create(MetisLogger *logger, unsigned mtu);

/**
 * Release a reference to the fragmenter
 *
 * Will destroy any packets in the receive and send queues.
 *
 * @param [in,out] fragmenterPtr A pointer to an allocated MetisHopByHopFragmenter
 *
 * Example:
 * @code
 * {
 *    MetisHopByHopFragmenter *fragmenter = metisHopByHopFragmenter_Create(logger, mtu);
 *    metisHopByHopFragmenter_Release(&fragmenter);
 * }
 * @endcode
 */
void metisHopByHopFragmenter_Release(MetisHopByHopFragmenter **fragmenterPtr);

/**
 * Receives a message as part of the fragmentation session
 *
 * Receives a fragment.  If this causes a reassembly to complete, the completed packet
 * will be placed in the receive queue and may be accessed by metisHopByHopFragmenter_PopReceiveQueue().
 * The caller is reponsible for releasing message.
 *
 * If a non-fragment packet is received, it is placed directly on the receive queue.
 *
 * The caller is responsible for releasing the message.
 *
 * @param [in] fragmenter An allocated MetisHopByHopFragmenter
 * @param [in] message An allocated MetisMessage
 *
 * @return true The receive buffer has an assembled packet ready for read
 * @return false the receive buffer does not have a complete packet ready.
 *
 * Example:
 * @code
 * {
 *   void
 *   acceptFragment(MetisHopByHopFragmenter *fragmenter, MetisMessage *message)
 *   {
 *      bool receiveQueueNotEmpty = metisHopByHopFragmenter_Receive(fragmenter, message);
 *      if (receiveQueueNotEmpty) {
 *         MetisMessage *assembled = NULL;
 *         while ((assembled = metisHopByHopFragmenter_PopReceiveQueue(fragmenter)) != NULL) {
 *            etherListener->stats.framesReassembled++;
 *            metisForwarder_Receive(etherListener->metis, assembled);
 *         }
 *      }
 *   }
 * }
 * @endcode
 */
bool metisHopByHopFragmenter_Receive(MetisHopByHopFragmenter *fragmenter, const MetisMessage *message);

/**
 * Pops the top assembed message from the receive queue
 *
 * Reads the top reassembled packet from the receive queue.  The caller must
 * release the returned message.
 *
 * @param [in] fragmenter An allocated MetisHopByHopFragmenter
 *
 * @return NULL The receive queue is empty (i.e. the current reassembly is not complete)
 * @return non-null A re-assembed message
 *
 * Example:
 * @code
 * {
 *   void
 *   acceptFragment(MetisHopByHopFragmenter *fragmenter, MetisMessage *message)
 *   {
 *      bool receiveQueueNotEmpty = metisHopByHopFragmenter_Receive(fragmenter, message);
 *      if (receiveQueueNotEmpty) {
 *         MetisMessage *assembled = NULL;
 *         while ((assembled = metisHopByHopFragmenter_PopReceiveQueue(fragmenter)) != NULL) {
 *            etherListener->stats.framesReassembled++;
 *            metisForwarder_Receive(etherListener->metis, assembled);
 *         }
 *      }
 *   }
 * }
 * @endcode
 */
MetisMessage *metisHopByHopFragmenter_PopReceiveQueue(MetisHopByHopFragmenter *fragmenter);

/**
 * Adds a message to the send buffer
 *
 * This may make multiple references to the original message where each fragment is
 * pointing as an extent in to the original message.
 *
 * @param [in] fragmenter An allocated MetisHopByHopFragmenter
 * @param [in] message An allocated MetisMessage
 *
 * @return true The message was fragmented and put on the send queue
 * @return false An error
 *
 * Example:
 * @code
 * {
 *   void sendFragments(MetisHopByHopFragmenter *fragmenter, const MetisMessage *message) {
 *      bool success = metisHopByHopFragmenter_Send(fragmenter, message);
 *
 *      MetisMessage *fragment;
 *      while (success && (fragment = metisHopByHopFragmenter_PopSendQueue(fragmenter)) != NULL) {
 *         success = _sendFrame(fragment);
 *         metisMessage_Release(&fragment);
 *      }
 *
 *      // if we failed, drain the other fragments
 *      if (!success) {
 *         while ((fragment = metisHopByHopFragmenter_PopSendQueue(fragmenter)) != NULL) {
 *         metisMessage_Release(&fragment);
 *      }
 *   }
 *   // caller must release message
 * }
 * @endcode
 */
bool metisHopByHopFragmenter_Send(MetisHopByHopFragmenter *fragmenter, MetisMessage *message);

/**
 * Pops the next message to send to the wire from the send queue
 *
 * Returns the front of the Send FIFO queue of fragments that should be
 * sent on the wire.
 *
 * @param [in] fragmenter An allocated MetisHopByHopFragmenter
 *
 * @return null there is no message awaiting transmit
 * @return non-null A message to send
 *
 * Example:
 * @code
 * {
 *   void sendIdleFragment(MetisHopByHopFragmenter *fragmenter) {
 *      bool success = metisHopByHopFragmenter_SendIdle(fragmenter);
 *
 *      MetisMessage *fragment;
 *      while (success && (fragment = metisHopByHopFragmenter_PopSendQueue(fragmenter)) != NULL) {
 *         success = _sendFrame(fragment);
 *         metisMessage_Release(&fragment);
 *      }
 *   }
 * }
 * @endcode
 */
MetisMessage *metisHopByHopFragmenter_PopSendQueue(MetisHopByHopFragmenter *fragmenter);
#endif /* defined(__Metis__metis_HopByHopFragmenter__) */
