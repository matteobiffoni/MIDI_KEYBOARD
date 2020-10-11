CC := avr-gcc
AS := avr-gcc

CFLAGS := -Wall -std=gnu99 -DF_CPU=16000000UL -O3 -funsigned-char\
		-funsigned-bitfields -fshort-enums -Wstrict-prototypes\
		-mmcu=atmega2560 -I$(INCDIR)/avr -I$(INCDIR) -DAVR -D__AVR_3_BYTE_PC__
ASFLAGS := -x assembler-with-cpp $(CFLAGS)

AVRDUDE := avrdude
AVRDUDE_PROGRAMMER := wiring
AVRDUDE_PORT := /dev/ttyACM0
AVRDUDE_CONFIG != find /usr/share/arduino/hardware -name avrdude.conf
AVRDUDE_NO_VERIFY := -V

AVRDUDE_FLAGS := -p m2560 -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) -b\
				115200 $(AVRDUDE_NO_VERIFY) $(AVRDUDE_VERBOSE) $(AVRDUDE_ERASE_COUNTER)\
				-D -q -C $(AVRDUDE_CONFIG)

avrdude_write_flash = -U flash:w:$(strip $(1)):i
avrdude_write_eeprom = -U eeprom:w:$(strip $(1)):i

%.eep:	%.elf
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load"\
	  --change-section-lma .eeprom=0 --no-change-warnings\
	  -O ihex $< $@ || exit 1

%.hex:	%.eep %.elf
	avr-objcopy -O ihex -R .eeprom $(patsubst %.hex, %.elf, $@) $@
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(call avrdude_write_flash, $@)\
	  $(call avrdude_write_eeprom, $(patsubst %.hex, %.eep, $@))

TARGET := target/avr/midi_keyboard.elf
$(TARGET):	$(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^