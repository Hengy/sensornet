// nRF24L01+ commands
#define R_REGISTER      0x00      // Read; Bits <5:0> = register map address (LSB first)
#define W_REGISTER      0x20      // Write; Bits <5:0> = register map address (LSB first)
#define R_RX_PAYLOAD    0x61      // Read RX payload 1-32 bytes (LSB first)
#define W_TX_PAYLOAD    0xA0      // Write TX payload 1-32 bytes (LSB first)
#define FLUSH_TX        0xE1      // Flush TX FIFO
#define FLUSH_RX        0xE2      // Flush RX FIFO
#define REUSE_TX_PL     0xE3      // TX; Reuse last transmitted payload; active until FLUSH_TX or W_TX_PAYLOAD
#define R_RX_PL_WID     0x60      // Read RX payload width for top R_RX_PAYLOAD in RX FIFO
#define W_ACK_PAYLOAD   0xA8      // RX; Write payload + ACK packet; <2:0> = write payload (LSB first)
#define W_TX_PAYLOAD_NO 0xB0      // TX; Disable AUTOACK on this specific packet
#define NRF_NOP         0xFF      // No operation; used as dummy data

// nRF24L01+ registers
#define CONFIG          0x00      // Configuration register
#define EN_AA           0x01      // Enable AUTOACK function
#define EN_RXADDR       0x02      // Enable RX addresses
#define SETUP_AW        0x03      // Setup address widths
#define SETUP_RETR      0x04      // Setup auto retransmission
#define RF_CH           0x05      // RF channel
#define RF_SETUP        0x06      // RF setup register
#define STATUS          0x07      // Status register
#define OBSERVE_TX      0x08      // Transmit observe register
#define RPD             0x09      // RPD (Carrier Detect)
#define RX_ADDR_P0      0x0A      // Receive address data for pipes 0-5
#define RX_ADDR_P1      0x0B
#define RX_ADDR_P2      0x0C
#define RX_ADDR_P3      0x0D
#define RX_ADDR_P4      0x0E
#define RX_ADDR_P5      0x0F
#define TX_ADDR         0x10      // Transmit address
#define RX_PW_P0        0x11      // Receive data width for pipes 0-5
#define RX_PW_P1        0x12
#define RX_PW_P2        0x13
#define RX_PW_P3        0x14
#define RX_PW_P4        0x15
#define RX_PW_P5        0x16
#define FIFO_STATUS     0x17      // FIFO status register
#define DYNPD           0x1C      // Enable dynamic payload length
#define FEATURE         0x1D		// Feature register

// nRF24L01+ interrupts
#define RX_DR			0x40		// Data received interrupt
#define TX_DS			0x20		// Data sent interrupt
#define MAX_RT			0x10		// Max retransmit interrupt