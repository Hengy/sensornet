#include <stdint.h>
#include <bcm2835.h>
#include <stdio.h>
#include <signal.h>

#define CLK_DIV BCM2835_PWM_CLOCK_DIVIDER_64 // 19.2MHz/64 = 300KHz PWM Frequency

#define PIN RPI_GPIO_P1_12 //RPi GPIO18

#define PWM_CHANNEL 0

#define RANGE 64 // 300KHz/64 = ~4687Hz steps

int keepAlive;

// catch Ctrl-C
void intHandler(int dummy=0) {
    keepAlive = 0;
}


int main(){

	keepAlive = 1;

	bcm2835_init();

	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_ALT5);
	
	bcm2835_pwm_set_clock(CLK_DIV);
    bcm2835_pwm_set_mode(PWM_CHANNEL, 1, 1);
    bcm2835_pwm_set_range(PWM_CHANNEL, RANGE);
	
	
	int data
	while(keepAlive) {
		
		if (data == 20) {
			data = 50;	// ~78%
		} else {
			data = 20;	// ~31%
		}
		bcm2835_pwm_set_data(PWM_CHANNEL, data);
		
		bcm2835_delay(1000);
	}
	
	bcm2835_close();
	return 0;
}
