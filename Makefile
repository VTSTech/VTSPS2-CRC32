VERSION = 0.2
NAME = VTSPS2-CRC32

EE_BIN = $(NAME).ELF
EE_BIN_PACKED = $(NAME)-packed.ELF
EE_BIN_STRIPPED = $(NAME)-stripped.ELF
EE_OBJS = $(NAME).o crc32.o usbd.o usbhdfsd.o
EE_LIBS = -lc -lpatches -ldebug

all:
	@echo "======================================="
	@echo "=== Building $(NAME) v$(VERSION) ==="
	@echo "======================================="
	$(MAKE) $(EE_BIN_PACKED)

clean:
	@echo "================"
	@echo "=== Cleaning ==="
	@echo "================"
	rm -fr *.ELF *.o *.bak

run: $(EE_BIN)
	ps2client execee host:$(EE_BIN)

reset:
	ps2client reset

$(EE_BIN_STRIPPED): $(EE_BIN)
	@echo "================="
	@echo "=== Stripping ==="
	@echo "================="
	$(EE_STRIP) -o $@ $<
	
$(EE_BIN_PACKED): $(EE_BIN_STRIPPED)
# Uncomment to compress ELF. Adjust path to match your environment
	@echo "==================="
	@echo "=== Compressing ==="
	@echo "==================="
	ps2-packer -v $< $@
	chmod +x $(EE_BIN_PACKED)

#thx KrahJohlito
usbd.s:
	bin2s $(PS2SDK)/iop/irx/usbd.irx usbd.s usbd
	
usbhdfsd.s:
	bin2s $(PS2SDK)/iop/irx/usbhdfsd.irx usbhdfsd.s usbhdfsd

crc32: crc32.c crc.h
	$(CC) $(CFLAGS) -o $@ $<
		
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
