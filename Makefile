.POSIX:
.SUFFIXES:
.PRECIOUS: $(OBJDIR)/%.o
ARCH ?= avr
SRCDIR := src
INCDIR := include
OBJDIR := obj/$(ARCH)
RESDIR := res
vpath %.c $(SRCDIR)/$(ARCH)
vpath %.c $(SRCDIR)
vpath %.s $(SRCDIR)/$(ARCH)
vpath %.s $(SRCDIR)
vpath %.o $(OBJDIR)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(wildcard $(SRCDIR)/*.c))\
			$(patsubst $(SRCDIR)/$(ARCH)/%.c, $(OBJDIR)/%.o, $(wildcard $(SRCDIR)/$(ARCH)/*.c))
include $(RESDIR)/arch-$(ARCH).mk
$(OBJDIR)/%.o:	%.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(OBJDIR)/%.o:	%.s
	$(AS) $(ASFLAGS) -c -o $@ $<
target: $(TARGET) ;
all:
	@make host
	@make avr
avr:
	@ARCH=avr make target
host:
	@ARCH=host make target
flash:
	@ARCH=avr make target/avr/midi_keyboard.hex
clean:
	rm -f $(OBJDIR)/../**/*.o $(OBJDIR)/../../target/**/*
.PHONY: clean all avr host flash install