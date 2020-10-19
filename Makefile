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
GSKITSRC = /homebrew/gsKit
EE_BIN  = VTSPS2-CRCGS.elf
EE_BIN_PACKED = VTSPS2-CRCGS-packed.elf
EE_OBJS = VTSPS2-CRCGS.o crc32.o

include Makefile.pref
include Makefile.global
