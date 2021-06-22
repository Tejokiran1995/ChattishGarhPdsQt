/** @file biometric_api.h 
 * GL11 IRISH APIs and Definitions
 *
 * Company Name: Linkwell Telesystems Pvt. Ltd (visiontek.co.in)
 * 
 * API Description: GL11-IRISH APIs.  
 */

/**
 * 
 * @file biometric_api.h 
 */



/** @brief
 *      Initializes the Iritech Device & library.
 * @return 
 *       0 on success , 
 *       -1 on failure.
 */

int iris_bio_init();

/** @brief
 *      unInitializes the  Iritech Device & library         
 * @return 
 * 	0 on success,
 * 	-1 on failure.
 */

int iris_bio_uninit();

/** @brief
 *      captures the Iris and add to the module          
 * @param[in] enrollID Enroll Id of the enrollee.
 * @param[in] time_out_period Timeout in seconds.
 * @param[out] quality Iris image quality,the output range is from 0 (lowest quality) to 100(highest quality).
 * @return 
 *      0 on success,
 *      -1 on failure.
 */

int iris_bio_enroll_by_capture(char *enrollID,int time_out_period,unsigned char quality);

/**@brief
 *      Add the Iris template to Gallery.             
 * @param[in] enrollID Enroll Id of the enrollee.
 * @param[in] template_data Template data.
 * @param[in] template_len Length of a template data.
 * @return
 *      0 on success
 *      -1 on failure
 */

int iris_bio_enroll_by_template(char *enrollID,char *template_data, int template_len);

/**@brief
 *      store the template into specified file by live capturing.
 * @param[in ,out] template_file On return contains template of  Iris data.
 * @param[in]  time_out_period Timeout in seconds.
 * @param[out] quality Iris image quality, the output range is from 0 (lowest quality) to 100 (highest quality).
 * @return
 *      0 on success
 *      -1 on failure
 */

int iris_bio_capture_template(char *template_file,int time_out_period,unsigned char quality);

/**@brief
 *      Store the template into file retrieve from the module.
 * @param[in] enrollID userid of the template to read
 * @param[out] template_file On return contain template of given userid.
 * @return
 *      0 on success
 *      -1 on failure
 */

int iris_bio_get_template(char *enrollID, char *template_file);

/**@brief
 *      Store the image into specified file by live capture.
 * @param[in] imageFormat Format of the image to be saved.
 * @param[in] imageType ISO image types.
 * @param[in ,out] image_file_name On return contains the Image data of the captured iris.
 * @param[in] time_out_period Timeout in seconds.
 * @param[out] imgData Output Iris image data.
 * @param[out] imgSize Output Iris image size.
 * @return
 *      0 on success
 *      -1 on failure
 */

int iris_bio_capture_image(char  imageFormat, char imageType, char *image_file_name, int time_out_period, unsigned char *imgData, int *imgSize);

/**@brief
 *      verifies the Iris against the module by live capture.       
 * @param[in] enrollID Userid which you want to verify.
 * @param[in] time_out_period Timeout in seconds.
 * @return
 *      0 on success
 *      -1 on failure
 */

int iris_bio_verify_by_capture(char *enrollID, int time_out_period);

/**@brief
 *      verify the Iris template in a file against live capture.         
 * @param[in] template_file_name Template file name in which the template is available.
 * @param[in] time_out_period Timeout in seconds.
 * @return
 *      0 on success
 *      -1 on failure
 */

int iris_bio_verify_host_template_file_by_capture(char *template_file_name,int time_out_period);

/**@brief
 *      Identify the Iris template(1:N matching) by live capture against enrolled templates in the Gallery.
 * @param[out] enrollID On return contains corresponding userid in which the template is matched against.
 * @param[in] time_out_period Timeout in seconds.
 * @return
 *      0 on success
 *      -1 on failure
 */

int iris_bio_identify_by_capture(char *enrollID,int time_out_period);

/**@brief
 *      Delete the user from the module
 * @param[in] enrollID Userid to delete.
 * @return
 *      0 on success
 *      -1 on failure
 */

int iris_bio_delete_template(char *enrollID);

/**@brief
 *      Delete all templates in Module.         
 * @return
 *      0 on success
 *      -1 on failure
 */

int iris_bio_delete_all_templates();
