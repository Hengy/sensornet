
#include <stdint.h>
#include <bcm2835.h>
#include <stdio.h>


int main(){
	uint8_t CEpin = 25;
	uint8_t IRQpin = 4;

	bcm2835_init();

	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);

	bcm2835_gpio_fsel(CEpin, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(IRQpin, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_write(CEpin, HIGH);

	printf("sending.\n");

	for (int i = 0; i < 3; i++) {
		bcm2835_spi_transfer(0xFF);
	}

	printf("done sending.\nwaiting for pin.\n");

	bcm2835_gpio_hen(RPI_V2_GPIO_P1_07);

	bcm2835_delayMicroseconds(200);

	while (!bcm2835_gpio_eds(RPI_V2_GPIO_P1_07)) {

	}

	printf("Exiting program.\n");

	return 0;
}
