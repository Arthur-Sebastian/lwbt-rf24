/**
@file config.h
@brief LWBT-RF24 configuration.
@author Artur Miller
@copyright (C) Artur Miller 2023
*/

#ifndef CONFIG
#define CONFIG

/**
@brief Enables debug output on serial link.
*/
#define DEBUG

/**
@brief CPU clock speed.
*/
#define F_CPU 16000000UL

/**
@brief Whether to use multiple radios.
*/
#define MULTI_RADIO

/**
@brief Channel listening duration in milliseconds.
*/
#define LISTEN_DURATION 20

#define RADIO_A_SS  PC5
#define RADIO_A_IRQ PC4
#define RADIO_B_SS  PC3
#define RADIO_B_IRQ PC2
#define RADIO_C_SS  PC1
#define RADIO_C_IRQ PC0
#endif
