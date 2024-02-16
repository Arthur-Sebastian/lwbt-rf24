/**
@file btle.h
@brief nRF24L01 BT 4.2 LE advertising stack driver.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#ifndef BTLE
#define BTLE

#include <stdint.h>

#define RADIO_DDR_CE  DDRB
#define RADIO_PORT_CE PORTB
#define RADIO_CE      0

typedef enum {
	BTLE_TX,
	BTLE_RX
} btle_mode_t;

typedef struct {
	uint8_t      ch;
	uint8_t      buffer[32];
	uint32_t     rx_crc;
	uint8_t      buffer_in;
	uint8_t      buffer_len;
	uint8_t      spi_ss;
	btle_mode_t  mode;
} btle_t;


void btle_init(btle_t *driver, uint8_t spi_ss, btle_mode_t mode);


void btle_enable(void);


uint8_t btle_received(btle_t *driver);


void btle_disable(void);


void btle_set_channel(uint8_t channel, btle_t *driver);


void btle_load(btle_t *driver);


void btle_decode(btle_t *driver);


void btle_advertise(btle_t *driver, uint8_t *data, uint8_t size);

#endif
