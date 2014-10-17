/*------------------------------------------------
 * File:        PIC18F_Template.c
 * Version:     0.0.1
 * --------------------------------------------
 * Author:      _
 * Date:        _
 * --------------------------------------------
 * uProc:       PIC18F_
 *  Fosc:           _MHz
 * Compiler:    MPLAB XC8 (Free)
 * --------------------------------------------
 * Description:
 *      _
------------------------------------------------*/

/*------------------------------------------------
 * Naming convention
 *  function:   funcName()
 *  variables:  varName
 *  constants:  CONST_NAME
------------------------------------------------*/


/*------------------------------------------------
 * Clock configuration
------------------------------------------------*/
#define _XTAL_FREQ 64000000     // Clock frequency w/ PLL


/*------------------------------------------------
 * Header files
------------------------------------------------*/
#include <xc.h>
#include <nRFSN.h>
#include <stdint.h>


/*------------------------------------------------
 * Configuration Bits
------------------------------------------------*/
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
#define ACT_LED         LATAbits.LATA0      // Activity

// nRF24L01+ pins
#define nRFSN_CE        LATAbits.LATA1      // nRF24L01+ chip enable
#define nRFSN_CSN       LATAbits.LATA2      // nRF24L01+ chip select negative
#define nRFSN_IRQ       PORTBbits.RB4       // nRF24L01+ IRQ


/*------------------------------------------------
 * Macro Definitions
------------------------------------------------*/
#define  testbit(var, bit)  ((var) & (1 <<(bit)))  //bit test macro


void main(void) {
    
}