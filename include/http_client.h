
#include <time.h>

struct dev_discovery {
	char status[10];
	char info[50];
	char cPath[50];
	char dPath[50];
};
typedef struct dev_discovery rd_status;

struct rd_capture {
        int  Fp_Count;
        char FP_Type[1 + 1];
        char PID_Type[1 + 1];
	char pidVer[3+1];
	char env[3+1];
        unsigned int  Timeout;
};
typedef struct rd_capture fp_capture;

struct rd_capture_ekyc {
        int otp;
        char wadh[100];
        fp_capture fp_data;
};
typedef struct rd_capture_ekyc fp_capture_ekyc;

struct device_info {
	char dc[64 + 1];
        char mi[20 + 1];
        char mc[2048 + 1];
        char dpId[20 + 1];
        char rdsId[20 + 1];
        char rdsVer[20 + 1];
};
typedef struct device_info info;

struct pid_data {
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
	char Hmac[100];
	char type[5];
	char *pid;
	info dev_data;
};
typedef struct pid_data pData;

struct device_data {
        char scanner_id[50];
        char mac_id[30];
        char pinpad_id[30];
};
typedef struct device_data devData;

int rd_service_discovery(int sockfd, rd_status *rdStatus);

int rd_fp_capture(int sockfd, fp_capture *rd, pData *pid_info);
int rd_fp_capture_buffer(int sockfd,fp_capture *rd,char **resp_buffer);
int rd_fp_capture_xml(int sockfd,unsigned char *xmlReq,char **resp_buffer);

int rd_fp_capture_ekyc(int sockfd, fp_capture_ekyc *rd, pData *pid_info);
int rd_dev_info(int sockfd, info *dev_info);

int scanner_status();
int GetFPScannerDetails(unsigned char *serverip,int port ,unsigned char *projectName );
int get_device_details(devData *dev_details);
unsigned char *rd_base64_decode(const char *data, size_t input_length, size_t *output_length);
int wadh_data(char *string, char **wadh);

int Parse_RDService_Pidblock(pData *pidInfo, unsigned char*buf_resp_dev_info);
