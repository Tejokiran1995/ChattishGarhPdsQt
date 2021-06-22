/*
    Copyright (C) 2011 Geodesic Limited, Mumbai, India

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
    PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE
    FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
    OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

    Except as contained in this notice, the name of the Geodesic Limited shall not be used
    in advertising or otherwise to promote the sale, use or other dealings in this Software
    without prior written authorization from the Geodesic Limited.

*/
#include "aadhaar.h"
#include "uid_auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/valid.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include "auth_1.6.pb-c.h"
char uidbuff[15];
//extern int NFIQ[10];
int NFIQ[10];
unsigned char *template_data = NULL;
int template_len = 0;
extern int lenOutput;
//extern char fbuff[20];
char fbuff[20];
extern char * get_expiry_date(char *expiry);
extern unsigned char *uid_get_skey_data (unsigned char *key);
extern unsigned char *uid_get_skey_data_nic (unsigned char *key, int *outlen);
extern unsigned char *uid_get_aes_encrypted_data_nic(unsigned char *in, int inlen, unsigned char *key, int *outlen);
unsigned char enc_ses_key_pack[512];

///
char bufExpiryStr[12];
char *hmac,*pload;
char *pidb, *encryptedSessKey;
int enc_sessn_key_len;

///

char gRes_Pid_TimeStamp[32];

extern unsigned char *uid_get_aes_encrypted_data (unsigned char *in, int inlen, 
		 unsigned char *key);
extern int uid_encode_b64(unsigned char *in, int inlen, unsigned char *out, 
			int *outlen);

//extern int Fp_pos;
int Fp_pos;
///////////////
unsigned char *template1;
int aes_enc_data_len, enc_sessn_key, hmac_len;
unsigned char enc_ses_key_pack[512];
extern unsigned char* biometric_proto_details(char *puid, char *template_data, int template_len);


/////////////////////



xmlChar * do_digital_signature(char *xmlInpData, xmlChar **out);
//extern int lenOutput ;
//extern unsigned char *pOutputBuffer;
			
/**********************************
	Validate Aadhaar ID
**********************************/
int validate_uid( char *uId )
{
	int dMultTable[10][10] = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
		{1, 2, 3, 4, 0, 6, 7, 8, 9, 5},
		{2, 3, 4, 0, 1, 7, 8, 9, 5, 6},
		{3, 4, 0, 1, 2, 8, 9, 5, 6, 7},
		{4, 0, 1, 2, 3, 9, 5, 6, 7, 8},
		{5, 9, 8, 7, 6, 0, 4, 3, 2, 1},
		{6, 5, 9, 8, 7, 1, 0, 4, 3, 2},
		{7, 6, 5, 9, 8, 2, 1, 0, 4, 3},
		{8, 7, 6, 5, 9, 3, 2, 1, 0, 4},
		{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}};
	int permTable[8][10] = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
		{1, 5, 7, 6, 2, 8, 3, 0, 9, 4},
		{5, 8, 0, 3, 7, 9, 6, 1, 4, 2},
		{8, 9, 1, 6, 0, 4, 3, 5, 2, 7},
		{9, 4, 5, 3, 1, 2, 6, 8, 7, 0},
		{4, 2, 8, 6, 5, 7, 3, 9, 0, 1},
		{2, 7, 9, 3, 8, 0, 6, 4, 1, 5},
		{7, 0, 4, 6, 9, 1, 3, 2, 5, 8}};

	short int i=0, c=0;
    //printf("validate_uid: %d:", c);
	for (i=0; i < 12; i++) {
		short int ni=0, newC=0;

		if (isdigit(uId[11-i]))
			ni = uId[11-i] - '0';
		else {
			printf("\n");
			return(-1);
		}
		newC = dMultTable[c][permTable[i%8][ni]];
        //printf("%d:", newC);
		c = newC;
	}
	printf("\n");
	if (c == 0)
		return(0);
	return(-1);
}


char * parse_expiry_data(char *timestamp, char *expiry)
{
	
	char *strTimestamp=NULL;
    int mon, year, date;
	char m[][4] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

	strTimestamp = strtok(timestamp," ");
	if (strTimestamp != NULL) {
		int j;

		for (j=0; j < 12; j++) {
			if (strcmp(m[j],strTimestamp) == 0) {
				mon = j+1;
				break;
			}
		}
	}
	strTimestamp = strtok(NULL," ");
	if (strTimestamp != NULL)
		date = atoi(strTimestamp);
	strTimestamp = strtok(NULL," ");
	strTimestamp = strtok(NULL," ");
	if (strTimestamp != NULL)
		year = atoi(strTimestamp);
        sprintf(expiry, "%d%02d%02d", year, mon, date);
#ifdef DEBUG	
	printf("parse_expiry_data: value is :%s:\n", expiry);
#endif
	return expiry;
}

//commented on 2108 by CNU as not used //


unsigned char* pidxml_demographic(char *pname)
{
	xmlNodePtr root, demo, bios, n;
	xmlDocPtr doc;
	xmlChar *xmlbuff;
	int buffersize;
	struct tm * curr_tm;
        time_t curr_time;
	char buff[50];

        curr_time = time(NULL);
        curr_tm = localtime(&curr_time);
        sprintf(buff, "%04d-%02d-%02dT%02d:%02d:%02d", curr_tm->tm_year+1900, 
		curr_tm->tm_mon+1, curr_tm->tm_mday, 
		curr_tm->tm_hour, curr_tm->tm_min, curr_tm->tm_sec);

	doc = xmlNewDoc(NULL);
	root = xmlNewNode(NULL, "Pid");
	xmlSetProp(root, "ts", buff);
	xmlSetProp(root, "ver", "1.0");
	xmlSetProp(root, "xmlns", "http://www.uidai.gov.in/authentication/uid-auth-request-data/1.0");
	xmlDocSetRootElement(doc, root);

	demo = xmlNewNode(NULL, "Demo");
	n = xmlNewNode(NULL, "Pi");
	xmlSetProp(n, "ms", "E");
	xmlSetProp(n, "name", pname ? pname : "");
	xmlAddChild(demo, n);
	xmlAddChild(root, demo);

	xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);

	xmlFreeDoc(doc);
#if 1
    //printf("\n############################################################\n%s\n", xmlbuff);
#endif
	return (unsigned char*)xmlbuff;
}


/***************************************************
	 Generate PID Xml - Biometric

	* tmplData - Encoded Biometric 
***************************************************/


unsigned char* pidxml_biometric(char *tmplData)
{
    char file_path[50];
    xmlNodePtr root, demo, bios, n;
	xmlDocPtr doc;
	xmlChar *xmlbuff;
    int buffersize,i=0;
	struct tm * curr_tm;
        time_t curr_time;
	char buff[50];

        curr_time = time(NULL);
        curr_tm = localtime(&curr_time);
        sprintf(buff, "%04d-%02d-%02dT%02d:%02d:%02d", curr_tm->tm_year+1900, 
		curr_tm->tm_mon+1, curr_tm->tm_mday, 
		curr_tm->tm_hour, curr_tm->tm_min, curr_tm->tm_sec);

	doc = xmlNewDoc(NULL);
    root = xmlNewNode(NULL, "Rbd");
	xmlSetProp(root, "ts", buff);
    xmlSetProp(root, "ver", "1.0");
    //xmlSetProp(root, "xmlns", "http://www.uidai.gov.in/authentication/uid-auth-request-data/1.0");
	xmlDocSetRootElement(doc, root);

    /************  Added Based on Kerala XML 2.0 ********/
    /*demo = xmlNewNode(NULL, "Demo");
    n = xmlNewNode(NULL, "Pi");
    xmlSetProp(n, "ms", "E");
    xmlSetProp(n,"mv","");
    xmlSetProp(n, "name","");// pname ? pname : ""); // Added on Oct 22 for 2.0
    xmlSetProp(n,"lname","");
    xmlSetProp(n,"lmv","");
    xmlSetProp(n,"gender","M");
    xmlSetProp(n,"dob","19/07/1984");
    xmlSetProp(n,"dobt","");  // age=”” phone=”” email=””
    xmlSetProp(n,"age","");
    xmlSetProp(n,"phone","");
    xmlSetProp(n,"email","");
    xmlAddChild(demo, n);
    xmlAddChild(root, demo);*/
    /**********************************************/
    /*if( fdc[0] != '\0')
	{
		n = xmlNewNode(NULL, "Meta");
		xmlSetProp(n, "fdc", fdc);
        xmlAddChild(root, n);
    }*/  // To Be Removed After testing

    //

    bios = xmlNewNode(NULL, "Bios");

    for(i=1;i<11;i++)
    {
        memset(file_path,0,sizeof(file_path));
        sprintf(file_path,"/mnt/jffs2/TEST/iso%d.pk",i);
        file_path[strlen(file_path)]='\0';
        if(i==1)
        {
            memset(fbuff,0,sizeof(fbuff));
            strcpy(fbuff,"LEFT_LITTLE");
            fbuff[strlen(fbuff)];
        }
        else if(i==2)
        {
            memset(fbuff,0,sizeof(fbuff));
            strcpy(fbuff,"LEFT_RING");
            fbuff[strlen(fbuff)];
        }
        else if(i==3)
        {
            memset(fbuff,0,sizeof(fbuff));
            strcpy(fbuff,"LEFT_MIDDLE");
            fbuff[strlen(fbuff)];
        }
        else if(i==4)
        {
            memset(fbuff,0,sizeof(fbuff));
            strcpy(fbuff,"LEFT_INDEX");
            fbuff[strlen(fbuff)];
        }
        else if(i==5)
        {
            memset(fbuff,0,sizeof(fbuff));
            strcpy(fbuff,"LEFT_THUMB");
            fbuff[strlen(fbuff)];
        }
        else if(i==6)
        {
            memset(fbuff,0,sizeof(fbuff));
            strcpy(fbuff,"RIGHT_LITTLE");
            fbuff[strlen(fbuff)];
        }
        else if(i==7)
        {
            memset(fbuff,0,sizeof(fbuff));
            strcpy(fbuff,"RIGHT_RING");
            fbuff[strlen(fbuff)];
        }
        else if(i==8)
        {
            memset(fbuff,0,sizeof(fbuff));
            strcpy(fbuff,"RIGHT_MIDDLE");
            fbuff[strlen(fbuff)];
        }
        else if(i==9)
        {
            memset(fbuff,0,sizeof(fbuff));
            strcpy(fbuff,"RIGHT_INDEX");
            fbuff[strlen(fbuff)];
        }
        else if(i==10)
        {
            memset(fbuff,0,sizeof(fbuff));
            strcpy(fbuff,"RIGHT_THUMB");
            fbuff[strlen(fbuff)];
        }

    LoadDataFromFile(file_path, &template_data, &template_len);
    //Read_file(file_path,template_data,template_len);
    printf("\n lenOutput : %d\n",lenOutput);
    unsigned char *out;
    unsigned char nbuff[10];
    unsigned int outlen=0;
    out = (unsigned char *)malloc(sizeof(unsigned char)*1024);
    uid_encode_b64(template_data,template_len,out,&outlen);
    printf("\n Out :  %s %d\n",out,NFIQ[i-1]);
    //
    memset(nbuff,0,sizeof(nbuff));
    sprintf(nbuff,"%d",NFIQ[i-1]);
    nbuff[strlen(nbuff)]='\0';



	n = xmlNewNode(NULL, "Bio");
    xmlSetProp(n, "nfiq", nbuff);
    xmlSetProp(n, "na", "3");
    xmlSetProp(n, "pos", fbuff);
    xmlNodeSetContent(n, out);
	xmlAddChild(bios, n);
	xmlAddChild(root, bios);
   }
	xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);

	xmlFreeDoc(doc);
	printf("\n############################################################\n%s\n", xmlbuff);
	return (unsigned char*)xmlbuff;

}

/***************************************************
	 Generate Biometric  - Auth Xml
***************************************************/

int get_biometric_protobuff_pid(void **buff, int *len, char *template_data, int template_len)
{
    Pbuf__Pid pid = PBUF__PID__INIT; // Pid
    Pbuf__Demo demo = PBUF__DEMO__INIT; // demo
    Pbuf__Bios bios = PBUF__BIOS__INIT; //bios

    pid.demo = &demo;
    pid.bios = &bios;


     printf("Buffering PID ################################\n");

     struct tm * curr_tm;
     time_t curr_time;
     char date[50];

     curr_time = time(NULL);
     curr_tm = localtime(&curr_time);
     sprintf(date, "%04d-%02d-%02dT%02d:%02d:%02d", curr_tm->tm_year+1900,
             curr_tm->tm_mon+1, curr_tm->tm_mday,
             curr_tm->tm_hour, curr_tm->tm_min, curr_tm->tm_sec);
     printf("%s-----date",date);
     pid.ts = strdup(date);
     pid.ver = strdup("1.0");

     bios.n_bio = 1; //No of finger prints
     if( bios.n_bio > 0){
         bios.bio = (Pbuf__Bio **)malloc((sizeof(Pbuf__Bio **)) * bios.n_bio );
     }

     int i;
    
     for(i=0; i<bios.n_bio; i++) {
         printf("i = %d\n", i);
         Pbuf__Bio bio = PBUF__BIO__INIT;
         bios.bio[i] = &bio;
         printf("i = %d\n", i);

         printf("Fp_pos------->%d",Fp_pos);
         bio.posh =13;//0 //12; //TODO The value should be set to UNKNOWN
         bio.type = PBUF__BIO_TYPE__FMR; //Assumed

         (bio.content).len = template_len;
         (bio.content).data = template_data;
     }

     *len = pbuf__pid__get_packed_size(&pid);
     *buff = malloc(*len);
     pbuf__pid__pack(&pid,*buff);

     //TODO add all the elements
     free(pid.ver); free(pid.ts);

     if( bios.n_bio > 0){
         free(bios.bio);
     }

         return 0;
}

unsigned char * authxml_biometric(char *puid)
{
    char *tmplData;
    xmlNodePtr root, n;
    xmlDocPtr doc;
    xmlChar *preDigSignedXmlBuff, *digSignedXmlBuff;
    int buffersize;
    char *pidb, *encryptedSessKey=NULL, *pload, *hmac;
    unsigned char sessKey[32], shaHash[65];
    unsigned char txnId[32], devId[16];
    memset(txnId,0,sizeof(txnId));
    sprintf(txnId, "%d", rand());
    txnId[strlen(txnId)]='\0';
    strcpy(duid, puid);
    printf("\n\nUid value is :%s\n",duid);


    struct timeval tp;
    gettimeofday(&tp,0);
    time_t curr_time = tp.tv_sec;
    struct tm * curr_tm ;
    char buff[512];

    //curr_time = time(NULL);
    curr_tm = localtime(&curr_time);
    sprintf(buff, "AuthDemoClient:STGKSITM01:%04d%02d%02d%02d%02d%02d%03d", curr_tm->tm_year+1900,
            curr_tm->tm_mon+1, curr_tm->tm_mday,
            curr_tm->tm_hour, curr_tm->tm_min, curr_tm->tm_sec,tp.tv_usec/1000);

    printf("%s\n", buff);

    doc = xmlNewDoc("1.0");
    doc->encoding = xmlStrdup("UTF-8");
    doc->standalone = 1;

    root = xmlNewNode(NULL,"Auth");
    n = xmlNewNode(NULL,"Ver");
    xmlAddChild(root, n);
    xmlNodeSetContent(n,"1.0");

    n = xmlNewNode(NULL,"SubAUACode");
    xmlAddChild(root, n);
    xmlNodeSetContent(n,"KSITM");

    n = xmlNewNode(NULL,"DeviceId");
    xmlAddChild(root, n);
    xmlNodeSetContent(n,"ksitm");

    n = xmlNewNode(NULL,"ReqType");
    xmlAddChild(root, n);
    xmlNodeSetContent(n,"bfd");

    n = xmlNewNode(NULL,"Txn");
    xmlAddChild(root, n);
    xmlNodeSetContent(n,buff);//"AuthDemoClient:STGKSITM01:20140821011553492");

    n = xmlNewNode(NULL,"UID");
    xmlAddChild(root, n);
    xmlNodeSetContent(n,duid);


    xmlDocSetRootElement(doc, root);

    //    n = xmlNewNode(NULL, "Uses");
    //    xmlSetProp(n, "otp", "n"); //dpin as otp
    //    xmlSetProp(n, "pin", "n");
    //    xmlSetProp(n, "bt", "");
    //    xmlSetProp(n, "bio", "n");
    //    xmlSetProp(n, "pfa", "n");
    //    xmlSetProp(n, "pa", "n");
    //    xmlSetProp(n, "pi", "n");

    //    xmlAddChild(root, n);


    n = xmlNewNode(NULL, (const xmlChar *)"Meta");

    xmlSetProp(n, "lov", "560103");//560094"); //Location Value
    xmlSetProp(n, "lot", "P"); //Location type.
    //xmlSetProp(n, "idc", "NA"); //Iris device code
    xmlSetProp(n, "fdc", "NC"); //Fingerprint device code
    xmlSetProp(n, "pip", "127.0.0.1"); //Public IP address of the device
    xmlSetProp(n, "udc", "UIDAI:SampleClient"); //Unique Device Code
    xmlAddChild(root, n);
    /*root = xmlNewNode(NULL, "Auth");
    xmlSetProp(root, "xmlns", "http://www.uidai.gov.in/authentication/uid-auth-request/1.0");
    xmlSetProp(root, "ver", "1.5");
    xmlSetProp(root, "tid", "public");
    xmlSetProp(root, "ac", "public");
    xmlSetProp(root, "sa", "public");
    xmlSetProp(root, "lk", LICENCE_KEY_ONE);
    xmlSetProp(root, "uid", puid ? puid : "");
    txnId[strlen(txnId)]='\0';
    xmlSetProp(root, "txn", (const xmlChar *)txnId);*/
    /*root = xmlNewNode(NULL, "Bfd");
    xmlSetProp(root, "sa", "public");
    xmlSetProp(root, "txn", (const xmlChar *)buff);
    xmlSetProp(root, "ver", "1.6");
    xmlSetProp(root, "tid", "public");
    xmlSetProp(root, "uid", puid ? puid : "");
    xmlSetProp(root, "ac", "public");
    xmlSetProp(root, "lk", LICENCE_KEY_ONE);
    xmlSetProp(root, "xmlns", "http://www.uidai.gov.in/authentication/uid-auth-request/1.0");

    xmlDocSetRootElement(doc, root);*/

    /*n = xmlNewNode(NULL, "Uses");
    xmlSetProp(n, "otp", "n"); //dpin as otp
    xmlSetProp(n, "pin", "n");
    xmlSetProp(n, "bio", "n");
    xmlSetProp(n, "pa", "n");
    xmlSetProp(n, "pfa", "n");
    xmlSetProp(n, "pi", "n");
    xmlSetProp(n, "bt", "");
    xmlAddChild(root, n);*/

    //For 1.6
    //n = xmlNewNode(NULL, (const xmlChar *)"Meta");
    /*xmlSetProp(n, "udc", txnId); //Unique Device Code
    xmlSetProp(n, "fdc", "NC"); //Fingerprint device code
    xmlSetProp(n, "idc", "NA"); //Iris device code
    xmlSetProp(n, "pip", "NC"); //Public IP address of the device
    xmlSetProp(n, "lot", "P"); //Location type.
    xmlSetProp(n, "lov", "560103"); //Location Value
    xmlAddChild(root, n);*/

    /*xmlSetProp(n, "lov", "560103");//560094"); //Location Value
     xmlSetProp(n, "lot", "P"); //Location type.
      xmlSetProp(n, "idc", "NA"); //Iris device code
       xmlSetProp(n, "fdc", "NC"); //Fingerprint device code
       xmlSetProp(n, "pip", "127.0.0.1"); //Public IP address of the device
       xmlSetProp(n, "udc", "UIDAI:SampleClient"); //Unique Device Code
       xmlAddChild(root, n);*/

    char bufExpiryStr[12];
    char *expiry=NULL;
    n = xmlNewNode(NULL, "Skey");
    xmlAddChild(root, n);
    {
        bzero(bufExpiryStr, 12);
        expiry = get_expiry_date(expiry);
        parse_expiry_data(expiry, bufExpiryStr);
        free(expiry);
        xmlSetProp(n, "ci", bufExpiryStr);
    }

    encryptedSessKey = uid_get_skey_data(sessKey);
    xmlNodeSetContent(n, encryptedSessKey);

    printf("\n tmpldata : ---->%s\n",tmplData);
    pidb = pidxml_biometric(tmplData);
    n = xmlNewNode(NULL, "Data");
    xmlSetProp(n, "type", "X");
    xmlAddChild(root, n);
    pload = uid_get_aes_encrypted_data(pidb, strlen(pidb), sessKey);
    xmlNodeSetContent(n, pload);

    printf("\nhMac Sha 256:\n");
    int res=hMacSha256(pidb, strlen(pidb), shaHash);

    printf("uid get aes encrypted data");
    hmac = uid_get_aes_encrypted_data(shaHash, SHA256_LENGTH, sessKey);
    n = xmlNewNode(NULL, "Hmac");
    xmlAddChild(root, n);
    xmlNodeSetContent(n, hmac);
    free(encryptedSessKey);
    free(hmac);

    xmlDocDumpFormatMemory(doc, &preDigSignedXmlBuff, &buffersize, 1);

    printf("\n\n AuthXML - 1:\n");
    printf("\n############################################################\n%s\n", preDigSignedXmlBuff);

#ifdef DEBUG	

    char str[7][256];

    sprintf(str[0], "echo size of base64 plain template buff is %d >> %s", strlen(tmplData), LOG_FILE);//Data in plain XML format
    system(str[0]);
    sprintf(str[1], "echo size of base64 template XML buff is %d >> %s", strlen(pidb), LOG_FILE);//Data in plain XML format
    system(str[1]);
    sprintf(str[2], "echo size of encrypted template buff is %d >> %s", strlen(pload), LOG_FILE);//Data in encrypted XML format
    system(str[2]);
    sprintf(str[3], "echo size of final encrypted xmlbuff is %d >> %s", strlen(preDigSignedXmlBuff), LOG_FILE); //final Data in Encrypted XML
    system(str[3]);
    sprintf(str[4], "echo -------------------------------------------------- >> %s", LOG_FILE);
    system(str[4]);
#endif	
    free(pidb);
    free(pload);

#ifdef XML_SECURITY
    printf("\n############################################################\n");
    printf(" Digital Signature using XML Security\n\n");
    do_digital_signature(preDigSignedXmlBuff,&digSignedXmlBuff);
    if(preDigSignedXmlBuff)
        free(preDigSignedXmlBuff);
    xmlFreeDoc(doc);
    FILE *fp = fopen("/tmp/out.xml","w");
    fwrite(digSignedXmlBuff,1,strlen(digSignedXmlBuff),fp);
    fclose(fp);
    return ((unsigned char*)digSignedXmlBuff);
#else

    xmlFreeDoc(doc);
    return ((unsigned char*)preDigSignedXmlBuff);
#endif
}

/***************************************************
	 Generate Biometric  - Auth Xml with Fdc
***************************************************/
// commented this on 2108 by CNU as not used //

/***********/
unsigned char * authxml_biometric_with_fdc(char *puid, char *pfdc, char *tmplData)	
{
	xmlNodePtr root, n;
	xmlDocPtr doc;
	xmlChar *preDigSignedXmlBuff, *digSignedXmlBuff;
	int buffersize;
	char *pidb, *encryptedSessKey=NULL, *pload, *hmac;
	unsigned char sessKey[32], shaHash[65];
	unsigned char txnId[32], devId[16];

	sprintf(txnId, "%d", rand());
	strcpy(duid, puid);
	printf("\n\nUid value is :%s\n",duid);

	// Fdc attribute value
    if (( pfdc != NULL ) && (strlen(pfdc) != 0))
		strcpy(fdc, pfdc);
	else
		fdc[0]='\0';

	doc = xmlNewDoc("1.0");
	root = xmlNewNode(NULL, "Auth");
	xmlSetProp(root, "xmlns", "http://www.uidai.gov.in/authentication/uid-auth-request/1.0");
	xmlSetProp(root, "ver", "1.5");
	xmlSetProp(root, "tid", "public");
	xmlSetProp(root, "ac", "public");
	xmlSetProp(root, "sa", "public");
	xmlSetProp(root, "lk", LICENCE_KEY_ONE);
	xmlSetProp(root, "uid", puid ? puid : "");
	xmlSetProp(root, "txn", (const xmlChar *)txnId);
	xmlDocSetRootElement(doc, root);

	char bufExpiryStr[12];
	char *expiry=NULL;
	n = xmlNewNode(NULL, "Skey");
	xmlAddChild(root, n);
	{
		bzero(bufExpiryStr, 12);
		expiry = get_expiry_date(expiry);
		parse_expiry_data(expiry, bufExpiryStr);
		free(expiry);
		xmlSetProp(n, "ci", bufExpiryStr);
	}
	
	encryptedSessKey = uid_get_skey_data(sessKey);
	xmlNodeSetContent(n, encryptedSessKey);

	n = xmlNewNode(NULL, "Uses");
	xmlSetProp(n, "otp", "n"); //dpin as otp
	xmlSetProp(n, "pin", "n");
	xmlSetProp(n, "bio", "y");
	xmlSetProp(n, "pa", "n");
	xmlSetProp(n, "pfa", "n");
	xmlSetProp(n, "pi", "n");
	xmlSetProp(n, "bt", "FMR");
	xmlAddChild(root, n);

    //For 1.6
    n = xmlNewNode(NULL, (const xmlChar *)"Meta");
    xmlSetProp(n, "udc", txnId); //Unique Device Code
    xmlSetProp(n, "fdc", "NC"); //Fingerprint device code
    xmlSetProp(n, "idc", "NC"); //Iris device code
    xmlSetProp(n, "pip", "NA"); //Public IP address of the device
    xmlSetProp(n, "lot", "P"); //Location type.
    xmlSetProp(n, "lov", "560094"); //Location Value
    xmlAddChild(root, n);

	pidb = pidxml_biometric(tmplData);
	n = xmlNewNode(NULL, "Data");
	xmlAddChild(root, n);
	pload = uid_get_aes_encrypted_data(pidb, strlen(pidb), sessKey);
	xmlNodeSetContent(n, pload);

	int res=hMacSha256(pidb, strlen(pidb), shaHash);

	hmac = uid_get_aes_encrypted_data(shaHash, SHA256_LENGTH, sessKey);
	n = xmlNewNode(NULL, "Hmac");
	xmlAddChild(root, n);
	xmlNodeSetContent(n, hmac);
	free(encryptedSessKey);
	free(hmac);

	xmlDocDumpFormatMemory(doc, &preDigSignedXmlBuff, &buffersize, 1);

	printf("\n\n AuthXML - 1:\n");
	printf("\n############################################################\n%s\n", preDigSignedXmlBuff);

#ifdef DEBUG	
	
	char str[7][256];

        sprintf(str[0], "echo size of base64 plain template buff is %d >> %s", strlen(tmplData), LOG_FILE);//Data in plain XML format
        system(str[0]);
        sprintf(str[1], "echo size of base64 template XML buff is %d >> %s", strlen(pidb), LOG_FILE);//Data in plain XML format
        system(str[1]);
        sprintf(str[2], "echo size of encrypted template buff is %d >> %s", strlen(pload), LOG_FILE);//Data in encrypted XML format
        system(str[2]);
        sprintf(str[3], "echo size of final encrypted xmlbuff is %d >> %s", strlen(preDigSignedXmlBuff), LOG_FILE); //final Data in Encrypted XML
        system(str[3]);
        sprintf(str[4], "echo -------------------------------------------------- >> %s", LOG_FILE);
        system(str[4]);
#endif	
	free(pidb);	
	free(pload);

#ifdef XML_SECURITY
	printf("\n############################################################\n");
	printf(" Digital Signature using XML Security\n\n");
	do_digital_signature(preDigSignedXmlBuff,&digSignedXmlBuff);
	if(preDigSignedXmlBuff)
		free(preDigSignedXmlBuff);
	xmlFreeDoc(doc);
	FILE *fp = fopen("/tmp/out.xml","w");
	fwrite(digSignedXmlBuff,1,strlen(digSignedXmlBuff),fp);
	fclose(fp);
	return ((unsigned char*)digSignedXmlBuff);
#else

	xmlFreeDoc(doc);
	return ((unsigned char*)preDigSignedXmlBuff);
#endif
}
/*****/

/***************************************************
	 Generate Demographic  - Auth Xml
***************************************************/
// Commented on 2108 by cnu as func not used //s

/*
unsigned char* authxml_demographic_details(char *puid, char *pname)	
{
	xmlNodePtr root, n;
	xmlDocPtr doc;
	xmlChar *preDigSignedXmlBuff, *digSignedXmlBuff;
	int buffersize;
	char *pidb, *encryptedSessKey=NULL, *pload, *hmac;
	unsigned char sessKey[32];
	unsigned char txnId[32], devId[16];
	unsigned char shaHash[64];	

	sprintf(txnId, "%d", rand());
	strcpy(duid, puid);

	doc = xmlNewDoc("1.0");
	root = xmlNewNode(NULL, "Auth");
	xmlSetProp(root, "xmlns", "http://www.uidai.gov.in/authentication/uid-auth-request/1.0");
	xmlSetProp(root, "ver", "1.6");
	xmlSetProp(root, "tid", "public");
	xmlSetProp(root, "ac", "public");
	xmlSetProp(root, "sa", "public");
	xmlSetProp(root, "lk", LICENCE_KEY_ONE);
	xmlSetProp(root, "uid", puid ? puid : "");
	xmlSetProp(root, "txn", (const xmlChar *)txnId);
	xmlDocSetRootElement(doc, root);

	n = xmlNewNode(NULL, "Uses");
	xmlSetProp(n, "otp", "n");
	xmlSetProp(n, "pin", "n");
	xmlSetProp(n, "bio", "n");
	xmlSetProp(n, "pa", "n");
	xmlSetProp(n, "pfa", "n");
	xmlSetProp(n, "pi", "y");
	xmlAddChild(root, n);


    //For 1.6
    n = xmlNewNode(NULL, (const xmlChar *)"Meta");
    xmlSetProp(n, "udc", txnId); //Unique Device Code
    xmlSetProp(n, "fdc", "NC"); //Fingerprint device code
    xmlSetProp(n, "idc", "NC"); //Iris device code
    xmlSetProp(n, "pip", "NA"); //Public IP address of the device
    xmlSetProp(n, "lot", "P"); //Location type.
    xmlSetProp(n, "lov", "560094"); //Location Value
    xmlAddChild(root, n);

	char bufExpiryStr[12];
	char *expiry=NULL;
	n = xmlNewNode(NULL, "Skey");
	{
		bzero(bufExpiryStr, 12);
		expiry = get_expiry_date(expiry);
		parse_expiry_data(expiry, bufExpiryStr);
		free(expiry);
		xmlSetProp(n, "ci", bufExpiryStr);
	}
	xmlAddChild(root, n);
	
	encryptedSessKey = uid_get_skey_data(sessKey);
	xmlNodeSetContent(n, encryptedSessKey);

	pidb = pidxml_demographic(pname);
	n = xmlNewNode(NULL, "Data");
	xmlAddChild(root, n);
	pload =  uid_get_aes_encrypted_data(pidb, strlen(pidb), sessKey);
	xmlNodeSetContent(n, pload);

	int res=hMacSha256(pidb, strlen(pidb), shaHash);

	hmac = uid_get_aes_encrypted_data(shaHash, SHA256_LENGTH, sessKey);
	n = xmlNewNode(NULL, "Hmac");
	xmlAddChild(root, n);
	xmlNodeSetContent(n, hmac);
	free(encryptedSessKey);
	free(hmac);

	xmlDocDumpFormatMemory(doc, &preDigSignedXmlBuff, &buffersize, 1);
	free(pidb);
	free(pload);

#ifdef DEBUG
	printf("\n\n AuthXML - 1:\n");
	printf("\n############################################################\n%s\n", preDigSignedXmlBuff);
#endif

#ifdef XML_SECURITY
	printf("\nDigital Signature using XML Security\n\n");
	do_digital_signature(preDigSignedXmlBuff,&digSignedXmlBuff);
	free(preDigSignedXmlBuff);
	xmlFreeDoc(doc);
#if 1
   // send_data(digSignedXmlBuff);
#endif

	return((unsigned char*)digSignedXmlBuff);
#endif	

	xmlFreeDoc(doc);
	return((unsigned char*)preDigSignedXmlBuff);
}

*/

/***************************************************
	 Generate Biometric-Protobuff  - Auth Xml
***************************************************/

unsigned char* biometric_proto_details_nic(char *puid, char *template_data, int template_len, int ScannerFlag)
{
    printf("In Biometric protodetails");

    unsigned char sessKey[32];
    unsigned char shaHash[64];
    int i = 0;
//    char bufExpiryStr[12];
    char *expiry=NULL;

    bzero(bufExpiryStr, 12);
    expiry = get_expiry_date(expiry);
    if(strcmp(expiry, "-2") == 0)
    {
        return "-2";
    }
    parse_expiry_data(expiry, bufExpiryStr);
    free(expiry);
    printf("\n expiry date = %s\n",bufExpiryStr);

//    int enc_sessn_key_len = 0;
    encryptedSessKey = uid_get_skey_data_nic(sessKey, &enc_sessn_key_len);



//    printf("The length of out rsa len is : %d\n\n",enc_sessn_key_len);
//    for(i=0; i<enc_sessn_key_len+6; i++)
//    {
//        fprintf(stdout, "%02X ", encryptedSessKey[i]);
//    }
//    fprintf(stdout, "\n###\n");



    int pid_len = 0;

    struct tm * curr_tm;
    time_t curr_time;
    curr_time = time(NULL);
    curr_tm = localtime(&curr_time);
    char date[32];
    memset(date, 0, sizeof(date));
    sprintf(date, "%04d-%02d-%02dT%02d:%02d:%02d",curr_tm->tm_year+1900,curr_tm->tm_mon+1,curr_tm->tm_mday,curr_tm->tm_hour,curr_tm->tm_min,curr_tm->tm_sec);

    memset(gRes_Pid_TimeStamp, 0, sizeof(gRes_Pid_TimeStamp));
    strcpy(gRes_Pid_TimeStamp, date);

    char cType[8];
    memset(cType, 0, sizeof(cType));
    if(ScannerFlag == 1)
        strcpy(cType, "FMR");
    else if(ScannerFlag == 2)
        strcpy(cType, "IIR");

    //char pidbuff[10240];
    fprintf(stdout, "\n=======================\n");

    char *pidbuff = (char *)malloc(strlen((char *)template_data)+2046);
    if (pidbuff == 0)
    {
        printf("ERROR: Out of memory\n");
        return "-1";
    }

    memset(pidbuff, 0, sizeof(pidbuff));
    sprintf(pidbuff, "<Pid ts=\"%s\" ver=\"1.0\"><Bios><Bio type=\"%s\" posh=\"UNKNOWN\">%s</Bio></Bios></Pid>", date, cType, template_data);

//    fprintf(stdout, "\npidbuff = %s\n", pidbuff);

//    int aes_enc_data_len = 0;
    pload = uid_get_aes_encrypted_data_nic((unsigned char *)pidbuff, strlen(pidbuff), (unsigned char *)sessKey, &aes_enc_data_len);

    printf("\n\nproto-buff For  aes encrypted data len  = %d\n",aes_enc_data_len);
//    printf("=========AES Encryption===========\n");
//    for(i=0;i<aes_enc_data_len;i++)
//        fprintf(stdout,"%02x ",pload[i]);
//    fprintf(stdout,"\n");

    memset(shaHash, 0, 64);
    int res=hMacSha256(pidbuff, strlen(pidbuff), shaHash);

//    int hmac_len = 0;
    hmac = uid_get_aes_encrypted_data_nic(shaHash, SHA256_LENGTH, sessKey,&hmac_len);
    printf("\nThe Length of HMac is: %d\n", hmac_len);
    printf("=========HMac is===============\n");
//    fprintf(stdout, "%s\n", hmac);
//    for(i=0; i<hmac_len; i++)
//        fprintf(stdout, "%02X ", hmac[i]);
//    fprintf(stdout,"\n");

    return pload;
}
unsigned char* biometric_proto_details_nic_fusion(char *puid, char *template_data, char *template_data2, int ScannerFlag)
{
    printf("In Biometric protodetails");

    printf("\n%s\n#########\n", template_data2);
    unsigned char sessKey[32];
    unsigned char shaHash[64];
    int i = 0;
//    char bufExpiryStr[12];
    char *expiry=NULL;

    bzero(bufExpiryStr, 12);
    expiry = get_expiry_date(expiry);
    if(strcmp(expiry, "-2") == 0)
    {
        return "-2";
    }
    parse_expiry_data(expiry, bufExpiryStr);
    free(expiry);
    printf("\n expiry date = %s\n",bufExpiryStr);

//    int enc_sessn_key_len = 0;
    encryptedSessKey = uid_get_skey_data_nic(sessKey, &enc_sessn_key_len);



//    printf("The length of out rsa len is : %d\n\n",enc_sessn_key_len);
//    for(i=0; i<enc_sessn_key_len+6; i++)
//    {
//        fprintf(stdout, "%02X ", encryptedSessKey[i]);
//    }
//    fprintf(stdout, "\n###\n");



    int pid_len = 0;

    struct tm * curr_tm;
    time_t curr_time;
    curr_time = time(NULL);
    curr_tm = localtime(&curr_time);
    char date[32];
    memset(date, 0, sizeof(date));
    sprintf(date, "%04d-%02d-%02dT%02d:%02d:%02d",curr_tm->tm_year+1900,curr_tm->tm_mon+1,curr_tm->tm_mday,curr_tm->tm_hour,curr_tm->tm_min,curr_tm->tm_sec);

    memset(gRes_Pid_TimeStamp, 0, sizeof(gRes_Pid_TimeStamp));
    strcpy(gRes_Pid_TimeStamp, date);

    char cType[8];
    memset(cType, 0, sizeof(cType));
    if(ScannerFlag == 1)
        strcpy(cType, "FMR");
    else if(ScannerFlag == 2)
        strcpy(cType, "IIR");

    //char pidbuff[10240];
    fprintf(stdout, "\n=======================\n");

    char *pidbuff = (char *)malloc(strlen((char *)template_data)+strlen((char *)template_data2)+2046+2046);
    if (pidbuff == 0)
    {
        printf("ERROR: Out of memory\n");
        return "-1";
    }

    memset(pidbuff, 0, sizeof(pidbuff));
    sprintf(pidbuff, "<Pid ts=\"%s\" ver=\"1.0\"><Bios><Bio type=\"%s\" posh=\"UNKNOWN\">%s</Bio><Bio type=\"%s\" posh=\"UNKNOWN\">%s</Bio></Bios></Pid>", date, cType, template_data, cType, template_data2);

//    fprintf(stdout, "\n***********\n\npidbuff = %s\n\n*******\n", pidbuff);

//    int aes_enc_data_len = 0;
    pload = uid_get_aes_encrypted_data_nic((unsigned char *)pidbuff, strlen(pidbuff), (unsigned char *)sessKey, &aes_enc_data_len);

    printf("\n\nproto-buff For  aes encrypted data len  = %d\n",aes_enc_data_len);
//    printf("=========AES Encryption===========\n");
//    for(i=0;i<aes_enc_data_len;i++)
//        fprintf(stdout,"%02x ",pload[i]);
//    fprintf(stdout,"\n");

    memset(shaHash, 0, 64);
    int res=hMacSha256(pidbuff, strlen(pidbuff), shaHash);

//    int hmac_len = 0;
    hmac = uid_get_aes_encrypted_data_nic(shaHash, SHA256_LENGTH, sessKey,&hmac_len);
    printf("\nThe Length of HMac is: %d\n", hmac_len);
    printf("=========HMac is===============\n");
//    fprintf(stdout, "%s\n", hmac);
//    for(i=0; i<hmac_len; i++)
//        fprintf(stdout, "%02X ", hmac[i]);
//    fprintf(stdout,"\n");

    return pload;
}



unsigned char * bfd_proto_details_nic(char *puid, char *rbd_template_data, int rbd_template_len)
{
    unsigned char sessKey[32];
    unsigned char shaHash[64];
    int i = 0;
    char *expiry=NULL;

    bzero(bufExpiryStr, 12);
    expiry = get_expiry_date(expiry);
    if(strcmp(expiry, "-2") == 0)
    {
        return "-2";
    }

    parse_expiry_data(expiry, bufExpiryStr);
    free(expiry);
    printf("\nBFD expiry date = %s\n",bufExpiryStr);

//    int enc_sessn_key_len = 0;
    encryptedSessKey = uid_get_skey_data_nic(sessKey, &enc_sessn_key_len);

//    int aes_enc_data_len = 0;
    pload = uid_get_aes_encrypted_data_nic((unsigned char *)rbd_template_data, rbd_template_len, (unsigned char *)sessKey, &aes_enc_data_len);

    printf("\n\nBFD For AES Encrypted Data Len  = %d\n", aes_enc_data_len);

    printf("\nBFD Pload = ");
    for(i=0;i<aes_enc_data_len;i++)
        fprintf(stdout,"%02x ",pload[i]);
    fprintf(stdout,"\n");

    memset(shaHash, 0, 64);
    int res=hMacSha256(rbd_template_data, rbd_template_len, shaHash);

//    int hmac_len = 0;
    hmac = uid_get_aes_encrypted_data_nic(shaHash, SHA256_LENGTH, sessKey,&hmac_len);
    printf("\nThe Length of HMac is: %d\n", hmac_len);
    printf("=========HMac is===============\n");
//    fprintf(stdout, "%s\n", hmac);

//    for(i=0; i<hmac_len; i++)
//        fprintf(stdout, "%02X ", hmac[i]);
//    fprintf(stdout,"\n");

    return pload;
}


unsigned char* biometric_proto_details(char *puid, char *template_data, int template_len)	
{
    printf("In Biometric protodetails");

    xmlNodePtr root, n;
    xmlDocPtr doc;
    xmlChar *preDigSignedXmlBuff, *digSignedXmlBuff;
    int buffersize;

	char *pidb, *encryptedSessKey=NULL, *pload, *hmac;

    unsigned char sessKey[32];
	unsigned char txnId[32], devId[16];
	unsigned char shaHash[64];	
    printf("\n\n in Biometric function\n\n");
    sprintf(txnId, "%d", rand());
	strcpy(duid, puid);
    printf("txnId====== %s\n",txnId);

    struct timeval tp;
    gettimeofday(&tp,0);
    time_t curr_time = tp.tv_sec;
    struct tm * curr_tm ;
    char buff[512];

        //curr_time = time(NULL);
        curr_tm = localtime(&curr_time);
        sprintf(buff, "AuthDemoClient:STGKSITM01:%04d%02d%02d%02d%02d%02d%03d", curr_tm->tm_year+1900,
        curr_tm->tm_mon+1, curr_tm->tm_mday,
        curr_tm->tm_hour, curr_tm->tm_min, curr_tm->tm_sec,tp.tv_usec/1000);


	doc = xmlNewDoc("1.0");
	root = xmlNewNode(NULL, "Auth");
    xmlSetProp(root, "sa", "public");
    xmlSetProp(root, "txn", (const xmlChar *)buff);
    xmlSetProp(root, "ver", "1.6");
    xmlSetProp(root, "tid", "public");
    xmlSetProp(root, "uid", puid ? puid : "");
    xmlSetProp(root, "ac", "public");
    //xmlSetProp(root, "xmlns", "http://www.uidai.gov.in/authentication/uid-auth-request/1.0");
    xmlSetProp(root, "lk", LICENCE_KEY_ONE);


	xmlDocSetRootElement(doc, root);

	n = xmlNewNode(NULL, "Uses");
	xmlSetProp(n, "otp", "n");
	xmlSetProp(n, "pin", "n");
    xmlSetProp(n, "bio", "y");
    xmlSetProp(n, "pfa", "n");
	xmlSetProp(n, "pa", "n");
    xmlSetProp(n, "pi", "n");
    xmlSetProp(n, "bt", "FMR");
	xmlAddChild(root, n);


    //For 1.6
    n = xmlNewNode(NULL,(const xmlChar *)"Meta");
    xmlSetProp(n, "lov", "560103");//560094"); //Location Value
     xmlSetProp(n, "lot", "P"); //Location type.
      xmlSetProp(n, "idc", "NA"); //Iris device code
       xmlSetProp(n, "fdc", "NC"); //Fingerprint device code
       xmlSetProp(n, "pip", "127.0.0.1"); //Public IP address of the device
       xmlSetProp(n, "udc", "UIDAI:SampleClient"); //Unique Device Code


    xmlAddChild(root, n);

	char bufExpiryStr[12];
	char *expiry=NULL;
	n = xmlNewNode(NULL, "Skey");
	{
		bzero(bufExpiryStr, 12);
		expiry = get_expiry_date(expiry);
		parse_expiry_data(expiry, bufExpiryStr);
		free(expiry);
		xmlSetProp(n, "ci", bufExpiryStr);
	}
	xmlAddChild(root, n);
	
	encryptedSessKey = uid_get_skey_data(sessKey);
	xmlNodeSetContent(n, encryptedSessKey);
    
    int pid_len;

	get_biometric_protobuff_pid(&pidb, &pid_len, template_data, template_len);

	n = xmlNewNode(NULL, "Data");
    xmlSetProp(n, "type", "P"); //XML/protobuff
	xmlAddChild(root, n);
	pload =  uid_get_aes_encrypted_data(pidb, pid_len, sessKey);
	xmlNodeSetContent(n, pload);

	int res=hMacSha256(pidb, pid_len, shaHash);

	hmac = uid_get_aes_encrypted_data(shaHash, SHA256_LENGTH, sessKey);
	n = xmlNewNode(NULL, "Hmac");
	xmlAddChild(root, n);
	xmlNodeSetContent(n, hmac);
    free(encryptedSessKey);
    free(hmac);

	xmlDocDumpFormatMemory(doc, &preDigSignedXmlBuff, &buffersize, 1);
	free(pidb);
	free(pload);
    printf("\n############################################################\n%s\n", preDigSignedXmlBuff);

#ifdef DEBUG
	printf("\n\n AuthXML - 1:\n");
	printf("\n############################################################\n%s\n", preDigSignedXmlBuff);
#endif

#ifdef XML_SECURITY
	printf("\nDigital Signature using XML Security\n\n");
	do_digital_signature(preDigSignedXmlBuff,&digSignedXmlBuff);
	free(preDigSignedXmlBuff);
	xmlFreeDoc(doc);
//#if 1
//    send_data(digSignedXmlBuff);
//#endif

	return((unsigned char*)digSignedXmlBuff);
#endif

	xmlFreeDoc(doc);
    return((unsigned char*)preDigSignedXmlBuff);
    //return((unsigned char*)digSignedXmlBuff);
}

// commented on 2108 by CNU, as func not in use //
/*
int uid_auth_demographic_details(char *puid, char *name)
{
	unsigned char *authb=NULL;
	int res;
	
	authb = authxml_demographic_details(puid, name);
	puts(authb);
	free(authb);
	
	return res;
}
*/
#if 1
  // For Testing
//int main()
int aadhar()
{
	//authxml_demographic_details("999999990019","Shivshankar Choudhury");

    //biometric_proto_details("999999990019", pOutputBuffer, lenOutput);

    return 0;
}

#endif

