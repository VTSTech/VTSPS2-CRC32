#include <debug.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <loadfile.h>
#include <tamtypes.h>
#include <sbv_patches.h>
#include <sifrpc.h>
#include <iopcontrol.h>
#include <iopheap.h>
#include <inttypes.h>
#include <fcntl.h>
#include "checksum.h"

extern void usbd;
extern void usbhdfsd;

extern u32 size_usbd;
extern u32 size_usbhdfsd;

