Copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL XEROX OR PARC BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

################################################################################
#
# PATENT NOTICE
#
# This software is distributed under the BSD 2-clause License (see LICENSE
# file).  This BSD License does not make any patent claims and as such, does
# not act as a patent grant.  The purpose of this section is for each contributor
# to define their intentions with respect to intellectual property.
#
# Each contributor to this source code is encouraged to state their patent
# claims and licensing mechanisms for any contributions made. At the end of
# this section contributors may each make their own statements.  Contributor's
# claims and grants only apply to the pieces (source code, programs, text,
# media, etc) that they have contributed directly to this software.
#
# There is no guarantee that this section is complete, up to date or accurate. It
# is up to the contributors to maintain their portion of this section and up to
# the user of the software to verify any claims herein.
#
# Do not remove this header notification.  The contents of this section must be
# present in all distributions of the software.  You may only modify your own
# intellectual property statements.  Please provide contact information.

- Palo Alto Research Center, Inc
This software distribution does not grant any rights to patents owned by Palo
Alto Research Center, Inc (PARC). Rights to these patents are available via
various mechanisms. As of January 2016 PARC has committed to FRAND licensing any
intellectual property used by its contributions to this software. You may
contact PARC at cipo@parc.com for more information or visit http://www.ccnx.org

@author Marc Mosko, Palo Alto Research Center (PARC)
@copyright (c) 2013-2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.

=================
Metis (mythology)
Metis was the the Titaness of wisdom and deep thought. The Greek word
"metis" meant a quality that combined wisdom and cunning.
=================

Metis is the CCNx 1.0 forwarder.  It uses a modular design so adding new
encapsulations, forwarding behavior, or event dispatching is
well-defined and has minimum (or no) side effects.

=================
Building

"make" will create "libmetis.a" and "metis".  The library is used by other
parts of Libccnx, such as Transport testing and the mobile builds.  The
executable "metis" is for command-line invocation of a forwarding daemon.

Metis uses a few things from parc/algol and ccnx/common.

Metis uses the LongBow XUnit test framework. "make check" will run the tests.

Metis is built with Libevent 2.0 for the dispatcher.  Libevent is visible in the
code in the following modules.  To use a different event dispatcher, these modules
would need to be updated: metisDispatcher.c, metisStreamBuffer.c, metisMessage.c,
metis_StreamConnection.c

=================
Code Layout

Each directory has its code and headers along with a test/ subdirectory with
LongBow unit tests.

config/
config/test
- Forwarder configuration, from a file or CLI or web
  The default telnet port is 2001.
  The default web port is 2002.

core/
core/test
- The forwarder (metisForwarder) that bundles the whole thing and the event
  dispatcher (metisDispatcher).  Also includes various utility classes, such as
  MetisMessage that wraps all messages inside the forwarder.

io/
io/test/
- I/O related primatives.  These include "listeners" that monitor for specific types
  of incoming packets.  io/ includes tunnels, ip overlays, and direct L2 encapsulation.

messenger/
messenger/test/
- event messages within the router, such as notificaitons when an interface
  comes up or goes down.

platforms/
- platform specific functions, such as dealing with interfaces and
  direct Ethernet encapsulation.  The files in here implement function
  prototypes (usually in core/ or io/), and are tested with unit tests of those
  prototypes.  So, there is no platforms/test/ directory.

processor/
processor/test/
- The message processor.  Forwards Interests and objects.

strategies/
strategies/test/
- Forwarding strategies.  Each FIB entry indicates the strategy that should
  be used to forward an interest among its entries.

libmetis.a
- The output library

test/
- These are system-level tests

