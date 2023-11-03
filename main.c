#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>

#define F_CPU 16000000UL
#include <util/delay.h>

#include "rf24.h"
#include "btle.h"

#define BAUD 9600
#define UBRR (F_CPU/16/BAUD-1)

#define PIN_CE    (1 << 0)

/* all on PB */
#define SPI_SCK  (1 << 5)
#define SPI_MISO (1 << 4)
#define SPI_MOSI (1 << 3)
#define SPI_SS   (1 << 2)

static uint8_t rx_buf[34];
static uint8_t rx_in = 0;

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


static void setup(void)
{
	/* enable INT0 */
	EIMSK = 0x01;
	EICRA = 0x02;
	/* board setup */
	DDRB = PIN_CE;
	/* config SPI */
	PORTB |= SPI_SS;
	DDRB |= SPI_SCK | SPI_MOSI | SPI_SS;
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	/* config UART */
	UBRR0H = (uint8_t)(UBRR >> 8);
	UBRR0L = (uint8_t) UBRR;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);

	rx_buf[32] = '\n';
	rx_buf[33] = '\0';
	uart_print("> START\n");
}


static uint8_t spi_sendByte(uint8_t byte)
{
	PORTB &= ~SPI_SS;
	SPDR = byte;
	/* wait for transmission complete */
	while ( !(SPSR & (1 << SPIF)) );;
	return SPDR;
}


static void rf_start(void)
{
	_delay_ms(RF_DELAY_START);
	spi_sendByte(RF_CMD_FLUSHRX);
	PORTB |= SPI_SS;
	spi_sendByte(RF_CMD_FLUSHTX);
	PORTB |= SPI_SS;
	_delay_ms(RF_DELAY_POWER);
	PORTB |= PIN_CE;
	_delay_us(RF_DELAY_SETTLE);
}


static void rf_config(void)
{
	// no auto ack
	spi_sendByte(RF_CMD_WREG | RF_REG_ENAA);
	spi_sendByte(0x00);
	PORTB |= SPI_SS;
	// set 32bit address
	spi_sendByte(RF_CMD_WREG | RF_REG_SETAW);
	spi_sendByte(0x02);
	PORTB |= SPI_SS;
	// data rate 1mbps
	spi_sendByte(RF_CMD_WREG | RF_REG_SETRF);
	spi_sendByte(0x06);
	PORTB |= SPI_SS;
	// disable retransmissions
	spi_sendByte(RF_CMD_WREG | RF_REG_SETRETR);
	spi_sendByte(0x00);
	PORTB |= SPI_SS;
	// set BTLE channel address
	spi_sendByte(RF_CMD_WREG | RF_REG_RXAP0);
	/* temporarily change bit order */
	SPCR |= (1 << DORD);
	spi_sendByte(BTLE_ADDR0);
	spi_sendByte(BTLE_ADDR1);
	spi_sendByte(BTLE_ADDR2);
	spi_sendByte(BTLE_ADDR3);
	/* restore bit order */
	SPCR ^= (1 << DORD);
	PORTB |= SPI_SS;
	// pipe 0 payload width
	spi_sendByte(RF_CMD_WREG | RF_REG_RXWP0);
	spi_sendByte(0x20);
	PORTB |= SPI_SS;
	// RX on pipe 0
	spi_sendByte(RF_CMD_WREG | RF_REG_RXADDR);
	spi_sendByte(0x01);
	PORTB |= SPI_SS;
	// clear flags
	spi_sendByte(RF_CMD_WREG | RF_REG_STATUS);
	spi_sendByte(0x70);
	PORTB |= SPI_SS;
	// power up, no CRC, PRIM_RX
	spi_sendByte(RF_CMD_WREG | RF_REG_CONFIG);
	spi_sendByte(0x33);
	PORTB |= SPI_SS;
	_delay_us(1500);
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
		spi_sendByte(RF_CMD_RREG | regs[i]);
		res = spi_sendByte(RF_CMD_NOP);
		PORTB |= SPI_SS;
		uart_bin(res);
	}

	spi_sendByte(RF_CMD_RREG | RF_REG_RXAP0);
	for (uint8_t i = 0; i < 4; i++) {
		res = spi_sendByte(RF_CMD_NOP);
		uart_bin(res);
	}
	PORTB |= SPI_SS;

	uart_print("> END DIAG\n");
}


static void btle_whiten(uint8_t *data, uint8_t len, uint8_t channel)
{
	uint8_t lfsr = channel | 0x40;
	uint8_t result;

	while (len--) {
		result = 0;

		for (uint8_t i = 0x01; i != 0; i <<= 1) {
			if (lfsr & 0x01) {
				lfsr ^= BLTE_WHITEN_POLY;
				result |= i;
			}
			lfsr >>= 1;
		}

		*data ^= result;
		data++;
	}
}


static uint8_t btle_checkpacket(uint8_t *data)
{
	if (*(data + BTLE_PACKET_HEADER) != BTLE_ADV_NONCONN_IND) {
		return -1;
	}
	if (*(data + BTLE_PACKET_LENGTH) != 27) {
		return -1;
	}
	/*
	if (*(data + BTLE_PACKET_PDU + 1) != BTLE_DATA_MANUFACTURER) {
		return -1;
	}
	*/

	return 0;
}


inline uint32_t btle_crc(uint8_t *data, uint8_t len);


ISR(INT0_vect)
{
	cli();
	spi_sendByte(RF_CMD_WREG | RF_REG_STATUS);
	spi_sendByte(RF_STATUS_ALL);
	PORTB |= SPI_SS;
	spi_sendByte(RF_CMD_READRX);
	SPCR |= (1 << DORD);
	for (uint8_t i = 0; i < 32; i++) {
		*(rx_buf + i) = spi_sendByte(RF_CMD_NOP);
	}
	PORTB |= SPI_SS;
	SPCR ^= (1 << DORD);
	rx_in = 1;
	sei();
}


int main(void)
{
	const uint8_t advch[] = {
		BTLE_CH_ADV1,
		BTLE_CH_ADV2,
		BTLE_CH_ADV3
	};
	uint8_t current = 0;
	uint32_t crc;
	uint8_t *crc_ptr = (uint8_t *) &crc;

	setup();
	rf_config();
	// rf_diag();
	rf_start();
	sei();

	while (1) {
		// bring CE high
		PORTB |= PIN_CE;
		_delay_ms(20);
		PORTB &= ~PIN_CE;
		if (rx_in != 0) {
			cli();
			rx_in = 0;
			/* whiten only header first for verification */
			btle_whiten((uint8_t *) rx_buf, 32 , 37 + current);
			if (btle_checkpacket((uint8_t *) rx_buf) == 0) {
				crc = btle_crc((uint8_t *) rx_buf, 29);
				uart_bin(*(crc_ptr));
				uart_bin(*(crc_ptr + 1));
				uart_bin(*(crc_ptr + 2));
				uart_bin(*(crc_ptr + 3));
				uart_print((char *) rx_buf);
			}
			sei();
		}
		// hop channel
		current = (current < 2) ? (current + 1) : 0;
		spi_sendByte(RF_CMD_WREG | RF_REG_RFCH);
		spi_sendByte(advch[current]);
		PORTB |= SPI_SS;
	}

	return 0;
}
