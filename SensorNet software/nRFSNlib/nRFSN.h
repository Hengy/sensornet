#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <sys/time.h>

#include <bcm2835.h>

#include "nRF24L01+.h"
#include "nRFSNCommands.h"

void nRFSN_init(char, char, char, char);
void stop(void);

void setTXMode(void);
void setRXMode(void);
char getMode(void);

void setPower(char);
char getPower(void);
void setChannel(char);
char getChannel(void);
void setMaxRT(char);
char getMaxRT(void);
void setMaxRTdelay(char);
char getMaxRTdelay(void);

void setTXAddr(char*, char);
void setRXAddr(char, char*, char);
char *getTXAddr(void);
char *getRXAddr(char pipe);

void clearInt(char);
char updateStatus(void);

void setReg(char, char);
char getReg(char);

void transmit(char len);
char getPayloadSize(void);
void getPayload(char);

void putBufOut(char*, char);
void putSingleBufOut(char);
char *getBufIn(char);

void flush(int);

void bcmDelay(int);
void bcmDelayM(int);

char *getSensorVals(char*, char, char);
void setSensorVals(char*, char*, char);
void setSensorValsFast(char*, char*, char);

void init_spi(char);

char CEpin;
char IRQpin;
char LEDpin;

volatile static char currMode;				// 1 = RX, 0 = TX

volatile static char Status;					// nRF24L01+ STATUS register


char BufOut[32];				// 32 char buffer for all outgoing SPI data
char BufIn[32];				// 32 char buffer for all incoming SPI data

/*------------------------------------------------
 * nRF24L01+ config variables
------------------------------------------------*/
char CONFIG_CURR;
char EN_AA_CURR;
char EN_RXADDR_CURR;
char SETUP_AW_CURR;
char SETUP_RETR_CURR;
char RF_CH_CURR;
char RF_SETUP_CURR;
char RX_PW_P0_CURR;
char DYNPD_CURR;
char FEATURE_CURR;
char RX_ADDRESS[4];
char TX_ADDRESS[4];
