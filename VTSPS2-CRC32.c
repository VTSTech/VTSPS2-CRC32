// A simple homebrew to demonstrate CRC32 on the PS2
// VTSPS2-CRC32 Written by VTSTech (veritas@vts-tech.org)
// v0.2 2020-05-30 7:29:44 PM
// Now using libcrc
//
// v0.1 2020-05-26 3:26:56 PM
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

void InitPS2()
{
	init_scr();
	ResetIOP();
	SifInitIopHeap();
	SifLoadFileInit();
	fioInit();
	//wipeUserMem();
	sbv_patch_disable_prefix_check();
	SifLoadModule("rom0:SIO2MAN", 0, NULL);
	SifLoadModule("rom0:MCMAN", 0, NULL);
	SifLoadModule("rom0:MCSERV", 0, NULL);
	SifLoadModule("rom0:PADMAN", 0, NULL);
	//sceCdInit(SCECdINoD);
	//cdInitAdd();
	sleep(1);
}

void banner(){
	scr_printf("VTSPS2-CRC32 v0.2 by VTSTech (05.30.2020) \n");
	scr_printf("=======================www.vts-tech.org== \n \n");
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
  FILE *fp;
  size_t len;
  char tmp[32] = "";
  char f_crc32[16] = "";
  char full_path[256] = "";
  //Build full_path string
  strcpy(full_path,device);
  strcat(full_path,path);
  strcat(full_path,fn);
	//8MB file buffer.
  char buf[8000000], *file = full_path;
  //Close the file
  fclose(fp);  
  if (NULL == (fp = fopen(file, "rb")))
  {
        printf("Error! Unable to open %s for reading\n", file);
        //return -1;
  }
  //read file, store length in len, file contents in buf
  len = fread(buf, sizeof(char), sizeof(buf), fp);
  scr_printf("%d bytes read\n", len);
  //Close the file
  fclose(fp);
  sleep(1);
  //Use sprintf to store crc_32() return value in tmp
  //  
  //If file is larger than buffer, update_crc_32() will
  //need to be looped to get large file CRC32
  sprintf(tmp,"%lX",crc_32(buf, len));
  //We only need the last 8 bytes of crc_32 return value
  substring(tmp,f_crc32,9,8);
  //Display CRC32
  scr_printf("CRC32: %s\n",f_crc32);
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
  scr_printf("%d bytes read\n", len);
  //scr_printf("The checksum of %s is:\n\n", file);
  sleep(1);
  sprintf(tmp,"%lX",crc_32(buf, strlen(full_str)));
  substring(tmp,f_crc32,9,8);
  scr_printf("CRC32: %s\n",f_crc32);
}
int main()
{
	//int YCheck;
	char str[8000000];
	char device[256], path[256], fn[256], full_path[256];
	//uint32_t crc_32_val;
	InitPS2();
	WaitTime = Timer();
	sleep(1);
	banner();
	strcpy(device,"mc0:/");
	strcpy(path,"APPS/");
	strcpy(fn,"1MB.test");
	strcpy(full_path,device);
	strcat(full_path,path);
	strcat(full_path,fn);  			
	scr_printf("\n%s\n",full_path);
	file_crc32(device,path,fn);
	strcpy(str," ");
	scr_printf("\n%s\n",str);
	str_crc32(str);
	strcpy(str,"a string");
	scr_printf("\n%s\n",str);
	str_crc32(str);
	strcpy(str,"147");
	scr_printf("\n%s\n",str);
	str_crc32(str);
	scr_printf(" \n* All operations complete. Exit in 10s...");
	sleep(10);
	return 0;
}
