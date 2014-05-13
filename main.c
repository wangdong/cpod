#include <stdio.h>
#include <string.h>

#include <nordic_common.h>
#include <nrf24le1.h>

#include <hal_delay.h>
#include <hal_spi.h>
#include <hal_w2.h>

#include <gzll_mcu.h>
#include <gzll.h>
#include <gzp.h>

#include <ssd1306.h>
#include <am2312.h>

// --------------------------------------------------------
static uint8_t payload[GZLL_MAX_PAYLOAD_LENGTH];


// --------------------------------------------------------
void init();
void loop();

void main() {
	for (init();;loop());
}

void init() {

	P1DIR &= ~(BIT_2|BIT_3);
	P0DIR = 0;
	P1DIR = 1;

	EA = 1;

	hal_w2_configure_master(HAL_W2_100KHZ);

	ssd1306_init();
	ssd1306_puts_8x16(0, 0, "nRF24LE1");
}

const char* ANIM_PROGRESS_KEYFRAMES[] = {">  ", ">> ", ">>>"};
int keyframe = 0;
#define countof(array) (sizeof(array)/sizeof(array[0]))

uint16_t stat_error = 0;
uint16_t stat_all   = 0;

void loop() {
	am2312_ACInfo ac = {0};
	char buff[32] = "";

	hal_w2_enable(true);
	ac = am2312_read_ac();
	hal_w2_enable(false);

	if (ac.isOK) {
		sprintf(buff, "TEMP: %0.1fC", ac.temperature/10.0f);
		puts8x16(1, 0, buff);

		sprintf(buff, "HUMI: %0.1f%c", ac.humidity/10.0f, '%');
		puts8x16(2, 0, buff);

		puts6x8(7, 122, " ");
	}
	else {
		puts6x8(7, 122, "E");
		++stat_error;
	}

	sprintf(buff, "%s", ANIM_PROGRESS_KEYFRAMES[keyframe]);
	puts8x16(0, 98, buff);
	keyframe = (keyframe+1) % countof(ANIM_PROGRESS_KEYFRAMES);

	stat_all += 1;
	sprintf(buff, "%d/%d", stat_error, stat_all);
	puts6x8(7, 0, buff);

	delay_s(2);
}
