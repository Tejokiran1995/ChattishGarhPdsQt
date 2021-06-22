#include <stdio.h>
#ifndef ISO8583_H
#define ISO8583_H 

#define ISO_BITMAP       0
#define ISO_NUMERIC      1
#define ISO_ALPHANUMERIC 2
#define ISO_BINARY       3
#define DE48BITMAPINDEX 129
#define DE127BITMAPINDEX 146 

typedef struct {
	int format;
	int lenflds;
	int flds;
	const char *dsc;
} isodef;

typedef struct {
	char *fld[224];
	int fldlen[224];
} isomsg;

#define LEN_ERROR 2

typedef struct {
        //long int Recpt;
        //int AcqId;
        //int IssId;

        bool Venable;
        char Mtype[8];
        char Pan[32];
        char SenderPan[32];
        char SenderUID[16];
        char UID[16];
        char ProsCode[16];
        char TxnAmt[16];
        char StanNo[16];
        char TxnTime[16];
        char TxnDate[16];

        char TxnDateTime[16];

        //char ExpDate[16];
        char DateCapture[8];
        char PosEntryMode[8];
        //char MCC[8];
        //char funcode[8];
        char NII[8];
        char PosCondCode[8];

        //char Track2Data[64];
        char RRN[64];
        char BIN[16];
        //char Agent_id[16];
        char AuthIdResp[1024];
        char RespCode[8];
        char TermId[32];
        char skey[64];
        char hmac[512];
        char ac[16];
        char ci[16];
        char sa[16];
        char lk[1024];
        char field63[1024];
        char field126[1024];
        char field127[1024];
        char MerId[16];
        char PinBlk[64];
        char AdditAmt[12];
        char data[1024];
        char Name[64];
        char conCode[32];
        char settlmt[128];
        char *biometric;
        char currcode[8];

        char AcquirerInstId[16];
        char CardAccpIDCode[64];
        char CardAccpName[128];
        char AgentInfo[128];    //1024
        char InterfaceVerNo[16];

        unsigned char enc_pin_blk[32];
        unsigned char Track2Data[64];
        int Track2DataLen;

        int dataLen;
        int AcqTableId;
        int IssuerTableId;

        char cAccountNo[16];    //Added on 19062014

        char ToCardNo[32];

        char ActionDate[16];

        //int NII;
        char Field90[64];
}ISO;

//void iso8583_init(isomsg *m);
//void iso8583_dump(FILE *fp,isodef *d, isomsg *m);
//void iso8583_free(isomsg *m);
//int getMTI(unsigned char mesclass,unsigned char mestype,isomsg *);

//int HexByte2AsciiStr(const unsigned char  *hex_val, int hex_len,char *str);
//int AsciiStr2HexByte(const char *ascii_str, int ascii_len,unsigned char* hex_val, int *hex_len);

//char getHexChar (unsigned char bt);
//int set_field(int ActualFldNumber,char *FieldData,isodef *d,isomsg *m,int ICCLen);
//t set_field(int,char *,isodef *,isomsg *);
//int GetSubFieldIndexs(char *FieldNumber,int *ActualFldNumber,int *SubFldIndex);
//int setMTI(unsigned char mesclass,unsigned char mesfun,isomsg *m);

#endif /* iso8583.h */
