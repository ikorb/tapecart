OUTPUT_FORMAT(elf32-littlearm)
ENTRY(_start)

MEMORY {
  rom    (rx)  : ORIGIN = 0x00000000, LENGTH = 7680
  eeprom (rw)  : ORIGIN = 0x00001e00, LENGTH = 512
  cram   (rwx) : ORIGIN = 0x20000000, LENGTH = 256
  ram    (rwx) : ORIGIN = 0x20000100, LENGTH = 3584
}

SECTIONS {
  .text : {
    KEEP(*(.vectors))
    KEEP(*(.init))
    *(.text)
    *(.text.*)
    *(.gnu.linkonce.t.*)

    __text_end = .;
  } > rom

  .rodata : ALIGN(4) {
    __rodata_start = .;
    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r.*)
    __rodata_end = .;
  } > rom

  .data : AT (ALIGN(__rodata_end,4)) {
    __data_start = .;
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    __data_end = .;
  } > ram

  __data_load_start = LOADADDR(.data);
  __data_load_end   = __data_load_start + SIZEOF(.data);

  .bss (NOLOAD) : ALIGN(4) {
    __bss_start__ = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    __bss_end__ = ALIGN(4);
  } > ram

  .ramexec (NOLOAD) : ALIGN(4) {
    __ramexec_start__ = .;
    *(.ramexec)
    *(.ramexec.*)
  } > cram

  PROVIDE(__stack = ORIGIN(ram) + LENGTH(ram));

  .eeprom (NOLOAD) :
  {
    *(.eeprom)
    *(.eeprom.*)
  } > eeprom
}
