#ifndef PRINTER_H
#define PRINTER_H

/*Done by T.V.R */
#define PRN_ERR -1
#define PRN_LEN_ERR -2
#define PRN_OFF     0
#define PRN_ON      1
#define PRN_BUSY    2
#define PRN_FAIL    0 
#define PRN_SUCCESS 1

//This function opens the printer device 
// returns SUCCESS if the device is opend successfully, returns BUSY if the device is already opend
int prn_open();  

//This fucntion closes the printer device 
//returns 0 if the device closed successfully, returns PRN_ERR if the device is not opend.
int prn_close(); 

//This fucntion advance the paper 
// no.of scanlines is to be passed as parameter ( 1 text line is equal to 17 scan lines)
//return PRN_ERR if the device not opend else returns the response from the ioctl function
int prn_paper_feed(int scanlines);

//This fucntion prints the text 
//pointer to the buffer from which the text to be printed, length of the text and font to be used to be 
//passed as arguments
//max lines of text can be 25 (42 charecters per line if the font is 1 and 21 charecters if the font is 2)
//presently there are two fonts are supported (1 and 2)
// returns SUCCESS if the fucntion successfull, returns PRN_ERR if the device not opend, returns PRN_LEN_ERR if the length is more 
int prn_write_text(unsigned char *text2, int len, int font); 

int prepare_the_print(unsigned int *hindi_string, int length, int font);
/* this function supports to print Hindi and English characters 
 * Input parameters:
 * 	hindi_string - unicode string
 * 	length - size of hindi string buffer  (For font '0' length should not exceed 160(40 characters) & 
 *					       For font '1' length should not exceed 560(140 Characters))
 * 	font  - 1 - For Normal font
 *	   	0 - For Bold font
 * Returns:
 * 	 0 on success 
 *	 1 on buf size exceeds
 * 	-1 on print error
 * 	-2 on No paper
 *	-3 on Low Battery
 *	-4 on Unicode error
 */
int prn_write_bmp(unsigned char *, long); // bitmap
						//returns no. of characters written or PRN_ERR if error occurs
void prn_abort(); // printing aborts

int ComboDisp(unsigned char HindiUnicode[], unsigned char *EngStr, int iRow, int iFont);

//int lk_disp_hindi(OutBuff,sizeof(OutBuff*4)+4, 1, &end1,&end2,&end3) ;
int lk_disp_hindi(unsigned char,int,int,int,int,int) ;
#endif
