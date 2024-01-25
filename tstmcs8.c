// --------------------------------------------------------------------------
// TSTMCS8.C : DMCS8.DLL Software driver C example
// --------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <time.h>

#undef DLL
#include "dmcs8.h"


HANDLE          hDLL = 0;  //��ΪDLL��������

IMPAGETSETTING  lpSet=NULL;
IMPANEWSTATUS   lpNewStat=NULL;
IMPAGETSTATUS   lpStat=NULL;
IMPARUNCMD      lpRun=NULL;
IMPAGETCNT      lpCnt=NULL;
IMPAGETROI      lpRoi=NULL;
IMPAGETDAT      lpDat=NULL;
IMPAGETSTR      lpStr=NULL;
IMPASERVEXEC    lpServ=NULL;
IMPAGETDATSET   lpGetDatSet=NULL;
IMPAGETMCSSET   lpGetMCSSet=NULL;
IMPADIGINOUT    lpDigInOut=NULL;
IMPASTART		lpStart=NULL;
IMPAHALT		lpHalt=NULL;
IMPACONTINUE    lpContinue=NULL;
IMPAERASE		lpErase=NULL;


ACQSETTING     Setting={0};
ACQDATA        Data={0};
ACQDEF         Def={0};
ACQSTATUS      Status={0};
DATSETTING     DatSetting={0};
BOARDSETTING   MCSSetting={0};

int nDev=0;

void help()
{
	printf("Commands:\n");
	printf("Q		Quit\n");
	printf("?		Help\n");
	printf("S       Show Status\n");
	printf("H		Halt\n");
	printf("T       Show Setting\n");
	printf("B       Show BoardSetting\n");
	printf("CHN=x   Switch to CHN #x \n");
	printf("rdig    Read Dig. I/O port\n");
	printf("w5 aabb		Write aabb into Reg.5\n");
    printf("r5		Read Reg 5\n");
	printf("rx0		Read Reg 0..f\n");
	printf("rx7		Read Reg 70..7f\n");
	printf("reep0		Read 64 byte from eeprom at address 0\n");
	printf("sleep=10	Sleep 10 msec\n");
	printf("run filename\n");
    printf("(... more see command language in MPANT help)\n");
    printf("\n");
}

void PrintMpaStatus(ACQSTATUS *Stat)
{
  if(Stat->started == 1) printf("ON\n"); 
  else if(Stat->started & 0x02) printf("READ OUT\n");
  else printf("OFF\n");
  printf("runtime=   %.3lf\n", Stat->cnt[ST_RUNTIME]);
  printf("sweeps=   %lf\n",    Stat->cnt[ST_SWEEPS]);
  printf("starts=   %lf\n\n",  Stat->cnt[ST_STARTS]);
}

void PrintStatus(ACQSTATUS *Stat)
{
  printf("totalsum=   %.0lf\n", Stat->cnt[ST_TOTALSUM]);
  printf("roisum=     %.0lf\n", Stat->cnt[ST_ROISUM]);
  printf("rate=     %.2lf\n", Stat->cnt[ST_ROIRATE]);
  printf("ofls=     %.2lf\n\n", Stat->cnt[ST_OFLS]);
}

void PrintDatSetting(DATSETTING *Set)
{
  printf("savedata= %d\n", Set->savedata);
  printf("autoinc=  %d\n", Set->autoinc);
  printf("fmt=      %d\n", Set->fmt);
  printf("mpafmt=   %d\n", Set->mpafmt);
  printf("sephead=  %d\n", Set->sephead);
  printf("filename= %s\n\n", Set->filename);
}

void PrintMCSSetting(BOARDSETTING *Set)
{
  printf("sweepmode=  0x%x\n", Set->sweepmode);
  printf("prena=      0x%x\n", Set->prena);
  printf("cycles=     %d\n", Set->cycles);
  printf("sequences=  %d\n", Set->sequences);
  printf("syncout=    0x%x\n", Set->syncout);
  printf("digio=      0x%x\n", Set->digio);
  printf("digval=     %d\n", Set->digval);
  printf("dac0=       0x%x\n", Set->dac0);
  printf("dac1=       0x%x\n", Set->dac1);
  printf("dac2=       0x%x\n", Set->dac2);
  printf("dac3=       0x%x\n", Set->dac3);
  printf("dac4=       0x%x\n", Set->dac4);
  printf("dac5=       0x%x\n", Set->dac5);
//  printf("fdac=       0x%x\n", Set->fdac);
  printf("tagbits=    %d\n", Set->tagbits);
  printf("extclk=     %d\n", Set->extclk);
  printf("periods=    %d\n", Set->periods);
  printf("serno=      %d\n", Set->serno);
  printf("ddruse=     0x%x\n", Set->ddruse);
  printf("active=     %d\n", Set->active);
  printf("holdafter=  %lg\n", Set->holdafter);
  printf("swpreset=   %lg\n", Set->swpreset);
  printf("fstchan=    %lg\n", Set->fstchan);
  printf("timepreset= %lg\n\n", Set->timepreset);
}

void PrintSetting(ACQSETTING *Set)
{
  printf("range=     %ld\n", Set->range);
  printf("cftfak=    0x%x\n", Set->cftfak);
  printf("roimin=    %ld\n", Set->roimin);
  printf("roimax=    %ld\n", Set->roimax);
  printf("nregions=  %d\n", Set->nregions);
  printf("caluse=    %d\n", Set->caluse);
  printf("calpoints= %d\n", Set->calpoints);
  printf("param=     0x%lx\n", Set->param);
  printf("offset=    0x%lx\n", Set->offset);
  printf("xdim=      %d\n", Set->xdim);
  printf("bitshift=  %d\n", Set->bitshift);
  printf("active=    0x%x\n", Set->active);
  printf("roipreset= %lg\n\n", Set->eventpreset);
}


int run(char *command)
{
	int err;
	if (!stricmp(command, "?"))           help();
	else if (!stricmp(command, "rdig")) 
	{		// read Dig I/O port
	  if (lpDigInOut)
	  {
	    err = (*lpDigInOut)(0xff,0);
	    printf("%x\n", err);
	  }
	}
	else if (!strnicmp(command, "wdig=", 5)) 
	{	// write Dig I/O port (open drain)
	  if (lpDigInOut) 
	  {
		unsigned int val;
		sscanf(command+5, "%x", &val);
		val &= 0xff;
	    err = (*lpDigInOut)(val,0xff);
	    printf("%x\n", err);
	  }
	}
	else if (!strnicmp(command, "pdig=", 5)) {	// write Dig I/O port (push-pull)
	  if (lpDigInOut) 
	  {
		unsigned int val;
		sscanf(command+5, "%x", &val);
	    err = (*lpDigInOut)(val,0xff);
	    printf("%x\n", err);
	  }
	}
	else if (!stricmp(command,"Q"))       return 1;
	//�˳�����
	else if (!stricmp(command,"S")) 
	{
      err = (*lpStat)(&Status, nDev);
	  if (nDev) PrintStatus(&Status);
	  else PrintMpaStatus(&Status);
	  return 0;
	}
	//���ﶼ�Ƕ�ȡ���õĲ�����
	else if (!stricmp(command,"T")) 
	{
	  	// spectra settings
        err = (*lpSet)(&Setting, nDev);
		printf("CHN %d:\n", nDev);
		PrintSetting(&Setting);
	  
	  if (nDev==0)
	  {        // MPA settings
        err = (*lpGetMCSSet)(&MCSSetting, 0);
		PrintMCSSetting(&MCSSetting);
		            // DATSettings
        err = (*lpGetDatSet)(&DatSetting);
		PrintDatSetting(&DatSetting);
	  }
	  return 0;
	}
	//��ָͣ��
	else if (!stricmp(command,"H")) 
	{
      (*lpHalt)(0);
	  return 0;
	}

	else if(!strnicmp(command, "CHN=", 4)) 
	{
	  sscanf(command+4, "%d", &nDev);
	  (*lpRun)(0, command);
	}

	else if (!stricmp(command,"MPA")) {
	  nDev=0;
	  (*lpRun)(0, command);
	}
	else {
		(*lpRun)(0, command);
		printf("%s\n", command);
	}
	return 0;
}

int readstr(char *buff, int buflen)
{
  int i=0,ic;

  while ((ic=getchar()) != 10)
  {
    if (ic == EOF) 
	{
      buff[i]='\0';
      return 1;
    }
    if (ic == 13) ic=0;
    buff[i]=(char)ic;
    i++;
    if (i==buflen-1) break;
  }
  buff[i]='\0';
  return 0;
}
//��ȡ���ڵ�ʱ�䲢��ʱ��������鵱��
int get_time_now(char *now_time)
{
	char tem_time[128];
	int i;
	time_t rawtime;
	memset(tem_time, 0, sizeof(tem_time));
	struct tm* tmp;
	rawtime = time(NULL);
	tmp = localtime(&rawtime);
	strftime(tem_time, sizeof(tem_time), "%Y-%m-%d %H:%M:%S", tmp);
	tem_time[10] = '-'; tem_time[13] = '-'; tem_time[16] = '-';
	printf("%s\n", tem_time);
	for (i = 0; i < 128;i++)
	{
		now_time[i] = tem_time[i];
	}
}
//�õ��ļ���������,�Լ�ԭ���ݵ�·��
int filename_settting(char *now_time, char *command_fil_name ,char *raw_file_path)
{
	int i;
	int length;
	char houzhui[128] = ".mpa";
	for(i=8;i<128;i++)
	{
		command_fil_name[i] = now_time[i - 8];
	}
	length= strlen(raw_file_path);
	for (i= length;i<128;i++)
	{
		raw_file_path[i]= now_time[i - length];
	}
	length = strlen(raw_file_path);
	for (i = length; i < 128; i++)
	{
		raw_file_path[i] = houzhui[i - length];
	}
}
//�����ļ��е�����
int read_data(char* raw_file_path,int length,char *store_data,char * now_time)
{
	int i;
	int channel_flag=99;
	int count_number = 0;
	char d_file[128] = "D:/c_txt_data/";
	FILE* fpRead = fopen(raw_file_path, "r");
	strcat(d_file, now_time); strcat(d_file, "-0.txt");
	FILE* outfile0; outfile0 = fopen(d_file, "w");
	for (i = 0; i < 128; i++) { d_file[i] = "\0"; }strcpy(d_file, "D:/c_txt_data/");
	strcat(d_file, now_time); strcat(d_file, "-1.txt");
	FILE* outfile1; outfile1 = fopen(d_file, "w");
	for (i = 0; i < 128; i++) { d_file[i] = "\0"; }strcpy(d_file, "D:/c_txt_data/");
	strcat(d_file, now_time); strcat(d_file, "-2.txt");
	FILE* outfile2; outfile2 = fopen(d_file, "w");
	for (i = 0; i < 128; i++) { d_file[i] = "\0"; }strcpy(d_file, "D:/c_txt_data/");
	strcat(d_file, now_time); strcat(d_file, "-3.txt");
	FILE* outfile3; outfile3 = fopen(d_file, "w");
	for (i = 0; i < 128; i++) { d_file[i] = "\0"; }strcpy(d_file, "D:/c_txt_data/");
	strcat(d_file, now_time); strcat(d_file, "-4.txt");
	FILE* outfile4; outfile4 = fopen(d_file, "w");
	for (i = 0; i < 128; i++) { d_file[i] = "\0"; }strcpy(d_file, "D:/c_txt_data/");
	strcat(d_file, now_time); strcat(d_file, "-5.txt");
	FILE* outfile5; outfile5 = fopen(d_file, "w");
	for (i = 0; i < 128; i++) { d_file[i] = "\0"; }strcpy(d_file, "D:/c_txt_data/");
	strcat(d_file, now_time); strcat(d_file, "-6.txt");
	FILE* outfile6; outfile6 = fopen(d_file, "w");
	for (i = 0; i < 128; i++) { d_file[i] = "\0"; }strcpy(d_file, "D:/c_txt_data/");
	strcat(d_file, now_time); strcat(d_file, "-7.txt");
	FILE* outfile7; outfile7 = fopen(d_file, "w");
	for (i = 0; i < 128; i++) { d_file[i] = "\0"; }strcpy(d_file, "D:/c_txt_data/");
	strcat(d_file, now_time); strcat(d_file, "-8.txt");
	FILE* outfile8; outfile8 = fopen(d_file, "w");
	for (i = 0; i < 128; i++) { d_file[i] = "\0"; }strcpy(d_file, "D:/c_txt_data/");
	strcat(d_file, now_time); strcat(d_file, "-9.txt");
	FILE* outfile9; outfile9 = fopen(d_file, "w");
	for (i = 0; i < 128; i++) { d_file[i] = "\0"; }strcpy(d_file, "D:/c_txt_data/");
	strcat(d_file, now_time); strcat(d_file, "-10.txt");
	FILE* outfile10; outfile10 = fopen(d_file, "w");
	for (i = 0; i < 128; i++) { d_file[i] = "\0"; }strcpy(d_file, "D:/c_txt_data/");
	strcat(d_file, now_time); strcat(d_file, "-11.txt");
	FILE* outfile11; outfile11 = fopen(d_file, "w");
	for (i = 0; i < 128; i++) { d_file[i] = "\0"; }strcpy(d_file, "D:/c_txt_data/");
	strcat(d_file, now_time); strcat(d_file, "-12.txt");
	FILE* outfile12; outfile12 = fopen(d_file, "w");
	for (i = 0; i < 128; i++) { d_file[i] = "\0"; }strcpy(d_file, "D:/c_txt_data/");
	strcat(d_file, now_time); strcat(d_file, "-13.txt");
	FILE* outfile13; outfile13 = fopen(d_file, "w");
	for (i = 0; i < 128; i++) { d_file[i] = "\0"; }strcpy(d_file, "D:/c_txt_data/");
	strcat(d_file, now_time); strcat(d_file, "-14.txt");
	FILE* outfile14; outfile14 = fopen(d_file, "w");
	for (i = 0; i < 128; i++) { d_file[i] = "\0"; }strcpy(d_file, "D:/c_txt_data/");
	strcat(d_file, now_time); strcat(d_file, "-15.txt");
	FILE* outfile15; outfile15 = fopen(d_file, "w");
	if (fpRead == NULL) printf("filepath is null\n");
	fscanf(fpRead, "%*[^\n]");  //������һ�� 
	for (i = 0; i < length; i++)
	{
		fscanf(fpRead, "%s", store_data);
		if (count_number != 0)
		{
			if (count_number < 1025)
			{
				switch (channel_flag)
				{
				    case 0:fprintf(outfile0, "%s\n", store_data); count_number = count_number - 1; break;
					case 1:fprintf(outfile1, "%s\n", store_data); count_number = count_number - 1; break;
					case 2:fprintf(outfile2, "%s\n", store_data); count_number = count_number - 1; break;
					case 3:fprintf(outfile3, "%s\n", store_data); count_number = count_number - 1; break;
					case 4:fprintf(outfile4, "%s\n", store_data); count_number = count_number - 1; break;
					case 5:fprintf(outfile5, "%s\n", store_data); count_number = count_number - 1; break;
					case 6:fprintf(outfile6, "%s\n", store_data); count_number = count_number - 1; break;
					case 7:fprintf(outfile7, "%s\n", store_data); count_number = count_number - 1; break;
					case 8:fprintf(outfile8, "%s\n", store_data); count_number = count_number - 1; break;
					case 9:fprintf(outfile9, "%s\n", store_data); count_number = count_number - 1; break;
					case 10:fprintf(outfile10, "%s\n", store_data); count_number = count_number - 1; break;
					case 11:fprintf(outfile11, "%s\n", store_data); count_number = count_number - 1; break;
					case 12:fprintf(outfile12, "%s\n", store_data); count_number = count_number - 1; break;
					case 13:fprintf(outfile13, "%s\n", store_data); count_number = count_number - 1; break;
					case 14:fprintf(outfile14, "%s\n", store_data); count_number = count_number - 1; break;
					case 15:fprintf(outfile15, "%s\n", store_data); count_number = count_number - 1; break;
				    default:count_number = 0; channel_flag = 99; break;
				}
			}
			if (count_number == 1025)count_number = count_number - 1;
		}
		if (!stricmp(store_data, "[TDAT0,1024")) { channel_flag = 0; count_number = 1025; printf("%d\n", channel_flag);}
		if (!stricmp(store_data, "[TDAT1,1024")) { channel_flag = 1; count_number = 1025; printf("%d\n", channel_flag);}
		if (!stricmp(store_data, "[TDAT2,1024")) { channel_flag = 2; count_number = 1025; }
		if (!stricmp(store_data, "[TDAT3,1024")) { channel_flag = 3; count_number = 1025; }
		if (!stricmp(store_data, "[TDAT4,1024")) { channel_flag = 4; count_number = 1025; }
		if (!stricmp(store_data, "[TDAT5,1024")) { channel_flag = 5; count_number = 1025; }
		if (!stricmp(store_data, "[TDAT6,1024")) { channel_flag = 6; count_number = 1025; }
		if (!stricmp(store_data, "[TDAT7,1024")) { channel_flag = 7; count_number = 1025; }
		if (!stricmp(store_data, "[TDAT8,1024")) { channel_flag = 8; count_number = 1025; }
		if (!stricmp(store_data, "[TDAT9,1024")) { channel_flag = 9; count_number = 1025; }
		if (!stricmp(store_data, "[TDAT10,1024")) { channel_flag = 10; count_number = 1025; }
		if (!stricmp(store_data, "[TDAT11,1024")) { channel_flag = 11; count_number = 1025; }
		if (!stricmp(store_data, "[TDAT12,1024")) { channel_flag = 12; count_number = 1025; }
		if (!stricmp(store_data, "[TDAT13,1024")) { channel_flag = 13; count_number = 1025; }
		if (!stricmp(store_data, "[TDAT14,1024")) { channel_flag = 14; count_number = 1025; }
		if (!stricmp(store_data, "[TDAT15,1024")) { channel_flag = 15; count_number = 1025; }
	}
	fclose(fpRead);
	fclose(outfile0);
	fclose(outfile1);
	fclose(outfile2);
	fclose(outfile3);
	fclose(outfile4);
	fclose(outfile5);
	fclose(outfile6);
	fclose(outfile7);
	fclose(outfile8);
	fclose(outfile9);
	fclose(outfile10);
	fclose(outfile11);
	fclose(outfile12);
	fclose(outfile13);
	fclose(outfile14);
	fclose(outfile15);
}


void main(int argc, char *argv[])  
{
  long Errset=0;
  char command[4096]={0};
  char now_time[128] = {0};
  char command_fil_name[128] = "mpaname=";
  char command_save_spectra[128] = "savempa";
  char raw_file_path[128] = {0};
  char file_path_out[128] = "D:/c_txt_data";
  char store_data[128] = {0};
  int i;

  hDLL = LoadLibrary("DMCS8.DLL");
  if(hDLL){
      lpSet=(IMPAGETSETTING)GetProcAddress(hDLL,"GetSettingData");
	  lpNewStat=(IMPANEWSTATUS)GetProcAddress(hDLL,"GetStatus");
	  lpStat=(IMPAGETSTATUS)GetProcAddress(hDLL,"GetStatusData");
	  lpRun=(IMPARUNCMD)GetProcAddress(hDLL,"RunCmd");
	  lpCnt=(IMPAGETCNT)GetProcAddress(hDLL,"LVGetCnt");
	  lpRoi=(IMPAGETROI)GetProcAddress(hDLL,"LVGetRoi");
	  lpDat=(IMPAGETDAT)GetProcAddress(hDLL,"LVGetDat");
	  lpStr=(IMPAGETSTR)GetProcAddress(hDLL,"LVGetStr");
	  lpServ=(IMPASERVEXEC)GetProcAddress(hDLL,"ServExec");
	  lpGetDatSet=(IMPAGETDATSET)GetProcAddress(hDLL,"GetDatSetting");
	  lpGetMCSSet=(IMPAGETMCSSET)GetProcAddress(hDLL,"GetMCSSetting");
	  lpStart=(IMPASTART)GetProcAddress(hDLL,"Start");
	  lpHalt=(IMPAHALT)GetProcAddress(hDLL,"Halt");
	  lpContinue=(IMPACONTINUE)GetProcAddress(hDLL,"Continue");
	  lpErase=(IMPAERASE)GetProcAddress(hDLL,"Erase");
	  lpDigInOut=(IMPADIGINOUT)GetProcAddress(hDLL,"DigInOut");
  }
  else return;

  // Initialize parameters

  Errset = (*lpNewStat)(0);
  Errset = (*lpStat)(&Status, 0);
  PrintMpaStatus(&Status);

  //help();

// while(TRUE)
//	{
//		readstr(command, 80);
//		if (!stricmp(command, "fun"))
//		{
			for (i = 0; i < 128; i++) { raw_file_path[i] = "\0"; }
			strcpy(raw_file_path, "F:/mcs8x64/");
			get_time_now(now_time);
			filename_settting(now_time, command_fil_name, raw_file_path);
			printf("%s\n", command_fil_name);
			printf("%s\n", raw_file_path);
			run(command_fil_name);
			run(command_save_spectra);
			//if (run(command_fil_name)) break;
			//if (run(command_save_spectra)) break;
			read_data(raw_file_path, 20000, store_data, now_time);
//		}
//	}

  FreeLibrary(hDLL);

  return;
}


//66666666
