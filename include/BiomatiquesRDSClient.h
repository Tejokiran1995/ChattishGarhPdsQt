#ifndef BIOMATIQUESRDSCLIENT_H
#define BIOMATIQUESRDSCLIENT_H

struct RDS_DISCOVERY
{
    int port;
    char status[10];
    char info[50];
    char capture_Path[50];
    char info_path[50];
    char raw_discovery[1000];

};


struct RDS_DEVICEINFO
{
    char dpid[100];
    char rdsid[50];
    char rdsver[50];
    char dc[40];
    char mi[20];
    char mc[2000];
    char raw_deviceInfo[3000];

};
struct RDS_CAPTURE
{
        char err_code[5];
        char err_info[100];
        char fCount[5];
        char fType[5];
        char iCount[5];
        char iType[5];
        char pCount[5];
        char pType[5];
        char nmPoints[50];
        char qScore[50];
        char ci[50];
        char Skey[500];
        char Hmac[500];
        char type[5];
        char pid[10000];
        struct RDS_DEVICEINFO dev_info;
        char raw_capture[11000];

};
int chekPort(int);

struct RDS_PIDOPTIONS
{

    char wadh[100];
    char env[3];
};

typedef struct RDS_DISCOVERY discoveryiris;
typedef struct RDS_DEVICEINFO dev_infoiris;
typedef struct RDS_CAPTURE captureiris;
typedef struct RDS_PIDOPTIONS pidopts;


#endif // BIOMATIQUESRDSCLIENT_H
