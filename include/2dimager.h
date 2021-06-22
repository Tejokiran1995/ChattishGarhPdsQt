
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<string.h> 

/*
 * @brief
 *    This API funtion is used to initialize the xenon 2D image and barcode reader
 *    Note: This function must be called before any xenon 2D image and barcode reader operations
 *@returns
 *    0 on success
 *   -1 on vendor id of device not found
 *   -2 on unable to open node for 2dimager
*/
int xenon_init(void);

/*
 *@brief
 *   This API function is used to read 2D and 1D barcode using xenon 2D image and barcode reader
 *   @data:
 *   Description: Arguement to store barcode data (output param)
 *   @timeout:
 *   Description: Arguement for timeout to scan barcode range(1-60)
 *   Note: This function should be called only for scaning barcode based applicatons using xenon 2D image and barcode reader
 *@returns
 *   0 on success
 *  -1 on failure
 *  -2 on timeout range exceeds 60 seconds
*/
int xenon_read_barcode(char *data,int timeout);

/*
 *@brief
 *    This API function is used to read image using xenon 2D image and barcode reader
 *    Note: This function should be called only for 2D image capturing based application using xenon 2D image and barcode reader
 *@returns
 *     0 on success
 *     -1
 *    -2 on failure
*/
int xenon_read_image(void);








