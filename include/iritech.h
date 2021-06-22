

struct iriTech_dev_discovery
{
        char status[10];
        char info[50];
        char cPath[50];
        char dPath[50];
};
typedef struct iriTech_dev_discovery iriTech_rd_status;

#if 1
struct iriTech_device_info1{
	char dc[64 + 1];
        char mi[64 + 1];
        char mc[4096 + 1];
        char dpId[64 + 5];
        char rdsId[64 + 5];
        char rdsVer[20 + 5];
};
typedef struct iriTech_device_info1 iriTech_info1;
#endif

struct iriTech_device_info {
        char dc[64 + 1];
        char mi[64 + 1];
        char mc[4096 + 1];
	char dpId[64 + 5]; 
        char rdsId[64 + 5];
        char rdsVer[20 + 5];
};
typedef struct iriTech_device_info iriTech_info;

struct iriTech_pid_data {
        char err_code[5];
        char err_info[512];
        char fCount[5];
        char fType[5];
        char iCount[5];
        char iType[5];
        char pCount[5];
        char pType[5];
        char nmPoints[50];
        char qScore[50];
        char ci[50];
        char Skey[2048];
        char Hmac[512];
        char type[5];
	char serialnumber[128];
        char pid[15000];
        iriTech_info iriTech_dev_data;
};
typedef struct iriTech_pid_data iriTech_pData;

struct iriTech_rd_capture
{
        int  eye_Count;
       // char IP_Type[1 + 1];
        //char PID_Type[1 + 1];
        char pidVer[3+1];
        char env[3+1];
        unsigned int  Timeout;
};
typedef struct iriTech_rd_capture iriTech_eye_capture;

