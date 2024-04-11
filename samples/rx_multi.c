#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "../config.h"

#include "samples.h"
#include "../btle.h"
#include "../uart.h"


static btle_t radio_a;
static btle_t radio_b;
static btle_t radio_c;


/* ISR macro usage sets up an interrupt vector,
 * we need to make sure is only happens when the
 * current module is used by the program */
#if OPERATION_MODE == RX_MULTI
ISR(PCINT1_vect, ISR_NOBLOCK)
{
	/* prevent simultaneous occurences of this interrupt
	 * by disabling its source, allowing others to run */
	PCICR  &=~ (1 << PCIE1);
	if((RADIO_A_PIN_REG & (1 << RADIO_A_IRQ)) == 0) {
		btle_rssi_threshold(&radio_a);
		btle_load(&radio_a);
		btle_decode(&radio_a);
	}
	if((RADIO_B_PIN_REG & (1 << RADIO_B_IRQ)) == 0) {
		btle_rssi_threshold(&radio_b);
		btle_load(&radio_b);
		btle_decode(&radio_b);
	}
	if((RADIO_C_PIN_REG & (1 << RADIO_C_IRQ)) == 0) {
		btle_rssi_threshold(&radio_c);
		btle_load(&radio_c);
		btle_decode(&radio_c);
	}
	PCICR  |= (1 << PCIE1);
}
#endif


void rx_multi_setup(void)
{
	/* enable PCINT 12, 10, 8 */
	PCMSK1 |= (1 << PCINT12) | (1 << PCINT10) | (1 << PCINT8);
	PCICR  |= (1 << PCIE1);

	btle_init(&radio_a, RADIO_A_SS, BTLE_RX);
	btle_set_channel(0, &radio_a);

	btle_init(&radio_b, RADIO_B_SS, BTLE_RX);
	btle_set_channel(1, &radio_b);

	btle_init(&radio_c, RADIO_C_SS, BTLE_RX);
	btle_set_channel(2, &radio_c);

	sei();
	btle_enable();
}


void rx_multi_loop(void)
{
	PCICR  &=~ (1 << PCIE1);
	if (btle_received(&radio_a)) {
		uart_print_csv(&radio_a);
	}
	if (btle_received(&radio_b)) {
		uart_print_csv(&radio_b);
	}
	if (btle_received(&radio_c)) {
		uart_print_csv(&radio_c);
	}
	PCICR  |= (1 << PCIE1);
}
