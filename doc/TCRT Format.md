# Introduction #

The tapecart image file format is intended to contain
all relevant data of a tapecart,
holding both the flash data as well as the settings and loader stored
in the microcontroller.
The recommended file extension is *.tcrt*,
but the format also includes a fixed header to allow detection
independent of the file name.
It can for example be used by an emulator to persist the state of an
emulated tapecart between sessions.

The file format as specified here assumes that a standard tapecart has
2 megabytes (2^21 bytes) of flash memory, organized in 512 byte pages
and 4096 byte erase blocks. In the unlikely event that a future
tapecart deviates from this, a newer version of the tapecart image
file format will be specified.

# History #

This document describes version 1 of the tapecart image file format.

2017-05-12: Changed offset 40 (loader flag) to a bit field (misc. flags)

# Format #

All data fields in a tapecart image are stored in little endian.
The data fiels in the file, in the order as they appear, are as
follows:

- Offset 0: 16 byte file signature  
    A tapecart image starts with the fixed 16 byte signature
    ```tapecartImage\015\012\032``` (in ASCII) or equivalently as hex
    numbers:  
    ```
    74 61 70 65  63 61 72 74  49 6d 61 67  65 0d 0a 1a
    ```
- Offset 16: 2 byte version number  
    This field specifies the version number of the tapecart image
    file. As this is version 1 of the specification, this field must
    have the value 1. If a higher value is found in this field, the
    meaning of the data following the version number may be completely
    different from this specification.

The following four fields correspond to the data accessed using the
READ_LOADINFO/WRITE_LOADINFO commands:

- Offset 18: 2 byte data address  
    This field specifies the offset of the data block sent in fastload
    mode.
- Offset 20: 2 byte data length  
    This field specifies the length of the data block sent in fastload
    mode.
- Offset 22: 2 byte call address  
    This field specifies the address that is jumped to after loading
    the data block in fastload mode.
- Offset 24: 16 bytes file name  
    This field specifies the file name that will be shown on the C64
    when the user loads the initial loader.

Although it is not recommended to change the initial loader, the
tapecart image file format includes it for completeness. Its presence
in the file is indicated by a flag in the next field:

- Offset 40: 1 byte misc. flags
    This field is a bit field which currently contains two flags using
    bits 0 and 1. The unused bits in this field must be set to 0.

    **Bit 0** of this flag indicates if the following 171 bytes
    contain a valid initial loader. If the bit is 0, the default
    initial loader is assumed, but not included in the TCRT file.
    Even if this flag is 0, the following 171 bytes must be included
    in the file and must be filled with 0x00. If this bit is 1, the
    following 171 bytes must contain a valid initial loader.

    The intent of this flag is to simplify the creation of TCRT files
    in the build process of software that is meant to be used with the
    default loader - they can just set the loader flag to 0 and do not
    need to worry about including the default loader with their
    sources just to build a valid TCRT file.

    When a non-default initial loader is included in the TCRT file,
    Bit 1 (see below) must always be set to 0.

    **Bit 1** of this flag indicates if the program stored in this
    TCRT file supports data block offsets. A TCRT-handling
    application can safely ignore the state of this bit if it does not
    intend to provide special support for programs that can support
    data block offsets.

    For more details about data block offset support, see the section
    *Data Block Offset option* in the [Programmer's Reference](ProgRef.md).

    A program that claims data block offset support by setting bit 1
    may not use a custom initial loader, so bit 0 of the flag field
    must be 0 if bit 1 is 1.

- Offset 41: 171 bytes loader code  
    This is the initial loader which is used for fastload mode and can
    also be read or written using the READ_LOADER/WRITE_LOADER
    commands.

    If the loader flag preceding this block of data is 0, this block
    should be filled with 0x00 instead and the software that uses the
    TCRT file should supply its own copy of the default loader code.

Finally, the contents of the data memory are stored in the file. Since
some applications might not use the full 2 MB of the tapecart, it is
possible to create a smaller TCRT file by omitting empty space at the
end. Any part of the data memory not covered by the data block in the
TCRT file is assumed to be erased, thus containing nothing but
0xff. The data block in the TCRT file always starts at address 0 of
the tapecart's data memory.

- Offset 212: 4 bytes flash content length  
    This field specifies the length of the following flash content
    block. It can be any value from zero, specifying that the
    tapecart's entire flash memory is erased, to 2097152 (0x200000)
    for the full flash memory. A TCRT file with a larger value in this
    field is invalid.

    Trimming empty space at the end of the flash memory content is not
    required - a file specifying the full 2 MB and flash memory content
    filled with just 0xff is just as valid as one specifying a length
    of 0.
- Offset 216: n bytes flash memory content  
    Starting at offset 216, the tape cart image file holds the
    contents of the tapecart's flash memory.
