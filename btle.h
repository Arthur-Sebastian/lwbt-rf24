/**
@file btle.h
@brief nRF24L01 BT 4.2 LE advertising stack driver.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#ifndef BTLE
#define BTLE

#include <stdint.h>

/* Bluetooth Core 4.2, Vol 6, Part B, 2.3 */
#define BTLE_ADV_IND         0x40
#define BTLE_ADV_NONCONN_IND 0x42
#define BTLE_ADV_SCAN_IND    0x46

#define BTLE_PACKET_HEADER 0
#define BTLE_PACKET_LENGTH 1
#define BTLE_PACKET_MAC    2
#define BTLE_PACKET_PDU    8

#define BTLE_DATA_MANUFACTURER 0xFF


/**
@brief Initializes the BTLE driver.

@param[out] buf Packet buffer address.
@return Nothing.
*/
void btle_init(uint8_t *buf);


/**
@brief Enables the radio.

@note This function should never be called before btle_hopChannel().
@see btle_hopChannel()

@return Nothing.
*/
void btle_radioEnable(void);


/**
@brief Checks for incoming data presence.

@return 1 when data present, 0 otherwise.
*/
uint8_t btle_dataPending(void);


/**
@brief Disables the radio.

@note This function should always be called before btle_hopChannel().
@see btle_hopChannel()

@return Nothing.
*/
void btle_radioDisable(void);


/**
@brief Hops to the next channel.

Changes the active channel to the next advertising channel
for listening or transmitting. This function should be called
after every transmission or after listening on a channel for
a given time.

@note Calling this function when the radio is enabled might cause
it to ignore the channel hop.
@see btle_radioEnable
@see btle_radioDisable

@return Nothing.
*/
void btle_hopChannel(void);


/* implemented in btle.S */
extern uint32_t btle_crc(uint8_t *data, uint8_t len);

/* implemented in btle.S */
extern void btle_whiten(uint8_t *data, uint8_t len, uint8_t channel);


#endif
