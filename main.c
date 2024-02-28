#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>

#include "config.h"
#include <stdint.h>

#include "spi.h"
#include "rf24def.h"
#include "btle.h"
#include "time.h"
#include "uart.h"

btle_t radio_a;
#ifdef MULTI_RADIO
btle_t radio_b;
btle_t radio_c;
#endif


static void setup(void)
{
	/* enable PCINT 12, 10, 8 */
	PCICR = (1 << PCIE1);
	PCMSK1 = (1 << PCINT12);
	tm_init();
	uart_init();
	#ifdef DEBUG
	uart_print("> BOOT\n");
	#endif
}


ISR(PCINT1_vect)
{
	cli();

	if((PINC & (1 << RADIO_A_IRQ)) == 0) {
		btle_load(&radio_a);
		btle_decode(&radio_a);
	}
	#ifdef MULTI_RADIO
	if((PINC & (1 << RADIO_B_IRQ)) == 0) {
		btle_load(&radio_b);
		btle_decode(&radio_b);
	}
	if((PINC & (1 << RADIO_C_IRQ)) == 0) {
		btle_load(&radio_c);
		btle_decode(&radio_c);
	}
	#endif

	sei();
}


static void rf_diag(uint8_t radio_ss)
{
	uint8_t res = 0;

	const uint8_t regs[] = {
		RF_REG_CONFIG,
		RF_REG_ENAA,
		RF_REG_SETAW,
		RF_REG_SETRF,
		RF_REG_SETRETR,
		RF_REG_RXWP0,
		RF_REG_RXADDR,
		RF_REG_RFCH
	};

	uart_print("> DEBUG\n");
	for (uint8_t i = 0; i < sizeof(regs); i++) {
		spi_select(radio_ss);
		spi_transfer(RF_CMD_RREG | regs[i]);
		res = spi_transfer(RF_CMD_NOP);
		spi_unselect(radio_ss);
		uart_bin(res);
	}
	uart_print(">\n");
}


int main(void)
{
	setup();
	#ifndef MULTI_RADIO
	uint8_t current = 0;
	#endif

	btle_init(&radio_a, RADIO_A_SS);
	#ifdef MULTI_RADIO
	btle_set_channel(0, &radio_a);

	btle_init(&radio_b, RADIO_B_SS);
	btle_set_channel(1, &radio_b);

	btle_init(&radio_c, RADIO_C_SS);
	btle_set_channel(2, &radio_c);

	btle_enable();
	#endif


	#ifdef DEBUG
	rf_diag(RADIO_A_SS);
	#ifdef MULTI_RADIO
	rf_diag(RADIO_B_SS);
	rf_diag(RADIO_C_SS);
	#endif
	#endif

	while (1) {
		#ifndef MULTI_RADIO
		btle_enable();
		tm_halt(LISTEN_DURATION);
		btle_disable();

		current = (current < 2) ? current + 1 : 0;
		btle_set_channel(current, &radio_a);
		#endif

		cli();
		if (btle_received(&radio_a)) {
			print_csv(&radio_a);
		}
		#ifdef MULTI_RADIO
		if (btle_received(&radio_b)) {
			print_csv(&radio_b);
		}
		if (btle_received(&radio_c)) {
			print_csv(&radio_c);
		}
		#endif
		sei();
	}

	return 0;
}
