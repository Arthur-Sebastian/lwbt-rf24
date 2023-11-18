/**
@file config.h
@brief LWBT-RF24 configuration.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#ifndef CONFIG
#define CONFIG

/**
@brief CPU clock speed.
*/
#define F_CPU 16000000UL

/*
Default configuration for ATmega168P
SCK  - PB5
MISO - PB4
MOSI - PB3
SS   - PB2
CE   - PB0
*/

/**
@brief Radio Chip Enable (CE) pin DDR register.
*/
#define RADIO_DDR_CE  DDRB
/**
@brief Radio Chip Enable (CE) pin PORT register.
*/
#define RADIO_PORT_CE PORTB
/**
@brief Radio Chip Enable (CE) pin number.
*/
#define RADIO_CE      0
/**
@brief Radio Slave Select (SS) pin DDR register.
*/
#define SPI_DDR_SS    DDRB
/**
@brief Radio Slave Select (SS) pin PORT register.
*/
#define SPI_PORT_SS   PORTB
/**
@brief Radio Slave Select (SS) pin number.
*/
#define SPI_SS        2
/**
@brief ATmega SPI hardware DDR register.
*/
#define SPI_DDR       DDRB
/**
@brief ATmega SPI hardware PORT register.
*/
#define SPI_PORT      PORTB
/**
@brief ATmega SPI hardware Serial Clock (SCK) pin number.
*/
#define SPI_SCK       5
/**
@brief ATmega SPI hardware Master Input Slave Output (MISO) pin number.
*/
#define SPI_MISO      4
/**
@brief ATmega SPI hardware Master Output Slave Input (MOSI) pin number.
*/
#define SPI_MOSI      3

#endif
