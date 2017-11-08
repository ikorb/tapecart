# tapecart hardware #

This directory contains KiCAD schematics and layout for three
different implementations of the tapecart:

* DIY

    This version of the tapecart is optimized for DIY builders - it
    does not require any SMD components. Since DIP versions of the
    flash chips are a bit hard to source, it has an alternate
    footprint for using an SMD flash chip instead. The firmware for
    this version must be compiled using the config file "config-diy".
    A precompiled hex file is located in the subdirectory "firmware".

* SMD-Onesided-R1 and SMD-Twosided-R1

    These two versions of the tapecart are optimized for cost, using
    SMD components. Both are electrically the same, but one of them
    saves a bit more board space by placing components on both sides
    of the board. The firmware for both of these versions must be
    compiled using the config file "config-release1". A precompiled
    binary is located in the subdirectory "firmware".

