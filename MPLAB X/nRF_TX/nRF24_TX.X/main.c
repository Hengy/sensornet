/*------------------------------------------------
 * File:        main.c
 * Version:     0.0.1
 * --------------------------------------------
 * Author:      Matthew Hengeveld
 * Date:        August 7, 2014
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
 * Pin Definitions
------------------------------------------------*/
// LEDs
#define ACT_LED     LATAbits.LATA0      // Activity LED

// nRF24L01+ pins
#define nRF_CE      LATAbits.LATA1      // nRF24L01+ chip enable
#define nRF_CSN     LATAbits.LATA2      // nRF24L01+ chip select negative
#define nRF_IRQ     LATAbits.LATA3      // nRF24L01+ IRQ

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
 * Misc. Definitions
------------------------------------------------*/
#define DUMMY_DATA      0xCC            // Dummy data for SPI

/*------------------------------------------------
 * Initial config settings
------------------------------------------------*/
char CONFIG_INIT = 0b00001010;              // Show all interrupts; Enable CRC - 1 byte; Power up; TX
char EN_RXADDR_INIT = 0b00000010;           // Enable data pipe 1
char SETUP_AW_INIT = 0b00000010;            // set for 4 byte address
char RF_CH_INIT = 0b01101001;               // Channel 105 (2.400GHz + 0.105GHz = 2.505GHz)
char RF_SETUP_INIT = 0b00000110;            // RF data rate to 1Mbps; 0dBm output power (highest)
char RX_ADDRESS[4] = {0xE7,0xE7,0xE7,0xE7}; // 4 byte initial RX address
char TX_ADDRESS[4] = {0xE7,0xE7,0xE7,0xE7}; // 4 byte initial TX address


/*------------------------------------------------
 * Function Definitions
------------------------------------------------*/
void portConfig(void);
void spiConfig_1(void);
void nrfConfig(void);
void spiWrite(char);
void nrfTX(char);
void delay10ms(int);
void nrfSetRXAddr(char, char[]);
void nrfSetTXAddr(char[]);

/*------------------------------------------------
 * Global Variables
------------------------------------------------*/
char dataBufIn[32];                           // 32 byte buffer for all incoming SPI data
char dataBufOut[32];                          // 32 byte buffer for all outgoing SPI data
int count = 0;

/*------------------------------------------------
 * Main
------------------------------------------------*/
void main(void) {

    portConfig();

    spiConfig_1();

    nrfConfig();

    for (;;) {
        ACT_LED = 1;

        nrfTX(count);
        count++;

        ACT_LED = 0;

        // Show that code is running with act_LED
        int i = 0;
        for (i=0;i<5;i++) {
            delay10ms(5);
        }
    }
}

/*------------------------------------------------
 * PORT setup function; 1=IN; 0=OUT
------------------------------------------------*/
void portConfig(void) {
    LATA = 0x00;                        // Set all pins to low
    LATB = 0x00;
    LATC = 0x00;

    TRISA = 0b00001000;                 // Configure PORTA I/O
    TRISB = 0b00000000;                 // Configure PORTB I/O
    TRISC = 0b00010000;                 // Configure PORTC I/O
}

/*------------------------------------------------
 * SPI 1 setup function - used for nRF24L01+
 *  -MSSP1
------------------------------------------------*/
void spiConfig_1(void) {
    SSP1CON1bits.CKP = 0;               // Clock polarity
    SSP1STATbits.CKE = 1;               // Clock edge detect
    SSP1STATbits.SMP = 1;               // Sample bit
    SSP1ADD = 0b00011111;               // Set to 31
    SSP1CON1bits.SSPM = 0b1010;         // Clock = Fosc/(SSP1ADD + 1)(4) = 500KHz
    //SSP1CON1bits.SSPM = 0b0010;         // Clock = Fosc/64 = 1MHz
    SSP1CON1bits.SSPEN = 1;             // Enable SPI
}

/*------------------------------------------------
 * nRF24L01+ setup function
------------------------------------------------*/
void nrfConfig(void) {
    spiWrite(W_REGISTER|CONFIG);        // Write to CONFIG register
    spiWrite(CONFIG_INIT);              // Show all interrupts; Enable CRC - 1 byte; Power up; TX
    spiWrite(W_REGISTER|EN_RXADDR);     // Write to EN_RXADDR register
    spiWrite(EN_RXADDR_INIT);           // Enable data pipe 1
    spiWrite(W_REGISTER|SETUP_AW);      // Write to SETUP_AW register
    spiWrite(SETUP_AW_INIT);            // set up for 4 byte address
    nrfSetTXAddr(TX_ADDRESS);           // set TX address
    nrfSetRXAddr(RX_ADDR_P0,RX_ADDRESS);// set RX address
    spiWrite(W_REGISTER|RF_CH);         // Write to RF channel register
    spiWrite(RF_CH_INIT);               // Channel 105 (2.400GHz + 0.105GHz = 2.505GHz)
    spiWrite(W_REGISTER|RF_SETUP);      // Write to RF setup register
    spiWrite(RF_SETUP_INIT);            // RF data rate to 1Mbps; 0dBm output power (highest)
    spiWrite(FLUSH_TX);                 // Flush TX FIFO
}

/*------------------------------------------------
 * nRF24L01+ set TX address
 * char addr - 4 byte address
------------------------------------------------*/
void nrfSetTXAddr(char addr[]) {
    spiWrite(W_REGISTER|TX_ADDR);       // Set new transmit address

    int i;                              // Send all 4 bytes
    for (i=0;i<4;i++) {
        spiWrite(addr[i]);
    }
}

/*------------------------------------------------
 * nRF24L01+ set RX for pipe address
 * char addrXX - which pipe to set address for
 * char addr - 4 byte address
------------------------------------------------*/
void nrfSetRXAddr(char addrXX, char addr[]) {
    spiWrite(W_REGISTER|addrXX);        // Set new receive address
    
    int i;                              // Send all 4 bytes
    for (i=0;i<4;i++) {
        spiWrite(addr[i]);
    }
}

/*------------------------------------------------
 * Get STATUS register from nRF24L01+
 * Sends NOP commmand and gets STATUS reg
 * returns char STATUS register
------------------------------------------------*/
char getSTATUS(void) {
    char statusByte;
    spiWrite(NRF_NOP);                  // Send NOP command
    statusByte = SSP1BUF;

    return statusByte;
}

/*------------------------------------------------
 * nRF24L01+ setup function
------------------------------------------------*/
void spiWrite(char data) {
    // toggle CSN pin
    nRF_CSN = 1;
    SSP1BUF = data;
    nRF_CSN = 0;
    __delay_us(500);
}

/*------------------------------------------------
 * SPI read data function
 * Sends len number of dummy chars
 * int len - number of bytes to read
 * returns pointer to array of data
------------------------------------------------*/
void spiRead(int len) {
    int i;
    for (i=0;i<len;i++) {
        // toggle CSN pin
        nRF_CSN = 1;
        SSP1BUF = DUMMY_DATA;
        nRF_CSN = 0;
        dataBufIn[i] = SSP1BUF;
    }

    __delay_us(500);
}

/*------------------------------------------------
 * nRF24L01+ send function
------------------------------------------------*/
void nrfTX(char data) {
    spiWrite(W_TX_PAYLOAD);             // Write to TX payload register
    spiWrite(data);                     // Write data

    // Toggle CE pin
    nRF_CE = 1;
    __delay_us(20);
    nRF_CE = 0;
}

/*------------------------------------------------
 * Delay (10 x ms_multi) ms
------------------------------------------------*/
void delay10ms(int ms_multi) {
    int i = 0;
    for (i=0;i<ms_multi;i++) {
        __delay_ms(10);
    }
}
