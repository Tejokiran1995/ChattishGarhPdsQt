#include "Iddk2000_utils.h"

#define ON		true		/* Device is on */
#define OFF		false		/* Device is off */

extern HIRICAMM g_hDevice;	/* Device handle */
extern char g_binDir[256];  /* assembly directory */

#define RIGHT_EYE_IDX 0
#define LEFT_EYE_IDX  1

////////////////////////////////////////////////

void login();

void logout();

void device_configuration();

void get_information();

void get_device_handle();

void capturing_process(bool bDefaultParams, bool bMultiple, bool bProcessResult);

void iris_recognition();

void power_management();

void finalize_device();

void handle_error(IddkResult error);

////////////////////////////////////////////////
