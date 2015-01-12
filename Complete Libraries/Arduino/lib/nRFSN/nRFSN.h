#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include <EEPROM.h>

#include "nRF24L01+.h"
#include "nRFSNCommands.h"

class nRFSNClass {
public:
	void init(uint8_t SPIDiv, uint8_t CEpin, uint8_t CSNpin, uint8_t IRQpin);
	
	void initSPI(uint8_t SPIDiv);
	void setTXMode(void);
	void setRXMode(void);
	uint8_t getMode(void);
	void setPower(uint8_t pwrLvl);
	uint8_t getPower(void);
	void setChannel(uint8_t ch);
	uint8_t getChannel(void);
	void setMaxRT(uint8_t numRT);
	uint8_t getMaxRT(void);
	void setTXAddr(uint8_t addr[], uint8_t len);
	void setRXAddr(uint8_t pipe, uint8_t addr[], uint8_t len);
	uint8_t *getTXAddr(void);
	uint8_t *getRXAddr(void);
	
	void nRF_ISR(void);
	void clearInt(uint8_t interrupt);
	uint8_t updateStatus(void);
	
	uint8_t sync(void);
	void setReg(uint8_t reg, uint8_t data);
	uint8_t getReg(uint8_t reg);
	
	void transfer(char wrn, uint8_t command, uint8_t len);
	void transmit(uint8_t len);
	void respond(uint8_t len);
	uint8_t getPayloadSize(void);
	void getPayload(uint8_t payloadSize);

	void putBufOut(uint8_t data, uint8_t len);
	uint8_t getBufIn(uint8_t len);

	volatile uint8_t Busy;			// nRF busy flag; set when transmitting

	volatile uint8_t RXInt;			// nRF received packet flag set by ISR
	volatile uint8_t TXInt;			// nRF packet sent flag set by ISR
	volatile uint8_t MAXInt;		// nRF max retransmit flag set by ISR

protected:
	uint8_t checkAddrs(void);
	uint8_t configReg(char wr, uint8_t command, uint8_t data);
	
	uint8_t nRFSN_CE;
	uint8_t nRFSN_CSN;
	uint8_t nRFSN_IRQ;
	
	uint8_t currMode;				// 1 = RX, 0 = TX
	
	uint8_t Status;					// nRF24L01+ STATUS register
	
	uint8_t BufIn[32];				// 32 uint8_t buffer for all incoming SPI data
	uint8_t BufOut[32];				// 32 uint8_t buffer for all outgoing SPI data
	
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

extern nRFSNClass nRFSN;