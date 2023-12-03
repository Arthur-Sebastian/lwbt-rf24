#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>

#include "config.h"
#include <stdint.h>
#include <util/delay.h>

#include "spi.h"
#include "rf24def.h"
#include "btle.h"


#define BAUD 9600
#define UBRR (F_CPU/16/BAUD-1)

btle_t radio_a;

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
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
	uart_print("> START\n");
}


ISR(PCINT1_vect)
{
	cli();

	if((PINC & (1 << PC4)) == 0) {
		btle_load(&radio_a);
		btle_decode(&radio_a);
	}

	sei();
}


static void rf_diag(void)
{
	uint8_t res = 0;

	const uint8_t regs[] = {
		RF_REG_CONFIG,
		RF_REG_ENAA,
		RF_REG_SETAW,
		RF_REG_SETRF,
		RF_REG_SETRETR,
		RF_REG_RXWP0,
		RF_REG_RXADDR
	};

	uart_print("> DIAG\n");
	for (uint8_t i = 0; i < sizeof(regs); i++) {
		spi_select(PC5);
		spi_transfer(RF_CMD_RREG | regs[i]);
		res = spi_transfer(RF_CMD_NOP);
		spi_unselect(PC5);
		uart_bin(res);
	}
	uart_print("> END DIAG\n");
}


void print_bytes(uint8_t* data, uint8_t length)
{
	for(uint8_t i = 0; i < length; i++) {
		//uart_bin(*(data + i));
		uart_hex(*(data + i));
	}
	uart_print("\n");
}


int main(void)
{
	radio_a.spi_ss = PC5;
	uint8_t current = 0;

	setup();
	btle_init(&radio_a);
	rf_diag();

	while (1) {
		btle_enable();
		_delay_ms(20);
		btle_disable();

		current = (current < 2) ? current + 1 : 0;
		btle_set_channel(current, &radio_a);

		if (!btle_received(&radio_a)) {
			continue;
		}

		uart_print("\nDUMP:\n");
		print_bytes(radio_a.rx_buffer, radio_a.rx_len + 5);
		uart_print("HEAD:\n");
		print_bytes(radio_a.rx_buffer, 2);
		uart_print("CRC:\n");
		print_bytes((uint8_t *) &radio_a.rx_crc, 3);
	}

	return 0;
}
