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
#define nRF_IRQ     LATAbits.LATA2      // nRF24L01+ IRQ

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
 * Function Definitions
------------------------------------------------*/
void portConfig(void);
void spiConfig_1(void);
void nrfConfig(void);
void spiWrite(int);
void nrfTX(int);
void delay10ms(int);

/*------------------------------------------------
 * Global Variables
------------------------------------------------*/
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
 * PORT setup function (
------------------------------------------------*/
void portConfig(void) {
    LATA = 0x00;                        // Set all pins to low
    LATB = 0x00;
    LATC = 0x00;

    TRISA = 0b00000100;                 // Configure PORTA I/O
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
    spiWrite(0b00001010);               // Show all interrupts; Enable CRC - 1 byte; Power up; TX
    spiWrite(W_REGISTER|SETUP_AW);      // Write to SETUP_AW register
    spiWrite(0b00000001);               // 3 byte address
    spiWrite(W_REGISTER|RF_CH);         // Write to RF channel register
    spiWrite(0b01101001);               // Channel 105 (2.400GHz + 0.105GHz = 2.505GHz)
    spiWrite(W_REGISTER|RF_SETUP);      // Write to RF setup register
    spiWrite(0b00000110);               // RF data rate to 1Mbps; 0dBm output power (highest)
    spiWrite(FLUSH_TX);                 // Flush TX FIFO
}

/*------------------------------------------------
 * nRF24L01+ setup function
------------------------------------------------*/
void spiWrite(int data) {
    SSP1BUF = data;
    __delay_us(500);
}

/*------------------------------------------------
 * nRF24L01+ send function
------------------------------------------------*/
void nrfTX(int data) {
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