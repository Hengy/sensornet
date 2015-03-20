#include "nRFSN.h"

nRFSNClass nRFSN;

/*------------------------------------------------
 * Initialize function
 * - Initializes CE, CSN, IRQ pins
 * - Initializes SPI
 * - Initializes nRF settings to default
 * - Sets TX, RX addresses to default/EEPROM stored values
 * - Initializes interrupt flags and enables IRQ interrupt
 * uint8_t SPIDiv: SPI frequency divider (default is SPI_CLOCK_DIV4)
 * - SPI_CLOCK_DIVx
 *	where valid x values are: 4, 8, 16, 32, 64, 128
------------------------------------------------*/
void nRFSNClass::init(uint8_t SPIDiv, uint8_t CEpin, uint8_t CSNpin, uint8_t IRQpin)
{
	nRFSN_CE = CEpin;
	nRFSN_CSN = CSNpin;
	nRFSN_IRQ = IRQpin;

	pinMode(nRFSN_CE, OUTPUT);
	pinMode(nRFSN_CSN, OUTPUT);
	pinMode(nRFSN_IRQ, INPUT);
	digitalWrite(nRFSN_CE, HIGH);
	digitalWrite(nRFSN_CSN, HIGH);

	if (!SPIDiv)
	{
		SPIDiv = SPI_CLOCK_DIV4;	// Default to Fosc/4 (4MHz on 16MHz Arduino)
	}
	initSPI(SPIDiv);

	// nRF defaults
	CONFIG_CURR           = B00001011;   // Show RX_DR and MAX_RT interrupts; Enable CRC - 1 uint8_t; Power up; RX
	EN_AA_CURR            = B00000011;   // Enable Auto Ack for pipe 0,1
	EN_RXADDR_CURR        = B00000011;   // Enable data pipe 0,1
	SETUP_AW_CURR         = B00000010;   // Set up for 4 address
	SETUP_RETR_CURR       = B00110000;   // 1000us retransmit delay; 10 retransmits
	RF_CH_CURR            = B01101001;   // Channel 105 (2.400GHz + 0.105GHz = 2.505GHz)
	RF_SETUP_CURR         = B00000110;   // RF data rate to 1Mbps; 0dBm output power (highest)
	//RX_PW_P0_CURR         = B00000011;   // 1 payload
	DYNPD_CURR            = B00000011;   // Set dynamic payload for pipe 0
	FEATURE_CURR          = B00000100;   // Enable dynamic payload

	if (!checkAddrs())		// Is there an address in EEPROM?
	{
		// If no EEPROM stored addresses, set deafults
		for (int i=0;i<4;i++)
		{
			RX_ADDRESS[i] = 0xC7;
			TX_ADDRESS[i] = 0xE7;
		}
	}

	// nRF24L01+ setup
	// Write to CONFIG register
	setReg(CONFIG,CONFIG_CURR);
	// Write to EN_RXADDR register  
	setReg(EN_RXADDR,EN_RXADDR_CURR);
	// Write to EN_AA register
	setReg(EN_AA,EN_AA_CURR);
	// Write to SETUP_AW register
	setReg(SETUP_AW,SETUP_AW_CURR);
	// Write to SETUP_RETR register
	setReg(SETUP_RETR,SETUP_RETR_CURR);
	// Write to RF channel register
	setReg(RF_CH,RF_CH_CURR);
	// Write to RF setup register  
	setReg(RF_SETUP,RF_SETUP_CURR);
	// set TX address
	setTXAddr(TX_ADDRESS,4);
	// set RX address
	setRXAddr(RX_ADDR_P1,TX_ADDRESS,4);
	// set RX address
	setRXAddr(RX_ADDR_P0,RX_ADDRESS,4);
	// Set dynamic payload for pipe 0
	setReg(DYNPD,DYNPD_CURR);
	// Write to FEATURE register
	setReg(FEATURE,FEATURE_CURR);
	// Flush RX FIFO
	transfer('n',FLUSH_RX,0);
	// Flush TX FIFO
	transfer('n',FLUSH_TX,0);
	
	// current mode is RX
	currMode = 1;	// 1 = RX, 0 = TX

	// Initialize interrupt flags
	RXInt = 0;
	TXInt = 0;
	MAXInt = 0;
	
	updateStatus();
}


/*------------------------------------------------
 * Check EEPROM for valid addresses
------------------------------------------------*/
uint8_t nRFSNClass::checkAddrs(void)
{
	// uint8_t newAddr = 0;

	// uint8_t addrByte = EEPROM.read(0);			// get first byte of EEPROM
	
	// if (addrByte != 0xFF)						// addresses NEVER start with 255. If 255 was read from EEPROM,
	// {											// then device has never been synced before
		// for (uint8_t i=0; i<4; i++)
		// {
			// TX_ADDRESS[i] = EEPROM.read(i);		// If valid addresses was found in EEPROM, get them
			// RX_ADDRESS[i] = EEPROM.read(i+4);
		// }

		// //set TX address
		// setTXAddr(TX_ADDRESS,4);
		// //set RX address
		// setRXAddr(RX_ADDR_P0,RX_ADDRESS,4);

		// newAddr = 1;							// flag success
	// }

	// return newAddr;								// return 0: no addresses found 1: addresses found
	
	return 0;
}


/*------------------------------------------------
 * Initialize SPI setting and enable
 * uint8_t SPIDiv: SPI frequency divider (default is SPI_CLOCK_DIV4)
 * - SPI_CLOCK_DIVx
 *	where valid x values are: 4, 8, 16, 32, 64, 128
------------------------------------------------*/
void nRFSNClass::initSPI(uint8_t SPIDiv)
{
	// SPI setup
	SPI.setBitOrder(MSBFIRST);              // Set most significant bit first
	SPI.setClockDivider(SPIDiv);			// Clock to Fosc/SPIDiv
	SPI.setDataMode(SPI_MODE0);             // Clock polarity 0; clock phase 0
	SPI.begin();                            // Start SPI
}


/*------------------------------------------------
 * Set nRF to transmit mode
------------------------------------------------*/
void nRFSNClass::setTXMode(void)
{
	setReg(CONFIG, (CONFIG_CURR & B11111110) );
	digitalWrite(nRFSN_CE, LOW);
	currMode = 0;
	delayMicroseconds(140);
}


/*------------------------------------------------
 * Set nRF to receive mode
------------------------------------------------*/
void nRFSNClass::setRXMode(void)
{
	setReg(CONFIG, (CONFIG_CURR | B00000001) );
	digitalWrite(nRFSN_CE, HIGH);
	currMode = 1;
	delayMicroseconds(140);
}


/*------------------------------------------------
 * Returns current mode - 0 TX, 1 RX
------------------------------------------------*/
uint8_t nRFSNClass::getMode(void)
{
	char currConfig = getReg(CONFIG);
	return (currConfig & B00000001);
}


/*------------------------------------------------
 * Set nRF output power. 0: lowest 3: highest
------------------------------------------------*/
void nRFSNClass::setPower(uint8_t pwrLvl)
{
	RF_SETUP_CURR = pwrLvl << 1;			// shift 1 bit left
	setReg(RF_SETUP,RF_SETUP_CURR);
}


/*------------------------------------------------
 * Gets power configuration of nRF
------------------------------------------------*/
uint8_t nRFSNClass::getPower(void)
{
	char currConfig = getReg(RF_SETUP);
	return (currConfig & B00000110) >> 1;
}


/*------------------------------------------------
 * Set nRF channel (Fo = 2400 + channel [MHz])
------------------------------------------------*/
void nRFSNClass::setChannel(uint8_t ch)
{
	RF_CH_CURR = ch;
	setReg(RF_CH,RF_CH_CURR);
}


/*------------------------------------------------
 * Gets current channel of nRF
------------------------------------------------*/
uint8_t nRFSNClass::getChannel(void)
{
	char currConfig = getReg(RF_CH);
	return currConfig;
}


/*------------------------------------------------
 * Set nRF max number of retransmits
 * uint8_t numRT: Auto Retransmit Count (0: 250us 1111(15): 4000us; each +1 = +150us)
------------------------------------------------*/
void nRFSNClass::setMaxRT(uint8_t numRT)
{
	// mask out current Auto Retransmit Delay, and OR it with numRT with upper 4 bits (numbers > 16) masked out
	// result is current ARD and new ARC values
	SETUP_RETR_CURR = (SETUP_RETR_CURR & B11110000) | (numRT & B00001111);
	setReg(SETUP_RETR,SETUP_RETR_CURR);
}


/*------------------------------------------------
 * Sets maximum number of retries
------------------------------------------------*/
uint8_t nRFSNClass::getMaxRT(void)
{
	char currConfig = getReg(SETUP_RETR);
	return (currConfig & B00001111);
}


/*------------------------------------------------
 * Set nRF max number of retransmits
 * uint8_t numRT: Auto Retransmit Count (0: 250us 1111(15): 4000us; each +1 = +150us)
------------------------------------------------*/
void nRFSNClass::setMaxRTdelay(uint8_t delay)
{
	// mask out current Auto Retransmit Delay, and OR it with numRT with upper 4 bits (numbers > 16) masked out
	// result is current ARD and new ARC values
	SETUP_RETR_CURR = ((delay << 4) & B11110000) | (SETUP_RETR_CURR & B00001111);
	setReg(SETUP_RETR,SETUP_RETR_CURR);
}


/*------------------------------------------------
 * Sets maximum number of retries
------------------------------------------------*/
uint8_t nRFSNClass::getMaxRTdelay(void)
{
	char currConfig = getReg(SETUP_RETR);
	return (currConfig & B11110000) >> 4;
}


/*------------------------------------------------
 * Sets new transmit address
 * uint8_t addr[]: new address
 * uint8_t len: length of address (nRFSN uses 4)
------------------------------------------------*/
void nRFSNClass::setTXAddr(uint8_t addr[], uint8_t len)
{
	digitalWrite(nRFSN_CSN, LOW);	// select nRF

	SPI.transfer(W_REGISTER|TX_ADDR);

	if (len != 0) {
        // Send address bytes
        for (int i=1;i<=len;i++) {
            SPI.transfer(addr[i-1]);
        }
    }

	digitalWrite(nRFSN_CSN, HIGH);	// deselect nRF
}


/*------------------------------------------------
 * Sets new receive address
 * uint8_t pipe: pipe number (0-5); nRFSN uses 0, 1
 * uint8_t addr[]: new address
 * uint8_t len: length of address (nRFSN uses 4)
------------------------------------------------*/
void nRFSNClass::setRXAddr(uint8_t pipe, uint8_t addr[], uint8_t len)
{
	digitalWrite(nRFSN_CSN, LOW);	// select nRF

	SPI.transfer(W_REGISTER|pipe);

	if (len != 0) {
        // Send address bytes
        for (int i=1;i<=len;i++) {
            SPI.transfer(addr[i-1]);
        }
    }

	digitalWrite(nRFSN_CSN, HIGH);	// deselect nRF
}


/*------------------------------------------------
 * Returns current TX address
------------------------------------------------*/
uint8_t *nRFSNClass::getTXAddr(void)
{
	static uint8_t addr[4];
	
	digitalWrite(nRFSN_CSN, LOW);	// select nRF

	SPI.transfer(R_REGISTER|TX_ADDR);

	// Send address bytes
	for (int i=1;i<=4;i++) {
		addr[i] = SPI.transfer(NRF_NOP);
	}

	digitalWrite(nRFSN_CSN, HIGH);	// deselect nRF
	
	return addr;
}


/*------------------------------------------------
 * Returns current RX address
------------------------------------------------*/
uint8_t *nRFSNClass::getRXAddr(uint8_t pipe)
{
	static uint8_t addr[4];
	
	digitalWrite(nRFSN_CSN, LOW);	// select nRF

	SPI.transfer(R_REGISTER|pipe);

	// Send address bytes
	for (int i=1;i<=4;i++) {
		addr[i] = SPI.transfer(NRF_NOP);
	}

	digitalWrite(nRFSN_CSN, HIGH);	// deselect nRF
	
	return addr;
}


/*------------------------------------------------
 * IRQ pin interrupt service routine
------------------------------------------------*/
void nRFSNClass::nRF_ISR(void)
{
	updateStatus();						// Get current nRF status

	if (Status & B01000000)		// If data received IRQ
	{
		RXInt = 1;
	}
	else if (Status & B00100000)	// If data sent IRQ
	{
		TXInt = 1;
		Busy = 0;					// if data sent, nRF no longer busy
	}
	else if (Status & B00010000)	// If max retransmits IRQ
	{
		MAXInt = 1;
		Busy = 0;					// nRF no longer busy
	}
}


/*------------------------------------------------
 * Clears interrupt flag on nRF
 * uint8_t interrupt: interrupt number (uses defined constants)
 *	- valid values: MAX_RT, RX_DR, TX_DS
------------------------------------------------*/
void nRFSNClass::clearInt(uint8_t interrupt)
{
	setReg(STATUS,(interrupt));
}


/*------------------------------------------------
 * Updates nRF status variable
------------------------------------------------*/
uint8_t nRFSNClass::updateStatus(void)
{
	digitalWrite(nRFSN_CSN, LOW);	// select nRF

	Status = SPI.transfer(NRF_NOP);

	digitalWrite(nRFSN_CSN, HIGH);	// deselect nRF
	
	return Status;
}


/*------------------------------------------------
 * nRFSN sync function. Syncs to Root (RPi) and stores new TX, RX addresses.
 * Returns 0: Sync fail 1: Sync succeed
------------------------------------------------*/
uint8_t nRFSNClass::sync(void)
{
	// make copy of current TX, RX addresses
	uint8_t prevTXAddr[4];
	memcpy(prevTXAddr,TX_ADDRESS,4);
	uint8_t prevRXAddr[4];
	memcpy(prevRXAddr,RX_ADDRESS,4);

	// set default addresses for syncing
	uint8_t addr1[4];
	uint8_t addr2[4];
	for (int i=0;i<4;i++)
	{
		RX_ADDRESS[i] = 0xE7;
		TX_ADDRESS[i] = 0xE7;
	}
	setRXAddr(RX_ADDR_P0,addr1,4);
	setTXAddr(addr2,4);

	// set to receive mode
	setRXMode();
	uint32_t time = 0;
	uint8_t synced = 0;
	while ((time < 300000) && (synced == 0))				// while less than 30s and unsynced
	{
		if (RXInt)									// has a packet been received?
		{
			uint8_t size = getPayloadSize();				// If so, get the size
			getPayload(size);								// then get the packet data and put in buffer

			if ((BufIn[0] == 0x03) && (size == 9))	// check for sync command and packet size of 9
			{
				for (uint8_t i=0; i<4; i++)					// if valid sync packet, write new addresses to EEPROM
				{
					EEPROM.write(i,BufIn[i+1]);
					EEPROM.write(i+4,BufIn[i+5]);
				}

				synced = 1;									// stop syncing
			}
			else
			{
				break;										// packet was not a valid sync packet
			}
		}
		delayMicroseconds(1000);							// wait 1ms
		time++;												// increment time variable
	}

	if (synced) {								// if sync was successful, set new addresses
		for (uint8_t i=0; i<4; i++)
		{
			TX_ADDRESS[i] = BufIn[i+1];
			RX_ADDRESS[i] = BufIn[i+5];
		}
		// set new TX address
		setTXAddr(TX_ADDRESS,4);
		// set new RX address
		setRXAddr(RX_ADDR_P0,RX_ADDRESS,4);
	} else {									// if sync failed, restore prev addresses and delete garbage
		memcpy(TX_ADDRESS,prevTXAddr,4);
		memcpy(RX_ADDRESS,prevRXAddr,4);
		delete prevTXAddr;
		delete prevRXAddr;
	}

	return synced;								// return 0 if failed, 1 if sucessful
}


/*------------------------------------------------
 * nRF configure register.
 * char wrn: write('w')/read('r')
 * uint8_t command: command byte
 *					- command byte is ORed with W_REGISTER, R_REGISTER
 * uint8_t data: data byte to send
 * returns data byte. If writing to register, will return 0x00
------------------------------------------------*/
// uint8_t nRFSNClass::configReg(char wr, uint8_t command, uint8_t data)
// {
	// digitalWrite(nRFSN_CSN, LOW);	// select nRF

	// if (wr == 'w') {                            // Write
        // SPI.transfer(W_REGISTER|command);       // Send command
        // SPI.transfer(data);
    // } else if (wr == 'r') {                     // Read
        // SPI.transfer(R_REGISTER|command);       // Send command
        // data = SPI.transfer(NRF_NOP);
	// }

	// digitalWrite(nRFSN_CSN, HIGH);	// deselect nRF

	// return data;
// }


/*------------------------------------------------
 * Sets config register of nRF
------------------------------------------------*/
void nRFSNClass::setReg(uint8_t reg, uint8_t data)
{
	digitalWrite(nRFSN_CSN, LOW);	// select nRF
	
	SPI.transfer(W_REGISTER|reg);       // Send command
	SPI.transfer(data);

	digitalWrite(nRFSN_CSN, HIGH);	// deselect nRF
}


/*------------------------------------------------
 * Gets config register of nRF
------------------------------------------------*/
uint8_t nRFSNClass::getReg(uint8_t reg)
{
	digitalWrite(nRFSN_CSN, LOW);	// select nRF
	
	SPI.transfer(R_REGISTER|reg);   // Send command
	uint8_t data = SPI.transfer(NRF_NOP);

	digitalWrite(nRFSN_CSN, HIGH);	// deselect nRF

	return data;
}


/*------------------------------------------------
 * nRF transfer data. DO NOT USE TO TRANSMIT!
 * char wrn: write('w')/read('r')/none('n')
 * uint8_t command: command byte
 *					- if wrn = 'w' or 'r', command byte is ORed with W_REGISTER, R_REGISTER
 * uint8_t len: length of data in bytes
				- data to be sent must be in output buffer
------------------------------------------------*/
void nRFSNClass::transfer(char wrn, uint8_t command, uint8_t len)
{
	digitalWrite(nRFSN_CSN, LOW);	// select nRF

	// construct command byte
	if (wrn == 'w') {                           // Write
        SPI.transfer(W_REGISTER|command);       // Send command
    } else if (wrn == 'r') {                    // Read
        SPI.transfer(R_REGISTER|command);       // Send command
    } else if(wrn == 'n') {
        SPI.transfer(command);                  // Send command
    }

	// if there is data, do it byte at a time
    if (len != 0) {
		for (int i=1;i<=len;i++) {
            BufIn[i-1] = SPI.transfer(BufOut[i-1]);
        }
    }

	digitalWrite(nRFSN_CSN, HIGH);	// deselect nRF
}


/*------------------------------------------------
 * nRF transmit data.
 * uint8_t len: length of data in bytes
				- data to be sent must be in output buffer
-------------------------------------------------*/
void nRFSNClass::transmit(uint8_t len)
{
	if (len > 0)
	{	
		digitalWrite(nRFSN_CSN, LOW);	// select nRF

		SPI.transfer(0xA0);				// Send write payload command

		// write payload
		for (int i=0;i<len;i++) {
			SPI.transfer(BufOut[i]);
		}

		digitalWrite(nRFSN_CSN, HIGH);	// deselect nRF

		// toggle CE pin to transmit
		digitalWrite(nRFSN_CE, HIGH);
		delayMicroseconds(12);
		digitalWrite(nRFSN_CE, LOW);

		Busy = 1;	// Flag nRF24L01+ is busy
		
		delay(10);
	}
}


/*------------------------------------------------
 * Respond function - used to transmit data back upon command
------------------------------------------------*/
void nRFSNClass::respond(uint8_t len)
{
	setTXMode();	// switch to transmit mode
	currMode = 0;

	transmit(len);	// transmit response

	setRXMode();	// switch back to receiving mode
	currMode = 1;
}


/*------------------------------------------------
 * Gets size of received payload. Must be < 32 bytes
------------------------------------------------*/
uint8_t nRFSNClass::getPayloadSize(void)
{
	digitalWrite(nRFSN_CSN, LOW);

	SPI.transfer(R_RX_PL_WID);
	uint8_t payloadSize = SPI.transfer(NRF_NOP);

	digitalWrite(nRFSN_CSN, HIGH);
	
	// do not try and read more than 32 bytes.
	if (payloadSize > 32)
	{
		payloadSize = 32;
	}

	return payloadSize;
}


/*------------------------------------------------
 * Gets payload from nRF
------------------------------------------------*/
void nRFSNClass::getPayload(uint8_t payloadSize)
{
	digitalWrite(nRFSN_CE, LOW);

	transfer('n',R_RX_PAYLOAD,payloadSize);

	digitalWrite(nRFSN_CE, HIGH);
}


/*------------------------------------------------
 * Puts buffer (out) contents
------------------------------------------------*/
void nRFSNClass::putBufOut(uint8_t data[], uint8_t len)
{
	int i;
	for (i=0; i<len; i++) {
		BufOut[i] = data[i];
	}
}


/*------------------------------------------------
 * Gets buffer (in) contents
------------------------------------------------*/
uint8_t *nRFSNClass::getBufIn(uint8_t len)
{
	uint8_t *data = (uint8_t*)calloc(32, sizeof(uint8_t));
	memcpy(data,BufIn,len);
	return data;
}


/*------------------------------------------------
 * Converts float to char (float to 1 decimal place, x10)
------------------------------------------------*/
unsigned char nRFSNClass::toChar(float num)
{
	int rounded = int(num + 0.5);
	return char(rounded);
}