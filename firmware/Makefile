MCU:= atmega324pb
PROGRAMMER:= usbtiny
PROGRAMMER_PORT:= usb

CC:=avr-gcc
OBJCOPY:=avr-objcopy
AVRDUDE:=avrdude
CFLAGS:= -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wextra -Wall -Wstrict-prototypes -std=gnu99
LIBS:=


#Fuse Settings
LFUSE:=0xE2
HFUSE:=0xD9
EFUSE:=0xF7

# Source filename, without extension
FILE:=robot_firmware


.PHONY: clean fuses download

# By default, compile and download.
all: $(FILE).bin download

# Burn fuses
fuses:
	@$(AVRDUDE) -p $(MCU) -c $(PROGRAMMER) -P $(PROGRAMMER_PORT) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m

# Compile & link
$(FILE).bin: $(FILE).c
	@python3 ../protocol/gen_files.py
	@$(CC) -mmcu=$(MCU) $(CFLAGS) -I. uart.c spi.c motor.c protocol.c $(FILE).c -o $(FILE).elf $(LIBS)
	@$(OBJCOPY) -O binary -R .eeprom $(FILE).elf $(FILE).bin

# Download firmware, without changing fuses or eeprom.
download:
	@$(AVRDUDE) -q -p $(MCU) -c $(PROGRAMMER) -P $(PROGRAMMER_PORT) -U flash:w:$(FILE).bin:r

#Delete generated files
clean:
	@rm -f *.hex *.elf *.bin