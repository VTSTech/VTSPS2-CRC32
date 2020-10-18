// A simple homebrew to demonstrate CRC32 on the PS2
// VTSPS2-CRC32 Written by VTSTech (veritas@vts-tech.org)
//
// v0.23 2020-10-17 6:05:11 PM
// Now handles any file size..
// ..Including files too large
// to fit in memory.
//
// v0.22 2020-10-15 9:03:22 AM
// Now handles any file size
//
// v0.21 2020-10-12 6:29:33 AM
// Now POSIX compliant
//
// v0.2  2020-05-30 7:29:44 PM
// Now using libcrc
//
// v0.1  2020-05-26 3:26:56 PM
// First release

#include "VTSPS2-CRC32.h"

extern u64 WaitTime;
extern u64 CurrTime;
u64 WaitTime;
u64 CurrTime;
u64 init_delay_start;
u64 timeout_start;
u64 Timer(void)
{
	return (clock() / (CLOCKS_PER_SEC / 1000));
}

//thx sp193
void ResetIOP()
{
	SifInitRpc(0);
	while(!SifIopReset("", 0)){};
	while(!SifIopSync()){};
	SifInitRpc(0);
}

void LoadModules(void)
{
	int ret;
	ret = SifExecModuleBuffer(&usbd, size_usbd, 0, NULL, NULL);
	if (ret < 0)
	{
		scr_printf("Could not load usbd.irx! %d \n", ret);
		//SleepThread();
	}

	ret = SifExecModuleBuffer(&usbhdfsd, size_usbhdfsd, 0, NULL, NULL);
	if (ret < 0)
	{
		scr_printf("Could not load usbhdfsd.irx! %d \n", ret);
		//SleepThread();
	}
}

void InitPS2()
{
	init_scr();
	ResetIOP();
	SifInitIopHeap();
	SifLoadFileInit();
	sbv_patch_disable_prefix_check();
	SifLoadModule("rom0:SIO2MAN", 0, NULL);
	SifLoadModule("rom0:MCMAN", 0, NULL);
	SifLoadModule("rom0:MCSERV", 0, NULL);
	SifLoadModule("rom0:PADMAN", 0, NULL);
	LoadModules();
	sleep(1);
}

void banner(){
	scr_printf(" \nVTSPS2-CRC32 v0.23 by VTSTech (10.17.2020) \n");
	scr_printf("========================www.vts-tech.org== \n \n");
}
void substring(char s[], char sub[], int p, int l) {
   int c = 0;
   while (c < l) {
      sub[c] = s[p+c-1];
      c++;
   }
   sub[c] = '\0';
}
void file_crc32(char device[], char path[], char fn[])
{
  char tmp[32] = "";
  char f_crc32[16] = "";
  uint32_t t_crc32 = 0xffffffffL;
  char full_path[256] = "";
  int chunks_curr = 1;
  int bytes_read;
  //Build full_path string
  strcpy(full_path,device);
  strcat(full_path,path);
  strcat(full_path,fn);
  //scr_printf("DEBUG (full_path): %s\n", full_path);
  FILE *fp = fopen(full_path, "rb");
  sleep(1);
  if (!fp)
  {
        scr_printf("ERROR: Unable to open %s for reading \n", full_path);
	return;
  }
  //read file, store length in len
  fseek(fp,0,SEEK_END);
  long len = ftell(fp);
  long fsize = len;
  fseek(fp,0,SEEK_SET);
  scr_printf("Filesize: %lu bytes \n", fsize);
  //4MB File Buffer. If less than that read entire into buf
  if (len <= 4194304) {
	char buf[len];
	while((fread(buf, 1, len, fp)) > 0){
		scr_printf("%lu bytes read \n", len);
	}
	//Close the file
	fclose(fp);
	sleep(1);
	//Use sprintf to store crc_32() return value in tmp
	//If file is larger than buffer, update_crc_32() will
	//need to be looped to get large file CRC32
	sprintf(tmp,"%lX", crc_32(buf, len));
  //4MB File Buffer. If more than that read byte by byte into ch
  //Calling update_crc_32 and passing the old CRC32 and new byte each time.
  } else {
  	//int chunks_left = len;
	char buf[1];
	int ch;
	//scr_printf("Chunks Left: %d\n ", chunks_left);
	ch=fgetc(fp);
	t_crc32 = update_crc_32(t_crc32,(unsigned char) ch);
	sprintf(tmp,"%lX", t_crc32);
	//scr_printf("DEBUG: Initial Buffer read, CRC: %lx \n",tmp);
	bytes_read = sizeof(buf);
	while((ch=fgetc(fp)) != EOF){
		t_crc32 = update_crc_32(t_crc32, (unsigned char) ch);
		//if ((chunks_left - chunks_curr) <=4){
		//	scr_printf("DEBUG: Current/Initial CRC: %lx/%lx \n", t_crc32,tmp);
		//}
		//sprintf(tmp,"%lx",t_crc32);
		bytes_read++;
		chunks_curr++;
		//scr_printf("%d bytes read \n", bytes_read);
	}
	//Close the file.
	fclose(fp);
	sleep(1);
	//crc lib requires this operation be preformed on final value
	t_crc32 ^= 0xffffffffL;
	//Copy the final CRC32 to tmp
	sprintf(tmp,"%lX",t_crc32);
  }

  //scr_printf("Debug: %s\n", tmp);
  //We only need the last 8 bytes of crc_32 return value
  //Sometimes it is twice as long preceded by 0xffffffff
  //copy processed value to f_crc32
  //scr_printf("Debug (tmp): %lx \n", tmp);
  if (strlen(tmp)>=9){
    substring(tmp,f_crc32,9,8);
  } else if (strlen(tmp)<=7) {
	    if (strlen(tmp) == 7) {
		sprintf(f_crc32,"0%s",tmp);
	    } else if (strlen(tmp) == 6) {
		sprintf(f_crc32,"00%s",tmp);
	    } else if (strlen(tmp) == 5) {
		sprintf(f_crc32,"000%s",tmp);
	    } else if (strlen(tmp) == 4) {
		sprintf(f_crc32,"0000%s",tmp);
	    } else if (strlen(tmp) == 3) {
		sprintf(f_crc32,"00000%s",tmp);
	    } else if (strlen(tmp) == 2) {
		sprintf(f_crc32,"000000%s",tmp);
	    } else if (strlen(tmp) == 1) {
		sprintf(f_crc32,"0000000%s",tmp);
	    }
    //substring(tmp,f_crc32,0,8);
  } else {
    substring(tmp,f_crc32,1,8);
  }
  //Display CRC32
  scr_printf("CRC32: %s \n",f_crc32);
}

void str_crc32(char str[])
{
  size_t len;
  char tmp[32] = "";
  char f_crc32[16] = "";
  char full_str[8000000] = "";
  char buf[8000000] = "";
  strncpy(full_str,str,strlen(str));
  strcpy(buf,full_str);
  len = strlen(full_str);
  scr_printf("%d bytes read \n", len);
  //scr_printf("The checksum of %s is:\n\n", file);
  sleep(1);
  sprintf(tmp,"%lX", crc_32(buf, strlen(full_str)));
  substring(tmp,f_crc32,9,8);
  scr_printf("CRC32: %s \n",f_crc32);
}
int main()
{
	//int YCheck;
	char str[8000000];
	char ldevice[256], path[256], fn[256], full_path[256];
	//char *tmp;
	//uint32_t crc_32_val;
	InitPS2();
	WaitTime = Timer();
	//sleep(1);
	banner();
	//strcpy(device,"mc0:/");
	//strcpy(path,"APPS/");
	getcwd(full_path,sizeof(full_path));
	if (strstr(full_path,"host:")) {
		strcpy(ldevice,"host:/");
	} else if (strstr(full_path,"mc0:")) {
		strcpy(ldevice,"mc0:/");
	} else if (strstr(full_path,"mc1:")) {
		strcpy(ldevice,"mc1:/");
	} else if (strstr(full_path,"mass:")) {
		strcpy(ldevice,"mass:/");
	} else if (strstr(full_path,"mass0:")) {
		strcpy(ldevice,"mass0:/");
	}
	substring(full_path,path,(strlen(ldevice)+1),(strlen(full_path)-strlen(ldevice))+1);
	strcpy(fn,"1MB.BIN");
	//strcpy(full_path,device);
	//strcat(full_path,path);
	//strcat(full_path,fn);
	scr_printf("%s ",fn);
	//scr_printf("(device): %s", device);
	file_crc32(ldevice,path,fn);
	strcpy(fn,"2MB.BIN");
	scr_printf("%s ",fn);
	file_crc32(ldevice,path,fn);
	strcpy(fn,"4MB.BIN");
	scr_printf("%s ",fn);
	file_crc32(ldevice,path,fn);
	strcpy(fn,"8MB.BIN");
	scr_printf("%s ",fn);
	file_crc32(ldevice,path,fn);
	strcpy(fn,"16MB.BIN");
	scr_printf("%s ",fn);
	file_crc32(ldevice,path,fn);
	strcpy(fn,"32MB.BIN");
	scr_printf("%s ",fn);
	file_crc32(ldevice,path,fn);
	strcpy(fn,"64MB.BIN");
	scr_printf("%s ",fn);
	file_crc32(ldevice,path,fn);
	strcpy(fn,"128MB.BIN");
	scr_printf("%s ",fn);
	file_crc32(ldevice,path,fn);
	strcpy(str," ");
	scr_printf("Space: '%s' \n",str);
	str_crc32(str);
	strcpy(str,"a string");
	scr_printf("Text: '%s' \n",str);
	str_crc32(str);
	strcpy(str,"147");
	scr_printf("Number: '%s' \n",str);
	str_crc32(str);
	scr_printf(" \n* All operations complete. Exit in 10s... \n");
	sleep(10);
	return 0;
}
