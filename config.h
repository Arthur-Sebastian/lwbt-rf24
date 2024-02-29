/**
@file config.h
@brief LWBT-RF24 configuration.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#ifndef CONFIG
#define CONFIG


#define F_CPU 16000000UL
#define DEBUG

#define RX_MULTI  1
#define RX_SINGLE 2
#define TX_SINGLE 3

#define OPERATION_MODE RX_MULTI
//#define OPERATION_MODE RX_SINGLE
//#define OPERATION_MODE TX_SINGLE

#define BTLE_PA_LEVEL RF_PA_HIGH

#define CHANNEL_LISTEN_TIME  20
#define ADVERTISING_INTERVAL 500


/* evaluation board specific */

#define RADIO_DDR_CE  DDRB
#define RADIO_PORT_CE PORTB
#define RADIO_CE      0

#define SPI_DDR       DDRB
#define SPI_PORT      PORTB
#define SPI_SCK       5
#define SPI_MISO      4
#define SPI_MOSI      3
#if OPERATION_MODE == TX_SINGLE
	#define SPI_DDR_SS    DDRB
	#define SPI_PORT_SS   PORTB
#else
	#define SPI_DDR_SS    DDRC
	#define SPI_PORT_SS   PORTC
#endif

#define RADIO_A_PIN_REG PINC
#if OPERATION_MODE == TX_SINGLE
	#define RADIO_A_SS      PB2
#else
	#define RADIO_A_SS      PC5
#endif
#define RADIO_A_IRQ     PC4

#define RADIO_B_PIN_REG PINC
#define RADIO_B_SS      PC3
#define RADIO_B_IRQ     PC2

#define RADIO_C_PIN_REG PINC
#define RADIO_C_SS      PC1
#define RADIO_C_IRQ     PC0

#endif
