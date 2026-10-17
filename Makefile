# Requires devkitPro with devkitPPC + libogc (pacman -S wii-dev)
ifeq ($(strip $(DEVKITPRO)),)
$(error Set DEVKITPRO, e.g. export DEVKITPRO=/opt/devkitpro)
endif
export PATH := $(DEVKITPRO)/devkitPPC/bin:$(DEVKITPRO)/tools/bin:$(PATH)

CC      = powerpc-eabi-gcc
MACHDEP = -DGEKKO -mrvl -mcpu=750 -meabi -mhard-float
CFLAGS  = -O2 -Wall $(MACHDEP) -I$(DEVKITPRO)/libogc/include
LDFLAGS = $(MACHDEP) -L$(DEVKITPRO)/libogc/lib/wii
LIBS    = -lasnd -lwiiuse -lbte -logc -lm

all: boot.dol

boot.elf: main.c
	$(CC) $(CFLAGS) main.c $(LDFLAGS) $(LIBS) -o $@

boot.dol: boot.elf
	elf2dol boot.elf boot.dol

package: boot.dol
	mkdir -p apps/FlashMii
	cp boot.dol apps/FlashMii/boot.dol

clean:
	rm -f boot.elf boot.dol
