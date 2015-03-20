#include "lib.hpp"

nRFSN::nRFSN(char SPIDiv, char CEpin, char IRQpin, char LEDpin)
{
	printf("Init main\n");

	this->CEpin = CEpin;
	this->IRQpin = IRQpin;
	this->LEDpin = LEDpin;

	bcm2835_init();

	bcm2835_delay(100);

	init_spi(SPIDiv);

	bcm2835_gpio_fsel(this->CEpin, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(this->LEDpin, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(this->IRQpin, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_write(this->CEpin, LOW);

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

	// setup addresses
	char RX[4] = {0xE7,0xE7,0xE7,0xE7}; // 4 byte initial RX address
	char TX[4] = {0xC7,0xC7,0xC7,0xC7}; // 4 byte initial TX address

	/*------------------------------------------------
	 * nRF24L01+ setup
	------------------------------------------------*/
	// Write to CONFIG register
	setReg(CONFIG, CONFIG_CURR);

	// Write to EN_RXADDR register
	setReg(EN_RXADDR, EN_RXADDR_CURR);

	// Write to EN_AA register
	setReg(EN_AA, EN_AA_CURR);

	// Write to SETUP_AW register
	setReg(SETUP_AW, SETUP_AW_CURR);

	// Write to SETUP_RETR register
	setReg(SETUP_RETR, SETUP_RETR_CURR);

	// Write to RF channel register
	setReg(RF_CH, RF_CH_CURR);

	// Write to RF setup register
	setReg(RF_SETUP, RF_SETUP_CURR);

	// set TX address
	setTXAddr(TX,4);

	// set RX address
	setRXAddr(RX_ADDR_P0,TX,4);

	// set RX address
	setRXAddr(RX_ADDR_P1,RX,4);

	// Set dynamic payload for pipe 0
	setReg(DYNPD, DYNPD_CURR);

	// Write to FEATURE register
	setReg(FEATURE, FEATURE_CURR);

	// Flush RX FIFO
	bcm2835_spi_transfer(FLUSH_RX);

	// Flush TX FIFO
	bcm2835_spi_transfer(FLUSH_TX);

	// clear all interrupts
	clearInt(7);
}


/*------------------------------------------------
 * Initialize bcm2835 SPI settings
 *     SPIDiv - SPI frequency divider
 *            - power of 2
------------------------------------------------*/
void nRFSN::init_spi(char SPIDiv)
{
	printf("Init SPI\n");

	bcm2835_spi_begin();

	bcm2835_delay(100);

	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);

	if (!SPIDiv) {
		SPIDiv = 64;
	}
	bcm2835_spi_setClockDivider(64);

	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
}


/*------------------------------------------------
 * Set nRF to transmit mode
------------------------------------------------*/
void nRFSN::setTXMode(void)
{
	setReg(CONFIG, (CONFIG_CURR & 0b11111110) );
	bcm2835_gpio_write(this->CEpin, LOW);
	currMode = 0;
	bcm2835_delayMicroseconds(140);
}


/*------------------------------------------------
 * Set nRF to receive mode
------------------------------------------------*/
void nRFSN::setRXMode(void)
{
	setReg(CONFIG, (CONFIG_CURR | 0b00000001) );
	bcm2835_gpio_write(this->CEpin, HIGH);
	currMode = 1;
	bcm2835_delayMicroseconds(140);
}


/*------------------------------------------------
 * Returns current mode - 0 TX, 1 RX
------------------------------------------------*/
char nRFSN::getMode(void)
{
	char currConfig = getReg(CONFIG);
	return (currConfig & 0b00000001);
}


/*------------------------------------------------
 * Set nRF output power. 0: lowest 3: highest
------------------------------------------------*/
void nRFSN::setPower(char pwrLvl)
{
	RF_SETUP_CURR = pwrLvl << 1;			// shift 1 bit left
	setReg(RF_SETUP, RF_SETUP_CURR);
}


/*------------------------------------------------
 * Gets power configuration of nRF
------------------------------------------------*/
char nRFSN::getPower(void)
{
	char currConfig = getReg(RF_SETUP);
	return (currConfig & 0b00000110) >> 1;
}


/*------------------------------------------------
 * Set nRF channel (Fo = 2400 + channel [MHz])
------------------------------------------------*/
void nRFSN::setChannel(char ch)
{
	RF_CH_CURR = ch;
	setReg(RF_CH, RF_CH_CURR);
}


/*------------------------------------------------
 * Gets current channel of nRF
------------------------------------------------*/
char nRFSN::getChannel(void)
{
	char currConfig = getReg(RF_CH);
	return currConfig;
}


/*------------------------------------------------
 * Set nRF max number of retransmits
 * char numRT: Auto Retransmit Count (0: 250us 1111(15): 4000us; each +1 = +150us)
------------------------------------------------*/
void nRFSN::setMaxRT(char numRT)
{
	// mask out current Auto Retransmit Delay, and OR it with numRT with upper 4 bits (numbers > 16) masked out
	// result is current ARD and new ARC values
	SETUP_RETR_CURR = (SETUP_RETR_CURR & 0b11110000) | (numRT & 0b00001111);
	setReg(SETUP_RETR, SETUP_RETR_CURR);
}


/*------------------------------------------------
 * Gets maximum number of retries
------------------------------------------------*/
char nRFSN::getMaxRT(void)
{
	char currConfig = getReg(SETUP_RETR);
	return (currConfig & 0b00001111);
}


/*------------------------------------------------
 * Set nRF max number of retransmits
 * char numRT: Auto Retransmit Count (0: 250us 1111(15): 4000us; each +1 = +150us)
------------------------------------------------*/
void nRFSN::setMaxRTdelay(char delay)
{
	// mask out current Auto Retransmit Delay, and OR it with numRT with upper 4 bits (numbers > 16) masked out
	// result is current ARD and new ARC values
	SETUP_RETR_CURR = ((delay << 4) & 0b11110000) | (SETUP_RETR_CURR & 0b00001111);
	setReg(SETUP_RETR,SETUP_RETR_CURR);
}


/*------------------------------------------------
 * Sets maximum number of retries
------------------------------------------------*/
char nRFSN::getMaxRTdelay(void)
{
	char currConfig = getReg(SETUP_RETR);
	return (currConfig & 0b11110000) >> 4;
}


/*------------------------------------------------
 * Sets new transmit address
 * char addr[]: new address
 * char len: length of address (nRFSN uses 4)
------------------------------------------------*/
void nRFSN::setTXAddr(char addr[], char len)
{
	memcpy(TX_ADDRESS,addr,4);

	char *data = (char*)calloc((len+1), sizeof(char));	// allocate space for temp array

	data[0] = W_REGISTER|TX_ADDR;		// set TX address command

	memcpy(&data[1], TX_ADDRESS, len);		// copy address into data

	bcm2835_spi_transfern(data, len+1);	// transfer all bytes

	free(data);							// free memory

}


/*------------------------------------------------
 * Sets new receive address
 * char pipe: pipe number (0-5); nRFSN uses 0, 1
 * char addr[]: new address
 * char len: length of address (nRFSN uses 4)
------------------------------------------------*/
void nRFSN::setRXAddr(char pipe, char addr[], char len)
{
	memcpy(RX_ADDRESS,addr,4);

	char *data = (char*)calloc((len+1), sizeof(char));	// allocate space for temp array

	data[0] = W_REGISTER|pipe;		// set TX address for pipe command

	memcpy(&data[1], RX_ADDRESS, len);		// copy address into data

	bcm2835_spi_transfern(data, len+1);	// transfer all bytes

	free(data);		// free memory

}


/*------------------------------------------------
 * Returns current TX address
------------------------------------------------*/
char *nRFSN::getTXAddr(void)
{
	char *data = (char*)calloc((5), sizeof(char));	// allocate space for temp array

	data[0] = R_REGISTER|TX_ADDR;		// set TX address for pipe command

	// Send address bytes
	for (int i=1;i<=4;i++) {
		data[i] = NRF_NOP;
	}

	bcm2835_spi_transfern(data, 5);	// transfer all bytes

	return &data[1];
}


/*------------------------------------------------
 * Returns current RX address
------------------------------------------------*/
char *nRFSN::getRXAddr(char pipe)
{
	char *data = (char*)calloc((5), sizeof(char));	// allocate space for temp array

	data[0] = R_REGISTER|pipe;		// set TX address for pipe command

	// Send address bytes
	for (int i=1;i<=4;i++) {
		data[i] = NRF_NOP;
	}

	bcm2835_spi_transfern(data, 5);	// transfer all bytes

	return &data[1];
}


/*------------------------------------------------
 * Clears interrupt flag on nRF
 * char interrupt: interrupt number (uses defined constants)
 *	- valid values: MAX_RT, RX_DR, TX_DS
------------------------------------------------*/
void nRFSN::clearInt(char interrupt)
{
	setReg(STATUS, (interrupt << 4));
}


/*------------------------------------------------
 * Updates nRF status variable
------------------------------------------------*/
char nRFSN::updateStatus(void)
{
	Status = bcm2835_spi_transfer(NRF_NOP);
	return Status;
}


/*------------------------------------------------
 * Sets config register of nRF
------------------------------------------------*/
void nRFSN::setReg(char reg, char regData)
{
	char *data = (char*)calloc(2, sizeof(char));	// allocate space for temp array

	data[0] = W_REGISTER|reg;		// set TX address for pipe command

	data[1] = regData;

	bcm2835_spi_transfern(data, 2);	// transfer all bytes

	free(data);		// free memory
}


/*------------------------------------------------
 * Gets config register of nRF
------------------------------------------------*/
char nRFSN::getReg(char reg)
{
	char *data = (char*)calloc(2, sizeof(char));	// allocate space for temp array

	data[0] = R_REGISTER|reg;		// set TX address for pipe command

	data[1] = NRF_NOP;

	bcm2835_spi_transfern(data, 2);	// transfer all bytes

	return data[1];
}


/*------------------------------------------------
 * nRF transmit data.
 * char len: length of data in bytes
				- data to be sent must be in output buffer
-------------------------------------------------*/
void nRFSN::transmit(char len)
{
    if (len > 0)
    {
    	char dataBuf[32];

		int i;
		for (i=1;i<=len;i++) {
			dataBuf[i] = BufOut[i-1];
		}

		dataBuf[0] = 0xA0;		// Send write payload command

        //bcm2835_gpio_write(LEDpin, HIGH);

        // write payload
        bcm2835_spi_transfern(dataBuf,len+1);

        // toggle CE pin to transmit
        bcm2835_gpio_write(CEpin, HIGH);
        bcm2835_delayMicroseconds(12);
        bcm2835_gpio_write(CEpin, LOW);

        bcm2835_delay(15);

        //bcm2835_gpio_write(LEDpin, LOW);

        bcm2835_delay(285);

        for (i=1;i<=len;i++) {
        	BufIn[i-1] = dataBuf[i];
		}
    }
}


/*------------------------------------------------
 * Gets size of received payload. Must be < 32 bytes
------------------------------------------------*/
char nRFSN::getPayloadSize(void)
{
	char data[2] = {R_RX_PL_WID, NRF_NOP};
	bcm2835_spi_transfern(data,2);
	return data[1];
}


/*------------------------------------------------
 * Gets payload from nRF
------------------------------------------------*/
void nRFSN::getPayload(char payloadSize)
{
	bcm2835_gpio_write(LEDpin, HIGH);

	char data[33];
	data[0] = R_RX_PAYLOAD;

	int i;
	for (i=1; i<=payloadSize; i++) {
		data[i] = NRF_NOP;
	}

	bcm2835_spi_transfern(data,(payloadSize+1));

	for (i=1;i<=payloadSize;i++) {
		BufIn[i-1] = data[i];
		printf("%#04x,", data[i]);
	}
	printf("\n");

	bcm2835_gpio_write(LEDpin, LOW);
}


/*------------------------------------------------
 * Put data of size len into SPI I/O buffer
 * len <= 32
------------------------------------------------*/
void nRFSN::putBufOut(char dataOut[], char len)
{
	int i;
	for (i=0;i<len;i++) {
		BufOut[i] = dataOut[i];
	}
}

/*------------------------------------------------
 * Put data of size len into SPI I/O buffer
 * len <= 32
------------------------------------------------*/
void nRFSN::putBufOut(char dataOut)
{
	BufOut[0] = dataOut;
}

/*------------------------------------------------
 * Get data of size len into SPI I/O buffer
------------------------------------------------*/
void *nRFSN::getBufIn(char dataIn[], char len)
{
	int i;
	for (i=0;i<len;i++) {
		dataIn[i] = BufIn[i];
	}
}


/*------------------------------------------------
 * Ends bcm2835 SPI and closes bcm2835 library
------------------------------------------------*/
void nRFSN::stop()
{
	bcm2835_spi_end();
	bcm2835_close();
}



/*------------------------------------------------
 * Test main program
------------------------------------------------*/
static volatile int done = 0;

void sig_handler(int signum)
{
	printf("SIGINT caught!\n");
	done = 1;
}

int main(void) {

	printf("Main\n");

	signal(SIGINT, sig_handler);

	nRFSN nRF(BCM2835_SPI_CLOCK_DIVIDER_64, RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_07, RPI_V2_GPIO_P1_11);

	nRF.clearInt(0x7);
	bcm2835_spi_transfer(FLUSH_RX);
	bcm2835_spi_transfer(FLUSH_TX);

	nRF.setTXMode();

	static char data = 0x11;
	nRF.putBufOut(data);

	printf("Initial status: %#04x\n", nRF.updateStatus());

	nRF.transmit(1);

	int k = 0;

	while ( (nRF.updateStatus() & 0b00100000) < 1 ) {
		bcm2835_delayMicroseconds(100);
		k++;
		if ( (nRF.updateStatus() & 0b00010000) == 1 || k > 8000  || done == 1) {
			printf("Transmit failure.\n");
			if (k > 8000) {
				printf("Timeout.\n");
			} else if (done == 1) {
				printf("Aborted.\n");
			} else {
				printf("Max retries used.\n");
			}
			printf("Failure status: %#04x\n", nRF.updateStatus());
			done = 1;
			break;
		} else if ( (nRF.updateStatus() & 0b00100000) == 1) {
			break;
		}
	}

	if (done == 0) {

		printf("Transmitted. Status: %#04x\n", nRF.updateStatus());

		nRF.clearInt(TX_DS);

		nRF.setRXMode();

		printf("RX mode. Status: %#04x\n", nRF.updateStatus());

		k = 0;
		done = 1;
		while ( k < 200000 ) {
			k++;

			if ( (nRF.updateStatus() & 0b01000000) > 0 ) {
				done = 0;
				printf("Data received. Status: %#04x\n", nRF.updateStatus());

				delay(10);

				char payloadSize = nRF.getPayloadSize();
				nRF.getPayload(payloadSize);

				printf("PLSize: %d\n", payloadSize);

				char buf[32];
				nRF.getBufIn(buf, payloadSize);
				printf("Data: %#04x\n", nRF.BufIn[0]);
				break;
			}
		}

		if (done == 1) {
			printf("Nothing received. Status: %#04x\n", nRF.updateStatus());
		}
	}

	bcm2835_spi_transfer(FLUSH_RX);
	bcm2835_spi_transfer(FLUSH_TX);
	nRF.clearInt(0x7);

	printf("Status: %#04x\n", nRF.updateStatus());

	nRF.stop();

	printf("Done\n");

	return 0;
}
