# -*- makefile -*- fragment for SAM D09

# Output format. (can be srec, ihex, binary)
FORMAT = binary

# List Assembler source files here.
#     Make them always end in a capital .S.  Files ending in a lowercase .s
#     will not be considered source files but generated files (assembler
#     output from the compiler), and will be deleted upon "make clean"!
#     Even though the DOS/Win* filesystem matches both .s and .S the same,
#     it will preserve the spelling of the filenames, and gcc itself does
#     care about how the name is spelled on its command-line.
ASRC = samd09/startup.S

SRC += samd09/system.c samd09/bitbanging.c samd09/timer.c samd09/eeprom.c samd09/uart.c

ifeq ($(CONFIG_HARDWARE_VARIANT),2)
  SRC += samd09/spi.c
else ifeq ($(CONFIG_HARDWARE_VARIANT),3)
  SRC += samd09/spi.c
endif

# ------- assemble final ARCH_* variables -------
ARCH_TRIPLE  := arm-none-eabi
ARCH_CFLAGS  := -mthumb -mcpu=cortex-m0plus -Isamd09/include -DDONT_USE_CMSIS_INIT -nostartfiles
ARCH_LDFLAGS := -Tscripts/samd09/samd09c.ld

