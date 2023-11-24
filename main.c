#include <avr/io.h>
#include <stddef.h>

#include "config.h"
#include <util/delay.h>

#include "spi.h"
#include "rf24def.h"
#include "btle.h"


#define BAUD 9600
#define UBRR (F_CPU/16/BAUD-1)

#define PIN_CE    (1 << 0)


static uint8_t rx_buf[34];
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
	/* enable INT0 */
	EIMSK = 0x01;
	EICRA = 0x02;
	/* config UART */
	UBRR0H = (uint8_t)(UBRR >> 8);
	UBRR0L = (uint8_t) UBRR;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);

	rx_buf[32] = '\n';
	rx_buf[33] = '\0';
	uart_print("> START\n");
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
		spi_setSelect();
		spi_transfer(RF_CMD_RREG | regs[i]);
		res = spi_transfer(RF_CMD_NOP);
		spi_clearSelect();
		uart_bin(res);
	}

	uart_print("> END DIAG\n");
}


static uint8_t btle_checkpacket(uint8_t *data)
{
	if (*(data + BTLE_PACKET_HEADER) != BTLE_ADV_NONCONN_IND) {
		return -1;
	}
	if (*(data + BTLE_PACKET_LENGTH) != 27) {
		return -1;
	}

	return 0;
}


void print_bytes(uint8_t* data, uint8_t length) {
	for(uint8_t i = 0; i < length; i++) {
		//uart_bin(*(data + i));
		uart_hex(*(data + i));
	}
	uart_print("\n");
}

int main(void)
{
	uint8_t current = 0;
	uint16_t header;
	uint32_t crc;

	setup();
	btle_init((uint8_t *) rx_buf);
	//rf_diag();

	while (1) {
		btle_radioEnable();
		_delay_ms(20);
		btle_radioDisable();

		if (btle_dataPending() != 0) {

			header = *((uint16_t *) rx_buf);
			btle_whiten((uint8_t *) &header, 2, 37 + current);

			if (btle_checkpacket((uint8_t *) &header) == 0) {
				uint8_t pdu_len = *((uint8_t *) &header + BTLE_PACKET_LENGTH);
				btle_whiten((uint8_t *) rx_buf, pdu_len + 5 , 37 + current);
				crc = btle_crc((uint8_t *) rx_buf, pdu_len + 2);

				uart_print("\nDUMP:\n");
				print_bytes(rx_buf, pdu_len + 5);
				uart_print("HEAD:\n");
				print_bytes(rx_buf, 2);
				uart_print("PDU:\n");
				print_bytes(rx_buf + BTLE_PACKET_MAC, pdu_len);
				uart_print("CRC:\n");
				print_bytes(rx_buf + BTLE_PACKET_MAC + pdu_len, 3);
				print_bytes((uint8_t *) &crc, 3);
			}
		}

		current = (current < 2) ? current + 1 : 0;
		btle_hopChannel();
	}

	return 0;
}
