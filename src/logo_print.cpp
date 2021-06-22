
#include "include/logo_print.h"

unsigned char bmpbuf[30000]="";

extern "C"
{
#include "gl11.h"
}

void LogoPrint::PrintLogo(int StateCode)
{
    if(StateCode == 5) //Uttarakhand
    {
        if(!access("/mnt/jffs2/uklogo.bmp",F_OK))
        {
            qDebug() << "File Exists uklogo";
            createHexArray(1,"/mnt/jffs2/uklogo.bmp",true);
            takePrint(1);
        }
    }
    if(StateCode == 10) //Bihar
    {
        if(!access("/mnt/jffs2/biharlogo.bmp",F_OK))
        {
            qDebug() << "File Exists biharlogo";
            createHexArray(1,"/mnt/jffs2/biharlogo.bmp",true);
            takePrint(1);
        }
    }
    else if(StateCode == 12) //Arunachal Pradesh
    {
        if(!access("/mnt/jffs2/MzpdsLogo.bmp",F_OK))
        {
            qDebug() << "File Exists nlpdsLogo";
            createHexArray(1,"/mnt/jffs2/MzpdsLogo.bmp",true);
            takePrint(1);
        }
    }
    else if(StateCode == 13) //Nagaland
    {
        if(!access("/mnt/jffs2/nlpdsLogo.bmp",F_OK))
        {
            qDebug() << "File Exists nlpdsLogo";
            createHexArray(1,"/mnt/jffs2/nlpdsLogo.bmp",true);
            takePrint(1);
        }
    }
    else if(StateCode == 14) //Manipur
    {
        if(!access("/mnt/jffs2/ManipurLogo.bmp",F_OK))
        {
            qDebug() << "File Exists ManipurLogo";
            createHexArray(1,"/mnt/jffs2/ManipurLogo.bmp",true);
            takePrint(1);
        }
    }
    else if(StateCode == 15) //Mizoram
    {
        if(!access("/mnt/jffs2/MzpdsLogo.bmp",F_OK))
        {
            qDebug() << "File Exists MzpdsLogo";
            createHexArray(1,"/mnt/jffs2/MzpdsLogo.bmp",true);
            takePrint(1);
        }
    }
    else if(StateCode == 17) //Meghalaya
    {
        if(!access("/mnt/jffs2/MeghalayaLogo.bmp",F_OK))
        {
            qDebug() << "File Exists MeghalayaLogo";
            createHexArray(1,"/mnt/jffs2/MeghalayaLogo.bmp",true);
            takePrint(1);
        }
    }
    else if(StateCode == 31) //Lakshadweep
    {
        if(!access("/mnt/jffs2/MzpdsLogo.bmp",F_OK))
        {
            qDebug() << "File Exists nlpdsLogo";
            createHexArray(1,"/mnt/jffs2/MzpdsLogo.bmp",true);
            takePrint(1);
        }
    }
    else if(StateCode == 35) //Andaman & Nicobar
    {
        if(!access("/mnt/jffs2/MzpdsLogo.bmp",F_OK))
        {
            qDebug() << "File Exists nlpdsLogo";
            createHexArray(1,"/mnt/jffs2/MzpdsLogo.bmp",true);
            takePrint(1);
        }
    }
    else if((StateCode == 25) || (StateCode == 26) || (StateCode == 37)) //Daman & Diu,Ladhak
    {
        if(!access("/mnt/jffs2/MzpdsLogo.bmp",F_OK))
        {
            qDebug() << "File Exists DamanLogo";
            createHexArray(1,"/mnt/jffs2/MzpdsLogo.bmp",true);
            takePrint(1);
        }
    }
}

void LogoPrint::PrintBmpText()
{
    createHexArray(1,"/mnt/jffs2/sample.bmp",true);
    takeTextPrint(1);
}

int LogoPrint::getLogoTypeFromDB()
{
    return 0;
}

void LogoPrint::get_bmp_file_name(int iLogoType, char *szLogoFile)
{
    switch(iLogoType)
    {
    case LOGO_ANDHRA:       strcpy(szLogoFile, "Andhra_Bank.bmp");      break;
    case LOGO_BARODA:       strcpy(szLogoFile, "Bank_of_Baroda.bmp");   break;
    case LOGO_DHANALAKSHMI: strcpy(szLogoFile, "Dhanlakshmi_Bank.bmp"); break;
    case LOGO_ICICI:        strcpy(szLogoFile, "ICICI.bmp");            break;
    case LOGO_JANALAKSHMI:  strcpy(szLogoFile, "Janalakshmi.bmp");      break;
    case LOGO_KOTAK:        strcpy(szLogoFile, "Kotak.bmp");            break;
    case LOGO_PUNJAB:       strcpy(szLogoFile, "Punjab.bmp");           break;
    case LOGO_UNION:        strcpy(szLogoFile, "Union.bmp");            break;
    case LOGO_APGOVT:       strcpy(szLogoFile, "AP.bmp");               break;
    case LOGO_CORP:         strcpy(szLogoFile, "Corporation_Bank.bmp"); break;
    case LOGO_FINO:         strcpy(szLogoFile, "Fino.bmp");             break;
    case LOGO_INDIAN:       strcpy(szLogoFile, "Indian_Bank.bmp");      break;
    case LOGO_JK:           strcpy(szLogoFile, "jk.bmp");               break;
    case LOGO_KRISHNA:      strcpy(szLogoFile, "Krishna_Bank.bmp");     break;
    case LOGO_PURVANCHAL:   strcpy(szLogoFile, "Purvanchal.bmp");       break;
    case LOGO_UNITED:       strcpy(szLogoFile, "United_Bank.bmp");      break;
    case LOGO_AXIS:         strcpy(szLogoFile, "Axis.bmp");             break;
    case LOGO_DECCAN:       strcpy(szLogoFile, "Deccan.bmp");           break;
    case LOGO_GURGOAN:      strcpy(szLogoFile, "Gurgoan_Bank.bmp");     break;
    case LOGO_ING:          strcpy(szLogoFile, "ING_Vysya.bmp");        break;
    case LOGO_JK_GRAMIN:    strcpy(szLogoFile, "J_&_K.bmp");            break;
    case LOGO_NMB:          strcpy(szLogoFile, "NMB_Bank.bmp");         break;
    case LOGO_UCO:          strcpy(szLogoFile, "UCO_Bank.bmp");         break;

    default: strcpy(szLogoFile, "No_File");                        break;
    }
    return;
}

void LogoPrint::createHexArray(int fileno,char * filepath,bool bargCheck )
{
    FILE *fp,*fo,*fs;

    unsigned char   c;
    unsigned int    count;
    unsigned long     i;
    // ***Header***
    unsigned char   signature[2];
    unsigned long   filesize;
    unsigned long   reserved;
    unsigned long   dataoffset;

    // ***InfoHeader***
    unsigned long   size1;
    unsigned long   width;
    unsigned long   height;
    unsigned short  planes;
    unsigned short  bitcount;
    unsigned long   compression;
    unsigned long   imagesize;
    unsigned long   xpixelsperm;
    unsigned long   ypixelsperm;
    unsigned long   colorsused;
    unsigned long   colorsimportant;

    unsigned char   res;

    char catCmd[1024] = "";

    char bmp_filename[60] = "";
    char hex_filename[60] = "";
    char hexArr_filename[255] = "";

    //bmp file path
    sprintf(bmp_filename,"%s",filepath);

    sprintf(hex_filename,"%s%d%s","bmpToHex_",fileno,".hex");
    sprintf(hexArr_filename,"%s%d%s","bmpHexArr_",fileno,".h");

    fp=fopen(bmp_filename,"rb");
    if(fp==NULL)
    {
        puts("cannot open input file");
        return;
    }

    fo=fopen(hex_filename,"wb");
    if(fo==NULL)
    {
        puts("cannot create output file");
        return;
    }

    //printf("\n\nBITMAP FILE HEADER:\n");

    for(i=0;i<2;i++){
        fread(&c,sizeof(c),1,fp);
        signature[i]=c;
    }
    /*
        printf("\n      Signature:");
        for(i=0;i<2;i++){
                printf("%c",signature[i]);
        }
    */

    fread(&filesize,sizeof(filesize),1,fp);
    //printf("\n      FileSize:%lu",filesize);

    fread(&reserved,sizeof(reserved),1,fp);
    //printf("\n      Reserved:%lu",reserved);

    fread(&dataoffset,sizeof(dataoffset),1,fp);
    //printf("\n      DataOffset:%lu",dataoffset);

    //printf("\n\nBITMAP INFOHEADER:\n");

    fread(&size1,sizeof(size1),1,fp);
    //printf("\n      Size:%lu",size);

    fread(&width,sizeof(width),1,fp);
    //printf("\n      Width:%lu",width);

    fread(&height,sizeof(height),1,fp);
    //printf("\n      Height:%lu",height);

    fread(&planes,sizeof(planes),1,fp);
    //printf("\n      Planes:%u",planes);

    fread(&bitcount,sizeof(bitcount),1,fp);
    //printf("\n      BitCount:%u",bitcount);

    fread(&compression,sizeof(compression),1,fp);
    //printf("\n      Compression:%lu",compression);

    fread(&imagesize,sizeof(imagesize),1,fp);
    //printf("\n      ImageSize:%lu",imagesize);

    fread(&xpixelsperm,sizeof(xpixelsperm),1,fp);
    //printf("\n      XpixelsPerM:%lu",xpixelsperm);

    fread(&ypixelsperm,sizeof(ypixelsperm),1,fp);
    //printf("\n      YpixelsPerM:%lu",ypixelsperm);

    fread(&colorsused,sizeof(colorsused),1,fp);
    //printf("\n      ColorsUsed:%lu",colorsused);

    fread(&colorsimportant,sizeof(colorsimportant),1,fp);
    //printf("\n      ColorsImportant:%lu",colorsimportant);

    count=0;

    for(i=0;i<=imagesize;i++){
        fseek(fp,-i,SEEK_END);
        fread(&res,sizeof(res),1,fp);
        //read from 1st byte not from 0th byte
        if(i!=0){

            if(bargCheck == true)
            {
                //res =~res;
                fprintf(fo,"0x%x,",res);//Logo Printing From BMP
            }
            else
            {
                res =~res;
                fprintf(fo,"0x%x,",res);//Signature & MultiLingual
            }

            count++;
        }
        if(count>=16){
            fprintf(fo,"\n");
            count=0;
        }
    }

    fclose(fp);
    fclose(fo);

    sprintf(catCmd,"%s%s%s%s","cat ",hex_filename," >> ",hexArr_filename);

    fs=fopen(hexArr_filename,"w");
    if(fs==NULL)
    {
        puts("cannot create array buffer file - 1");
        return;
    }

    fprintf(fs,"unsigned char shot_bmp[]=\n{\n");

    fclose(fs);

    //printf("cat command is : %s\n", catCmd);
    system(catCmd);

    fs=fopen(hexArr_filename,"a");
    if(fs==NULL)
    {
        puts("cannot open array buffer file - 2");
        return;
    }

    fprintf(fs,"};");

    fclose(fs);

}

int LogoPrint::takePrint(int fileno)
{

    char bmp[30000]="",hexArr_filename[255]="";
    int ret,len,ret_bmp;

    sprintf(hexArr_filename,"%s%d%s","bmpHexArr_",fileno,".h");

    Check_bitmap(hexArr_filename);
    len=Get_bitmap(bmp,hexArr_filename);
    //    printf("Length of the Hex arr buff %d\n",len);

    ret_bmp = prepare_bmp_image((unsigned char*)bmp,len);
    //    printf("Lenght of the ret buff %d\n\n",ret_bmp);
    ret = prn_write_bmp(((unsigned char*)bmpbuf),ret_bmp);
    //    printf("Return of the prn_write_bmp %d\n",ret);

    return ret;
}

int LogoPrint::takeTextPrint(int fileno)
{

    char bmp[30000]="",hexArr_filename[255]="";
    int ret,len,ret_bmp;

    sprintf(hexArr_filename,"%s%d%s","bmpHexArr_",fileno,".h");

    Check_bitmap(hexArr_filename);
    len=Get_bitmap(bmp,hexArr_filename);
    //    printf("Length of the Hex arr buff %d\n",len);

    ret_bmp = prepare_bmp_text_image((unsigned char*)bmp,len);
    //    printf("Lenght of the ret buff %d\n\n",ret_bmp);
    ret = prn_write_bmp(((unsigned char*)bmpbuf),ret_bmp);
    //    printf("Return of the prn_write_bmp %d\n",ret);

    return ret;
}

int LogoPrint::Get_bitmap(char *buf,char *file_name)
{
    char ch;
    int val=0, i=0;
    FILE *fp;

    fp=fopen(file_name,"r");
    if(fp==NULL)
        return FILEERROR;

    while(!feof(fp)){
        ch=fgetc(fp);
        if(ch=='x')
        {
            fscanf(fp,"%x",&val);
            buf[i++]=val;
        }
    }
    fclose(fp);
    //fprintf(stdout,"Bitmap size: %d\n",i);
    return i;
}

int LogoPrint::prepare_bmp_image(unsigned char *data,int len)
{
    int i,j;
    for(i=0,j=0; i<len; i++){
        if(j >29800){
            return ERROR;
        }
        if(i%48 == 0)
            j=j+(48*4);
        bmpbuf[j+(47-(i%48))] = ~data[i];
        bmpbuf[j+48+(47-(i%48))] = ~data[i];
        bmpbuf[j+96+(47-(i%48))] = ~data[i];
        bmpbuf[j+48+96+(47-(i%48))] = ~data[i];
    }
    return j;
}

int LogoPrint::prepare_bmp_text_image(unsigned char *data,int len)
{
    int i,j;
    for(i=0,j=0; i<len; i++){
        if(j >29800){
            return ERROR;
        }
        if(i%48 == 0)
        {
            j=j+(48*4);
            //                        qDebug()<<"*******J Value = "<<j;
        }
        bmpbuf[j+(47-(i%48))] = ~data[i];
        bmpbuf[j+48+(47-(i%48))] = ~data[i];
        bmpbuf[j+96+(47-(i%48))] = ~data[i];
        bmpbuf[j+48+96+(47-(i%48))] = ~data[i];
    }
    return j;
}

int LogoPrint::Check_bitmap(char *file_name)
{
    char ch;
    char *n=NULL;
    int i=0;
    FILE *fp;
    char *buff;
    struct stat buf;

    //printf("file_name =%s\n",file_name);
    i = stat (file_name, &buf );
    if((i<0) || (buf.st_size == 0)){
        printf("empty file received\n");
        remove(file_name);
        printf("Bitmap is empty\n");
        return FILE_EMPTY_RCVD;
    }
    else {

        fp=fopen(file_name,"r");
        if(fp!=NULL){
            buff= (char *)malloc(buf.st_size+1);
            ch=fgetc(fp);
            while(!feof(fp)){
                buff[i]=ch;
                i++;
                buff[i]='\0';
                ch=fgetc(fp);
            }
            buff[i]='\0';
            fclose(fp);
            n=strchr(buff,'}');
            if(n!=NULL){
                free(buff);
                return SUCCESS;
            }
            else{
                remove(file_name);
                printf("Bitmap is invalid\n");
                free(buff);
                return INVALID_BITMAP;
            }
        }
        else{
            remove(file_name);
            printf("Bitmap doesn't exists\n");
            return FILENEXISTS;
        }
    }
    return SUCCESS;
}

//device id=1607156852=220
