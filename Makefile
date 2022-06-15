#  ____     ___ |    / _____ _____
# |  __    |    |___/    |     |
# |___| ___|    |    \ __|__   |     gsKit Open Source Project.
# ----------------------------------------------------------------------
# Copyright 2004 - Chris "Neovanglist" Gilbert <Neovanglist@LainOS.org>
# Licenced under Academic Free License version 2.0
# Review gsKit README & LICENSE files for further details.
#
# examples/font/Makefile - Makefile for "font" example.
#
VERSION = 0.1
NAME = VTSPS2-CRCGS
EE_BIN = $(NAME).ELF
EE_BIN_PACKED = $(NAME)-packed.ELF
EE_BIN_STRIPPED = $(NAME)-stripped.ELF

EE_OBJS = VTSPS2-CRCGS.o crc32.o VTSPS2-CRC32.o

EE_LIBS += -lc -ldebug -lpatches -lgskit_toolkit -lgskit -ldmakit

EE_INCS += -I$(GSKIT)/include -I$(GSKIT)/ee/dma/include -I$(GSKIT)/ee/gs/include -I$(GSKIT)/ee/toolkit/include
# linker flags
EE_LIB_DIRS += -L$(GSKIT)/lib
EE_LIB_DIRS += -L$(PS2SDK)/ee/lib
EE_LDFLAGS += -Wl,--allow-multiple-definition $(EE_LIB_DIRS)
EE_CFLAGS += -Wno-pointer-sign -Wno-implicit-function-declaration -Wno-strict-aliasing -Wno-format-overflow -Wno-format-truncation
all:
	@echo "======================================="
	@echo "=== Building $(NAME) v$(VERSION) ==="
	@echo "======================================="
	$(MAKE) $(EE_BIN_PACKED)

clean:
	rm -f *.elf *.ELF *.o *.s

$(EE_BIN_STRIPPED): $(EE_BIN)
	$(EE_STRIP) -o $@ $<

$(EE_BIN_PACKED): $(EE_BIN_STRIPPED)
	ps2-packer $(EE_BIN) $(EE_BIN_PACKED)
	chmod +x $(EE_BIN_PACKED)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
