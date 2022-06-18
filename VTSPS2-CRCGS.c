#include "VTSPS2-CRCGS.h"

u64 White, Black, BlackFont, WhiteFont, RedFont, GreenFont, BlueFont, BlueTrans, RedTrans, GreenTrans, WhiteTrans;
u64 TealFont, YellowFont;

GSGLOBAL *gsGlobal;
//GSFONT *gsFont;
GSFONTM *gsFontM;

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

int main(int argc, char *argv[])
{
	InitPS2();

	gsGlobal = gsKit_init_global();
	//gsFont = gsKit_init_font(GSKIT_FTYPE_BMP_DAT, "mc0:/APPS/dejavu.bmp");
	gsFontM = gsKit_init_fontm();
	
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
	gsGlobal->Width = 640;
	// Buffer Init
	gsGlobal->PrimAAEnable = GS_SETTING_ON;
	gsGlobal->DoubleBuffering = GS_SETTING_OFF;
	gsGlobal->ZBuffering      = GS_SETTING_OFF;	
	gsFontM->Spacing = 0.95f;
	int text_height,edge_size;
	text_height = (26.0f * gsFontM->Spacing * 0.5f);
	edge_size = text_height;
	gsKit_init_screen(gsGlobal);
	gsKit_mode_switch(gsGlobal, GS_PERSISTENT);
	//gsKit_font_upload(gsGlobal, gsFont);
	gsKit_fontm_upload(gsGlobal, gsFontM);
	gsKit_clear(gsGlobal, Black);
	//Enable transparency
	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	//Line1, 10
	//Line2, 27
	//Line3, 44
	//Line4, 61..
	//78..95..112
	int rownumber = 0;
	int rowoffset = 0;
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, 10, 1, 0.6f,TealFont, "libcrc for PS2 0.25 written by VTSTech\ncrc32 routines written by Lammert Bies\n=====================www.VTS-Tech.org=");
	rownumber = 3;
	char* str = "";
	char* fnl = "Filename: ";
	//gsKit_fontm_print_scaled(gsGlobal, gsFontM, col, row, layer, scale, RedFont, str);
	//gsKit_fontm_print_scaled(gsGlobal, gsFontM, 48, 48, 1, 0.6f, RedFont, "48x48");
	//gsKit_fontm_print_scaled(gsGlobal, gsFontM, 64, 64, 1, 0.6f, RedFont, "64x64");
	//gsKit_fontm_print_scaled(gsGlobal, gsFontM, 128, 128, 1, 0.6f, RedFont, "128x1228");
	gsKit_queue_exec(gsGlobal);
	gsKit_sync_flip(gsGlobal);
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
	rownumber++;
	rowoffset = 10+(rownumber*17);	
	strncpy(str,"",1);
	strcpy(fn,"1MB.BIN");
	strcpy(str,fnl);
	strcat(str,fn);
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, rowoffset, 1, 0.6f, WhiteFont, str);
	strcpy(str,file_crc32(ldevice,path,fn));
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 320, rowoffset, 1, 0.6f, GreenFont, str);
	gsKit_queue_exec(gsGlobal);
	gsKit_sync_flip(gsGlobal);
	rownumber++;
	rowoffset = 10+(rownumber*17);
	strncpy(str,"",1);
	strcpy(fn,"2MB.BIN");
	strcpy(str,"Filename: ");
	strcpy(str,fnl);
	strcat(str,fn);
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, rowoffset, 1, 0.6f, WhiteFont, str);
	strncpy(str,"",1);
	strcpy(str,file_crc32(ldevice,path,fn));
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 320, rowoffset, 1, 0.6f, GreenFont, str);
	gsKit_queue_exec(gsGlobal);
	gsKit_sync_flip(gsGlobal);
	rownumber++;
	rowoffset = 10+(rownumber*17);	
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, rowoffset, 1, 0.6f, WhiteFont, "String: ' '");
	strcpy(str,str_crc32(" "));
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 320, rowoffset, 1, 0.6f, GreenFont, str);
	gsKit_queue_exec(gsGlobal);
	gsKit_sync_flip(gsGlobal);
	rownumber++;
	rowoffset = 10+(rownumber*17);	
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, rowoffset, 1, 0.6f, WhiteFont, "String: 'a string' ");
	strcpy(str,str_crc32("a string"));
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 320, rowoffset, 1, 0.6f, GreenFont, str);
	gsKit_queue_exec(gsGlobal);
	gsKit_sync_flip(gsGlobal);
	rownumber++;
	rowoffset = 10+(rownumber*17);	
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, rowoffset, 1, 0.6f, WhiteFont, "String: '147' ");
	strcpy(str,str_crc32("147"));
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 320, rowoffset, 1, 0.6f, GreenFont, str);
	gsKit_queue_exec(gsGlobal);
	gsKit_sync_flip(gsGlobal);
	rownumber++;
	rowoffset = 10+(rownumber*17);	
	//Line12...
	//while(1)
	//{
	//	gsKit_queue_exec(gsGlobal);
	//	gsKit_sync_flip(gsGlobal);
	//}
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, 10, rowoffset, 1, 0.6f, TealFont, "* All Done. Exit in 10s ...");
	gsKit_queue_exec(gsGlobal);
	gsKit_sync_flip(gsGlobal);
	sleep(10);
	return 0;
}
