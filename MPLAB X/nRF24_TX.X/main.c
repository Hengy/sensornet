/*------------------------------------------------
 * File:        main.c
 * Version:     0.1.0
 * --------------------------------------------
 * Author:      Matthew Hengeveld
 * Date:        September 8, 2014
 * --------------------------------------------
 * uProc:       PIC18F24K22
 *  Fosc:           64MHz
 * Compiler:    MPLAB XC8 (Free)
 * --------------------------------------------
 * Description:
 *      Tests nRF24L01+ transmitting using PIC18F
------------------------------------------------*/

/*------------------------------------------------
 * Naming convention
 *  function:   funcName()
 *  variables:  varName
 *  constants:  CONST_NAME
------------------------------------------------*/

/*------------------------------------------------
MCLR    -[1 /RE3       RB7/28]-       PGD
LED     -[2 /RA0       RB6/27]-       PGC
CE      -[3 /RA1       RB5/26]-       
CSN     -[4 /RA2       RB4/25]-       
IRQ     -[5 /RA3       RB3/24]-       
        -[6 /RA4       RB2/23]-       
        -[7 /RA5       RB1/22]-       
Vss     -[8 /          RB0/21]-       
OSC1    -[9 /RA7          /20]-       Vdd
OSC2    -[10/RA6          /19]-       Vss
        -[11/RC0       RC7/18]-       
        -[12/RC1       RC6/17]-       
        -[13/RC2       RC5/16]-       SDO
SCK     -[14/RC3       RC4/15]-       SDI
------------------------------------------------*/

/*------------------------------------------------
 * Header Files
------------------------------------------------*/
#include <xc.h>

/*------------------------------------------------
 * Configuration Bits
------------------------------------------------*/
// Clock Freq
#define _XTAL_FREQ 64000000     // Clock frequency w/ PLL

// CONFIG1H
#pragma config FOSC = HSHP      // Oscillator Selection bits (HS oscillator (high power > 16 MHz))
#pragma config PLLCFG = ON      // 4X PLL Enable (Oscillator multiplied by 4)
#pragma config PRICLKEN = ON    // Primary clock enable bit (Primary clock enabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = ON      // Power-up Timer Enable bit (Power up timer enabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 190       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (Watch dog timer is always disabled. SWDTEN has no effect.)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC1  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<5:0> pins are configured as digital I/O on Reset)
#pragma config CCP3MX = PORTB5  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC output and ready status are not delayed by the oscillator stable status)
#pragma config T3CMX = PORTC0   // Timer3 Clock input mux bit (T3CKI is on RC0)
#pragma config P2BMX = PORTB5   // ECCP2 B output mux bit (P2B is on RB5)
#pragma config MCLRE = EXTMCLR  // MCLR Pin Enable bit (MCLR pin enabled, RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled if MCLRE is also 1)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))


/*------------------------------------------------
 * nRF24L01+ command and register definitions
------------------------------------------------*/
// nRF24L01+ commands
#define R_REGISTER      0x00            // Read; Bits <5:0> = register map address (LSB first)
#define W_REGISTER      0x20            // Write; Bits <5:0> = register map address (LSB first)
#define R_RX_PAYLOAD    0x61            // Read RX payload 1-32 bytes (LSB first)
#define W_TX_PAYLOAD    0xA0            // Write TX payload 1-32 bytes (LSB first)
#define FLUSH_TX        0xE1            // Flush TX FIFO
#define FLUSH_RX        0xE2            // Flush RX FIFO
#define REUSE_TX_PL     0xE3            // TX; Reuse last transmitted payload; active until FLUSH_TX or W_TX_PAYLOAD
#define R_RX_PL_WID     0x60            // Read RX payload width for top R_RX_PAYLOAD in RX FIFO
#define W_ACK_PAYLOAD   0xA8            // RX; Write payload + ACK packet; <2:0> = write payload (LSB first)
#define W_TX_PAYLOAD_NO 0xB0            // TX; Disable AUTOACK on this specific packet
#define NRF_NOP         0xFF            // No operation

// nRF24L01+ registers
#define CONFIG          0x00            // Configuration register
#define EN_AA           0x01            // Enable AUTOACK function
#define EN_RXADDR       0x02            // Enable RX addresses
#define SETUP_AW        0x03            // Setup address widths
#define SETUP_RETR      0x04            // Setup auto retransmission
#define RF_CH           0x05            // RF channel
#define RF_SETUP        0x06            // RF setup register
#define STATUS          0x07            // Status register
#define OBSERVE_TX      0x08            // Transmit observe register
#define RPD             0x09            // RPD (Carrier Detect)
#define RX_ADDR_P0      0x0A            // Receive address data for pipes 0-5
#define RX_ADDR_P1      0x0B
#define RX_ADDR_P2      0x0C
#define RX_ADDR_P3      0x0D
#define RX_ADDR_P4      0x0E
#define RX_ADDR_P5      0x0F
#define TX_ADDR         0x10            // Transmit address
#define RX_PW_P0        0x11            // Receive data width for pipes 0-5
#define RX_PW_P1        0x12
#define RX_PW_P2        0x13
#define RX_PW_P3        0x14
#define RX_PW_P4        0x15
#define RX_PW_P5        0x16
#define FIFO_STATUS     0x17            // FIFO status register
#define DYNPD           0x1C            // Enable dynamic payload length
#define FEATURE         0x1D            // Feature register

/*------------------------------------------------
 * Pin Definitions
------------------------------------------------*/
// LEDs
#define ACT_LED     LATAbits.LATA0      // Activity LED

// nRF24L01+ pins
#define nRF_CE      LATAbits.LATA1      // nRF24L01+ chip enable
#define nRF_CSN     LATAbits.LATA2      // nRF24L01+ chip select negative
#define nRF_IRQ     PORTBbits.RB4       // nRF24L01+ IRQ

/*------------------------------------------------
 * Misc. Definitions
------------------------------------------------*/
//#define DUMMY_DATA      0xCC            // Dummy data for SPI (0b11001100)
#define HIGH            1
#define LOW             0

#define  testbit(var, bit)  ((var) & (1 <<(bit)))  //bit test macro

/*------------------------------------------------
 * Current config settings - TX
------------------------------------------------*/
unsigned char CONFIG_CURR       = 0b01011010;   // Show all TX interrupts; Enable CRC - 1 byte; Power Up; PTX
unsigned char EN_AA_CURR        = 0b00000011;   // Enable Auto Ack on pipe 0,1
unsigned char EN_RXADDR_CURR    = 0b00000011;   // Enable data pipe 0,1
unsigned char SETUP_AW_CURR     = 0b00000010;   // set for 4 byte address
unsigned char SETUP_RETR_CURR   = 0b00110101;   // 1000us retransmit delay; 5 auto retransmits
//unsigned char SETUP_RETR_CURR   = 0b00100001;   // 750us retransmit delay; Disable auto retransmit
unsigned char RF_CH_CURR        = 0b01101001;   // Channel 105 (2.400GHz + 0.105GHz = 2.505GHz)
unsigned char RF_SETUP_CURR     = 0b00000110;   // RF data rate to 1Mbps; 0dBm output power (highest)
unsigned char RX_PW_P0_CURR     = 0b00000001;   // Set pipe 0 payload width to 1
unsigned char DYNPD_CURR        = 0b00000001;   // Set dynamic payload for pipe 0
unsigned char FEATURE_CURR      = 0b00000100;   // Enable dynamic payload
unsigned char RX_ADDRESS[4] = {0xE7,0xE7,0xE7,0xE7}; // 4 byte initial RX address
unsigned char TX_ADDRESS[4] = {0xC7,0xC7,0xC7,0xC7}; // 4 byte initial TX address


/*------------------------------------------------
 * Function Definitions
------------------------------------------------*/
void main(void);
void interrupt highISR(void);
void interrupt low_priority lowISR(void);
void intConfig(void);
void portConfig(void);
void spiConfig_1(unsigned int);
void nrfConfig(void);
void setCSN(int);
void nrfGetStatus(void);
unsigned char nrfConfigReg(char, unsigned char, unsigned char);
void nrfSetRXAddr(unsigned char, unsigned char *, int);
void nrfSetTXAddr(unsigned char *, int);
void spiTransfer(char, unsigned char,int);
unsigned char spiTransferByte(unsigned char);
void nrfTXData(int);
void delay10ms(int);


/*------------------------------------------------
 * Global Variables
------------------------------------------------*/
unsigned char dataBufIn[32];                // 32 byte buffer for all incoming SPI data
unsigned char dataBufOut[32];               // 32 byte buffer for all outgoing SPI data

unsigned char nrfSTATUS;                    // nRF STATUS register copy

unsigned char spiTXFlag = 0;                // SPI transmit complete flag

unsigned char nrfBusy = 0;                  // nRF busy (transmitting) flag

volatile unsigned char nrfInterrupt = 0;             // nRF Interrupt flag

/*------------------------------------------------
 * Main
------------------------------------------------*/
void main(void) {

    // Configure system
    portConfig();                           // Config I/O and initial levels
    spiConfig_1(40);                        // Config MSSP 1 for SPI Master mode 0
    nrfConfig();                            // Config nRF24L01+
    intConfig();

    delay10ms(1);                           // Wait for nRF power up

    dataBufOut[1] = 0x48;
    dataBufOut[2] = 0x4F;
    dataBufOut[3] = 88;
    dataBufOut[4] = 0x49;
    dataBufOut[5] = 0x5F;
    dataBufOut[6] = 47;
    dataBufOut[7] = 0x74;
    dataBufOut[8] = 0x6F;
    dataBufOut[9] = 58;
    dataBufOut[10] = 0xA8;
    dataBufOut[11] = 0x6F;
    dataBufOut[12] = 99;
    dataBufOut[13] = 0x88;
    dataBufOut[14] = 0x5F;
    dataBufOut[15] = 72;
    dataBufOut[16] = 0x98;
    dataBufOut[17] = 0x6F;
    dataBufOut[18] = 92;
    dataBufOut[19] = 0x4A;
    dataBufOut[20] = 0x6F;
    dataBufOut[21] = 62;
    dataBufOut[22] = 0x62;
    dataBufOut[23] = 0x6F;
    dataBufOut[24] = 82;
    dataBufOut[25] = 0x86;
    dataBufOut[26] = 0x7F;
    dataBufOut[27] = 72;
    dataBufOut[28] = 102;
    dataBufOut[29] = 0xC0;
    dataBufOut[30] = 0x5F;
    dataBufOut[31] = 122;

    // Transmit count; read nRF STATUS reg; forever
    int count = 1;
    unsigned int TXtime = 20000;

    // Reset interrupt flags
    dataBufOut[0] = 0b00110000;
    spiTransfer('w',STATUS,1);

    for (;;) {

        if (nrfInterrupt) {

            // Reset interrupt flags
            dataBufOut[0] = 0b00110000;
            spiTransfer('w',STATUS,1);

            nrfBusy = 0;
            nrfInterrupt = 0;
        }

        if ((TXtime >= 12800) && (!nrfBusy)) {

            dataBufOut[0] = count;
            nrfTXData(5);
            count++;
            TXtime = 0;
        } else if (!nrfBusy) {

            ACT_LED = 1;
            __delay_us(20);
            ACT_LED = 0;
        }

        __delay_us(50);
        TXtime++;
    }
}

/*------------------------------------------------
 * High-priority ISR function
------------------------------------------------*/
void interrupt highISR(void) {

    if (INTCONbits.RBIF) {

        if (nRF_IRQ == 0) {                         // If falling edge change
            nrfInterrupt = 1;
        }

        INTCONbits.RBIF = 0;                    // Clear flag
    }
}

/*------------------------------------------------
 * Low-priority ISR function
------------------------------------------------*/
void interrupt low_priority lowISR(void) {
    
}

/*------------------------------------------------
 * Low-priority ISR function
------------------------------------------------*/
void intConfig(void) {

    // Clear MSSP transmit flag
    PIR1bits.SSP1IF = 0;

    // Enable PORTB interrupt-on-change
    IOCBbits.IOCB4 = 1;
    INTCONbits.RBIF = 0;
    INTCON2bits.RBIP = 1;               // High priority
    INTCONbits.RBIE = 1;

    // Enable global interrupts
    RCONbits.IPEN = 1;                  // Enable interrupt priority mode
    INTCONbits.GIEH = 1;                // Enable high priority interrupts
    INTCONbits.GIEL = 1;                // Enable low priority interrupts
}

/*------------------------------------------------
 * PORT setup function; 1=IN; 0=OUT
------------------------------------------------*/
void portConfig(void) {
    TRISA = 0b00000000;                 // Configure PORTA I/O
    TRISB = 0b00010000;                 // Configure PORTB I/O
    TRISC = 0b00010000;                 // Configure PORTC I/O

    LATA = 0b00000100;                  // Set all pins to low except CSN
    LATB = 0x00;                        // Set all pins to low
    LATC = 0x00;
}

/*------------------------------------------------
 * SPI 1 setup function - used for nRF24L01+
 *  -MSSP1
 * unsigned int spiSpeed - value, in 100KHz, of desired SPI bus frequency
 *      Ex. for 2MHz SPI, spiSpeed = (2MHz/100KHz) = 20
------------------------------------------------*/
void spiConfig_1(unsigned int spiSpeed) {

    SSP1CON1bits.CKP = 0;               // Clock polarity
    SSP1STATbits.CKE = 1;               // Clock edge detect
    SSP1STATbits.SMP = 1;               // Sample bit


    unsigned int sspAddVal = (((_XTAL_FREQ/100000)/spiSpeed)/4)-1;
    SSP1ADD = sspAddVal;                   // Set to 7
    SSP1CON1bits.SSPM = 0b1010;         // Clock = Fosc/(SSP1ADD + 1)(4) = spiSpeed/10 (MHz)

    SSP1CON1bits.SSPEN = 1;             // Enable SPI

    nRF_CSN = 1;                        // Start chip select pin high
}

/*------------------------------------------------
 * nRF24L01+ setup function
------------------------------------------------*/
void nrfConfig(void) {

    // Write to CONFIG register
    nrfConfigReg('w',CONFIG,CONFIG_CURR);
    // Write to EN_AA register
    nrfConfigReg('w',EN_AA,EN_AA_CURR);
    // Write to EN_RXADDR register
    nrfConfigReg('w',EN_RXADDR,EN_RXADDR_CURR);
    // Write to SETUP_AW register
    nrfConfigReg('w',SETUP_AW,SETUP_AW_CURR);
    // Write to SETUP_RETR register
    nrfConfigReg('w',SETUP_RETR,SETUP_RETR_CURR);
    // Write to RF channel register
    nrfConfigReg('w',RF_CH,RF_CH_CURR);
    // Write to RF setup register
    nrfConfigReg('w',RF_SETUP,RF_SETUP_CURR);
    // set RX address
    nrfSetRXAddr(RX_ADDR_P0,RX_ADDRESS,4);
    // set RX address
    nrfSetRXAddr(RX_ADDR_P1,TX_ADDRESS,4);
    // set TX address
    nrfSetTXAddr(TX_ADDRESS,4);
    // Set pipe 0 payload width
    nrfConfigReg('w',RX_PW_P1,RX_PW_P0_CURR);
    // Set pipe 1 payload width
    nrfConfigReg('w',RX_PW_P0,RX_PW_P0_CURR);
    // Write to DYNPD register
    nrfConfigReg('w',DYNPD,DYNPD_CURR);
    // Write to FEATURE register
    nrfConfigReg('w',FEATURE,FEATURE_CURR);
    // Flush TX FIFO
    spiTransfer('n',FLUSH_TX,0);
    // Flush RX FIFO
    spiTransfer('n',FLUSH_RX,0);
}

/*------------------------------------------------
 * Sets CSN pin HIGH or LOW w/ delay
------------------------------------------------*/
void setCSN(int level) {

    if (level == 1) {           // If setting HIGH
        nRF_CSN = 1;
    } else {                    // If setting LOW
        nRF_CSN = 0;
    }
}


/*------------------------------------------------
 * Gets STATUS reg from nRF
------------------------------------------------*/
void nrfGetStatus(void) {

    setCSN(LOW);                             // Set CSN low

    nrfSTATUS = spiTransferByte(NRF_NOP);    // return STATUS

    setCSN(HIGH);                            // Set CSN high
}

/*------------------------------------------------
 * Writes or eads nRF reg
------------------------------------------------*/
unsigned char nrfConfigReg(char wr, unsigned char command, unsigned char data) {

    setCSN(LOW);                                // Set CSN low

    if (wr == 'w') {                            // Write
        spiTransferByte(W_REGISTER|command);    // Send command
        spiTransferByte(data);
    } else if (wr == 'r') {                     // Read
        spiTransferByte(R_REGISTER|command);    // Send command
        data = spiTransferByte(NRF_NOP);
    }

    setCSN(HIGH);                               // Set CSN high

    return data;
}

/*------------------------------------------------
 * Writes new transmit address to nRF
 * addr[] - array of bytes containing address
 * len - length of address (in bytes)
------------------------------------------------*/
void nrfSetTXAddr(unsigned char addr[], int len) {

    setCSN(LOW);                            // Set CSN low

    spiTransferByte(W_REGISTER|TX_ADDR);    // Send command

    if (len != 0) {
        // Send address bytes
        for (int i=0;i<len;i++) {
            spiTransferByte(addr[i]);
        }
    }

    setCSN(HIGH);                           // Set CSN high
}

/*------------------------------------------------
 * Writes new receive address to nRF
 * pipe - which pipe to write address to
 * addr[] - array of bytes containing address
 * len - length of address (in bytes)
------------------------------------------------*/
void nrfSetRXAddr(unsigned char pipe, unsigned char addr[], int len) {

    setCSN(LOW);                        // Set CSN low

    spiTransferByte(W_REGISTER|pipe);   // Send command

    if (len != 0) {
        // Send address bytes
        for (int i=0;i<len;i++) {
            spiTransferByte(addr[i]);
        }
    }

    setCSN(HIGH);                       // Set CSN high
}

/*------------------------------------------------
 * SPI transfer function: send and receive multiple bytes w/ command
 * wrn - 'read', 'write' or 'neither'
 * command - unsigned char
 * len - size of data
 * send data - stored in dataBufOut[]
 * receive data - stored in dataBufIn[]
------------------------------------------------*/
void spiTransfer(char wrn, unsigned char command,int len) {
    
    setCSN(LOW);                                // Set CSN low

    if (wrn == 'w') {                           // Write
        spiTransferByte(W_REGISTER|command);    // Send command
    } else if (wrn == 'r') {                    // Read
        spiTransferByte(R_REGISTER|command);    // Send command
    } else if(wrn == 'n') {
        spiTransferByte(command);               // Send command
    }

    if (len != 0) {
        for (int i=0;i<len;i++) {
            dataBufIn[i] = spiTransferByte(dataBufOut[i]);
        }
    }

    setCSN(HIGH);                               // Set CSN high
}

/*------------------------------------------------
 * SPI transfer function: send and receive 1 byte
------------------------------------------------*/
unsigned char spiTransferByte(unsigned char data) {

    SSP1BUF = data;                     // Write data to MSSP
    
    while (!PIR1bits.SSP1IF) { }        // Wait for transfer to complete
    PIR1bits.SSP1IF = 0;                // Clear flag

    return SSP1BUF;                     // return recieved data
}

/*------------------------------------------------
 * nRF transmit function - loads bytes into nRF
 * using write payload command
------------------------------------------------*/
void nrfTXData(int len) {

    setCSN(LOW);                        // Set CSN low

    spiTransferByte(W_TX_PAYLOAD);

    if (len != 0) {
        for (int i=0;i<len;i++) {
            spiTransferByte(dataBufOut[i]);
        }
    }

    setCSN(HIGH);                       // Set CSN high

    // pulse CE pin to transmit
    nRF_CE = 1;
    __delay_us(12);                     // Wait min 10us
    nRF_CE = 0;
//    __delay_us(180);
//    for (int i=0;i<len;i++) {
//        __delay_us(8);
//    }

    nrfBusy = 1;                        // Flag that nrf is busy transmitting
}

/*------------------------------------------------
 * Delay (10 x ms_multi) ms
------------------------------------------------*/
void delay10ms(int ms_multi) {
    int i = 0;
    for (i=0;i<ms_multi;i++) {
        __delay_ms(11);
    }
}
