#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_LINE_SIZE 2048
#define NUM_OF_LB_SOC 6
#define NUM_OF_MCU_LB 2

long g_slLbSizes[NUM_OF_LB_SOC] = {0};
long g_slMcuLbSizes[NUM_OF_MCU_LB] = {0};
const char *g_pscSocLbNames[NUM_OF_LB_SOC] = {
	"bootloader",
	"usr",
	"Applications",
	"GUI_App_Asset",
	"Movie",
	"Sound"
};
const char *g_pscMcuLbNames[NUM_OF_MCU_LB] = {
	"BIN_LB0",
	"BIN_LB1"
};

int makePdxSoC(void)
{
	int wk_siRes = 0;
	char wk_scCmd[64] = {0};
	int wk_siState = 0;

	do
	{
		switch(wk_siState)
		{
		case 0:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), "rm -f TDB_GP.pdx");
			break;
		case 1:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), "cp index_soc.xml index.xml");
			break;
		case 2:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), "zip -r TDB_GP.pdx index.xml ODX_Data");
			break;
		case 3:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), "rm -r index.xml ODX_Data");
			break;
		default:
			break;
		}
		if(system(&wk_scCmd[0]))
		{
			fprintf(stderr, "system(%s):%s\n", &wk_scCmd[0], strerror(errno));
			wk_siRes = -1;
			break;
		}
		else
		{
			wk_siState++;
		}
	}
	while(wk_siState < 4);

	return wk_siRes;
}
int makePdxMCU(void)
{
	int wk_siRes = 0;
	char wk_scCmd[64] = {0};
	int wk_siState = 0;

	do
	{
		switch(wk_siState)
		{
		case 0:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), "rm -f TDB.pdx");
			break;
		case 2:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), "cp index_mcu.xml index.xml");
			break;
		case 3:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), "zip -r TDB.pdx index.xml ODX_Data");
			break;
		case 4:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), "rm -r index.xml ODX_Data");
			break;
		default:
			break;
		}
		if(system(&wk_scCmd[0]))
		{
			fprintf(stderr, "system(%s):%s\n", &wk_scCmd[0], strerror(errno));
			wk_siRes = -1;
			break;
		}
		else
		{
			wk_siState++;
		}
	}
	while(wk_siState < 5);

	return wk_siRes;
}

int tarToGzBin(void)
{
	int wk_siRes = 0;
	char wk_scCmd[128] = {0};
	int wk_siI = 0;
	int wk_siState = 0;

	system("rm -rf ODX_Data");
	for(wk_siI = 0; wk_siI < NUM_OF_LB_SOC;)
	{
		switch(wk_siState)
		{
		case 0:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), "rm -rf  %s", g_pscSocLbNames[wk_siI]);
			break;
		case 1:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), "mkdir -p  %s", g_pscSocLbNames[wk_siI]);
			break;
		case 2:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), "tar xfv input_tar/%s.tar -C %s", g_pscSocLbNames[wk_siI], g_pscSocLbNames[wk_siI]);
			break;
		case 3:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), /* cd */"%s", g_pscSocLbNames[wk_siI]);
			break;
		case 4:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), "mkdir -p ../ODX_Data");
			break;
		case 5:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), "rm -f ../ODX_Data/%s.bin", g_pscSocLbNames[wk_siI]);
			break;
		case 6:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), "tar cfvz ../ODX_Data/%s.bin ./", g_pscSocLbNames[wk_siI]);
			break;
		case 7:
			snprintf(&wk_scCmd[0], sizeof(wk_scCmd), /* cd */"..");
			break;
		default:
			break;
		}
		if(wk_siState == 3 || wk_siState == 7)
		{
			if(chdir(&wk_scCmd[0]))
			{
				fprintf(stderr, "chdir(%s):%s\n", &wk_scCmd[0], strerror(errno));
				wk_siRes = -1;
				break;
			}
			else
			{
				/* keep */
			}
		}
		else if(system(&wk_scCmd[0]))
		{
			fprintf(stderr, "system(%s):%s\n", &wk_scCmd[0], strerror(errno));
			wk_siRes = -1;
			break;
		}
		else
		{
			/* keep */
		}
		if(wk_siState < 7)
		{
			wk_siState++;
		}
		else
		{
			wk_siState = 0;
			wk_siI++;
		}
	}
	return wk_siRes;
}

int bin2bin(void)
{
	int wk_siRes = 0;
	char wk_scCmd[128] = {0};
	int wk_siI = 0;

	system("rm -rf ODX_Data");
	system("mkdir -p ODX_Data");
	for(wk_siI = 0; wk_siI < NUM_OF_MCU_LB; wk_siI++)
	{
		if(!snprintf(&wk_scCmd[0], sizeof(wk_scCmd), "cp input_bin/%s.bin ODX_Data", g_pscMcuLbNames[wk_siI]))
		{
			fprintf(stderr, "snprintf,%s\n", strerror(errno));
			wk_siRes = -1;
		}
		else if(system(&wk_scCmd[0]))
		{
			fprintf(stderr, "system(%s):%s\n", &wk_scCmd[0], strerror(errno));
			wk_siRes = -1;
			break;
		}
		else
		{
			/* keep */
		}
	}
	return wk_siRes;
}

int getSocLbSizes(void)
{
	int wk_Res = 0;
	char wk_scName[64] = {0};
	FILE *wk_pstFp = NULL;
	int wk_siI = 0;

	for(wk_siI = 0; wk_siI < NUM_OF_LB_SOC; wk_siI++)
	{
		if(!snprintf(&wk_scName[0], sizeof(wk_scName), "ODX_Data/%s.bin", g_pscSocLbNames[wk_siI]))
		{
			perror("snprintf");
			wk_Res = -1;
			break;
		}
		else if(!(wk_pstFp = fopen(&wk_scName[0], "r")))
		{
			perror("fopen");
			wk_Res = -1;
			break;
		}
		else
		{
			if(fseek(wk_pstFp, 0L, SEEK_END))
			{
				perror("fseek");
				wk_Res = -1;
				break;
			}
			else
			{
				/* keep */
			}
			if((g_slLbSizes[wk_siI] = ftell(wk_pstFp)) == -1)
			{
				perror("ftell");
				wk_Res = -1;
				break;
			}
			else
			{
				/* keep */
			}
			fclose(wk_pstFp);
		}
	}
	return wk_Res;
}
int getMcuLbSizes(void)
{
	int wk_Res = 0;
	char wk_scName[64] = {0};
	FILE *wk_pstFp = NULL;
	int wk_siI = 0;

	for(wk_siI = 0; wk_siI < NUM_OF_MCU_LB; wk_siI++)
	{
		if(!snprintf(&wk_scName[0], sizeof(wk_scName), "ODX_Data/%s.bin", g_pscMcuLbNames[wk_siI]))
		{
			perror("snprintf");
			wk_Res = -1;
			break;
		}
		else if(!(wk_pstFp = fopen(&wk_scName[0], "r")))
		{
			perror("fopen");
			wk_Res = -1;
			break;
		}
		else
		{
			if(fseek(wk_pstFp, 0L, SEEK_END))
			{
				perror("fseek");
				wk_Res = -1;
				break;
			}
			else
			{
				/* keep */
			}
			if((g_slMcuLbSizes[wk_siI] = ftell(wk_pstFp)) == -1)
			{
				perror("ftell");
				wk_Res = -1;
				break;
			}
			else
			{
				/* keep */
			}
			fclose(wk_pstFp);
		}
	}
	return wk_Res;
}
int makePdxMcuMain(void)
{
	int wk_siRes = -1;
	FILE *wk_pstFpIn = NULL;
	FILE *wk_pstFpOut = NULL;
	char *wk_pscLine = NULL;
	char *wk_pscStrPos = NULL;
	size_t wk_ulLen = MAX_LINE_SIZE - 1;
	ssize_t wk_slLen = 0;
	int wk_siLbIndex = 0;

	if(bin2bin())
	{
		fprintf(stderr, "bin2bin error\n");
	}
	else if(getMcuLbSizes())
	{
		fprintf(stderr, "getMcuLbSizes error\n");
	}
	else if(!(wk_pstFpIn = fopen("adas31Ecu_mcu.odx-f.tmpl", "r")))
	{
		perror("fopen");
	}
	else if(!(wk_pstFpOut = fopen("ODX_Data/adas31Ecu.odx-f", "w+")))
	{
		perror("fopen");
		fclose(wk_pstFpIn);
	}
	else if(!(wk_pscLine = (char *)calloc(1, MAX_LINE_SIZE)))
	{
		perror("calloc");
		fclose(wk_pstFpIn);
		fclose(wk_pstFpOut);
	}
	else
	{
		do
		{
			if((wk_slLen = getline(&wk_pscLine, &wk_ulLen, wk_pstFpIn)) == -1)
			{
				break;
			}
			else
			{
				/* keep */
			}
			if((wk_pscStrPos = strstr(wk_pscLine, "<COMPRESSED-SIZE>")))
			{
				/* change this line */
				snprintf(wk_pscStrPos, wk_ulLen, "<COMPRESSED-SIZE>%ld</COMPRESSED-SIZE>\n", g_slMcuLbSizes[wk_siLbIndex]);
			}
			else if((wk_pscStrPos = strstr(wk_pscLine, "<UNCOMPRESSED-SIZE>")))
			{
				/* change this line */
				snprintf(wk_pscStrPos, wk_ulLen, "<UNCOMPRESSED-SIZE>%ld</UNCOMPRESSED-SIZE>\n", g_slMcuLbSizes[wk_siLbIndex]);
				wk_siLbIndex++;
			}
			else
			{
				/* don't touch this line */
			}
			fwrite(wk_pscLine, 1, strlen(wk_pscLine), wk_pstFpOut);
		}
		while(1);

		fclose(wk_pstFpIn);
		fclose(wk_pstFpOut);
		if(makePdxMCU())
		{
			/* keep */
		}
		else
		{
			wk_siRes = 0;
		}
	}
	return wk_siRes;
}
int makePdxSocMain(void)
{
	int wk_siRes = -1;
	FILE *wk_pstFpIn = NULL;
	FILE *wk_pstFpOut = NULL;
	char *wk_pscLine = NULL;
	char *wk_pscStrPos = NULL;
	ssize_t wk_slLen = 0;
	size_t wk_ulLen = MAX_LINE_SIZE - 1;
	int wk_siLbIndex = 0;
	int wk_siI = 0;

	if(tarToGzBin())
	{
		fprintf(stderr, "tarToGzBin error\n");
	}
	else if(getSocLbSizes())
	{
		fprintf(stderr, "getSocLbSizes error\n");
	}
	else if(!(wk_pstFpIn = fopen("adas31Ecu_soc.odx-f.tmpl", "r")))
	{
		perror("fopen");
	}
	else if(!(wk_pstFpOut = fopen("ODX_Data/adas31Ecu.odx-f", "w+")))
	{
		perror("fopen");
		fclose(wk_pstFpIn);
	}
	else if(!(wk_pscLine = (char *)calloc(1, MAX_LINE_SIZE)))
	{
		perror("calloc");
		fclose(wk_pstFpIn);
		fclose(wk_pstFpOut);
	}
	else
	{
		do
		{
			if((wk_slLen = getline(&wk_pscLine, &wk_ulLen, wk_pstFpIn)) == -1)
			{
				break;
			}
			else
			{
				/* keep */
			}
			if((wk_pscStrPos = strstr(wk_pscLine, "<COMPRESSED-SIZE>")))
			{
				/* change this line */
				snprintf(wk_pscStrPos, wk_ulLen, "<COMPRESSED-SIZE>%ld</COMPRESSED-SIZE>\n", g_slLbSizes[wk_siLbIndex]);
			}
			else if((wk_pscStrPos = strstr(wk_pscLine, "<UNCOMPRESSED-SIZE>")))
			{
				/* change this line */
				snprintf(wk_pscStrPos, wk_ulLen, "<UNCOMPRESSED-SIZE>%ld</UNCOMPRESSED-SIZE>\n", g_slLbSizes[wk_siLbIndex]);
				wk_siLbIndex++;
			}
			else
			{
				/* don't touch this line */
			}
			fwrite(wk_pscLine, 1, strlen(wk_pscLine), wk_pstFpOut);
		}
		while(1);

		fclose(wk_pstFpIn);
		fclose(wk_pstFpOut);

		if(makePdxSoC())
		{
			/* keep */
		}
		else
		{
			wk_siRes = 0;
		}

		for(wk_siI = 0; wk_siI < NUM_OF_LB_SOC; wk_siI++)
		{
			snprintf(wk_pscLine, wk_ulLen, "rm -rf %s", g_pscSocLbNames[wk_siI]);
			system(wk_pscLine);
		}

		free(wk_pscLine);
	}

	return wk_siRes;
}
int main(int argc, char *argv[])
{
	int wk_siRes = 0;

	if(makePdxSocMain())
	{
		fprintf(stderr, "make soc pdx fail!\n");
		wk_siRes = -1;
	}
	else
	{
		/* keep */
	}
	if(makePdxMcuMain())
	{
		fprintf(stderr, "make mcu pdx fail!\n");
		wk_siRes = -1;
	}
	else
	{
		/* keep */
	}
	return wk_siRes;
}
