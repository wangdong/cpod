#include <nordic_common.h>
#include <nrf24le1.h>

#include <hal_delay.h>
#include <hal_spi.h>

#include "ssd1306.h"

// --------------------------------------------------------
sbit BLINK = P0 ^ 4;


// --------------------------------------------------------
void init();
void loop();

void main() {	
	for (init();;loop());
}

void init() {
	P0DIR = 0x00;
	P1DIR = 0x00;

	EA = 1;

	ssd1306_init();
	ssd1306_puts_8x16(0,  0, "nRF24LE1");
	ssd1306_puts_6x8 (7,  0, "connected:1EFCE29" );
	
	ssd1306_puts_8x16(1,  0, "TEMP: 23.6" );
	ssd1306_puts_8x16(2,  0, "HUMI: 82%" );
}

void loop() {	
	BLINK = ~BLINK;
	delay_ms(500);
}
