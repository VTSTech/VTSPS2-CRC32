# VTSPS2-CRC32
An example CRC32 implementation for the PS2

<img src="https://i.gyazo.com/cafcd4c147c2c52a9b8e863fe6789618.png">

# Usage:
# file_crc32(char device[], char path[], char fn[])

Expected param format

device "mc0:/"

path   "APPS/"

fn     "FILE.EXT"

<pre>
	strcpy(ldevice,"mc0:/");
	strcpy(path,"APPS/");
	strcpy(fn,"1MB.BIN");
	scr_printf("%s \n",file_crc32(ldevice,path,fn));
</pre>

# str_crc32(char str[])
Expected param format

str    "any string"

<pre>
	strcpy(str,"a string");
	scr_printf("CRC32: %s \n",str_crc32(str));
</pre>

crc32.c written by Lammert Bies, (c) 1999-2016 Lammert Bies. File is licensed under the MIT License
https://github.com/lammertb/libcrc/blob/master/src/crc32.c

Compiled with PS2SDK
https://github.com/ps2dev/ps2sdk