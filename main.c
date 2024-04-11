#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>

#include "config.h"
#include <stdint.h>

#include "spi.h"
#include "rf24def.h"
#include "time.h"
#include "uart.h"

#include "samples/samples.h"

static void setup(void)
{
	tm_init();
	uart_init();
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

	#if   OPERATION_MODE == RX_MULTI
	rx_multi_setup();
	#elif OPERATION_MODE == RX_SINGLE
	rx_single_setup();
	#elif OPERATION_MODE == TX_SINGLE
	tx_single_setup();
	#endif

	#ifdef DEBUG
		rf_diag(RADIO_A_SS);
		#if OPERATION_MODE == RX_MULTI
		rf_diag(RADIO_B_SS);
		rf_diag(RADIO_C_SS);
		#endif
	#endif

	while (1) {
		#if   OPERATION_MODE == RX_MULTI
		rx_multi_loop();
		#elif OPERATION_MODE == RX_SINGLE
		rx_single_loop();
		#elif OPERATION_MODE == TX_SINGLE
		tx_single_loop();
		#endif
	}

	return 0;
}
