
int gsm_ppp_open(void);
// Success 0
// Failure -1
void gsm_ppp_close(void);

int gsm_signal_strength(void);
// Return Signal strength
// -2 Modem response error

int gsm_sim_status(void);
// Returns 0 on success
// Returns -1 on failure


int gsm_network_registration(void);
// Returns 0 on success
// Returns -1 on Failure

int gsm_communication_type (void);
//  Returns 0 on 2G
//  Returns 1 on 3G
//  Returns -2 on Error
// returns -1 on Modem response error

int gsm_modem_disable(void);
//  Disables Modem functionality

//  Returns  0  on success
//  Returns -1  on Error

int gsm_modem_enable(void);
	
// Enables Modem Functionality
//  Returns  0  on success
//  Returns -1  on Error



int gsm_update_scripts(char *apn);

//  Returns  0  on success
//  Returns -1  on Error


int gsm_sim_operator (char *response);

// Return on Command success

// Returns -1 on failure and  the buffer size should be minim 512 bytes;


