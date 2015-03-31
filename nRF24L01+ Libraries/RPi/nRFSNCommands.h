/*------------------------------------------------
	* nRFSN command definitions
------------------------------------------------*/
// Sensor commands ("Request sensor value")
#define SENV_0		0x10
#define SENV_1		0x11
#define SENV_2		0x12
#define SENV_3		0x13
#define SENV_4		0x14
#define SENV_5		0x15
#define SENV_6		0x16
#define SENV_7		0x17
#define SENV_8		0x18
#define SENV_9		0x19
#define SENV_10		0x1A
#define SENV_11		0x1B
#define SENV_12		0x1C
#define SENV_13		0x1D
#define SENV_14		0x1E
#define SENV_15		0x1F

// Automation commands ("Update automation value")
#define AUTOV_0		0x20
#define AUTOV_1		0x21
#define AUTOV_2		0x22
#define AUTOV_3		0x23
#define AUTOV_4		0x24
#define AUTOV_5		0x25
#define AUTOV_6		0x26
#define AUTOV_7		0x27
#define AUTOV_8		0x28
#define AUTOV_9		0x29
#define AUTOV_10	0x2A
#define AUTOV_11	0x2B
#define AUTOV_12	0x2C
#define AUTOV_13	0x2D
#define AUTOV_14	0x2E
#define AUTOV_15	0x2F

// System commands
#define BATT_LVL	0x30	// Request VCC voltage level
#define	CURR_LVL	0x31	// Request current consumption