# tapecart - a tape port storage pod for the C64
#
# Copyright (C) 2013-2022  Ingo Korb <ingo@akana.de>
# All rights reserved.
# Idea by enthusi
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
# THE POSSIBILITY OF SUCH DAMAGE.
#
#
# Makefile: program construction rules
#

VERSION := 1.2a

CFLAGS  := -Oi -DVERSION=\"$(VERSION)\" -Ieload/src
ASFLAGS :=
SYSTEM  := c64

# Enable verbose compilation with "make V=1"
ifdef V
 Q :=
 E := @:
else
 Q := @
 E := @echo
endif

FLASHTOOL_CSRC := flashtool.c tapecartif.c conutil.c debugmenu.c advancedmenu.c tcrt.c io.c drivemenu.c utils.c
FLASHTOOL_ASRC := minidelay.s getbyte_fast.s blockwrite.s todinit.s

# calculate object file names
FLASHTOOL_OBJ  := $(FLASHTOOL_CSRC:.c=.o65) $(FLASHTOOL_ASRC:.s=.o65)

all: loader.bin flashtool.prg
	$(Q)ls -l loader.bin

flashtool.prg: $(FLASHTOOL_OBJ) eload/eload.lib flashtool.cfg
	$(E) "  LINK   $@"
	$(Q)ld65 -m flashtool.map -o $@ -C flashtool.cfg $(FLASHTOOL_OBJ) eload/eload.lib $(SYSTEM).lib

%.s: %.c
	$(E) "  CC     $<"
	$(Q)cc65 -t $(SYSTEM) $(CFLAGS) -o $@ $<

%.o65: %.s
	$(E) "  AS     $<"
	$(Q)ca65 -t $(SYSTEM) $(ASFLAGS) -o $@ $<

loader.h: loader.bin
	$(E) "  BIN2H  $<"
	$(Q)./bin2h.pl $< $@

loader.bin: loader.asm
	$(E) "  XA     $<"
	$(Q)xa -M -o $@ $<

eload/eload.lib:
	$(E) "  MAKE eload.lib"
	$(Q)$(MAKE) -C eload eload.lib

clean:
	$(E) "  CLEAN"
	$(Q)rm -f loader.bin
	$(Q)rm -f $(FLASHTOOL_OBJ)
	$(Q)rm -f flashtool.prg
	$(Q)$(MAKE) -C eload clean

# incomplete set of manual dependencies
flashtool.o65: loader.h tapecartif.h conutil.h
