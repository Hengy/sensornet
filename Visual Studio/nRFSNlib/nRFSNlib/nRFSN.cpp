#include "nRF24L01+.h"
#include "nRFSN.h"

void nRFSN::init(uint8_t SPIDiv, uint8_t CEpin, uint8_t CSNpin, uint8_t IRQpin, uint8_t intNum)
{
	nRFSN_CE = CEpin;
	nRFSN_CSN = CSNpin;
	nRFSN_IRQ = IRQpin;

	pinMode(nRFSN_CE, OUTPUT);
	pinMode(nRFSN_CE, OUTPUT);
	pinMode(nRFSN_IRQ, INPUT);
	digitalWrite(nRFSN_CE, HIGH);
	digitalWrite(nRFSN_CSN, HIGH);

	initSPI(SPIDiv);

	CONFIG_CURR           = B00101011;   // Show RX_DR and MAX_RT interrupts; Enable CRC - 1 uint8_t; Power up; RX
	EN_AA_CURR            = B00000011;   // Enable Auto Ack for pipe 0,1
	EN_RXADDR_CURR        = B00000011;   // Enable data pipe 0,1
	SETUP_AW_CURR         = B00000010;   // Set up for 4 address
	SETUP_RETR_CURR       = B00100000;   // 750us retransmit delay; Disable auto retransmit
	RF_CH_CURR            = B01101001;   // Channel 105 (2.400GHz + 0.105GHz = 2.505GHz)
	RF_SETUP_CURR         = B00000110;   // RF data rate to 1Mbps; 0dBm output power (highest)
	RX_PW_P0_CURR         = B00000001;   // 1 payload
	DYNPD_CURR            = B00000011;   // Set dynamic payload for pipe 0
	FEATURE_CURR          = B00000100;   // Enable dynamic payload
	RX_ADDRESS[4]         = {0xC7,0xC7,0xC7,0xC7};
	TX_ADDRESS[4]         = {0xC7,0xC7,0xC7,0xC7};

	// nRF24L01+ setup
	// Write to CONFIG register
	configReg('w',CONFIG,CONFIG_CURR);
	// Write to EN_RXADDR register  
	configReg('w',EN_RXADDR,EN_RXADDR_CURR);
	// Write to EN_AA register
	configReg('w',EN_AA,EN_AA_CURR);
	// Write to SETUP_AW register
	configReg('w',SETUP_AW,SETUP_AW_CURR);
	// Write to SETUP_RETR register
	configReg('w',SETUP_RETR,SETUP_RETR_CURR);
	// Write to RF channel register
	configReg('w',RF_CH,RF_CH_CURR);
	// Write to RF setup register  
	configReg('w',RF_SETUP,RF_SETUP_CURR);
	// set TX address
	setTXAddr(TX_ADDRESS,4);
	// set RX address
	setRXAddr(RX_ADDR_P0,RX_ADDRESS,4);
	// Set dynamic payload for pipe 0
	configReg('w',DYNPD,DYNPD_CURR);
	// Write to FEATURE register
	configReg('w',FEATURE,FEATURE_CURR);
	// Flush RX FIFO
	spiTransfer('n',FLUSH_RX,0);
	// Flush TX FIFO
	spiTransfer('n',FLUSH_TX,0);

	nRFSN_RXInt = 0;
	attachInterrupt(intNum,RX_ISR,LOW);
}

void nRFSN::RX_ISR(void)
{
	nRFSN_RXInt = 1;
}


void nRFSN::setPower(uint8_t pwrLvl)
{
}


void nRFSN::setTXMode(void)
{
}


void nRFSN::setRXMode(void)
{
}


void nRFSN::setMAX_RT(uint8_t numRT)
{
}


void nRFSN::setChannel(uint8_t ch)
{
}


uint8_t nRFSN::transmit(void)
{
	return 0;
}


uint8_t nRFSN::getPayloadSize(void)
{
	return 0;
}


uint8_t nRFSN::getPayload(void)
{
	return 0;
}


void nRFSN::configReg(char wr, uint8_t command, uint8_t data)
{
}

void nRFSN::setTXAddr(uint8_t addr[], uint8_t len)
{
}


void nRFSN::setRXAddr(uint8_t pipe, uint8_t addr[], uint8_t len)
{
}


void nRFSN::initSPI(uint8_t SPIDiv)
{
	// SPI setup
	SPI.setBitOrder(MSBFIRST);              // Set most significant bit first
	SPI.setClockDivider(SPIDiv);			// Clock to Fosc/SPIDiv
	SPI.setDataMode(SPI_MODE0);             // Clock polarity 0; clock phase 0
	SPI.begin();                            // Start SPI
}


void nRFSN::updateStatus(void)
{
	digitalWrite(nRFSN_CSN, LOW);

	nRFSN_Status = SPI.transfer(NRF_NOP);

	digitalWrite(nRFSN_CSN, HIGH);
}
