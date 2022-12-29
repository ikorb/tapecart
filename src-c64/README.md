# tapecart flash tool sources #

tapecart - a tape port storage pod for the C64

Copyright (C) 2013-2017 by Ingo Korb <ingo@akana.de>  
All rights reserved.  
Idea by enthusi  

ELoad (c) 2011 Thomas Giesel

# About #

This directory contains the sources for the C64-based flashing tool
for the Tapecart as well as the code for the default loader.

# Compiling #

The flash tool is mostly written in C and can be compiled with
cc65. It has been successfully compiled with "cc65 V2.15 - Git
202ad62", no other versions have been tested.

The initial fastloader is written in assembly and needs xa to build.

Furthermore, you need Perl versoin 5.10 or later to run a utility
script during the build process and the Makefile assumes GNU make.

# Using #

Please see the [doc](../doc) directory one level up.

# License #

The software is distributed under the following conditions:

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


ELoad (in the eload subdirectory) is distributed under the following conditions:

    (c) 2011 Thomas Giesel
    transfer code based on work by Per Olofsson

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.

    Thomas Giesel skoe@directbox.com
