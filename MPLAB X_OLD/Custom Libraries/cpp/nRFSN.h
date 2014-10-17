#include <xc.h>

#include "nRF24L01+.h"
#include "nRFSNCommands.h"

#define HIGH 1
#define LOW  0

#ifndef NRFSN_H
#define	NRFSN_H

class nRFSNClass {
public:
    void init(void);
    uint8_t sync(void);
    void nRF_ISR(void);
    void setPower(uint8_t pwrLvl);
    void setTXMode(void);
    void setRXMode(void);
    void setMAX_RT(uint8_t numRT);
    void setChannel(uint8_t ch);
    void transfer(char wrn, uint8_t command, uint8_t len, uint8_t offset);
    void transmit(uint8_t len);
    uint8_t getPayloadSize(void);
    void getPayload(uint8_t payloadSize, uint8_t offset);
    void initSPI(void);
    void setSPIFreq(uint16_t freq);
    void updateStatus(void);
    void clearInt(uint8_t intNum);

    uint8_t BufIn[32];			// 32 uint8_t buffer for all incoming SPI data
    uint8_t BufOut[32];			// 32 uint8_t buffer for all outgoing SPI data

    uint8_t Status;                         // nRF24L01+ STATUS register

    volatile uint8_t Busy;			// nRF busy flag; set when transmitting

    volatile uint8_t RXInt;			// nRF received packet flag set by ISR
    volatile uint8_t TXInt;			// nRF packet sent flag set by ISR
    volatile uint8_t MAXInt;		// nRF max retransmit flag set by ISR

protected:
    uint8_t checkAddrs(void);
    uint8_t configReg(char wr, uint8_t command, uint8_t data);
    void setTXAddr(uint8_t addr[], uint8_t len);
    void setRXAddr(uint8_t pipe, uint8_t addr[], uint8_t len);
    uint8_t spiTransfer(uint8_t data);
    void wEEPROM(uint8_t data, uint8_t address);
    uint8_t rEEPROM(uint8_t address);

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

#endif	/* NRFSN_H */