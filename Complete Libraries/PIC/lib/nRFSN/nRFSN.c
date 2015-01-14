#include <xc.h>
#include <string.h>

#include "nRFSN.h"

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
void init(void)
{
    nRFSN_CE = HIGH;
    nRFSN_CSN = HIGH;

    initSPI();

    // nRF defaults
    uint8_t CONFIG_CURR       = 0b01001010;   // Show all TX interrupts; Enable CRC - 1 byte; Power Up; PTX
    uint8_t EN_AA_CURR        = 0b00000011;   // Enable Auto Ack on pipe 0,1
    uint8_t EN_RXADDR_CURR    = 0b00000011;   // Enable data pipe 0,1
    uint8_t SETUP_AW_CURR     = 0b00000010;   // set for 4 byte address
    uint8_t SETUP_RETR_CURR   = 0b00110101;   // 1000us retransmit delay; 5 auto retransmits
    uint8_t RF_CH_CURR        = 0b01101001;   // Channel 105 (2.400GHz + 0.105GHz = 2.505GHz)
    uint8_t RF_SETUP_CURR     = 0b00000110;   // RF data rate to 1Mbps; 0dBm output power (highest)
    uint8_t DYNPD_CURR        = 0b00000001;   // Set dynamic payload for pipe 0
    uint8_t FEATURE_CURR      = 0b00000100;   // Enable dynamic payload
    uint8_t RX_ADDRESS[4] = {0xE7,0xE7,0xE7,0xE7}; // 4 byte initial RX address
    uint8_t TX_ADDRESS[4] = {0xC7,0xC7,0xC7,0xC7}; // 4 byte initial TX address

    if (!checkAddrs())		// Is there an address in EEPROM?
    {
        // If no EEPROM stored addresses, set deafults
        for (int i=0;i<4;i++)
        {
            RX_ADDRESS[i] = 0xE7;
            TX_ADDRESS[i] = 0xE7;
        }
    }

    // nRF24L01+ setup
    // Write to CONFIG register
    setReg('w',CONFIG,CONFIG_CURR);
    // Write to EN_RXADDR register
    setReg('w',EN_RXADDR,EN_RXADDR_CURR);
    // Write to EN_AA register
    setReg('w',EN_AA,EN_AA_CURR);
    // Write to SETUP_AW register
    setReg('w',SETUP_AW,SETUP_AW_CURR);
    // Write to SETUP_RETR register
    setReg('w',SETUP_RETR,SETUP_RETR_CURR);
    // Write to RF channel register
    setReg('w',RF_CH,RF_CH_CURR);
    // Write to RF setup register
    setReg('w',RF_SETUP,RF_SETUP_CURR);
    // set TX address
    setTXAddr(TX_ADDRESS,4);
    // set RX address
    setRXAddr(RX_ADDR_P0,RX_ADDRESS,4);
    // Set dynamic payload for pipe 0
    setReg('w',DYNPD,DYNPD_CURR);
    // Write to FEATURE register
    setReg('w',FEATURE,FEATURE_CURR);
    // Flush RX FIFO
    transfer('n',FLUSH_RX,0,0);
    // Flush TX FIFO
    transfer('n',FLUSH_TX,0,0);

	// current mode is RX
	currMode = 1;	// 1 = RX, 0 = TX
	
    // Initialize interrupt flags
    RXInt = 0;
    TXInt = 0;
    MAXInt = 0;
}


/*------------------------------------------------
 * Check EEPROM for valid addresses
------------------------------------------------*/
uint8_t checkAddrs(void)
{
    uint8_t newAddr = 0;

    uint8_t addrByte = rEEPROM(0);		// get first byte of EEPROM

    if (addrByte != 0xFF)			// addresses NEVER start with 255. If 255 was read from EEPROM,
    {                                           // then device has never been synced before
        for (uint8_t i=0; i<4; i++)
        {
            TX_ADDRESS[i] = rEEPROM(i);         // If valid addresses was found in EEPROM, get them
            RX_ADDRESS[i] = rEEPROM(i+4);
        }

        // set TX address
        setTXAddr(TX_ADDRESS,4);
        // set RX address
        setRXAddr(RX_ADDR_P0,RX_ADDRESS,4);

        newAddr = 1;				// flag success
    }

    return newAddr;				// return 0: no addresses found 1: addresses found
}


/*------------------------------------------------
 * Initialize SPI setting and enable
 * uint8_t SPISpeed: SPI frequency divider (default is SPI_CLOCK_DIV4)
 * - SPI_CLOCK_DIVx
 *	where valid x values are: 4, 8, 16, 32, 64, 128
------------------------------------------------*/
void initSPI()
{
    SSP1CON1bits.CKP = 0;               // Clock polarity
    SSP1STATbits.CKE = 1;               // Clock edge detect
    SSP1STATbits.SMP = 1;               // Sample bit

    SSP1ADD = 0x03;                     // Set to 3
    SSP1CON1bits.SSPM = 0b1010;         // Clock = Fosc/(SSP1ADD + 1)(4) = 4MHz (default)

    SSP1CON1bits.SSPEN = 1;             // Enable SPI

    nRFSN_CSN = HIGH;                   // Start chip select pin high
}


/*------------------------------------------------
 * Sets speed of SPI bus
 * uint16_t freq: SPI frequency in KHz
------------------------------------------------*/
void setSPIFreq(uint16_t freq)
{
    // Clock = Fosc/(SSP1ADD + 1)(4) = freq/1000 (MHz)
    uint16_t sspAddVal = (((_XTAL_FREQ/1000)/freq)/4)-1;
    SSP1ADD = sspAddVal;
}


/*------------------------------------------------
 * Set nRF to transmit mode
------------------------------------------------*/
void setTXMode(void)
{
    CONFIG_CURR = 0b01001010;
    setReg('w',CONFIG,CONFIG_CURR);
	currMode = 0;
}


/*------------------------------------------------
 * Set nRF to receive mode
------------------------------------------------*/
void setRXMode(void)
{
    CONFIG_CURR = 0b00101011;
    setReg('w',CONFIG,CONFIG_CURR);
	currMode = 1;
}


/*------------------------------------------------
 * Returns current mode - 0 TX, 1 RX
------------------------------------------------*/
uint8_t getMode(void)
{
	return currMode;
}


/*------------------------------------------------
 * Set nRF output power. 0: lowest 3: highest
------------------------------------------------*/
void setPower(uint8_t pwrLvl)
{
    RF_SETUP_CURR = pwrLvl << 1;		// shift 1 bit left
    setReg('w',RF_SETUP,RF_SETUP_CURR);
}


/*------------------------------------------------
 * Gets power configuration of nRF
------------------------------------------------*/
uint8_t getPower(void)
{
	return RF_SETUP_CURR & 6;
}


/*------------------------------------------------
 * Set nRF channel (Fo = 2400 + channel [MHz])
------------------------------------------------*/
void setChannel(uint8_t ch)
{
    RF_CH_CURR = ch;
    setReg('w',RF_CH,RF_CH_CURR);
}


/*------------------------------------------------
 * Gets current channel of nRF
------------------------------------------------*/
uint8_t getChannel(void)
{
	return RF_CH_CURR & 127;
}


/*------------------------------------------------
 * Set nRF max number of retransmits
 * uint8_t numRT: Auto Retransmit Count (0: 250us 1111(15): 4000us; each +1 = +150us)
------------------------------------------------*/
void setMaxRT(uint8_t numRT)
{
    // mask out current Auto Retransmit Delay, and OR it with numRT with upper 4 bits (numbers > 16) masked out
    // result is current ARD and new ARC values
    SETUP_RETR_CURR = (SETUP_RETR_CURR & 0b11110000) | (numRT & 0b00001111);
    setReg('w',SETUP_RETR,SETUP_RETR_CURR);
}


/*------------------------------------------------
 * Sets maximum number of retries
------------------------------------------------*/
uint8_t getMaxRT(void)
{
	return SETUP_RETR_CURR & 15;
}


/*------------------------------------------------
 * Sets new transmit address
 * uint8_t addr[]: new address
 * uint8_t len: length of address (nRFSN uses 4)
------------------------------------------------*/
void setTXAddr(uint8_t addr[], uint8_t len)
{
    nRFSN_CSN = LOW;	// select nRF

    spiTransfer(W_REGISTER|TX_ADDR);

    if (len != 0) {
        // Send address bytes
        for (int i=1;i<=len;i++) {
            spiTransfer(addr[i-1]);
        }
    }

    nRFSN_CSN = HIGH;	// deselect nRF
}


/*------------------------------------------------
 * Sets new receive address
 * uint8_t pipe: pipe number (0-5); nRFSN uses 0, 1
 * uint8_t addr[]: new address
 * uint8_t len: length of address (nRFSN uses 4)
------------------------------------------------*/
void setRXAddr(uint8_t pipe, uint8_t addr[], uint8_t len)
{
    nRFSN_CSN = LOW;	// select nRF

    spiTransfer(W_REGISTER|pipe);

    if (len != 0) {
        // Send address bytes
        for (int i=1;i<=len;i++) {
            spiTransfer(addr[i-1]);
        }
    }

    nRFSN_CSN = HIGH;	// deselect nRF
}


/*------------------------------------------------
 * Returns current TX address
------------------------------------------------*/
uint8_t *getTXAddr(void)
{
	static uint8_t addr[4];
	memcpy(addr,TX_ADDRESS,4);
	return addr;
}


/*------------------------------------------------
 * Returns current RX address
------------------------------------------------*/
uint8_t *getRXAddr(void)
{
	static uint8_t addr[4];
	memcpy(addr,RX_ADDRESS,4);
	return addr;
}


/*------------------------------------------------
 * IRQ pin interrupt service routine
------------------------------------------------*/
void nRF_ISR(void)
{
    updateStatus();			// Get current nRF status

    if (Status & 0b01000000)		// If data received IRQ
    {
        RXInt = 1;
    }
    else if (Status & 0b00100000)	// If data sent IRQ
    {
        TXInt = 1;
        Busy = 0;			// if data sent, nRF no longer busy
    }
    else if (Status & 0b00010000)	// If max retransmits IRQ
    {
        MAXInt = 1;
        Busy = 0;			// nRF no longer busy
    }
}


/*------------------------------------------------
 * Clears interrupt flag on nRF
 * uint8_t interrupt: interrupt number (uses defined constants)
 *	- valid values: MAX_RT, RX_DR, TX_DS
------------------------------------------------*/
void clearInt(uint8_t intNum)
{
    setReg('w',STATUS,(intNum << 4));
}


/*------------------------------------------------
 * Updates nRF status variable
------------------------------------------------*/
uint8_t updateStatus(void)
{
    nRFSN_CSN = LOW;	// select nRF

    Status = spiTransfer(NRF_NOP);

    nRFSN_CSN = HIGH;	// deselect nRF
	
	return Status;
}


/*------------------------------------------------
 * nRFSN sync function. Syncs to Root (RPi) and stores new TX, RX addresses.
 * Returns 0: Sync fail 1: Sync succeed
------------------------------------------------*/
uint8_t sync(void)
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
        addr1[i] = 0xE7;
        addr2[i] = 0xE7;
    }
    setRXAddr(RX_ADDR_P0,addr1,4);
    setTXAddr(addr2,4);

    // set to receive mode
    setRXMode();
    uint32_t time = 0;
    uint8_t synced = 0;
    while ((time < 600000) && (synced == 0))		// while less than 30s and unsynced
    {
        if (RXInt)					// has a packet been received?
        {
            uint8_t size = getPayloadSize();            // If so, get the size
            getPayload(size,0);				// then get the packet data and put in buffer

            if ((BufIn[0] == SYNC_ADDR) && (size == 9))	// check for sync command and packet size of 9
            {

                for (uint8_t i=0; i<4; i++)		// if valid sync packet, write new addresses to EEPROM
                {
                    wEEPROM(i,BufIn[i+1]);
                    wEEPROM(i+4,BufIn[i+5]);
                }

                synced = 1;				// stop syncing
            }
            else
            {
                break;					// packet was not a valid sync packet
            }
        }
        __delay_us(500);				// wait 1ms
        time++;						// increment time variable
    }

    if (synced) {					// if sync was successful, set new addresses
        for (uint8_t i=0; i<4; i++)
        {
            TX_ADDRESS[i] = BufIn[i+1];
            RX_ADDRESS[i] = BufIn[i+5];
        }
    } else {						// if sync failed, restore prev addresses and delete garbage
        memcpy(TX_ADDRESS,prevTXAddr,4);
        memcpy(RX_ADDRESS,prevRXAddr,4);
    }

    // set new TX address
    setTXAddr(TX_ADDRESS,4);
    // set new RX address
    setRXAddr(RX_ADDR_P0,RX_ADDRESS,4);

    RXInt = 0;

    return synced;					// return 0 if failed, 1 if sucessful
}


/*------------------------------------------------
 * nRF configure register.
 * char wrn: write('w')/read('r')
 * uint8_t command: command byte
 *					- command byte is ORed with W_REGISTER, R_REGISTER
 * uint8_t data: data byte to send
 * returns data byte. If writing to register, will return 0x00
------------------------------------------------*/
// uint8_t configReg(char wr, uint8_t command, uint8_t data)
// {
    // nRFSN_CSN = LOW;	// select nRF

    // if (wr == 'w') {                           // Write
        // spiTransfer(W_REGISTER|command);       // Send command
        // spiTransfer(data);
    // } else if (wr == 'r') {                    // Read
        // spiTransfer(R_REGISTER|command);       // Send command
        // data = spiTransfer(NRF_NOP);
    // }

    // nRFSN_CSN = HIGH;	// deselect nRF

    // return data;
// }


/*------------------------------------------------
 * Sets config register of nRF
------------------------------------------------*/
void setReg(uint8_t reg, uint8_t data)
{
	nRFSN_CSN = LOW;	// select nRF
	
	spiTransfer(W_REGISTER|reg);       // Send command
	spiTransfer(data);

	nRFSN_CSN = HIGH;	// deselect nRF
}


/*------------------------------------------------
 * Gets config register of nRF
------------------------------------------------*/
uint8_t getReg(uint8_t reg)
{
	nRFSN_CSN = LOW;	// select nRF
	
	spiTransfer(R_REGISTER|reg);   // Send command
	data = spiTransfer(NRF_NOP);

	nRFSN_CSN = HIGH;	// deselect nRF

	return data;
}


/*------------------------------------------------
 * nRF transfer data. DO NOT USE TO TRANSMIT!
 * char wrn: write('w')/read('r')/none('n')
 * uint8_t command: command byte
 *					- if wrn = 'w' or 'r', command byte is ORed with W_REGISTER, R_REGISTER
 * uint8_t len: length of data in bytes
				- data to be sent must be in output buffer
 * uint8_t offet: where in the buffer to start read/write
------------------------------------------------*/
void transfer(char wrn, uint8_t command, uint8_t len)
{
    nRFSN_CSN = LOW;	// select nRF

    // construct command byte
    if (wrn == 'w') {                          // Write
        spiTransfer(W_REGISTER|command);       // Send command
    } else if (wrn == 'r') {                   // Read
        spiTransfer(R_REGISTER|command);       // Send command
    } else if(wrn == 'n') {
        spiTransfer(command);                  // Send command
    }

	// if there is data, do it byte at a time
    if (len != 0) {
        for (int i=1;i<=len;i++) {
            BufIn[i-1] = spiTransfer(BufOut[i-1]);
        }
    }

    nRFSN_CSN = HIGH;	// deselect nRF
}


/*------------------------------------------------
 * SPI transfer function: send and receive 1 byte
 * uint8_t data: byte to be sent
 * returns: uint8_t: byte received
------------------------------------------------*/
uint8_t spiTransfer(uint8_t data)
{
    SSP1BUF = data;                     // Write data to MSSP

    while (!PIR1bits.SSP1IF) { }        // Wait for transfer to complete
    PIR1bits.SSP1IF = 0;                // Clear flag

    return SSP1BUF;                     // return recieved data
}


/*------------------------------------------------
 * nRF transmit data.
 * uint8_t len: length of data in bytes
				- data to be sent must be in output buffer
-------------------------------------------------*/
void transmit(uint8_t len)
{
    if (len > 0)
    {
        nRFSN_CSN = LOW;	// select nRF

        spiTransfer(0xA0);		// Send write payload command

        // write payload
        for (int i=1;i<=len;i++) {
            spiTransfer(BufOut[i-1]);
        }

        nRFSN_CSN = HIGH;	// deselect nRF

        // toggle CE pin to transmit
        nRFSN_CE = HIGH;
        __delay_us(12);
        nRFSN_CE = LOW;

        Busy = 1;                       // Flag nRF24L01+ is busy
    }
}


/*------------------------------------------------
 * Respond function - used to transmit data back upon command
------------------------------------------------*/
void respond(uint8_t len)
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
uint8_t getPayloadSize(void)
{
    spiTransfer(R_RX_PL_WID);
    uint8_t payloadSize = spiTransfer(NRF_NOP);

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
void getPayload(uint8_t payloadSize)
{
    transfer('r',R_RX_PAYLOAD,payloadSize);
}


/*------------------------------------------------
 * EEPROM write function
------------------------------------------------*/
void wEEPROM(uint8_t data, uint8_t address)
{
    EEADR = address;            // Point to address
    EEDATA = data;              // Write data to register

    EECON1bits.EEPGD = 0;       // Access EEPROM
    EECON1bits.CFGS = 0;        // Access EEPROM/Flash
    EECON1bits.WREN = 1;        // Enable write access

    INTCONbits.GIEH = 0;        // Disable interrupts
    INTCONbits.GIEL = 0;

    // Start write sequence
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;          // Initiate write operation

    INTCONbits.GIEH = 1;        // Enable interrupts
    INTCONbits.GIEL = 1;

    while (!PIR2bits.EEIF) { }; // Wait for operation to complete
    PIR2bits.EEIF = 0;          // Clear interrupt flag

    EECON1bits.WREN = 1;        // Disable write access
}


/*------------------------------------------------
 * EEPROM read function
------------------------------------------------*/
uint8_t rEEPROM(uint8_t address)
{
    EEADR = address;            // Point to address

    EECON1bits.EEPGD = 0;       // Access EEPROM
    EECON1bits.CFGS = 0;        // Access EEPROM/Flash
    EECON1bits.RD = 1;          // Initiate read operation

    return EEDATA;              // Return read data
}