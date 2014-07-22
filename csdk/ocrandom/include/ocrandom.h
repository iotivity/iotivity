//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


#ifndef _RANDOM_H
#define _RANDOM_H

#include <stdint.h>
#include <stdlib.h>

#if defined(__ANDROID__) || defined(__linux__)
#include <time.h>
#elif defined ARDUINO
//#include <Arduino.h>
//#include <wiring_analog.c>
#define ANALOG_IN (15)
#endif

/** 
 * Seed the random number generator. Seeding depends on platform.
 * Android and Linux uses current time. Arduino uses Analog reading on pin ANALOG_IN
 * @retval 0 for Success, otherwise some error value
 */
int8_t OCSeedRandom();

/** 
 * Generate a uniformly [0,2^32] distributed random number
 * @retval On Success, it returns the random value.
 */
uint32_t OCGetRandom();

/**
 * Generate a uniformly [0,2^8] distributed random number
 * @retval On Success, it returns the random value, otherwise -1 for error.
 */
uint8_t OCGetRandomByte(void);

/**
 * Generate a uniformly distributed 8-bit (byte) array random numbers
 * @param[out] location
 *              memory location to start filling with random bytes
 * @param[in] len
 *              length of array to be filled with random bytes
 */
void OCFillRandomMem(uint8_t * location, uint16_t len);

#endif //_RANDOM_H
