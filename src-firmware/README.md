# tapecart firmware sources #

tapecart - a tape port storage pod for the C64

Copyright (C) 2013-2016 by Ingo Korb <ingo@akana.de>  
All rights reserved.  
Idea by enthusi  

# About #

This directory contains the firmware sources for the Tapecart. They
can be built for various hardware, currently there are configurations
for early and late prototypes as well as the first release carts.

# Compiling #

To compile an AVR version of the firmware (used in prototypes), a
standard AVR gcc toolchain and avr-libc supporting at least the C11
standard is required.

To compile a SAMD09 version of the firmware (used in the first release
carts), an embedded ARM GCC toolchain is required. The [GNU ARM
Embedded
Toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm)
should work fine.

The selection of the target configuration is made on the `make`
command line using the `CONFIG` variable, e.g. `make
CONFIG=configs/config-earlyproto` to compile a version of the firmware
suitable for early prototype boards.

# Using #

Please see the [doc](../doc) directory one level up.

# License #

The firmware itself is distributed under the following conditions:

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.


Please note that the source code includes third-party header files
taken from the Atmel Software Framework in the
[samd09/include](samd09/include) subdirectory, which come with their
own license conditions. Please check the files themselves for details.

