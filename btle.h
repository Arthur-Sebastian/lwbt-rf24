#ifndef BTLE
#define BTLE

/* Bluetooth Core 4.0, Volume 6, Part B, Section 2.1.2 */
#define BTLE_ADDR0       0x8E
#define BTLE_ADDR1       0x89
#define BTLE_ADDR2       0xBE
#define BTLE_ADDR3       0xD6

/* Bluetooth Core 4.0, Volume 6, Part B, Section 1.4.1 */
#define BTLE_CH_ADV1 2
#define BTLE_CH_ADV2 26
#define BTLE_CH_ADV3 80
#define BTLE_IN_ADV1 37
#define BTLE_IN_ADV2 38
#define BTLE_IN_ADV3 39

/* Bluetooth Core 4.0, Volume 6, Part B, Section 2.3 */
#define BTLE_ADV_IND         0x40
#define BTLE_ADV_NONCONN_IND 0x42
#define BTLE_ADV_SCAN_IND    0x46

#define BTLE_PACKET_HEADER 0
#define BTLE_PACKET_LENGTH 1
#define BTLE_PACKET_MAC    2
#define BTLE_PACKET_PDU    8

#define BTLE_DATA_MANUFACTURER 0xFF

/* Bluetooth Core 4.0, Volume 6, Part B, Section 3.2 */
#define BLTE_WHITEN_POLY 0x88

#endif
