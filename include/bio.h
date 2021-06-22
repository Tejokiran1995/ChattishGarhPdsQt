
#define BIO_SUPREMA_IN_ISO_FORMAT 1
#define BIO_SUPREMA 2


// conversion types
#define RAW2WSQ			1
#define WSQ2RAW			2
#define ISO2BIR			3
#define BIR2ISO			4


int bio_init(int sensor_settings) ;
void bio_uninit(int fd) ;
int bio_change_user_security_level(int fd, unsigned long user_id, int security_level) ;
int bio_enroll_by_scan(int fd, unsigned long *userid, int sensor_timeout_period, int quality) ;
int bio_enroll_by_image(int fd, unsigned long *userid, unsigned char *image_data, int image_len, int quality) ;
int bio_enroll_by_template(int fd, unsigned long *userid, unsigned char *template_data, int template_len);
int bio_read_last_scanned_image(int fd, char *image_file_name, short int file_type) ; 
int bio_read_last_scanned_template(int fd, char *template_file_name) ;
int bio_read_template(int fd, unsigned long *userid, char *template_file_name) ;
int bio_scan_template(int fd, char *template_file_name, int sensor_timeout_period, int quality) ;
int bio_scan_image(int fd, char *image_file_name, short int file_type, int sensor_timeout_period, int quality) ;
int bio_scan_image_gray(int fd, char *image_file_name, short int file_type, int sensor_timeout_period, int quality); 
int bio_scan_template_and_image(int fd,char *template_file_name,char *image_file_name,int sensor_timeout_period,int quality) ;
int bio_verify_by_scan(int fd, unsigned long *userid, int sensor_timeout_period) ;
int bio_verify_by_template_data(int fd, unsigned long *userid,  unsigned char* template_data, int template_len) ;
int bio_verify_host_template_file_by_scan(int fd, char *template_file_name, int sensor_timeout_period) ;
int bio_verify_host_template_data_by_scan(int fd,unsigned char *template_data, int template_len, int sensor_timeout_period) ;
int bio_verify_host_template_data_by_scan_360_degree(int fd, char *template_data, int template_len, int sensor_timeout_period) ;
int  bio_verify_multiple_host_template_data_by_scan(int fd, unsigned char **template_data, int *template_len, int nTemplate, int sensor_timeout_period) ;
int  bio_verify_multiple_host_template_data_by_scan_360_degree(int fd, unsigned char **template_data, int *template_len, int nTemplate, int sensor_timeout_period);
int bio_identify_by_scan(int fd, unsigned long *userid, int sensor_timeout_period) ;
int bio_identify_by_template_file(int fd, char *template_file_name, unsigned long *userid) ;
int bio_identify_by_template_data(int fd, unsigned long *userid,  unsigned char* template_data, int template_len) ;
int bio_delete_template(int fd, unsigned long * userid);
int bio_delete_multiple_templates(int fd, unsigned long *start_id, unsigned long *end_id) ;
int bio_delete_all_templates(int fd) ;
int bio_verify_host_image_data_by_scan(int fd, unsigned char *image_data, int height, int width, int sensor_timeout_period) ;
int bio_enroll_by_non_suprema_image(int fd, unsigned long *user_id, unsigned char *image_data, int height, int width,int img_length) ;
int  bio_reverse_verify_multiple_host_template_data_by_scan(int fd, unsigned char **template_data, int *template_len, int nTemplate, int *matchedIndex, int sensor_timeout_period) ;
int bio_rectify_iso_template(unsigned char *ISO_templates, int* template_length) ;
int bio_Image_Conversion(int fd,int Conv_Type, char *Src_File, char *Dst_File);
int bio_Template_Conversion(int Conv_Type, unsigned int lenInput, unsigned char *pInputBuffer, char *OutFileName);




