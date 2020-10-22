#include "VTSPS2-CRCGS.h"

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
