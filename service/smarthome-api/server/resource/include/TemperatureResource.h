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
#ifndef SMARTHOME_API_SERVER_TEMPERATURE_H_
#define SMARTHOME_API_SERVER_TEMPERATURE_H_

#include <SHBaseResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            class TemperatureResourceDelegate
            {
            public:
                virtual ResultCode setTemperatureCallback(double temperature,
                                                          std::string units) = 0;
            };

            class TemperatureResource : public SHBaseResource, public SHBaseResourceDelegate
            {
            public:
                TemperatureResource();
                virtual ~TemperatureResource();

                std::string getTemperatureUnits();
                void setTemperatureUnits(std::string units);

                double getTemperature();
                void setTemperature(double temperature);

                double getMinRange();
                void setMinRange(double temperature);

                double getMaxRange();
                void setMaxRange(double temperature);

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
