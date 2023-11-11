/*
 * LWBT-RF24
 *
 * nRF24L01 BT 4.2 LE advertising stack driver
 * (C) Artur Sebastian Miller 2023
*/


#ifndef BTLE
#define BTLE

/* Bluetooth Core 4.2, Vol 6, Part B, 2.3 */
#define BTLE_ADV_IND         0x40
#define BTLE_ADV_NONCONN_IND 0x42
#define BTLE_ADV_SCAN_IND    0x46

#define BTLE_PACKET_HEADER 0
#define BTLE_PACKET_LENGTH 1
#define BTLE_PACKET_MAC    2
#define BTLE_PACKET_PDU    8

#define BTLE_DATA_MANUFACTURER 0xFF


void btle_init(uint8_t *buf);


void btle_radioEnable(void);


uint8_t btle_dataPending(void);


void btle_radioDisable(void);


void btle_hopChannel(void);


/* implemented in btle.S */
extern uint32_t btle_crc(uint8_t *data, uint8_t len);

/* implemented in btle.S */
extern void btle_whiten(uint8_t *data, uint8_t len, uint8_t channel);


#endif
