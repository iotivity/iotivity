/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

/**
 * This file contains the declaration of classes and its members related to TemperatureResource.
 */

#ifndef SMARTHOME_API_SERVER_TEMPERATURE_H_
#define SMARTHOME_API_SERVER_TEMPERATURE_H_

#include <SHBaseResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions for callback of
             * TemperatureResource class.
             */
            class TemperatureResourceDelegate
            {
            public:
                /**
                 * API to handle current temperature set request.
                 *
                 * @param[in] temperature temperature to be changed
                 * @param[in] units unit of temperature
                 *
                 * @return ResultCode ::SUCCESS if success
                 */
                virtual ResultCode setTemperatureCallback(double temperature,
                                                          std::string units) = 0;
            };

            /**
             * This class contains a set of functions to change the properties of Temperature.
             */
            class TemperatureResource : public SHBaseResource, public SHBaseResourceDelegate
            {
            public:
                TemperatureResource();
                virtual ~TemperatureResource();

                /**
                 * API to get the unit of temperature.
                 *
                 * @return unit of temperature
                 */
                std::string getTemperatureUnits();

                /**
                 * API to set the unit of temperature.
                 *
                 * @param[in] units unit
                 */
                void setTemperatureUnits(std::string units);

                /**
                 * API to get the current temperature.
                 *
                 * @return current temperature
                 */
                double getTemperature();

                /**
                 * API to set the current temperature.
                 *
                 * @param[in] temperature temperature
                 */
                void setTemperature(double temperature);

                /**
                 * API to get the min range of temperature.
                 *
                 * @return min range
                 */
                double getMinRange();

                /**
                 * API to set the min range of temperature.
                 *
                 * @param[in] temperature min range of temperature
                 */
                void setMinRange(double temperature);

                /**
                 * API to get the max range of temperature.
                 *
                 * @return max range of temperature
                 */
                double getMaxRange();

                /**
                 * API to set the max range of termperature.
                 *
                 * @param[in] max range of temperature
                 */
                void setMaxRange(double temperature);

                /**
                 * API to get the status of the switch.
                 *
                 * @return true if the switch is on
                 */
                void setTemperatureResourceDelegate(TemperatureResourceDelegate *switchDelegate);

            private:
                double convertCelsiusToFahrenheit(double celsius);
                double convertCelsiusToKelvin(double celsius);
                double convertFahrenheitToCelsius(double fahrenheit);
                double convertFahrenheitToKelvin(double fahrenheit);
                double convertKelvinToCelsius(double kelvin);
                double convertKelvinToFahrenheit(double kelvin);
                bool convertUnits(std::string units, PropertyBundle &outBundle);

                ResultCode onGet(RequestId requestId, const ResourceQuery &query);
                ResultCode onSet(RequestId requestId, const PropertyBundle &bundle, const ResourceQuery &query);

            private:
                TemperatureResourceDelegate *m_userDelegate;
            };
        }
    }
}
#endif /* SMARTHOME_API_SERVER_TEMPERATURE_H_ */
