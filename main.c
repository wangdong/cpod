#include <stdio.h>
#include <string.h>

#include <nordic_common.h>
#include <nrf24le1.h>

#include <hal_delay.h>
#include <hal_spi.h>

#include <gzll_mcu.h>
#include <gzll.h>
#include <gzp.h>

#include <ssd1306.h>

// --------------------------------------------------------
sbit BLINK = P0 ^ 4;
static uint8_t payload[GZLL_MAX_PAYLOAD_LENGTH];


// --------------------------------------------------------
void init();
void loop();

void main() {	
	for (init();;loop());
}

void init() {
	mcu_init();
	gzll_init();
	gzp_init();
	gzp_pairing_enable(true);

	// Open pipe 2. (Pipe 0 and 1 are reserved by pairing library).
	gzll_set_param(GZLL_PARAM_RX_PIPES, gzll_get_param(GZLL_PARAM_RX_PIPES) | (1 << 2));

	
	P0DIR = 0x00;
	P1DIR = 0x00;

	EA = 1;

	// Enter host mode (start monitoring for data)
	gzll_rx_start();

	ssd1306_init();
	ssd1306_puts_8x16(0,  0, "nRF24LE1");
}

void decode_radio() {
	memset(payload, 0, GZLL_MAX_PAYLOAD_LENGTH);

	// If gzpair_host_execute() returns true, a pairing request has been received
	gzp_host_execute();

	// If Host ID request received
	if(gzp_id_req_received())
	{
	  // Always grant request
		gzp_id_req_grant();
	}

	// If any data received (plaintext on pipe 2 or encrypted through Gazell pairing library)
	if((gzll_get_rx_data_ready_pipe_number() == 2) || (gzp_crypt_user_data_received()))
	{
	  // Plaintext data received? 
		if(!gzll_rx_fifo_read(payload, NULL, NULL))
			gzp_crypt_user_data_read(payload, NULL);
	}
}

void loop() {	
	char buff[128] = "";

	decode_radio();

	sprintf(buff, "RX[%d]", payload[0]);
	ssd1306_puts_8x16(1, 0, buff);

	BLINK = ~BLINK;
	delay_ms(200);
}
