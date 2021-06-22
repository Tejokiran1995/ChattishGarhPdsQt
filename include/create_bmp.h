#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include"tfmapi.h"
#include"tfmtypes.h"
#include"tfmerror.h"
#include <string.h>

typedef unsigned short   WORD;
typedef unsigned long    DWORD;
typedef long             LONG;
typedef unsigned char    BYTE;

//PT_CONNECTION conn;
//PT_STATUS     status;
//PT_WORD       imageWidth=0x100, imageHeight=0x168;
//PT_INFO       *pInfo;
//BYTE *pBitmap = NULL;


struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
}__attribute__((packed));
typedef struct tagBITMAPFILEHEADER BITMAPFILEHEADER;

struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
}__attribute__((packed));
typedef struct tagBITMAPINFOHEADER BITMAPINFOHEADER;

struct tagRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
}__attribute__((packed));
typedef struct tagRGBQUAD RGBQUAD;

struct bmpHEADER {
        BITMAPFILEHEADER bmpFileHeader;
        BITMAPINFOHEADER bmpInfoHeader;
        RGBQUAD bmpColors[256];
}__attribute__((packed));
typedef struct bmpHEADER BMPHEADER;

int create_bmp( char *pos,int i) ;
/*int create_bmp( char *pos) 
{

	FILE *fp=NULL;
	FILE *fw;
	char *fname;
	int i=0;
	struct stat sb;
	int retVal=0;
	int Data_len;

	char filename1[100]="";
	char filename[100]="";

	memset(filename1,0,sizeof(filename1));
	memset(filename,0,sizeof(filename));


	unsigned char *Data;

	sprintf(filename1,"/root/raw_images/%s.raw",pos);
	sprintf(filename,"/root/bmp_images/%s.bmp",pos);

	fp=fopen(filename1,"rb");

	if(fp==NULL)
		printf("cannot open the file\n");

	stat(filename1,&sb);

	Data=(BYTE*)malloc(sb.st_size);
	Data_len=sb.st_size;

	printf("DATA LEN=%d\n",Data_len);

	fread(Data,1,sb.st_size,fp);

	pBitmap = (BYTE*)malloc(sizeof(BMPHEADER) + imageWidth*imageHeight);
	memset(pBitmap, 0,( sizeof(BMPHEADER)+ imageWidth*imageHeight));
	((BMPHEADER*)pBitmap)->bmpFileHeader.bfType = 0x4d42;
	((BMPHEADER*)pBitmap)->bmpFileHeader.bfSize = sizeof(BMPHEADER) + imageWidth*imageHeight;
	((BMPHEADER*)pBitmap)->bmpFileHeader.bfOffBits = sizeof(BMPHEADER);
	((BMPHEADER*)pBitmap)->bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	((BMPHEADER*)pBitmap)->bmpInfoHeader.biWidth = imageWidth;
	((BMPHEADER*)pBitmap)->bmpInfoHeader.biHeight = imageHeight;
	((BMPHEADER*)pBitmap)->bmpInfoHeader.biPlanes = 1;
	((BMPHEADER*)pBitmap)->bmpInfoHeader.biBitCount = 8;
	((BMPHEADER*)pBitmap)->bmpInfoHeader.biCompression=0;
	((BMPHEADER*)pBitmap)->bmpInfoHeader.biSizeImage = imageWidth*imageHeight;
	for (i = 0; i < 256; i++)
	{
		((BMPHEADER*)pBitmap)->bmpColors[i].rgbBlue = (BYTE) i;
		((BMPHEADER*)pBitmap)->bmpColors[i].rgbGreen = (BYTE) i;
		((BMPHEADER*)pBitmap)->bmpColors[i].rgbRed = (BYTE) i;
	}
	for (i = 0; i < (int)imageHeight; i++)
	{
		memcpy(pBitmap + sizeof(BMPHEADER) + (i*imageWidth),(BYTE*)(Data + Data_len - (i+1)*imageWidth), (int)imageWidth);
	}
	i=0;

	if (NULL == (fw = fopen(filename, "wb")))
	{
		printf("Error: Failed to open file '%s'!\n", filename);
		retVal = -1;

	}
	fwrite(pBitmap, 1, ((BMPHEADER*)pBitmap)->bmpFileHeader.bfSize, fw);
	fclose(fw);
	free(pBitmap);
free(pos);
free(Data);
	printf("Image successfully stored in file '%s'.\n", filename);


	return 0;
}


*/
