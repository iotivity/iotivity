//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __ANDROID__
    #include <android/log.h>
#elif defined ARDUINO
    #include "Arduino.h"
    #include <avr/pgmspace.h>
#endif

// Use the PCF macro to wrap strings stored in FLASH on the Arduino
// Example:  OC_LOG(INFO, TAG, PCF("Entering function"));
#ifdef ARDUINO
    #define PCF(str)  ((const prog_char*)(F(str)))
#else
    #define PCF(str) str
#endif

// Max buffer size used in variable argument log function
#define MAX_LOG_V_BUFFER_SIZE (256)

// Log levels
typedef enum {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
    FATAL
} LogLevel;


#if defined(__ANDROID__) || defined(__linux__)
    /**
     * Output a variable argument list log string with the specified priority level.
     * Only defined for Linux and Android
     *
     * @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
     * @param tag    - Module name
     * @param format - variadic log string
     */
    void OCLogv(LogLevel level, const char * tag, const char * format, ...);

    /**
     * Output a log string with the specified priority level.
     * Only defined for Linux and Android
     *
     * @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
     * @param tag    - Module name
     * @param logStr - log string
     */
    void OCLog(LogLevel level, const char * tag, const char * logStr);

    /**
     * Output the contents of the specified buffer (in hex) with the specified priority level.
     *
     * @param level      - DEBUG, INFO, WARNING, ERROR, FATAL
     * @param tag        - Module name
     * @param buffer     - pointer to buffer of bytes
     * @param bufferSize - max number of byte in buffer
     */
    void OCLogBuffer(LogLevel level, const char * tag, const uint8_t * buffer, uint16_t bufferSize);
#endif

#ifdef ARDUINO
    /**
     * Initialize the serial logger for Arduino
     * Only defined for Arduino
     */
    void OCLogInit();

    /**
     * Output a log string with the specified priority level.
     * Only defined for Arduino.  Uses PROGMEM strings
     *
     * @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
     * @param tag    - Module name
     * @param logStr - log string
     */
    void OCLog(LogLevel level, const prog_char * tag, const prog_char * logStr);

    /**
     * Output the contents of the specified buffer (in hex) with the specified priority level.
     *
     * @param level      - DEBUG, INFO, WARNING, ERROR, FATAL
     * @param tag        - Module name
     * @param buffer     - pointer to buffer of bytes
     * @param bufferSize - max number of byte in buffer
     */
    void OCLogBuffer(LogLevel level, const prog_char * tag, const uint8_t * buffer, uint16_t bufferSize);
#endif

#ifdef TB_LOG
    // These macros are defined for Linux, Android, and Arduino
    #define OC_LOG(level, tag, logStr)  OCLog((level), (tag), (logStr))
    #define OC_LOG_BUFFER(level, tag, buffer, bufferSize)  OCLogBuffer((level), (tag), (buffer), (bufferSize))

    #ifdef ARDUINO
        // Use full namespace for logInit to avoid function name collision
        #define OC_LOG_INIT()    OCLogInit()
        // Don't define variable argument log function for Arduino
        #define OC_LOG_V(level, tag, ...)
    #else
        // Don't define LOG_INIT for Linux and Android
        #define OC_LOG_INIT()
        // Define variable argument log function for Linux and Android
        #define OC_LOG_V(level, tag, ...)  OCLogv((level), (tag), __VA_ARGS__)
    #endif

#else
    #define OC_LOG(level, tag, logStr)
    #define OC_LOG_V(level, tag, ...)
    #define OC_LOG_BUFFER(level, tag, buffer, bufferSize)
    #define OC_LOG_INIT()
#endif

#endif /* LOGGER_H_ */
