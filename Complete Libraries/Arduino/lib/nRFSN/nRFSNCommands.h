/*------------------------------------------------
	* nRFSN command definitions
------------------------------------------------*/
// Sensor commands ("Request sensor value")
#define AUTOV_0		0x10
#define AUTOV_1		0x11
#define AUTOV_2		0x12
#define AUTOV_3		0x13
#define AUTOV_4		0x14
#define AUTOV_5		0x15
#define AUTOV_6		0x16
#define AUTOV_7		0x17
#define AUTOV_8		0x18
#define AUTOV_9		0x19
#define AUTOV_10		0x1A
#define AUTOV_11		0x1B
#define AUTOV_12		0x1C
#define AUTOV_13		0x1D
#define AUTOV_14		0x1E
#define AUTOV_15		0x1F

// System commands
#define BATT_LVL	0x30	// Request VCC voltage level
#define	CURR_LVL	0x31	// Request current consumption

// Acknowledgement commands
#define REQ_ERR		0xAA	// Request error - unimplemented Sensor / Automation command