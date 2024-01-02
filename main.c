#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>

#include "config.h"
#include <stdint.h>
#include <util/delay.h>

#include "spi.h"
#include "rf24def.h"
#include "btle.h"


/* Effectively doubled by setting U2X0 bit in UCSR0A */
#define BAUD 76800
#define UBRR (F_CPU/16/BAUD-1)

btle_t radio_a;
#ifdef MULTI_RADIO
btle_t radio_b;
btle_t radio_c;
#endif

static const char toHex[] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'A', 'B',
	'C', 'D', 'E', 'F'
};


static void uart_print(char *data)
{
	while (*data != '\0') {
		while ( !(UCSR0A & (1 << UDRE0)) );;
		UDR0 = *data;
		data++;
	}
}


static void uart_bin(uint8_t data)
{
	char buf[10];
	buf[9] = '\0';
	buf[8] = '\n';

	for (uint8_t i = 0; i < 8; i++) {
		buf[i] = ((data & 0x80) != 0) ? '1' : '0';
		data <<= 1;
	}

	uart_print(buf);
}

static void uart_hex(uint8_t data)
{
	uint8_t h_hex, l_hex;
	l_hex = data & 0x0F;
	h_hex = data >> 4;
	char buf[3];
	buf[0] = toHex[h_hex];
	buf[1] = toHex[l_hex];
	buf[2] = '\0';
	uart_print(buf);
}


static void setup(void)
{
	/* enable PCINT 12, 10, 8 */
	PCICR = (1 << PCIE1);
	PCMSK1 = (1 << PCINT12);
	/* config UART */
	UBRR0H = (uint8_t)(UBRR >> 8);
	UBRR0L = (uint8_t) UBRR;
	UCSR0A = (1 << U2X0);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
	uart_print("> BOOT\n");
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


void print_bytes(uint8_t* data, uint8_t length)
{
	for(uint8_t i = 0; i < length; i++) {
		uart_hex(*(data + i));
	}
	uart_print("\n");
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
		_delay_ms(LISTEN_DURATION);
		btle_disable();

		current = (current < 2) ? current + 1 : 0;
		btle_set_channel(current, &radio_a);
		#endif

		cli();
		if (btle_received(&radio_a)) {
			print_bytes(radio_a.rx_buffer, radio_a.rx_len + 2);
			print_bytes(radio_a.rx_buffer + radio_a.rx_len + 2, 3);
			print_bytes((uint8_t *) &radio_a.rx_crc, 3);
		}
		#ifdef MULTI_RADIO
		if (btle_received(&radio_b)) {
			print_bytes(radio_b.rx_buffer, radio_b.rx_len + 2);
			print_bytes(radio_b.rx_buffer + radio_b.rx_len + 2, 3);
			print_bytes((uint8_t *) &radio_b.rx_crc, 3);
		}
		if (btle_received(&radio_c)) {
			print_bytes(radio_c.rx_buffer, radio_c.rx_len + 2);
			print_bytes(radio_c.rx_buffer + radio_c.rx_len + 2, 3);
			print_bytes((uint8_t *) &radio_c.rx_crc, 3);
		}
		#endif
		sei();
	}

	return 0;
}
