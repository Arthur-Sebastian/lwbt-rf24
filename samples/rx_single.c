#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "../config.h"

#include "samples.h"
#include "../btle.h"
#include "../time.h"
#include "../uart.h"


btle_t radio_a;


/* ISR macro usage sets up an interrupt vector,
 * we need to make sure is only happens when the
 * current module is used by the program */
#if OPERATION_MODE == RX_SINGLE
ISR(PCINT1_vect, ISR_NOBLOCK)
{
	/* prevent simultaneous occurences of this interrupt
	 * by disabling its source, allowing others to run */
	PCICR  &=~ (1 << PCIE1);
	if((RADIO_A_PIN_REG & (1 << RADIO_A_IRQ)) == 0) {
		btle_load(&radio_a);
		btle_decode(&radio_a);
	}
	PCICR  |= (1 << PCIE1);
}
#endif


void rx_single_setup(void)
{
	/* enable PCINT 12 */
	PCMSK1 |= (1 << PCINT12);
	PCICR  |= (1 << PCIE1);

	btle_init(&radio_a, RADIO_A_SS, BTLE_RX);
	btle_set_channel(0, &radio_a);

	sei();
	btle_enable();
}


void rx_single_loop(void)
{
	static uint32_t then = 0;
	static uint8_t  current_ch = 0;

	if (btle_received(&radio_a)) {
		uart_print_csv(&radio_a);
	}

	if(tm_ms() - then > CHANNEL_LISTEN_TIME) {
		then = tm_ms();

		current_ch = (current_ch < 2) ? current_ch + 1 : 0;
		btle_disable();
		btle_set_channel(current_ch, &radio_a);
		btle_enable();
	}
}
