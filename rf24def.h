/**
@file rf24def.h
@brief nRF24L01 definition macros.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#ifndef RF24
#define RF24

/* nRF24L01+ Preliminary Product Specification 8.3.1, 9.1 */

#define RF_DELAY_START  5
#define RF_DELAY_POWER  5

#define RF_REG_CONFIG  0x00
#define RF_REG_ENAA    0x01
#define RF_REG_RXADDR  0x02
#define RF_REG_SETAW   0x03
#define RF_REG_SETRETR 0x04
#define RF_REG_RFCH    0x05
#define RF_REG_SETRF   0x06
#define RF_REG_STATUS  0x07
#define RF_REG_TXOBSV  0x08
#define RF_REG_CD      0x09
#define RF_REG_RXAP0   0x0A
#define RF_REG_RXAP1   0x0B
#define RF_REG_RXAP2   0x0C
#define RF_REG_RXAP3   0x0D
#define RF_REG_RXAP4   0x0E
#define RF_REG_RXAP5   0x0F
#define RF_REG_TXADDR  0x10
#define RF_REG_RXWP0   0x11

#define RF_CMD_RREG    0x00
#define RF_CMD_WREG    0x20
#define RF_CMD_READRX  0x61
#define RF_CMD_WRITETX 0xA0
#define RF_CMD_FLUSHRX 0xE2
#define RF_CMD_FLUSHTX 0xE1
#define RF_CMD_NOP     0xFF

#define RF_STATUS_RXREADY 0x40
#define RF_STATUS_TXREADY 0x20
#define RF_STATUS_MAXRT   0x10
#define RF_STATUS_ALL     0x70

#endif
