#include <bcm2835.h>
#include <stdio.h>
#include <string.h>

#include "nRFSNCommands.h"
#include "nRF24L01+.h"

using namespace std;

class nRFSN {
public:
	nRFSN(char CEpin, char IRQpin);
	void clearInt(char intNum);
	char updateStatus(void);
	void close(void);
	void setPower(char pwrLvl);
	void setTXMode(void);
	void setRXMode(void);
	void setMAX_RT(char numRT);
	void setChannel(char ch);
	void setTXAddr(char addr[], char len);
	void setRXAddr(char pipe, char addr[], char len);
	void putBuffer(char* bufData, char len);
	void putBuffer(char* bufData, char command, char len);
	void getBuffer(char* bufData, char len);
	int getPayloadSize(void);
	void getPayload(char payloadSize, char offset);
	char configReg(char wr, char command, char data);
	void transfer(char wrn, char command, char len, char offset);
	void transmit(char len);
	void main(void);
protected:
	char Status;
	char Buf[32];
	char CEpin;
	char IRQpin;

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
};


nRFSN::nRFSN(char CEpin, char IRQpin) {
	this->CEpin = CEpin;
	this->IRQpin = IRQpin;

	bcm2835_init();

	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);

	bcm2835_gpio_fsel(CEpin, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(IRQpin, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_write(CEpin, LOW);

	// nRF defaults
	this->CONFIG_CURR       = 0b00001010;   // Show all TX interrupts; Enable CRC - 1 byte; Power Up; PTX
	this->EN_AA_CURR        = 0b00000011;   // Enable Auto Ack on pipe 0,1
	this->EN_RXADDR_CURR    = 0b00000011;   // Enable data pipe 0,1
	this->SETUP_AW_CURR     = 0b00000010;   // set for 4 byte address
	this->SETUP_RETR_CURR   = 0b00110101;   // 1000us retransmit delay; 5 auto retransmits
	this->RF_CH_CURR        = 0b01101001;   // Channel 105 (2.400GHz + 0.105GHz = 2.505GHz)
	this->RF_SETUP_CURR     = 0b00000110;   // RF data rate to 1Mbps; 0dBm output power (highest)
	this->DYNPD_CURR        = 0b00000011;   // Set dynamic payload for pipe 0
	this->FEATURE_CURR      = 0b00000100;   // Enable dynamic payload

	for (int i = 0; i < 4; i++) {
		this->RX_ADDRESS[i] = 0xE7; // 4 byte initial RX address
		this->TX_ADDRESS[i] = 0xC7; // 4 byte initial TX address
	}

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
	setRXAddr(RX_ADDR_P0,TX_ADDRESS,4);
	// set RX address
	setRXAddr(RX_ADDR_P1,RX_ADDRESS,4);
	// Set dynamic payload for pipe 0
	configReg('w',DYNPD,DYNPD_CURR);
	// Write to FEATURE register
	configReg('w',FEATURE,FEATURE_CURR);
	// Flush RX FIFO
	transfer('n',FLUSH_RX,0,0);
	// Flush TX FIFO
	transfer('n',FLUSH_TX,0,0);

	clearInt(0x70);
}


void nRFSN::clearInt(char intNum) {
	configReg('w',STATUS,intNum);
}


char nRFSN::updateStatus(void) {
	this->Status = bcm2835_spi_transfer(NRF_NOP);
	return Status;
}


/*------------------------------------------------
 * Set nRF output power. 0: lowest 3: highest
------------------------------------------------*/
void nRFSN::setPower(char pwrLvl)
{
	RF_SETUP_CURR = pwrLvl << 1;			// shift 1 bit left
	configReg('w',RF_SETUP,RF_SETUP_CURR);
}


/*------------------------------------------------
 * Set nRF to transmit mode
------------------------------------------------*/
void nRFSN::setTXMode(void)
{
	CONFIG_CURR = 0b01001010;
	configReg('w',CONFIG,CONFIG_CURR);
}


/*------------------------------------------------
 * Set nRF to receive mode
------------------------------------------------*/
void nRFSN::setRXMode(void)
{
	CONFIG_CURR = 0b00101011;
	configReg('w',CONFIG,CONFIG_CURR);
}


/*------------------------------------------------
 * Set nRF max number of retransmits
 * char numRT: Auto Retransmit Count (0: 250us 1111(15): 4000us; each +1 = +150us)
------------------------------------------------*/
void nRFSN::setMAX_RT(char numRT) {
	// mask out current Auto Retransmit Delay, and OR it with numRT with upper 4 bits (numbers > 16) masked out
	// result is current ARD and new ARC values
	SETUP_RETR_CURR = (SETUP_RETR_CURR & 0b11110000) | (numRT & 0b00001111);
	configReg('w',SETUP_RETR,SETUP_RETR_CURR);
}


/*------------------------------------------------
 * Set nRF channel (Fo = 2400 + channel [MHz])
------------------------------------------------*/
void nRFSN::setChannel(char ch) {
    RF_CH_CURR = ch;
    configReg('w',RF_CH,RF_CH_CURR);
}


/*------------------------------------------------
 * Sets new transmit address
 * char addr[]: new address
 * char len: length of address (nRFSN uses 4)
------------------------------------------------*/
void nRFSN::setTXAddr(char addr[], char len)
{
	char packet[6];
    packet[0] = W_REGISTER|TX_ADDR;

    if (len != 0) {
        // Send address bytes
        for (int i=1;i<=len;i++) {
            packet[i] = addr[i-1];
        }
    }

    bcm2835_spi_transfern(packet,len+1);
}


/*------------------------------------------------
 * Sets new receive address
 * char pipe: pipe number (0-5); nRFSN uses 0, 1
 * char addr[]: new address
 * char len: length of address (nRFSN uses 4)
------------------------------------------------*/
void nRFSN::setRXAddr(char pipe, char addr[], char len)
{
	char packet[6];
	packet[0] = W_REGISTER|pipe;

    if (len != 0) {
        // Send address bytes
        for (int i=1;i<=len;i++) {
        	packet[i] = addr[i-1];
        }
    }

    bcm2835_spi_transfern(packet,len+1);
}


/*------------------------------------------------
 * Put data of size len into SPI I/O buffer
 * len <= 32
------------------------------------------------*/
void nRFSN::putBuffer(char* bufData, char len) {
	if (len != 0) {
		for (int i = 0; i < len; i++) {
			Buf[i+1] = bufData[i];
		}
	}
}


/*------------------------------------------------
 * Get data of size len into SPI I/O buffer
------------------------------------------------*/
void nRFSN::getBuffer(char* bufData, char len) {
	if (len != 0) {
		for (int i = 0; i < len; i++) {
			 bufData[i] = Buf[i];
		}
	}
}


int nRFSN::getPayloadSize() {
	char packet[2] = {R_RX_PL_WID, NRF_NOP};
	bcm2835_spi_transfern(packet,2);
	return packet[1];
}


void nRFSN::getPayload(char payloadSize, char offset) {

	transfer('r',R_RX_PAYLOAD,payloadSize,offset);
}


/*------------------------------------------------
 * nRF configure register.
 * char wrn: write('w')/read('r')
 * char command: command byte
 *					- command byte is ORed with W_REGISTER, R_REGISTER
 * char data: data byte to send
 * returns data byte. If writing to register, will return 0x00
------------------------------------------------*/
char nRFSN::configReg(char wr, char command, char data) {
	char packet[2];
	if (wr == 'w') {                           // Write
		packet[1] = data;
        packet[0] = W_REGISTER|command;        // Construct command
        bcm2835_spi_transfern(packet,2);
    } else if (wr == 'r') {                    // Read
    	packet[1] = NRF_NOP;
    	packet[0] = R_REGISTER|command;        // Construct command
    	bcm2835_spi_transfern(packet,2);
    }

    return packet[1];
}


/*------------------------------------------------
 * nRF transfer data. DO NOT USE TO TRANSMIT!
 * char wrn: write('w')/read('r')/none('n')
 * char command: command byte
 *					- if wrn = 'w' or 'r', command byte is ORed with W_REGISTER, R_REGISTER
 * char len: length of data in bytes
				- data to be sent must be in i/o buffer
 * char offet: where in the buffer to start read/write
------------------------------------------------*/
void nRFSN::transfer(char wrn, char command, char len, char offset)
{
    // construct command byte
    if (wrn == 'w') {                       // Write
        Buf[0] = W_REGISTER|command;     // Send command
    } else if (wrn == 'r') {                // Read
    	Buf[0] = R_REGISTER|command;     // Send command
    } else if(wrn == 'n') {
    	Buf[0] = command;                // Send command
    	len++;								// make sure command is sent
    }

	// if there is data, do it byte at a time
    if (len != 0) {
        bcm2835_spi_transfern(Buf,len);
    }
}


/*------------------------------------------------
 * nRF transmit data.
 * char len: length of data in bytes
				- data to be sent must be in output buffer
-------------------------------------------------*/
void nRFSN::transmit(char len)
{
    if (len > 0)
    {
        Buf[0] = 0xA0;		// Send write payload command

        // write payload
        bcm2835_spi_transfern(Buf,len+1);

        // toggle CE pin to transmit
        bcm2835_gpio_write(CEpin, HIGH);
        bcm2835_delayMicroseconds(10);
        bcm2835_gpio_write(CEpin, LOW);
    }
}


void nRFSN::close(void) {
	bcm2835_spi_end();
	bcm2835_close();
}


int main(void) {
	printf("Starting test.\n");
	bcm2835_init();

	nRFSN net(RPI_V2_GPIO_P1_22 , RPI_V2_GPIO_P1_07);
	net.setTXMode();

	char str[33];
	while (1) {
		printf ("Enter string: ");
		scanf ("%32s",str);

		if (str[0] == 'e' && str[1] == 'x' && str[2] == 'i' && str[3] == 't') {
			break;
		}

		if (str[0] == 's' && str[1] == 't' && str[2] == 'a' && str[3] == 't' && str[4] == 'u' && str[5] == 's') {
			printf("STATUS: %#04x\n",net.updateStatus());
		} else {

			int end = strlen(str);
			if (end >= 32) {
				end = 31;
			}
			printf("Length: %d\n",end);

			net.putBuffer(str,end);
			net.transmit(end);

			bcm2835_delayMicroseconds(160 + (end*8));

			//bcm2835_delayMicroseconds(2000);

			char s = 0;
			while(1) {
				s = net.updateStatus();
				if (s > 0x0F) {
					printf("STATUS: %#04x\n",net.updateStatus());
					net.clearInt(0x70);
					break;
				} else if (s == 0x0F) {
					printf("STATUS: %#04x\n",net.updateStatus());
					break;
				}
				bcm2835_delayMicroseconds(200);
			}
		}
	}

	net.close();
	printf("Finished test.\n");

	return 0;
}
