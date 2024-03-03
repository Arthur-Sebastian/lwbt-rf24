#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "../config.h"

#include "samples.h"
#include "../btle.h"
#include "../uart.h"
#include "../time.h"


static btle_t radio_a;


/* ISR macro usage sets up an interrupt vector,
 * we need to make sure is only happens when the
 * current module is used by the program */
#if OPERATION_MODE == TX_SINGLE
ISR(INT0_vect, ISR_NOBLOCK)
{
	/* there is no way that this interrupt fires twice,
	 * skip disabling its source for speed */
	btle_disable();
}
#endif


void tx_single_setup(void)
{
	/* enable INT0 */
	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC01);
	
	btle_init(&radio_a, RADIO_A_SS, BTLE_TX);
	btle_set_channel(0, &radio_a);

	sei();
}


void tx_single_loop(void)
{
	static uint32_t then = 0;
	static uint8_t  current_ch = 0;
	static uint32_t packet_id = 0;

	uint8_t payload[] = {
		0x42, 0x1B, /* ADV TYPE, PDU LENGTH */
		0xC8, 0x53, 0x5F, 0x05, 0xC2, 0x73, /* MAC ADDRESS */
		0x14, 0xFF, 0xEF, 0xBE, /* MANUFACTURER DATA, COMPANY IDENTIFIER */
		0xFF, 0xFF, 0x00, 0x00 ,0xAA, 0xAA, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF,
		0x00, 0x00, 0x00 /* PACKET ID */
	};


	if(tm_ms() - then > ADVERTISING_INTERVAL) {
		then = tm_ms();

		*((uint32_t *) (payload + sizeof(payload) - 3)) = packet_id;
		btle_advertise(&radio_a, payload, sizeof(payload));

		btle_decode(&radio_a);
		uart_print_csv(&radio_a);

		packet_id++;
	
		current_ch = (current_ch < 2) ? current_ch + 1 : 0;
		btle_set_channel(current_ch, &radio_a);
		/* disable in interrupt routine after transmission */
		btle_enable();
	}
}
