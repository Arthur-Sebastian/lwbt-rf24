/**
@file config.h
@brief LWBT-RF24 configuration.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#ifndef CONFIG
#define CONFIG

#include "./rf24def.h"

/**
@brief Enables debug output on serial link.
*/
//#define DEBUG

/**
@brief CPU clock speed.
*/
#define F_CPU 16000000UL

/**
@brief Whether to use multiple radios.
*/
#define MULTI_RADIO

#define BTLE_MODE BTLE_TX


#define BTLE_PA_LEVEL RF_PA_MID

/**
@brief Operation (RX or TX) duration in milliseconds.
*/
#define OPERATION_DURATION 200

#define RADIO_A_SS  PC5
#define RADIO_A_IRQ PC4
#define RADIO_B_SS  PC3
#define RADIO_B_IRQ PC2
#define RADIO_C_SS  PC1
#define RADIO_C_IRQ PC0
#endif
