#include "nRFSN.h"

void nRFSN::init(uint8_t SPIFreq, uint8_t CEpin, uint8_t CSNpin, uint8_t IRQpin, uint8_t intNum)
{
	const uint8_t nRFSN_CE = CEpin;
	const uint8_t nRFSN_CSN = CSNpin;
	const uint8_t nRFSN_IRQ = IRQpin;

	attachInterrupt(intNum,RX_ISR,LOW);

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
}

void nRFSN::RX_ISR(void)
{
	intRXData++;
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
