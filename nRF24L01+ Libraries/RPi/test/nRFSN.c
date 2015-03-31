#include <stdint.h>
#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
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
void init(uint8_t SPIDiv, uint8_t CEpin, uint8_t IRQpin) {

	printf("Pins.\n");
	nRFSN_CE = CEpin;
	nRFSN_IRQ = IRQpin;
	
	printf("bcm2835 Init().\n");
	bcm2835_init();
	
	printf("Begin.\n");
	bcm2835_spi_begin();
	
	printf("Bit order.\n");
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	printf("Mode0.\n");
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
	
	printf("SPI clock.\n");
	// if (!SPIDiv) {
		// SPIDiv = 64;
	// }
	bcm2835_spi_setClockDivider(64);
	
	printf("Chip select; polarity.\n");
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);

	printf("Pin settings.\n");
	bcm2835_gpio_fsel(nRFSN_CE, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(nRFSN_IRQ, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_write(nRFSN_CE, LOW);

	printf("Defaults.\n");
	// nRF defaults
	CONFIG_CURR       = 0b00001010;   // Show all TX interrupts; Enable CRC - 1 byte; Power Up; PTX
	EN_AA_CURR        = 0b00000011;   // Enable Auto Ack on pipe 0,1
	EN_RXADDR_CURR    = 0b00000011;   // Enable data pipe 0,1
	SETUP_AW_CURR     = 0b00000010;   // set for 4 byte address
	SETUP_RETR_CURR   = 0b00110101;   // 1000us retransmit delay; 5 auto retransmits
	RF_CH_CURR        = 0b01101001;   // Channel 105 (2.400GHz + 0.105GHz = 2.505GHz)
	RF_SETUP_CURR     = 0b00000110;   // RF data rate to 1Mbps; 0dBm output power (highest)
	DYNPD_CURR        = 0b00000011;   // Set dynamic payload for pipe 0
	FEATURE_CURR      = 0b00000100;   // Enable dynamic payload
	
	printf("Addresses.\n");
	int i;
	for (i = 0; i < 4; i++) {
		RX_ADDRESS[i] = 0xE7; // 4 byte initial RX address
		TX_ADDRESS[i] = 0xC7; // 4 byte initial TX address
	}

	printf("Configuration.\n");
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
	setRXAddr(RX_ADDR_P0,TX_ADDRESS,4);
	// set RX address
	setRXAddr(RX_ADDR_P1,RX_ADDRESS,4);
	// Set dynamic payload for pipe 0
	setReg(DYNPD,DYNPD_CURR);
	// Write to FEATURE register
	setReg(FEATURE,FEATURE_CURR);
	// Flush RX FIFO
	bcm2835_spi_transfer(FLUSH_RX);
	// Flush TX FIFO
	bcm2835_spi_transfer(FLUSH_TX);
}


/*------------------------------------------------
 * Set nRF to transmit mode
------------------------------------------------*/
void setTXMode(void)
{
	CONFIG_CURR = 0b01001010;
	setReg(CONFIG,CONFIG_CURR);
	currMode = 0;
}


/*------------------------------------------------
 * Set nRF to receive mode
------------------------------------------------*/
void setRXMode(void)
{
	CONFIG_CURR = 0b00101011;
	setReg(CONFIG,CONFIG_CURR);
	currMode = 1;
}


/*------------------------------------------------
 * Set nRF output power. 0: lowest 3: highest
------------------------------------------------*/
void setPower(uint8_t pwrLvl)
{
	RF_SETUP_CURR = pwrLvl << 1;			// shift 1 bit left
	setReg(RF_SETUP,RF_SETUP_CURR);
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
	setReg(RF_CH,RF_CH_CURR);
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
void setMAX_RT(uint8_t numRT) {
	// mask out current Auto Retransmit Delay, and OR it with numRT with upper 4 bits (numbers > 16) masked out
	// result is current ARD and new ARC values
	SETUP_RETR_CURR = (SETUP_RETR_CURR & 0b11110000) | (numRT & 0b00001111);
	setReg(SETUP_RETR,SETUP_RETR_CURR);
}


/*------------------------------------------------
 * Gets maximum number of retries
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
	uint8_t *data = (uint8_t*)calloc((len+1), sizeof(uint8_t));	// allocate space for temp array
	
	data[0] = W_REGISTER|TX_ADDR;		// set TX address command
	
	if (len != 0) {
		int i;
		for (i=1; i<=len; i++) {
			data[i] = addr[i-1];
		}
	}
	
	bcm2835_spi_transfern(data, len+1);	// transfer all bytes
	
	free(data);		// free memory
	
	// uint8_t packet[6];
    // packet[0] = W_REGISTER|TX_ADDR;

    // if (len != 0) {
        // //Send address bytes
        // for (int i=1;i<=len;i++) {
            // packet[i] = addr[i-1];
        // }
    // }

    // bcm2835_spi_transfern(packet,len+1);
}


/*------------------------------------------------
 * Sets new receive address
 * uint8_t pipe: pipe number (0-5); nRFSN uses 0, 1
 * uint8_t addr[]: new address
 * uint8_t len: length of address (nRFSN uses 4)
------------------------------------------------*/
void setRXAddr(uint8_t pipe, uint8_t addr[], uint8_t len)
{
	uint8_t *data = (uint8_t*)calloc((len+1), sizeof(uint8_t));	// allocate space for temp array
	
	data[0] = W_REGISTER|pipe;		// set TX address for pipe command
	
	if (len != 0) {
		int i;
		for (i=1; i<=len; i++) {
			data[i] = addr[i-1];
		}
	}
	
	bcm2835_spi_transfern(data, len+1);	// transfer all bytes
	
	free(data);		// free memory
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
 * Clears interrupt flag on nRF
 * uint8_t interrupt: interrupt number (uses defined constants)
 *	- valid values: MAX_RT, RX_DR, TX_DS
------------------------------------------------*/
void clearInt(uint8_t interrupt)
{
	setReg(STATUS,(interrupt << 4));
}


/*------------------------------------------------
 * Updates nRF status variable
------------------------------------------------*/
uint8_t updateStatus(void) {
	Status = bcm2835_spi_transfer(NRF_NOP);
	return Status;
}


/*------------------------------------------------
 * Sets config register of nRF
------------------------------------------------*/
void setReg(uint8_t reg, uint8_t regData)
{
	uint8_t *data = (uint8_t*)calloc(2, sizeof(uint8_t));	// allocate space for temp array
	
	data[0] = W_REGISTER|reg;		// set TX address for pipe command
	
	data[1] = regData;
	
	bcm2835_spi_transfern(data, 2);	// transfer all bytes
	
	free(data);		// free memory
	
	// digitalWrite(nRFSN_CSN, LOW);	// select nRF
	
	// SPI.transfer(W_REGISTER|reg);       // Send command
	// SPI.transfer(data);

	// digitalWrite(nRFSN_CSN, HIGH);	// deselect nRF
}


/*------------------------------------------------
 * Gets config register of nRF
------------------------------------------------*/
uint8_t getReg(uint8_t reg)
{
	uint8_t *data = (uint8_t*)calloc(2, sizeof(uint8_t));	// allocate space for temp array
	
	data[0] = R_REGISTER|reg;		// set TX address for pipe command
	
	data[1] = NRF_NOP;
	
	bcm2835_spi_transfern(data, 2);	// transfer all bytes
	
	free(data);		// free memory

	return data[1];
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
		uint8_t *data = (uint8_t*)calloc((len+1), 1);
		memcpy(&data[1], BufOut, len);
        data[0] = 0xA0;		// Send write payload command

        // write payload
        bcm2835_spi_transfernb(data,BufIn,len+1);

        // toggle CE pin to transmit
        bcm2835_gpio_write(nRFSN_CE, HIGH);
        bcm2835_delayMicroseconds(11);
        bcm2835_gpio_write(nRFSN_CE, LOW);
    }
}


/*------------------------------------------------
 * Gets size of received payload. Must be < 32 bytes
------------------------------------------------*/
uint8_t getPayloadSize(void) {

	uint8_t data[2] = {R_RX_PL_WID, NRF_NOP};
	bcm2835_spi_transfern(data,2);
	return data[1];
}

/*------------------------------------------------
 * Gets payload from nRF
------------------------------------------------*/
void getPayload(uint8_t payloadSize) {
	
	uint8_t *data = (uint8_t*)calloc(payloadSize, sizeof(uint8_t));
	data[0] = R_RX_PL_WID;
	
	int i;
	for (i=1; i<payloadSize; i++) {
		data[i] = NRF_NOP;
	}
	
	bcm2835_spi_transfernb(BufIn,data,32);
}


/*------------------------------------------------
 * Put data of size len into SPI I/O buffer
 * len <= 32
------------------------------------------------*/
void putBufOut(uint8_t* data, uint8_t len) {

	memcpy(BufOut,data,len);

	// if (len != 0) {
		// for (int i = 0; i < len; i++) {
			// Buf[i+1] = bufData[i];
		// }
	// }
}


/*------------------------------------------------
 * Get data of size len into SPI I/O buffer
------------------------------------------------*/
uint8_t *getBufIn(uint8_t len) {
	
	static uint8_t data[32];
	memcpy(data,BufIn,len);
	return data;

	// if (len != 0) {
		// for (int i = 0; i < len; i++) {
			 // bufData[i] = Buf[i];
		// }
	// }
}



/*------------------------------------------------
 * nRF configure register.
 * uint8_t wrn: write('w')/read('r')
 * uint8_t command: command byte
 *					- command byte is ORed with W_REGISTER, R_REGISTER
 * uint8_t data: data byte to send
 * returns data byte. If writing to register, will return 0x00
------------------------------------------------*/
// uint8_t configReg(uint8_t wr, uint8_t command, uint8_t data) {
	// uint8_t packet[2];
	// if (wr == 'w') {                           // Write
		// packet[1] = data;
        // packet[0] = W_REGISTER|command;        // Construct command
        // bcm2835_spi_transfern(packet,2);
    // } else if (wr == 'r') {                    // Read
    	// packet[1] = NRF_NOP;
    	// packet[0] = R_REGISTER|command;        // Construct command
    	// bcm2835_spi_transfern(packet,2);
    // }

    // return packet[1];
// }


/*------------------------------------------------
 * nRF transfer data. DO NOT USE TO TRANSMIT!
 * uint8_t wrn: write('w')/read('r')/none('n')
 * uint8_t command: command byte
 *					- if wrn = 'w' or 'r', command byte is ORed with W_REGISTER, R_REGISTER
 * uint8_t len: length of data in bytes
				- data to be sent must be in i/o buffer
 * uint8_t offet: where in the buffer to start read/write
------------------------------------------------*/
// void transfer(uint8_t wrn, uint8_t command, uint8_t len, uint8_t offset)
// {
    // // construct command byte
    // if (wrn == 'w') {                       // Write
        // Buf[0] = W_REGISTER|command;     // Send command
    // } else if (wrn == 'r') {                // Read
    	// Buf[0] = R_REGISTER|command;     // Send command
    // } else if(wrn == 'n') {
    	// Buf[0] = command;                // Send command
    	// len++;								// make sure command is sent
    // }

	// // if there is data, do it byte at a time
    // if (len != 0) {
        // bcm2835_spi_transfern(Buf,len);
    // }
// }



void close(void) {
	bcm2835_spi_end();
	bcm2835_close();
}


int main(void) {
	printf("Starting test.\n");
	bcm2835_init();
	
	printf("Initializing.\n");
	init(64, RPI_GPIO_P1_22, RPI_GPIO_P1_07);
	
	printf("Setting TX mode.\n");
	setTXMode();

	printf("Closing.\n");
	close();
	
	printf("Finished test.\n");

	return 0;
}
