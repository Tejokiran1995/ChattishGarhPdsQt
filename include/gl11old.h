/** ==========================================================
 * @file gl11.h
 * GL11 APIs and Definitions
 *
 * Company Name: Linkwell Telesystems Pvt. Ltd (visiontek.co.in)
 * Description: gl11 LCD, KPD, RTC,Barcode, RFID, Magnetic swipe, and Printer APIs.  
 */

/**
 * @file gl11.h
 * @author  Rajkumar.M
 *
 * @brief GL11 apis
 */

#ifndef GL11_H
#define GL11_H

/** @defgroup  A LCD,KPD,RTC module header   
 *    @{ 
 */

/** @brief 
 * This API Function is used to open io control devices. 
 * Note: This function must be called before any LCD and Keypad,buzzer functions.
 * @return
 *      0 on Success.
 *     -1 on failure. 
 */
int gl11_iodev_open(void);

/** @brief 
 * This API Function is used to close io control devices. 
 * Note: This function must be called after any LCD and Keypad,buzzer functions.
 * @return
 *      0 on Success.
 */
int gl11_iodev_close(void);


/** @brief 
 *      This API Function used to power on magnetic swipe.
 *      Description: power on magnetic swipe 
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int gl11_magswipe_pwron(void);


/** @brief 
 *     This API Function used to power off magnetic swipe.
 *     Description: power off magnetic swipe 
 *  @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int gl11_magswipe_pwroff(void);


int gl11_gsm_pwroff(void);//


/** @brief 
 *     This API Function used to select gsm card1 or card2.
 *  
 *  @param sel
 *        Description: select card1 or card2  
 *        type: Input 
 *             '1' - card1  
 *             '2' - card2 
 *  @param baudrate
 *  	  Description: open gsm modem with given baudrate
 *  	  type: Input
 *  	       preferably '9600' or 115200              
 *                  
 * @return      
 *      '0' on Success.
 *      '-1' on Error(AT command response).
 *      '-3' Sim not present	
 */
int gl11_gsm_select(int sel,int baudrate);



/** @brief 
 *     This API Function used to enable lcd backlight.
 *  
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int gl11_lcdbklt_on(void);


/** @brief 
 *     This API Function used to disable lcd backlight.
 *  
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int gl11_lcdbklt_off(void);



/** @brief 
 *     This API Function used to set lcd backlight timeout with given time duration.
 *
 *  @param duration
 *     Description: setting lcd backlight timeout 
 *     type: Input 
 *     Range: 4-30 secs      
 *
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int gl11_lcdbklt_time_duration(int duration);


/** @brief 
 *     This API Function used to enable buzzer sound.
 *  
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int gl11_buzz_on(void);

/** @brief 
 *     This API Function used to disable buzzer sound.
 *  
 * @return      
 *    '0' on Success.
 *      '-1' on Error.
 */
int gl11_buzz_off(void);


/** @brief 
 *     This API Function used to set keypad beep timeout.
 *  
 *  @param timeout
 *              Description: setting keypad beep time duration 
 *              type: Input 
 *              Range: 0-8      
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int gl11_kpdtouch_beep_timeout(int timeout);


/** @brief 
 *     This API Function used enable power to audio module.
 *  
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int gl11_audio_power_on(void);

/** @brief 
 *     This API Function used disable power to audio module.
 *  
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int gl11_audio_power_off(void);


/** @brief 
 *     This API Function used to check Adapter presence,check battery charging status& battery voltage
 *  
 *      Description: checking for Adapter presence,battery charging status,battery voltage
 *                   type: Input 
 *     @param AdapPresent 
 *     Description: Argument to store adapter presence
 *     Type: output
 *
 *	@param BattSt 
 *      Description: Argument to store Battery status
 *      Type: output
 *
 *      @param BattVt
 *      Description: Argument to store Battery voltage
 *      Type:output 
 *
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int gl11_GetBatteryInfo(int *AdapPresent,int *BattSt,int *BattVt);

/** @brief 
 *     This API Function used to enable buzzer sound for touch screen
 *  
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int gl11_touch_buzz_on(void);

/** @brief 
 *     This API Function used to disable buzzer sound for touch screen
 *  
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int gl11_touch_buzz_off(void);

/**
 * @brief This API function is used to get the Hardware ID.
 *              (Pre Requirements: This id should be set at u-boot prompt before call this API like below 
 *              VISIONTEK # setenv hwid 87654321
 *              VISIONTEK # saveenv)     
 * @param *hwid  
 *              Description: Read Hardware ID of 8 digits from u-boot environment variables.
 *              Type: Output
 *              Range: 8 digits (bytes)
 * @return 
 *      0 on Success 
 *      -1 on Failure
 */
int gl11_gethwid (char *hwid);

/**
 * @brief This API function is used to get the PIN PAD ID.
 *              (Pre Requirements: This id should be set at u-boot prompt before call this API like below 
 *              VISIONTEK # setenv machineid 1234567890
 *              VISIONTEK # saveenv)     
 * @param *pid 
 *              Description: Read machine ID of 10 digits from u-boot environment variables.
 *              Type: Output
 *              Range: 10 digits (bytes) 
 * @return Return Values 
 *      0 on SUCCESS
 *      -1 on Failure
 */
int gl11_getpinpadid (char *pid);


/** @brief 
 *     This API Function used to set RTC time
 *  
 * @param *rtc_tm
 *     Description: Setting RTC time 
 *     type: input
 *
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int gl11_setrtc (struct tm *rtc_tm);


/** @brief 
 *     This API Function used to get RTC time
 *  
 * @param *curt
 *     Description: Getting RTC time 
 *     type: output
 *
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int gl11_getrtc(struct tm *curt);


/** @brief 
 *     This API Function used to get kernel version details
 *  
 * @param *data
 *     Description: Getting kernel version 
 *     type: output
 *
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int kernel_details (char *data);

/** @brief 
 *     This API Function used to get rootfs version details
 *  
 * @param *data
 *     Description: Getting rootfs version 
 *     type: output
 *
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int rootfs_details(char *data);


/** @brief 
 *     This API Function used to get bootloader version details
 *  
 * @param *data
 *     Description: Getting bootloader version 
 *     type: output
 *
 * @return      
 *      '0' on Success.
 *      '-1' on Error.
 */
int bootloader_details( char *data);


/** @brief 
 *     This API Function used to set lcd default brightness and contrast
 *  
 * @return      
 *      '0' on Success.
 *      '-2' on Error.
 */
int gl11_lcd_default(void);

/** @brief 
 *     This API Function used to change lcd contrast
 * @param chngval
 * 	Description: change LCD contrast level 
 * 	type: input
 * 	Range:0-31	
 *
 * @return      
 *      '0' on Success.
 *      '-2' on failed to set.
 */
int gl11_lcd_contrast(int chngval);


/** @brief 
 *     This API Function used to change lcd brightness
 * @param chngval
 *      Description: change LCD brightness level 
 *      type: input
 *      Range:0-127      
 *
 * @return      
 *      '0' on Success.
 *      '-2' on failed to set .
 */
int gl11_lcd_brightness(int chngval);


/** @defgroup magswipe A magswipe module header 
 * @{ 
 * */

/** @brief 
 * This API Function is used to open magswipe device. 
 * Note: This function must be called before any magswipe function.
 * @return
 *      1 on Success.
 *      MAG_SW_ERR (-1) on Failure
 */
int mscr_open(void);

/** @brief 
 *      This API function reads the data of the both tracks from the card reader .
 *
 * @param track_1_data,track_2_data,track_3_data: 
 *
 * Description: Pointers to the buffers where tracks data to be copied are to be passed as arguments, track1 and track2
          are charecter pointer data types and the memory allocated to them should be length of the expected data+1 
 *            (suggested 256 bytes each)
 *            type: input
 *            Range: 256 bytes 
 * 
 * @return      
 *           '1' if only track1 data is valid and copied data into track_1_data buffer
 *           '2' if only track2 data is valid and copied data into track_2_data buffer
 *           '3' if only track2 data is valid and copied data into track_3_data buffer
 *           '4' if track 1 & 2 are valid and copied data into track_1_data buffer & track_2_data buffer
 *           '5' if track 2 & 3 are valid and copied data into track_2_data buffer & track_3_data buffer
 *           '6' if track 1 & 3 are valid and copied data into track_1_data buffer & track_3_data buffer
 *           '7' if track 1, 2 & 3 are valid and copied into all three buffers, 
 *           MAG_SW_ERR (-1) if there is no valid data of all three tracks or device not opened.
 */
int mscr_read(char *track_1_data, char *track_2_data, char *track_3_data);

/** @brief 
 * This API Function clears the card reader device buffer returns SUCCESS on success, returns MAG_SW_ERR if the device not opened. 
 * @return
 *      1 on Success.
 *      MAG_SW_ERR (-1) on Failure
 */
int mscr_flush(void);

/** @brief 
 * This API Function is used to check wether the device is having valid data ( both track1 and track2 data).
 * @return
 *      1 on Success.
 *      MAG_SW_ERR (-1) if there is no valid data.
 */
int mscr_getstate(void);

/** @brief 
 * This API Function is to close magnetic swipe device.
 * @return
 *      1 on Success.
 *      MAG_SW_ERR (-1) on failure.
 */
int mscr_close(void);


/** @brief 
 * This API Function is used to open RFID device. 
 * Note: This function must be called before any rfid function.
 * @return
 *      opened file descriptor on success 
 *      < 1 on failure
 */
int tama_open(void);

/** @brief 
 * This API Function is used to close rfid device. 
 * Note: This function must be called before any rfid function.
 */
void tama_close(void);

/** @brief 
 *      This API Function is used to detect the targets of RFID Card reader
 * @param Tg_Nos: 
 *              Description: Number of targets to detect(Max. 2).
 *              type: INPUT
 *              Range: Max. 2 
 * @param Tg_Detected: 
 *              Description:Address of memory to store number of Tags detected
 *              type: INPUT 
 * @param Tg_ID: 
 *              Description:Address of memory to store Tag ID
 *              type: INPUT
 * @return      
 *     Number of Tags read/initialized is stored in the address passed to Tg_Detected.The read Tag is stored in location pointed by Tag ID pointer.
 *          0 on success.
 *         -1 on failure
 */
int tama_detectTarget(unsigned char Tg_Nos, unsigned char *Tg_Detected, unsigned char *Tg_ID);


/** @brief 
 *      This API Function authenticates a Tag's Block for writing/reading with the said key
 * @param Tg_No: 
 *              Description: Tag Number to authenticate(0 - Card 1, 1 - Card 2).
 *              type: INPUT
 * @param Blk_Addr: 
 *              Description: Block address
 *              type: INPUT 
 * @param index: 
 *              Description:key index
 *              type: INPUT
 * @param Key_Type: 
 *              Description:Key type (Key A - 0x0A /Key B - 0x0B)
 *              type: INPUT
 * @return      .
 *          0 on success.
 *          < 1 on failure.
 */
//int tama_authenticate_mifare(unsigned char Tg_No, unsigned char Blk_Addr, unsigned char index, unsigned char Key_Type);
int tama_authenticate_mifare(unsigned char Tg_No, unsigned char Blk_Addr, unsigned char* index, unsigned char Key_Type);


/** @brief 
 *      This API Function authenticates a Tag's Block for writing/reading with the said key
 * @param Tg_No: 
 *              Description: Tag Number to authenticate(0 - Card 1, 1 - Card 2).
 *              type: INPUT
 * @param Blk_Addr: 
 *              Description: Block address
 *              type: INPUT 
 * @param *Key: 
 *              Description:key
 *              type: INPUT
 * @param Key_Type: 
 *              Description:Key type (Key A - 0x0A /Key B - 0x0B)
 *              type: INPUT
 * @return      .
 *          0 on success.
 *          < 1 on failure.
 */
int tama_authenticate_mifare_key (unsigned char Tg_No, unsigned char Blk_Addr, unsigned char *Key, unsigned char Key_Type);

/** @brief 
 *      This API Function halts (Deselects) a tag for access
 * @param Tg_LID: 
 *              Description:  Tag's logical ID.
 *              type: INPUT
 * @return      .
 *          0 on success.
 *          < 1 on failure.
 */
int tama_halt_mifare(unsigned char Tg_LID);

/** @brief 
 *      This API Function reads the block data and stores it into passed address
 * @param Blk_Addr: 
 *              Description: Block address to read.
 *              type: INPUT
 * @param Data_In: 
 *              Description:pointer to the data in buffer.
 *              type: INPUT
 * @return      .
 *          0 on success.
 *          < 1 on failure.
 */
int tama_read_target(unsigned char Blk_Addr, unsigned char *Data_In);

/** @brief 
 *      This API Function writes the data sent to the said block
 * @param Blk_Addr: 
 *              Description: Block address to write.
 *              type: INPUT
 * @param Data_Out: 
 *              Description:pointer to the data .
 *              type: INPUT
 * @param Dlen: 
 *              Description:length of data to write .
 *              type: INPUT
 * @return      .
 *          0 on success.
 *          < 1 on failure.
 */
int tama_write_target(unsigned char Blk_Addr, unsigned char *Data_Out, int Dlen);



/** @defgroup  printer A printer module header 
 *    @{ */

/** @brief 
 * This API Function is used to open the printer device . 
 * Note: This function must be called before any printer function.
 * @return
 *      1 on success 
 *      2 if the device is already opend
 */
int prn_open(void);

/** @brief 
 * This API Function is used to close the printer device . 
 * @return
 *       0 if the device closed successfully
 *      -1 if the device is not opened
 */
int prn_close(void);

/** @brief 
 *      This API Function is used to advance the paper .
 * @param scanlines: 
 *              Description: scanlines is to be passed as parameter (1 text line is equal to 17 scan lines).
 *              type: INPUT
 * @return    
 *          -1 if the device not opened,
 *           else the response from the ioctl function.
 */
int prn_paper_feed(int scanlines);

/** @brief 
 *      This API Function is used to prints the text  .
 * @param text2: 
 *              Description: pointer to the buffer from which the text to be printed.
 *              type: INPUT
 * @param len: 
 *              Description:  length of the text.
 *              type: INPUT
 * @param font: 
 *              Description: font of the the text presently there are two fonts are supported (1 and 2).
 *              type: INPUT
 * @return    
 *          1 on success,
 *         -1 if the device not opened .
 *         -2 if the length is more .
 */
int prn_write_text(unsigned char *text2, int len, int font);

/** @brief 
 *      This API Function is used to print bmp data  .
 * @param bmp: 
 *              Description: pointer to the buffer from which the data to be printed.the image is 144x144 pixels
 *              type: INPUT
 * @param len: 
 *              Description:  length of the text.
 *              type: INPUT
 * @return    
 *          no. of characters written on success
 *          -1 if error occurs
 */
int prn_write_bmp(unsigned char *bmp, int len);


/** @brief 
 *      This API Function is used toabort printing.
 */
void prn_abort(void);



/** @defgroup  barcode A barcode module header 
 *    @{ 
 */


/** @brief 
 * This API Function is used to open barcode device. 
 * Note: This function must be called before any barcode function.
 * @return
 *      0 on Success.
 *      -1 on failure. 
 */
int bar_code_open(void);

/** @brief 
 *      This API Function reads the barcode.
 * @param rxbuf: 
 *              Description: buffer used to store barcode data.
 *              type: output 
 *              Range: 0 - 256 
 * @param len: 
 *              Description:len means size of data to be read (For future expansion. Now, it will return on enter)
 *              Returns data size read.
 *              type: input
 *              Range: 0 - 256 
 * @return      
 *      >0 on Success.
 *      '-1' on Error.
 */
int bar_code_read (unsigned char *rxbuf,int len);


/** @brief 
 *      This API Function closes the barcode.
 * @return      
 *      0 on Success.
 *      '-1' on Error.
 */
int bar_code_close (void);


/**
 *    @{ End of "A barcode module header "
 */


/** @defgroup SAM A SAM module header 
 * @{ 
 */


/** @brief
 *	This function checks for the existence of the Sam card reader device and if found prepares it for further communication. Before attempting any operat * ions related to Sam card reader device this function should be called. 
 *
 * @return      
 *    '0' on Success.
 *    '-1' on Error	
 */
int SAM_Open(void);


/** @brief
 *	This function closes the SAM device. After finishing all the operations related to SAM card reader this function should get called.
 *@return
 * '0' on Success.
 * '-1' on Error 
 */
int SAM_Close();

/** @brief
 *	This API reads ATR from the card.
 *
 * @param *length: 
 *              Description: length  of ATR from card in bytes.
 *              type: output 
 * @param *buffer
 * 		Description: pointer to buffer which holds ATR response from the card .
 * 		type: output             
 *  
 *  @return
 *  		'0' on Success
 *  		'-1' on Error 'if unable to receive ATR this happens only if card is not present in slot'.
 */
int SAM_GetAtr(unsigned int *length, unsigned char *buffer);


/** @brief
 *This API is used to send all the EMV complaint CAPDUs to the card. The API call sends the command contained in CAPDU Data buffer and gives the response and *its length out through RAPDU Data and RLen variables respectively.
 *
 * @param *CAPDU_Data
 * 	Description:Buffer holding the command APDU in the standard format
 * 	type:input
 *
 * @param CLen
 * 	Description:length of the CAPDU Data buffer
 * 	type:input
 *
 * @param *RAPDU_Data
 *	Description:Buffer to hold the response APDU in the standard format
 *	type:output
 *
 * @param RLen
 * 	Description:variable to hold response length. 
 * 	type:output
 *
 * @return
 * 	'0' on Success
 * 	'-1' on Failure 'communication error occurred and the device is closed'. 
 *
 */
int SAM_SendCommand (unsigned char *CAPDU_Data,unsigned int CLen,unsigned char *RAPDU_Data,unsigned int RLen );

/** @brief
 *This API is to power up the card,after this call Vcc is applied as selected during card select API. After this call only we can do any command operations o *n card.
 *
 *@param selection
 *	Description: 	1- card1 1.8v	
 *			2- card1 3v
 *			3- card1 5v
 *			4- card2 1.8v
 *			5- card2 3v
 *			6- card3 5v	
 *	type:input		
 *
 *@return 
 *	'0' on Success
 *	'-1' on Failure
 *
 * */
int SAM_ColdReset(int selection);

/**
 *    @{ End of "A SAM module header."
 */


/** @defgroup SCR A SCR module header 
 *  * @{ 
 *   */

/** @brief
 *This API is used to open the scr device
 *
 *@return
 *	'0' on Success
 *	'-1' on failure
 */
int scr_open(void);

/** @brief
 *This API is used to close the scr device
 *
 *@return
 *      '0' on Success
 *      '-1' on failure
 */
int scr_close (void );


/** @brief
 *  This API is used to select card1 or card2
 *
 * @param card
 * 	Description: select card1 or card2
 * 	type:input
 *
 *@return
 *      '0' on Success
 *      '-1' on failure
 */
int select_card(int card);


/** @brief
 *  This API is used to power up smart card
 *  
 *  @return
 *        '0' on Success
 *        '-1' on failure
 */
int powerup_5v();

/** @brief
 *  This API is used to check smart card presence in the reader
 *  
 *  @return
 *        '0' on Success
 *       '-1' on failure
 */
int SCR_Checkup_CardPresence();

/** @brief
 *This API is used ro send APDU to smart card.
 *
 * @param *CAPDU_Data
 * 	Description:	sending Command data
 * 	type:input
 *
 * @param CLen
 * 	Description:   Command length
 * 	type:input
 *
 * @param *RAPDU_Data
 * 	Description:  Response data
 * 	type:output
 *
 * @param *RLen
 * 	Description: Response length
 * 	type:output
 *
 * @return
 * 	'0' on Success
 * 	'-1' on Failure
 *
 */
unsigned char SCR_SendCommand(unsigned char *CAPDU_Data,unsigned char CLen,unsigned char *RAPDU_Data,unsigned int *RLen );

/**
 *    @{ End of "A SCR module header."
 */


/** @defgroup Optical A Optical scanner header 
 * @{ 
 */


/** @brief
 *	This API is used to call optical finger print module for enroll or verification
 *
 * @param MenuIndex
 *	Description: 6 - Enroll
 *		     7 - Identify
 *		     8 - Verify
 *		     9 - Identify match .etc
 *	type:input
 *
 * @param *BioData
 *	Description: Finger print template data
 *	type:input
 *
 * @return
 *	'0' on success
 *	'-1' on failure	
 */

/** @brief
 *  This API Function used to disable or enable the ETH.
 * Note :
 * 	If we disable the eth_control, it disable all the IP settings includeing routeing.
 *	When ever we enable the eth_control we need to re-assign the IP setting.
 *  @param state
 *     Description: Depends on input state API should be enable or disable the ETH ;
 *     Type: input:
 *     Range: 0 for disable the ETH ;
 *            1 for enable the ETH ;
 * @return
 *             '0' on Success;
 *             '-2' on Argument error;
 *
 */
int gl11_eth_control(int );


int OptFPCall(int MenuIndex,char *BioData);


/**
 *    @{ End of "A Optical scanner header."
 */
//
//int gsm_comm(unsigned int, int);
int gl11_simselect(int);
int gl11_gsmpwrctrl(int);
int gl11_mresetctrl(int);
int modemopen ( char *portname);
void modemparams(int baudrate);
int modemclose(void);
int write_to_modemm(char *buffer,int no_of_bytes);
int Test_modem_response(char *command,char *response,char *expect[],int timeout);

//Added by Sanjeet
int SCR_Powerdown();
int prn_paperstatus(void);
int gl11_optcap_pwron();
int gl11_optcap_pwroff();
int gl11_bklight_intensity (int);
int gl11_host_pwroff();
int gl11_host_pwron();

//Added by Shashidhar
int gl11_gps_pwron(void);
int gl11_gps_pwroff(void);

int SAM_PowerUp(int,int);
int gl11_gpsctrl(int);

#endif

