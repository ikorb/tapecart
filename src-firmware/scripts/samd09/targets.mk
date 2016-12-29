# -*- makefile -*- fragment for SAM D09

# Program the device.  
program: $(TARGET).bin
	$(E) "  FLASH  $(TARGET).bin"
	$(Q)openocd -f scripts/samd09/openocd-samd09.cfg -c init -c "flash_bin $(TARGET).bin" -c shutdown
