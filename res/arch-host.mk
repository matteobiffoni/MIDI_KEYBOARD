CC := gcc
CFLAGS := -std=gnu99 -Wall -I$(INCDIR)/host -I$(INCDIR)\
		-funsigned-bitfields -fshort-enums -Wno-missing-braces
TARGET := target/host/midi_keyboard
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -lopenal -lm -lpthread