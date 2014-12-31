//******************************************************************
//
// Copyright 2014 Intel Corporation.
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

#include <stdlib.h>
#include <math.h>
#include "mraa.h"

#define ONBOARD_LED_PIN 13
#define TEMPERATURE_AIO_PIN 0
#define LIGHT_SENSOR_AIO_PIN 2

namespace Sensors
{
mraa_gpio_context led_gpio = NULL;
mraa_aio_context tmp_aio = NULL;
mraa_aio_context light_aio = NULL;

inline void SetupPins()
{
    led_gpio = mraa_gpio_init(ONBOARD_LED_PIN); // Initialize pin 13
    if (led_gpio != NULL)
        mraa_gpio_dir(led_gpio, MRAA_GPIO_OUT); // Set direction to OUTPUT
    tmp_aio = mraa_aio_init(TEMPERATURE_AIO_PIN);   // initialize pin 0
    light_aio = mraa_aio_init(LIGHT_SENSOR_AIO_PIN);   // initialize pin 2
}

inline void ClosePins()
{
    mraa_gpio_close(led_gpio);
    mraa_aio_close(tmp_aio);
    mraa_aio_close(light_aio);
}

inline void SetOnboardLed(int on)
{
    if (led_gpio == NULL)
    {
        led_gpio = mraa_gpio_init(ONBOARD_LED_PIN); // Initialize pin 13
        if (led_gpio != NULL)
            mraa_gpio_dir(led_gpio, MRAA_GPIO_OUT); // Set direction to OUTPUT
    }
    if (led_gpio != NULL)
        mraa_gpio_write(led_gpio, on); // Writes into GPIO
}

inline float GetTemperatureInC()
{
    float ret = 0;
    if (tmp_aio == NULL)
    {
        tmp_aio = mraa_aio_init(TEMPERATURE_AIO_PIN); // initialize pin 0
    }
    if (tmp_aio != NULL)
    {
        uint16_t adc_value = mraa_aio_read(tmp_aio); // read the raw value
        //convert reading to temperature
        float beta = 4090.0; //the beta of the thermistor, magic number
        ret = beta / (log((4095.0 * 10 / adc_value - 10) / 10) + beta / 298.0) - 273.0;
    }
    return ret;
}

// This function returns light level between 0 and 4095
inline int GetLightLevel()
{
    uint16_t adc_value = 0;
    if (light_aio == NULL)
        light_aio = mraa_aio_init(LIGHT_SENSOR_AIO_PIN); // initialize pin 2
    if (light_aio != NULL)
        adc_value = mraa_aio_read(light_aio); // read the raw value
    return adc_value;
}
}
