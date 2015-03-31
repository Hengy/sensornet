#include <stdint.h>
#include <bcm2835.h>
#include <stdio.h>
#include <signal.h>

#define CLK_DIV BCM2835_PWM_CLOCK_DIVIDER_2048 // 19.2MHz/64 = 300KHz PWM Frequency

#define PIN RPI_GPIO_P1_12 //RPi GPIO18

#define PWM_CHANNEL 0

#define RANGE 32 // 300KHz/64 = ~4687Hz steps

static int keepAlive;

static int verbose = 0;

// catch Ctrl-C
void intHandler(int sig) {

	signal(sig, SIG_IGN);

	keepAlive = 0;
}


int getTemp(void) {
	FILE *temperatureFile;
	double T;
	temperatureFile = fopen ("/sys/class/thermal/thermal_zone0/temp", "r");

	fscanf (temperatureFile, "%lf", &T);
	T /= 1000;

	fclose (temperatureFile);

	return T;
}


int main(int argc, char *argv[]) {

	if ( argc > 1 ) {
		if ( (strcmp(argv[1], "-v") ) || (strcmp(argv[1], "-verbose") ) ) {
			printf("Verbose mode ON\n");
			verbose = 1;
		}
	}

	keepAlive = 1;

	signal(SIGINT, intHandler);

	bcm2835_init();

	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_ALT5);

	bcm2835_pwm_set_clock(CLK_DIV);
	bcm2835_pwm_set_mode(PWM_CHANNEL, 1, 1);
    	bcm2835_pwm_set_range(PWM_CHANNEL, RANGE);

	int speed = 0;
	double temp = 100.0;
	double tempPercent = 1.0;
	double fanSpeed;

	while (keepAlive) {

		temp = getTemp();

		if (verbose) {
			printf("temp: %2.1f\n", temp);
		}

		tempPercent = temp / 35.0;
		if (tempPercent > 1.0) {
			tempPercent = 1.0;
		}

		if (verbose) {
			printf("tempPercent: %1.4f\n", tempPercent);
		}

		if (temp > 32) {
			fanSpeed = tempPercent * 16;
		} else {
			fanSpeed = (tempPercent*tempPercent) * 16;
		}

		speed = 16 - (int) fanSpeed;

		if (verbose) {
			printf("speed: %2d\n", speed);
		}

		bcm2835_pwm_set_data(PWM_CHANNEL, speed);

		if (temp <= 30) {
			bcm2835_delay(10000);
		} else if (temp < 33) {
			bcm2835_delay(5000);
		} else {
			bcm2835_delay(2000);
		}
	}

	bcm2835_pwm_set_mode(PWM_CHANNEL, 1, 0);

	bcm2835_close();

	printf("Exit was safe!\n");

	return 0;
}

