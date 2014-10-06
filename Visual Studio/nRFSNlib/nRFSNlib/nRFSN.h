#ifndef SPI_H_
#include <SPI.h>
#endif

#include "nRF24L01+.h"
#include <stdint.h>

class nRFSN
{

	uint8_t dataBufIn[32];		// 32 uint8_t buffer for all incoming SPI data
	uint8_t dataBufOut[32];		// 32 uint8_t buffer for all outgoing SPI data

	uint8_t nrfSTATUS;			// nRF24L01+ STATUS register

	volatile int intRXData;     // nRF received packet flag; set by ISR

public:
	void init(uint8_t SPIFreq, uint8_t CEpin, uint8_t CSNpin, uint8_t IRQpin, uint8_t intNum);
	void RX_ISR(void);
	void setPower(uint8_t pwrLvl);
	void setTXMode(void);
	void setRXMode(void);
	void setMAX_RT(uint8_t numRT);
	void setChannel(uint8_t ch);
	uint8_t transmit(void);
	uint8_t getPayloadSize(void);
	uint8_t getPayload(void);
protected:
	void configReg(char wr, uint8_t command, uint8_t data);
	void setTXAddr(uint8_t addr[], uint8_t len);
	void setRXAddr(uint8_t pipe, uint8_t addr[], uint8_t len);
	
	/*------------------------------------------------
	 * nRF24L01+ current config settings
	------------------------------------------------*/
	uint8_t CONFIG_CURR           = B00101011;   // Show RX_DR and MAX_RT interrupts; Enable CRC - 1 uint8_t; Power up; RX
	uint8_t EN_AA_CURR            = B00000011;   // Enable Auto Ack for pipe 0,1
	uint8_t EN_RXADDR_CURR        = B00000011;   // Enable data pipe 0,1
	uint8_t SETUP_AW_CURR         = B00000010;   // Set up for 4 uint8_t address
	uint8_t SETUP_RETR_CURR       = B00100000;   // 750us retransmit delay; Disable auto retransmit
	uint8_t RF_CH_CURR            = B01101001;   // Channel 105 (2.400GHz + 0.105GHz = 2.505GHz)
	uint8_t RF_SETUP_CURR         = B00000110;   // RF data rate to 1Mbps; 0dBm output power (highest)
	uint8_t RX_PW_P0_CURR         = B00000001;   // 1 uint8_t payload
	uint8_t DYNPD_CURR            = B00000011;   // Set dynamic payload for pipe 0
	uint8_t FEATURE_CURR          = B00000100;   // Enable dynamic payload
	uint8_t RX_ADDRESS[4]         = {0xC7,0xC7,0xC7,0xC7};
	uint8_t TX_ADDRESS[4]         = {0xC7,0xC7,0xC7,0xC7};
};

