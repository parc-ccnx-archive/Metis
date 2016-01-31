#
# Copyright (c) 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Patent rights are not granted under this agreement. Patent rights are
#       available under FRAND terms.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# @author Marc Mosko, Palo Alto Research Center (PARC)
# @copyright 2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC).  All rights reserved.
#
#
# This makefile is not executed as part of the normal build process.  A user must cd
# in to this directory and make or clean the documentation from here
#

include ../../config.mk

.PHONY: manpages

SUBDIRS =

OUTDIR= ../manpage

MAN = $(OUTDIR)/metis_daemon.1 $(OUTDIR)/metis_control.1 $(OUTDIR)/metis.cfg.5
TXT = $(OUTDIR)/metis_daemon.1.txt $(OUTDIR)/metis_control.1.txt $(OUTDIR)/metis.cfg.5.txt
HTML = $(OUTDIR)/metis_daemon.1.html $(OUTDIR)/metis_control.1.html $(OUTDIR)/metis.cfg.5.html
PDF = $(OUTDIR)/metis_daemon.1.pdf $(OUTDIR)/metis_control.1.pdf $(OUTDIR)/metis.cfg.5.pdf

all:
	@echo "usage: make manpages"

manpages: $(TXT) $(MAN) $(HTML) $(PDF)

$(OUTDIR):
	mkdir -p $(OUTDIR)

######
$(OUTDIR)/metis_daemon.1: metis_daemon.sgml $(OUTDIR)
	docbook-to-man $< > $@

$(OUTDIR)/metis_daemon.1.txt: $(OUTDIR)/metis_daemon.1 $(OUTDIR)
	groff -man -Tutf8 $< > $@

$(OUTDIR)/metis_daemon.1.html: $(OUTDIR)/metis_daemon.1 $(OUTDIR)
	groff -man -Thtml $< > $@

$(OUTDIR)/metis_daemon.1.pdf: $(OUTDIR)/metis_daemon.1 $(OUTDIR)
	groff -man -Tps $< | ps2pdf - $@

######
$(OUTDIR)/metis_control.1: metis_control.sgml $(OUTDIR)
	docbook-to-man $< > $@

$(OUTDIR)/metis_control.1.txt: $(OUTDIR)/metis_control.1 $(OUTDIR)
	groff -man -Tascii $< > $@

$(OUTDIR)/metis_control.1.html: $(OUTDIR)/metis_control.1 $(OUTDIR)
	groff -man -Thtml $< > $@

$(OUTDIR)/metis_control.1.pdf: $(OUTDIR)/metis_control.1 $(OUTDIR)
	groff -man -Tps $< | ps2pdf - $@

######
$(OUTDIR)/metis.cfg.5: metis.cfg.sgml $(OUTDIR)
	docbook-to-man $< > $@

$(OUTDIR)/metis.cfg.5.txt: $(OUTDIR)/metis.cfg.5 $(OUTDIR)
	groff -man -Tascii $< > $@

$(OUTDIR)/metis.cfg.5.html: $(OUTDIR)/metis.cfg.5 $(OUTDIR)
	groff -man -Thtml $< > $@

$(OUTDIR)/metis.cfg.5.pdf: $(OUTDIR)/metis.cfg.5 $(OUTDIR)
	groff -man -Tps $< | ps2pdf - $@


######
#clobber: clean
#	${RM} -rf $(OUTDIR)
#
#CLEANFILES=$(TXT) $(MAN) $(HTML)

