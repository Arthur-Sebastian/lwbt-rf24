/*
 * LWBT-RF24
 *
 * nRF24L01 BT 4.2 LE advertising stack driver
 * (C) Artur Sebastian Miller 2023
*/


#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "spi.h"
#include "rf24def.h"


/* Bluetooth Core 4.2, Vol 6, Part B, 2.1.2 */
#define BTLE_ADV_ADDR0       0x8E
#define BTLE_ADV_ADDR1       0x89
#define BTLE_ADV_ADDR2       0xBE
#define BTLE_ADV_ADDR3       0xD6


/* Bluetooth Core 4.2, Vol 6, Part B, 1.4.1 */
static const uint8_t ch_phy[] = {  2, 26, 80 };
static const uint8_t ch_ind[] = { 37, 38, 39 };


static struct {

	uint8_t channel;
	uint8_t *rx_buffer;
	uint8_t rx_in;
	uint8_t pdu_header[2];

} btle_driver;


static void btle_radioCommand(uint8_t cmd)
{
	spi_setSelect();
	spi_transfer(cmd);
	spi_clearSelect();
}


static void btle_radioSetReg(uint8_t reg, uint8_t value)
{
	spi_setSelect();
	spi_transfer(RF_CMD_WREG | reg);
	spi_transfer(value);
	spi_clearSelect();
}


static inline void btle_radioSetup()
{
	// no auto ack
	btle_radioSetReg(RF_REG_ENAA, 0x00);
	// set 32bit address
	btle_radioSetReg(RF_REG_SETAW, 0x02);
	// data rate 1mbps
	btle_radioSetReg(RF_REG_SETRF, 0x06);
	// disable retransmissions
	btle_radioSetReg(RF_REG_SETRETR, 0x00);
	// set BTLE channel address
	spi_setSelect();
	spi_transfer(RF_CMD_WREG | RF_REG_RXAP0);
	spi_setLSBFirst();
	spi_transfer(BTLE_ADV_ADDR0);
	spi_transfer(BTLE_ADV_ADDR1);
	spi_transfer(BTLE_ADV_ADDR2);
	spi_transfer(BTLE_ADV_ADDR3);
	spi_setMSBFirst();
	spi_clearSelect();
	// payload width
	btle_radioSetReg(RF_REG_RXWP0, 32);
	// RX on pipe 0
	btle_radioSetReg(RF_REG_RXADDR, 0x01);
	// clear flags
	btle_radioSetReg(RF_REG_STATUS, RF_STATUS_ALL);
	// power up, no CRC, PRIM_RX
	btle_radioSetReg(RF_REG_CONFIG, 0x33);
	_delay_ms(RF_DELAY_START);
}


void btle_radioDisable(void)
{
	RADIO_PORT_CE &= ~(1 << RADIO_CE);
}


void btle_radioEnable(void)
{
	RADIO_PORT_CE |= (1 << RADIO_CE);
	_delay_us(RF_DELAY_SETTLE);
}


void btle_init(uint8_t *buf)
{
	btle_driver.rx_buffer = buf;
	RADIO_DDR_CE |= (1 << RADIO_CE);
	spi_init();
	btle_radioSetup();
	btle_radioCommand(RF_CMD_FLUSHRX);
	btle_radioCommand(RF_CMD_FLUSHTX);
	sei();
}


uint8_t btle_dataPending(void)
{
	return btle_driver.rx_in--;
}


void btle_hopChannel(void)
{
	btle_driver.channel = (btle_driver.channel < 2) ? btle_driver.channel + 1 : 0;
	btle_radioSetReg(RF_REG_RFCH, ch_phy[btle_driver.channel]);
}


ISR(INT0_vect)
{
	cli();
	btle_radioSetReg(RF_REG_STATUS, RF_STATUS_ALL);
	spi_setSelect();
	spi_transfer(RF_CMD_READRX);
	spi_setLSBFirst();
	for (uint8_t i = 0; i < 32; i++) {
		*(btle_driver.rx_buffer + i) = spi_transfer(RF_CMD_NOP);
	}
	spi_setMSBFirst();
	spi_clearSelect();
	btle_driver.rx_in = 1;
	sei();
}
