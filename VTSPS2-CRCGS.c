#include "VTSPS2-CRCGS.h"

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
	sbv_patch_disable_prefix_check();
	SifLoadModule("rom0:SIO2MAN", 0, NULL);
	SifLoadModule("rom0:MCMAN", 0, NULL);
	SifLoadModule("rom0:MCSERV", 0, NULL);
	SifLoadModule("rom0:PADMAN", 0, NULL);
	//sleep(1);
}

void substring(char s[], char sub[], int p, int l) {
   int c = 0;
   while (c < l) {
      sub[c] = s[p+c-1];
      c++;
   }
   sub[c] = '\0';
}

char* file_crc32(char device[], char path[], char fn[])
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
        //scr_printf("ERROR: Unable to open %s for reading \n", full_path);
	return("ERROR: Unable to open file for reading");
  }
  //read file, store length in len
  fseek(fp,0,SEEK_END);
  long len = ftell(fp);
  //long fsize = len;
  fseek(fp,0,SEEK_SET);
  //scr_printf("Filesize: %lu bytes \n", fsize);
  //4MB File Buffer. If less than that read entire into buf
  if (len <= 4194304) {
	char buf[len];
	while((fread(buf, 1, len, fp)) > 0){
		//scr_printf("%lu bytes read \n", len);
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
	//sleep(1);
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
  //scr_printf("CRC32: %s \n",f_crc32);
  return(f_crc32);
}

char* str_crc32(char str[])
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
  //sleep(1);
  sprintf(tmp,"%lX", crc_32(buf, strlen(full_str)));
  substring(tmp,f_crc32,9,8);
  //scr_printf("CRC32: %s \n",f_crc32);
  return(f_crc32);
}

int main(int argc, char *argv[])
{
	InitPS2();

	u64 White, Black, BlackFont, WhiteFont, RedFont, GreenFont, BlueFont, BlueTrans, RedTrans, GreenTrans, WhiteTrans;
	u64 TealFont, YellowFont;

	GSGLOBAL *gsGlobal = gsKit_init_global();
	GSFONT *gsFont = gsKit_init_font(GSKIT_FTYPE_BMP_DAT, "mc0:/APPS/dejavu.bmp");

	//char str[8000000];
	char ldevice[256], path[256], fn[256], full_path[256];
	getcwd(full_path,sizeof(full_path));

	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
		    D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00);
	White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);

	WhiteFont = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x80,0x00);
	BlackFont = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x80,0x00);
	RedFont = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x80,0x00);
	GreenFont = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x80,0x00);
	BlueFont = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x80,0x00);
	TealFont = GS_SETREG_RGBAQ(0x00,0xFF,0xFF,0x80,0x00);
	YellowFont = GS_SETREG_RGBAQ(0xFF,0xFF,0x00,0x80,0x00);

	BlueTrans = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x40,0x00);
	RedTrans = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x60,0x00);
	GreenTrans = GS_SETREG_RGBAQ(0x00,0xFF,0x00,0x50,0x00);
	WhiteTrans = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x50,0x00);

	gsGlobal->PrimAlpha = GS_BLEND_FRONT2BACK;
	gsGlobal->PSM = GS_PSM_CT16;
	gsGlobal->PSMZ = GS_PSMZ_16;

	gsKit_init_screen(gsGlobal);
	gsKit_mode_switch(gsGlobal, GS_PERSISTENT);
	gsKit_font_upload(gsGlobal, gsFont);
	gsKit_clear(gsGlobal, Black);

	//Line1, 10
	//Line2, 27
	//Line3, 44
	//Line4, 61..
	//78..95..112
	gsKit_font_print(gsGlobal, gsFont, 10, 10, 1, TealFont, "VTSPS2-CRC32 v0.24 written by VTSTech\n=============www.VTS-Tech.org=\n");
	//Line3...
	//gsKit_font_print(gsGlobal, gsFont, 48, 48, 1, RedFont, "48x48");
	//gsKit_font_print(gsGlobal, gsFont, 64, 64, 1, RedFont, "64x64");
	//gsKit_font_print(gsGlobal, gsFont, 128, 128, 1, RedFont, "128x128");

	if (strstr(full_path,"host:")) {
		strcpy(ldevice,"mc0:/");
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
	if (strstr(full_path,"host:")){
		strcpy(path,"APPS/");
	}
	strcpy(fn,"1MB.BIN");
	char* fnl = "Filename: ";
	char* str = "";
	strcpy(str,fnl);
	strcat(str,fn);
	gsKit_font_print(gsGlobal, gsFont, 10, 78, 1, WhiteFont, str);
	strcpy(str,file_crc32(ldevice,path,fn));
	gsKit_font_print(gsGlobal, gsFont, 150, 78, 1, GreenFont, str);
	gsKit_queue_exec(gsGlobal);
	gsKit_sync_flip(gsGlobal);
	strncpy(str,"",1);
	strcpy(fn,"2MB.BIN");
	strcpy(str,"Filename: ");
	strcpy(str,fnl);
	strcat(str,fn);
	gsKit_font_print(gsGlobal, gsFont, 10, 95, 1, WhiteFont, str);
	strncpy(str,"",1);
	strcpy(str,file_crc32(ldevice,path,fn));
	gsKit_font_print(gsGlobal, gsFont, 150, 95, 1, GreenFont, str);
	gsKit_queue_exec(gsGlobal);
	gsKit_sync_flip(gsGlobal);
	//str = str_crc32("a string");
	//gsKit_font_print(gsGlobal, gsFont, 128, 112, 1, GreenFont, str);
	gsKit_font_print(gsGlobal, gsFont, 10, 112, 1, WhiteFont, "String: ' '");
	strcpy(str,str_crc32(" "));
	gsKit_font_print(gsGlobal, gsFont, 150, 112, 1, GreenFont, str);
	gsKit_queue_exec(gsGlobal);
	gsKit_sync_flip(gsGlobal);
	gsKit_font_print(gsGlobal, gsFont, 10, 129, 1, WhiteFont, "String: 'a string' ");
	strcpy(str,str_crc32("a string"));
	gsKit_font_print(gsGlobal, gsFont, 150, 129, 1, GreenFont, str);
	gsKit_queue_exec(gsGlobal);
	gsKit_sync_flip(gsGlobal);
	gsKit_font_print(gsGlobal, gsFont, 10, 146, 1, WhiteFont, "String: '147' ");
	strcpy(str,str_crc32("147"));
	gsKit_font_print(gsGlobal, gsFont, 150, 146, 1, GreenFont, str);
	gsKit_queue_exec(gsGlobal);
	gsKit_sync_flip(gsGlobal);
	//Line12...
	//gsKit_font_print(gsGlobal, gsFont, 50, 80, 1, RedFont, "red");
	//gsKit_font_print(gsGlobal, gsFont, 50, 110, 1, GreenFont, "green");
	//gsKit_font_print(gsGlobal, gsFont, 50, 140, 1, BlueFont, "blue");

	//gsKit_font_print_scaled(gsGlobal, gsFont, 400, 160, 2, 2.0f, BlueFont, "scaled 1\n"\
	//                                                                       "scaled 2\n"\
	//                                                                       "scaled 3\n");

	//gsKit_font_print(gsGlobal, gsFont, 100, 200, 2, WhiteFont, "Testing 1\n"\
	//			 				     "Testing 2\n"\
	//						       	     "Testing 3\n"\
	//						             "Testing 4\n"\
	//		 					     "Testing 5\n"\
	//							     "Testing 6\n"\
	//							     "Testing 7\n"\
	//							     "Testing 8\n"\
	//							     "Testing 9\n"\
	//							     "Testing 10\n");

	while(1)
	{
		gsKit_queue_exec(gsGlobal);
		gsKit_sync_flip(gsGlobal);
	}

	return 0;
}
