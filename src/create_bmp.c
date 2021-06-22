#include "include/create_bmp.h"


PT_WORD       imageWidth=0x100, imageHeight=0x168;
BYTE *pBitmap = NULL;

int create_bmp( char *pos,int j) 
{
	FILE *fp=NULL;
	FILE *fw=NULL;
    //char *fname;
	int i=0;
	struct stat sb;
	int retVal=0;
	int Data_len;

	char filename1[300]="";
	char filename[300]="";

	memset(filename1,0,sizeof(filename1));
	memset(filename,0,sizeof(filename));

    system((char *)"mkdir -p /mnt/jffs2/bmp_images");

	unsigned char *Data;

    //sprintf(filename1,"/root/raw_images/%d.raw",j);
    sprintf(filename1,"/mnt/jffs2/raw_images/%d.raw",j);
    sprintf(filename,"/mnt/jffs2/bmp_images/%d.bmp",j);
	#ifdef DEBUG 
	system("ls -al filename1");
	system("ls -al filename");
	#endif
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
	free(Data);
	
	printf("Image successfully stored in file '%s'.\n", filename);

	return 0;
}



