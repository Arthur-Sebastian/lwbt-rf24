/**
@file btle.c
@brief nRF24L01 BT 4.2 LE advertising stack driver.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#include <avr/io.h>
#include <stdint.h>

#include "config.h"

#include "time.h"
#include "btle.h"
#include "rf24def.h"
#include "spi.h"


/* Bluetooth Core 4.2, Vol 6, Part B, 2.1.2 */
#define BTLE_ADV_ADDR0       0x8E
#define BTLE_ADV_ADDR1       0x89
#define BTLE_ADV_ADDR2       0xBE
#define BTLE_ADV_ADDR3       0xD6

/* Bluetooth Core 4.2, Vol 6, Part B, 2.3 */
#define BTLE_ADV_NONCONN_IND 0x42

#define BTLE_PACKET_HEADER  0
#define BTLE_PACKET_LENGTH  1
#define BTLE_PACKET_PAYLOAD 2

#define BTLE_HEADER_LENGTH  2
#define BTLE_CRC_LENGTH     3
#define BTLE_MIN_PDU_LENGTH 6
#define BTLE_MAX_PDU_LENGTH 27
#define BTLE_BASE_CHANNEL   37

/* Bluetooth Core 4.2, Vol 6, Part B, 1.4.1 */
static const uint8_t ch_phy[] = {  2, 26, 80 };


/* implemented in btle.S */
extern uint32_t btle_crc(uint8_t *data, uint8_t len);
extern void btle_whiten(uint8_t *data, uint8_t len, uint8_t channel);


static void btle_send_command(uint8_t cmd, btle_t *driver)
{
	uint8_t spi_ss = driver -> spi_ss;

	spi_select(spi_ss);
	spi_transfer(cmd);
	spi_unselect(spi_ss);
}


static void btle_set_register(uint8_t reg, uint8_t value, btle_t *driver)
{
	uint8_t spi_ss = driver -> spi_ss;

	spi_select(spi_ss);
	spi_transfer(RF_CMD_WREG | reg);
	spi_transfer(value);
	spi_unselect(spi_ss);
}


static inline void btle_radio_setup(btle_t *driver)
{
	uint8_t spi_ss = driver -> spi_ss;

	// no auto ack
	btle_set_register(RF_REG_ENAA,    0x00, driver);
	// set 32bit address
	btle_set_register(RF_REG_SETAW,   0x02, driver);
	// disable retransmissions
	btle_set_register(RF_REG_SETRETR, 0x00, driver);
	// data rate 1mbps, TX -12dBm
	btle_set_register(RF_REG_SETRF,   BTLE_PA_LEVEL, driver);
	// set BTLE channel addresses for RX
	spi_select(spi_ss);
	spi_transfer(RF_CMD_WREG | RF_REG_RXAP0);
	spi_setLSBFirst();
	spi_transfer(BTLE_ADV_ADDR0);
	spi_transfer(BTLE_ADV_ADDR1);
	spi_transfer(BTLE_ADV_ADDR2);
	spi_transfer(BTLE_ADV_ADDR3);
	spi_setMSBFirst();
	spi_unselect(spi_ss);
	// set BTLE channel addresses for TX
	spi_select(spi_ss);
	spi_transfer(RF_CMD_WREG | RF_REG_TXADDR);
	spi_setLSBFirst();
	spi_transfer(BTLE_ADV_ADDR0);
	spi_transfer(BTLE_ADV_ADDR1);
	spi_transfer(BTLE_ADV_ADDR2);
	spi_transfer(BTLE_ADV_ADDR3);
	spi_setMSBFirst();
	spi_unselect(spi_ss);
	// payload width
	btle_set_register(RF_REG_RXWP0,    32, driver);
	// RX on pipe 0
	btle_set_register(RF_REG_RXADDR, 0x01, driver);
	// clear flags
	btle_set_register(RF_REG_STATUS, RF_STATUS_ALL, driver);
	// PRIM_RX = 0 -> MASK_RX_DR = 1 | PRIM_RX = 1 -> MASK_TX_DS = 1
	uint8_t int_mask = (0x40 >> (driver -> mode));
	uint8_t config = 0x12
		| driver -> mode
		| int_mask;
	// power up, no CRC, PRIM_RX = mode, interrupt mask dependent on mode
	btle_set_register(RF_REG_CONFIG, config, driver);
}


void btle_disable(void)
{
	RADIO_PORT_CE &= ~(1 << RADIO_CE);
}


void btle_enable(void)
{
	RADIO_PORT_CE |= (1 << RADIO_CE);
}


void btle_init(btle_t *driver, uint8_t spi_ss, btle_mode_t mode)
{
	driver -> mode = mode;
	driver -> spi_ss = spi_ss;
	RADIO_DDR_CE |= (1 << RADIO_CE);

	spi_init();
	spi_bind(spi_ss);

	btle_radio_setup(driver);

	btle_send_command(RF_CMD_FLUSHRX, driver);
	btle_send_command(RF_CMD_FLUSHTX, driver);
}


uint8_t btle_received(btle_t *driver)
{
	uint8_t status = driver -> buffer_in;
	driver -> buffer_in = 0;
	return status;
}


void btle_set_channel(uint8_t channel, btle_t *driver)
{
	driver -> ch = channel;
	btle_set_register(RF_REG_RFCH, ch_phy[channel], driver);
}

void btle_load(btle_t *driver)
{
	uint8_t spi_ss = driver -> spi_ss;
	btle_set_register(RF_REG_STATUS, RF_STATUS_ALL, driver);

	spi_select(spi_ss);
	spi_transfer(RF_CMD_READRX);
	spi_setLSBFirst();
	for (uint8_t i = 0; i < 32; i++) {
		*(driver -> buffer + i) = spi_transfer(RF_CMD_NOP);
	}
	spi_setMSBFirst();
	spi_unselect(spi_ss);
}


void btle_decode(btle_t *driver)
{
	uint8_t *buffer_ptr = driver -> buffer;
	driver -> buffer_in = 0;
	/* check packet validity */
	uint16_t header = *((uint16_t *) buffer_ptr);
	uint8_t channel = driver -> ch + BTLE_BASE_CHANNEL;
	btle_whiten((uint8_t *) &header, 2, channel);
	uint8_t pdu_type = *((uint8_t *) &header + BTLE_PACKET_HEADER);
	if(pdu_type != BTLE_ADV_NONCONN_IND) {
		return;
	}
	uint8_t length = *((uint8_t *) &header + BTLE_PACKET_LENGTH);
	if(length > BTLE_MAX_PDU_LENGTH || length < BTLE_MIN_PDU_LENGTH) {
		return;
	}

	/* decode the rest of it when valid */
	driver -> buffer_len = length;
	btle_whiten(buffer_ptr, length + BTLE_HEADER_LENGTH + BTLE_CRC_LENGTH, channel);
	driver -> rx_crc = btle_crc(buffer_ptr, length + BTLE_HEADER_LENGTH);
	driver -> buffer_in = 1;
}

void btle_advertise(btle_t* driver, uint8_t* data, uint8_t size) {
	if(driver -> mode != BTLE_TX) {
		return;
	}

	uint8_t spi_ss = driver -> spi_ss;
	uint8_t channel = driver -> ch + BTLE_BASE_CHANNEL;
	uint8_t *buffer_ptr = driver -> buffer;

	for(uint8_t i = 0; i < size; i++) {
		*(buffer_ptr + i) = *(data + i);
	}
	uint32_t crc = btle_crc(buffer_ptr, size);
	*((uint32_t *) (buffer_ptr + size)) = crc;

	btle_whiten(buffer_ptr, size + BTLE_CRC_LENGTH, channel);

	btle_send_command(RF_CMD_FLUSHTX, driver);

	spi_select(spi_ss);
	spi_transfer(RF_CMD_WRITETX);
	spi_setLSBFirst();
	for(uint8_t i = 0; i < size + BTLE_CRC_LENGTH; i++) {
		spi_transfer(*(buffer_ptr + i));
	}
	spi_setMSBFirst();
	spi_unselect(spi_ss);
}
