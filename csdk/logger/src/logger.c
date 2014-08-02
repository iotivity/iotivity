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


#include "logger.h"
#include "string.h"



static const uint16_t LINE_BUFFER_SIZE = (16 * 2) + 16 + 1;  // Show 16 bytes, 2 chars/byte, spaces between bytes, null termination

// Convert LogLevel to platform-specific severity level.  Store in PROGMEM on Arduino
#ifdef __ANDROID__
    static android_LogPriority LEVEL[] = {ANDROID_LOG_DEBUG, ANDROID_LOG_INFO, ANDROID_LOG_WARN, ANDROID_LOG_ERROR, ANDROID_LOG_FATAL};
#elif defined __linux__
    //static const char * LEVEL[] = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
    static const char * LEVEL[] __attribute__ ((unused)) = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
#elif defined ARDUINO

    #define MAX_ARDUINO_LEVEL_STR_SIZE   (8)

    prog_char level0[] PROGMEM = "DEBUG";
    prog_char level1[] PROGMEM = "INFO";
    prog_char level2[] PROGMEM = "WARNING";
    prog_char level3[] PROGMEM = "ERROR";
    prog_char level4[] PROGMEM = "FATAL";

    PROGMEM const prog_char * LEVEL[]  = {level0, level1, level2, level3, level4};

    static void OCLogString(LogLevel level, const prog_char * tag, const char * logStr);
#endif


#if defined(__ANDROID__) || defined(__linux__)
/**
 * Output a variable argument list log string with the specified priority level.
 * Only defined for Linux and Android
 *
 * @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
 * @param tag    - Module name
 * @param format - variadic log string
 */
void OCLogv(LogLevel level, const char * tag, const char * format, ...) {
    if (!format || !tag) {
        return;
    }
    char buffer[MAX_LOG_V_BUFFER_SIZE];
    memset(buffer, 0, sizeof buffer);
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof buffer - 1, format, args);
    va_end(args);
    OCLog(level, tag, buffer);
}

/**
 * Output a log string with the specified priority level.
 * Only defined for Linux and Android
 *
 * @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
 * @param tag    - Module name
 * @param logStr - log string
 */
void OCLog(LogLevel level, const char * tag, const char * logStr) {
    if (!logStr || !tag) {
        return;
    }

    #ifdef __ANDROID__
        __android_log_write(LEVEL[level], tag, logStr);
    #elif defined __linux__
        printf("%s: %s: %s\n", LEVEL[level], tag, logStr);
    #endif
}

/**
 * Output the contents of the specified buffer (in hex) with the specified priority level.
 *
 * @param level      - DEBUG, INFO, WARNING, ERROR, FATAL
 * @param tag        - Module name
 * @param buffer     - pointer to buffer of bytes
 * @param bufferSize - max number of byte in buffer
 */
void OCLogBuffer(LogLevel level, const char * tag, const uint8_t * buffer, uint16_t bufferSize) {
    if (!buffer || !tag || (bufferSize == 0)) {
        return;
    }

    char lineBuffer[LINE_BUFFER_SIZE];
    memset(lineBuffer, 0, sizeof lineBuffer);
    int lineIndex = 0;
    int i;
    for (i = 0; i < bufferSize; i++) {
        // Format the buffer data into a line
        sprintf(&lineBuffer[lineIndex++ * 3], "%02X ", buffer[i]);
        // Output 16 values per line
        if (((i+1)%16) == 0) {
            OCLog(level, tag, lineBuffer);
            memset(lineBuffer, 0, sizeof lineBuffer);
            lineIndex = 0;
        }
    }
    // Output last values in the line, if any
    if (bufferSize % 16) {
        OCLog(level, tag, lineBuffer);
    }
}

#endif



#ifdef ARDUINO
    /**
     * Initialize the serial logger for Arduino
     * Only defined for Arduino
     */
    void OCLogInit() {
    #ifdef TB_LOG
        Serial.begin(115200);
    #endif
    }

    /**
     * Output a log string with the specified priority level.
     * Only defined for Arduino.  Only uses PROGMEM strings
     * for the tag parameter
     *
     * @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
     * @param tag    - Module name
     * @param logStr - log string
     */
    void OCLogString(LogLevel level, const prog_char * tag, const char * logStr) {
    #ifdef TB_LOG
        if (!logStr || !tag) {
          return;
        }

        char buffer[MAX_ARDUINO_LEVEL_STR_SIZE] = {0};
        strcpy_P(buffer, (char*)pgm_read_word(&(LEVEL[level])));
        Serial.print(buffer);

        char c;
        Serial.print(F(": "));
        while ((c = pgm_read_byte(tag))) {
          Serial.write(c);
          tag++;
        }
        Serial.print(F(": "));

        Serial.println(logStr);
    #endif
    }

    /**
     * Output the contents of the specified buffer (in hex) with the specified priority level.
     *
     * @param level      - DEBUG, INFO, WARNING, ERROR, FATAL
     * @param tag        - Module name
     * @param buffer     - pointer to buffer of bytes
     * @param bufferSize - max number of byte in buffer
     */
    void OCLogBuffer(LogLevel level, const prog_char * tag, const uint8_t * buffer, uint16_t bufferSize) {
    #ifdef TB_LOG
        if (!buffer || !tag || (bufferSize == 0)) {
            return;
        }

        char lineBuffer[LINE_BUFFER_SIZE] = {0};
        uint8_t lineIndex = 0;
        for (uint8_t i = 0; i < bufferSize; i++) {
            // Format the buffer data into a line
            sprintf(&lineBuffer[lineIndex++ * 3], "%02X ", buffer[i]);
            // Output 16 values per line
            if (((i+1)%16) == 0) {
                OCLogString(level, tag, lineBuffer);
                memset(lineBuffer, 0, sizeof lineBuffer);
                lineIndex = 0;
            }
        }
        // Output last values in the line, if any
        if (bufferSize % 16) {
            OCLogString(level, tag, lineBuffer);
        }
    #endif
    }

    /**
     * Output a log string with the specified priority level.
     * Only defined for Arduino.  Uses PROGMEM strings
     *
     * @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
     * @param tag    - Module name
     * @param logStr - log string
     */
    void OCLog(LogLevel level, const prog_char * tag, const prog_char * logStr) {
    #ifdef TB_LOG
        if (!logStr || !tag) {
          return;
        }

        char buffer[MAX_ARDUINO_LEVEL_STR_SIZE] = {0};
        strcpy_P(buffer, (char*)pgm_read_word(&(LEVEL[level])));
        Serial.print(buffer);

        char c;
        Serial.print(F(": "));
        while ((c = pgm_read_byte(tag))) {
          Serial.write(c);
          tag++;
        }
        Serial.print(F(": "));

        while ((c = pgm_read_byte(logStr))) {
          Serial.write(c);
          logStr++;
        }
        Serial.println();
    #endif
    }
#endif


