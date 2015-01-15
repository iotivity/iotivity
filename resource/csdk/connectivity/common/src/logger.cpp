//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

static const uint16_t LINE_BUFFER_SIZE = (16 * 2) + 16 +
        1; // Show 16 bytes, 2 chars/byte, spaces between bytes, null termination

// Convert LogLevel to platform-specific severity level.  Store in PROGMEM on Arduino
#ifdef __ANDROID__
static android_LogPriority LEVEL[] =
{   ANDROID_LOG_DEBUG, ANDROID_LOG_INFO, ANDROID_LOG_WARN, ANDROID_LOG_ERROR, ANDROID_LOG_FATAL};

#elif defined __linux__
#include <dlog/dlog.h>
#define LOG_TAG "CA"
#define FONT_COLOR_RESET    "\033[0m"
#define FONT_COLOR_RED      "\033[31m"
#define FONT_COLOR_YELLOW   "\033[33m"
#define FONT_COLOR_GREEN    "\033[32m"

//#elif defined __linux__
static const char *LEVEL[] =
{   "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
static const char *LEVEL[] __attribute__((unused)) =
{   "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
#elif defined ARDUINO
#include <stdarg.h>
#include "Arduino.h"

PROGMEM const char level0[] = "DEBUG";
PROGMEM const char level1[] = "INFO";
PROGMEM const char level2[] = "WARNING";
PROGMEM const char level3[] = "ERROR";
PROGMEM const char level4[] = "FATAL";

PROGMEM const char * const LEVEL[]  = {level0, level1, level2, level3, level4};

//    static void OCLogString(LogLevel level, PROGMEM const char * tag, PROGMEM const char * logStr);
#endif

#if defined(__ANDROID__) || defined(__linux__) || defined(__TIZEN__)
/**
 * Output a variable argument list log string with the specified priority level.
 * Only defined for Linux and Android
 *
 * @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
 * @param tag    - Module name
 * @param format - variadic log string
 */
void OICLogv(LogLevel level, const char *tag, const char *format, ...)
{
    if (!format || !tag)
    {
        return;
    }
    char buffer[MAX_LOG_V_BUFFER_SIZE];
    memset(buffer, 0, sizeof buffer);
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof buffer - 1, format, args);
    va_end(args);
    OICLog(level, tag, buffer);
}

/**
 * Output a log string with the specified priority level.
 * Only defined for Linux and Android
 *
 * @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
 * @param tag    - Module name
 * @param logStr - log string
 */
void OICLog(LogLevel level, const char *tag, const char *logStr)
{
    if (!logStr || !tag)
    {
        return;
    }

#ifdef __ANDROID__
    __android_log_write(LEVEL[level], tag, logStr);
#else //#elif __TIZEN__
    LOGD(logStr);
    /*#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG tag
#endif
    if(level == DEBUG || level == INFO){
        LOGD(logStr);
    }else if(level == WARNING){
        LOGW(logStr);
    }else if(level == ERROR || level == FATAL){
        LOGE(logStr);
    }*/
    //#elif defined __linux__
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
/*
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
 */

#endif

#ifdef ARDUINO
/**
 * Initialize the serial logger for Arduino
 * Only defined for Arduino
 */
void OICLogInit()
{
    Serial.begin(115200);
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
/*
 void OCLogString(LogLevel level, PROGMEM const char * tag, const char * logStr) {
 if (!logStr || !tag) {
 return;
 }

 char buffer[LINE_BUFFER_SIZE] = {0};
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
 }
 */
/**
 * Output the contents of the specified buffer (in hex) with the specified priority level.
 *
 * @param level      - DEBUG, INFO, WARNING, ERROR, FATAL
 * @param tag        - Module name
 * @param buffer     - pointer to buffer of bytes
 * @param bufferSize - max number of byte in buffer
 */

/*
 void OCLogBuffer(LogLevel level, PROGMEM const char * tag, const uint8_t * buffer, uint16_t bufferSize) {
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
 }
 */
/**
 * Output a log string with the specified priority level.
 * Only defined for Arduino.  Uses PROGMEM strings
 *
 * @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
 * @param tag    - Module name
 * @param logStr - log string
 */
void OICLog(LogLevel level, PROGMEM const char *tag, const int16_t lineNum, PROGMEM const char *logStr)
{
    if (!logStr || !tag)
    {
        return;
    }
    char buffer[LINE_BUFFER_SIZE] = {0};
    strcpy_P(buffer, (char*)pgm_read_word(&(LEVEL[level])));
    Serial.print(buffer);
    char c;
    Serial.print(F(": "));
    while ((c = pgm_read_byte(tag)))
    {
        Serial.write(c);
        tag++;
    }
    Serial.print(F(": "));
    Serial.print(lineNum);
    Serial.print(F(": "));
    while ((c = pgm_read_byte(logStr))) {
     Serial.write(c);
     logStr++;
     }
    Serial.println();
}

/**
 * Output a variable argument list log string with the specified priority level.
 * Only defined for Arduino as depicted below.
 *
 * @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
 * @param tag    - Module name
 * @param format - variadic log string
 */
void OICLogv(LogLevel level, const char *tag, const int16_t lineNum, const char *format, ...)
{
    char buffer[LINE_BUFFER_SIZE];
    va_list ap;
    va_start(ap, format);
    //strcpy_P(buffer, (char*)pgm_read_word(&(LEVEL[level])));
    //Serial.print(buffer);

    Serial.print(LEVEL[level]);
    //char c;
    Serial.print(F(": "));
    /*while ((c = pgm_read_byte(tag))) {
     Serial.write(c);
     tag++;
     }*/
    Serial.print(tag);
    Serial.print(F(": "));
    Serial.print(lineNum);
    Serial.print(F(": "));
    vsnprintf(buffer, sizeof(buffer), format, ap);
    for (char *p = &buffer[0]; *p; p++)
    {
        // emulate cooked mode for newlines
        if (*p == '\n')
        {
            //Serial.write('\r');
            Serial.print('\r');
        }
        //Serial.write(*p);
        Serial.print(*p);
    }
    Serial.println();
    va_end(ap);
}
/**
 * Output a variable argument list log string with the specified priority level.
 * Only defined for Arduino as depicted below.
 *
 * @param level  - DEBUG, INFO, WARNING, ERROR, FATAL
 * @param tag    - Module name
 * @param format - variadic log string
 */
void OICLogv(LogLevel level, const char *tag, const __FlashStringHelper *format, ...)
{
    Serial.print("OCLOGV : ");
    char buffer[LINE_BUFFER_SIZE];
    va_list ap;
    va_start(ap, format);
    // strcpy_P(buffer, (char*)pgm_read_word(&(LEVEL[level])));
    // Serial.print(buffer);

    Serial.print(LEVEL[level]);
    // char c;
    Serial.print(F(": "));

    /*while ((c = pgm_read_byte(tag))) {
     Serial.write(c);
     tag++;
     }*/
    Serial.print(tag);
    Serial.print(F(": "));

#ifdef __AVR__
    vsnprintf_P(buffer, sizeof(buffer), (const char *)format, ap); // progmem for AVR
#else
    vsnprintf(buffer, sizeof(buffer), (const char *)format, ap); // for the rest of the world
#endif
    for (char *p = &buffer[0]; *p; p++)
    {
        // emulate cooked mode for newlines
        if (*p == '\n')
        {
            // Serial.write('\r');
            Serial.print('\r');
        }
        //Serial.write(*p);
        Serial.print(p);
    }
    Serial.println();
    va_end(ap);
}
#endif

