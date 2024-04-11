/**
@file btle.h
@brief nRF24L01 BT 4.2 LE advertising stack driver.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#ifndef BTLE
#define BTLE

#include <stdint.h>


typedef enum {
	BTLE_TX,
	BTLE_RX
} btle_mode_t;


typedef struct {
	uint8_t  data[32];
	uint8_t  size;
	uint32_t crc;
} buffer_t;

typedef struct {
	uint8_t      ch;
	buffer_t     *buffer;
	buffer_t     *back_buffer;
	buffer_t     a_buffer;
	buffer_t     b_buffer;

	uint8_t      spi_ss;
	uint8_t      rssi_min;
	btle_mode_t  mode;
} btle_t;


void btle_init(btle_t *driver, uint8_t spi_ss, btle_mode_t mode);


void btle_enable(void);


uint8_t btle_received(btle_t *driver);


void btle_disable(void);


void btle_set_channel(uint8_t channel, btle_t *driver);


void btle_load(btle_t *driver);


void btle_swap_buffers(btle_t *driver);


void btle_decode(btle_t *driver);


void btle_advertise(btle_t *driver, uint8_t *data, uint8_t size);


void btle_rssi_threshold(btle_t *driver);

#endif
