/*
 * LWBT-RF24
 *
 * driver configuration
 * (C) Artur Sebastian Miller 2023
*/


#ifndef CONFIG
#define CONFIG

#define F_CPU 16000000UL
/*
 * SCK  - PB5
 * MISO - PB4
 * MOSI - PB3
 * SS   - PB2
 * CE   - PB0
*/
#define RADIO_DDR_CE  DDRB
#define RADIO_PORT_CE PORTB
#define RADIO_CE      0
#define SPI_DDR_SS    DDRB
#define SPI_PORT_SS   PORTB
#define SPI_SS        2
#define SPI_DDR       DDRB
#define SPI_PORT      PORTB
#define SPI_SCK       5
#define SPI_MISO      4
#define SPI_MOSI      3

#endif
