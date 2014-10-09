#ifndef SPI_H_
#include <SPI.h>
#endif

#ifndef EEPROM_H_
#include <EEPROM.h>
#endif

#include <stdint.h>

class nRFSN
{

public:
	void init(uint8_t SPIDiv, uint8_t CEpin, uint8_t CSNpin, uint8_t IRQpin, uint8_t intNum);
	uint8_t sync(void);
	void setPower(uint8_t pwrLvl);
	void setTXMode(void);
	void setRXMode(void);
	void setMAX_RT(uint8_t numRT);
	void setChannel(uint8_t ch);
	void transfer(char wrn, uint8_t command, uint8_t len);
	uint8_t getPayloadSize(void);
	void getPayload(uint8_t payloadSize);
	void initSPI(uint8_t SPIDiv);
	void updateStatus(void);
	void clearInt(uint8_t interrupt);

	uint8_t nRFSN_CE;
	uint8_t nRFSN_CSN;
	uint8_t nRFSN_IRQ;

	uint8_t nRFSN_BufIn[32];		// 32 uint8_t buffer for all incoming SPI data
	uint8_t nRFSN_BufOut[32];		// 32 uint8_t buffer for all outgoing SPI data

	uint8_t nRFSN_Status;			// nRF24L01+ STATUS register

	volatile uint8_t nRFSN_Busy;	// nRF busy flag; set when transmitting

	volatile uint8_t nRFSN_RXInt;		// nRF received packet flag set by ISR
	volatile uint8_t nRFSN_TXInt;       // nRF packet sent flag set by ISR
	volatile uint8_t nRFSN_MAXInt;      // nRF max retransmit flag set by ISR

protected:
	uint8_t checkAddrs(void);
	void nRF_ISR(void);
	uint8_t configReg(char wr, uint8_t command, uint8_t data);
	void setTXAddr(uint8_t addr[], uint8_t len);
	void setRXAddr(uint8_t pipe, uint8_t addr[], uint8_t len);
	
	/*------------------------------------------------
	 * nRF24L01+ config variables
	------------------------------------------------*/
	uint8_t CONFIG_CURR;
	uint8_t EN_AA_CURR;
	uint8_t EN_RXADDR_CURR;
	uint8_t SETUP_AW_CURR;
	uint8_t SETUP_RETR_CURR;
	uint8_t RF_CH_CURR;
	uint8_t RF_SETUP_CURR;
	uint8_t RX_PW_P0_CURR;
	uint8_t DYNPD_CURR;
	uint8_t FEATURE_CURR;
	uint8_t RX_ADDRESS[4];
	uint8_t TX_ADDRESS[4];
};

