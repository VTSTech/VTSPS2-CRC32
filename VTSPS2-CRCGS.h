#include <gsKit.h>
#include <dmaKit.h>
#include <malloc.h>
#include <gsToolkit.h>
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

//extern void usbd;
//extern void usbhdfsd;

//extern u32 size_usbd;
//extern u32 size_usbhdfsd;

char* file_crc32(char device[], char path[], char fn[]);
char* str_crc32(char str[]);
