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
#include <TemperatureResource.h>
#include <PropertyBundle.h>
#include <ResourceQuery.h>
#include <CommonException.h>
#include <CommonApi.h>
#include <iostream>

const static std::string TEMPERATURE_URI = "/temperature";
const static std::string TEMPERATURE_PROP_TEMPERATURE = "temperature";
const static std::string TEMPERATURE_PROP_UNITS = "units";
const static std::string TEMPERATURE_PROP_RANGE = "range";

const static std::string TEMPERATURE_CELSIUS = "C";
const static std::string TEMPERATURE_FAHRENHEIT = "F";
const static std::string TEMPERATURE_KELVIN = "K";

const static double TEMPERATURE_DEFAULT_TEMP = (0.0);
const static std::string TEMPERATURE_DEFAULT_UNITS = TEMPERATURE_CELSIUS;
const static double TEMPERATURE_DEFAULT_MIN_RANGE = (0.0);
const static double TEMPERATURE_DEFAULT_MAX_RANGE = (100.0);



namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            TemperatureResource::TemperatureResource() :
                    m_userDelegate(NULL), SHBaseResource(TEMPERATURE_URI,
                            RESOURCE_TYPE::TEMPERATURE,
                            RESOURCE_PROPERTY::DISCOVERABLE | RESOURCE_PROPERTY::OBSERVABLE)
            {
                double temperature = TEMPERATURE_DEFAULT_TEMP;
                std::string units = TEMPERATURE_DEFAULT_UNITS;
                std::list<double> range;
                range.push_back(TEMPERATURE_DEFAULT_MIN_RANGE);
                range.push_back(TEMPERATURE_DEFAULT_MAX_RANGE);

                PropertyBundle bundle;
                bundle.setValue(TEMPERATURE_PROP_TEMPERATURE, temperature);
                bundle.setValue(TEMPERATURE_PROP_UNITS, units);
                bundle.setValue(TEMPERATURE_PROP_RANGE, range);
                setPropertyBundle(bundle);
            }

            TemperatureResource::~TemperatureResource()
            {
            }

            double TemperatureResource::getMinRange()
            {
                std::cout << "[TemperaturResource] getMinRange" << std::endl;

                PropertyBundle bundle = getPropertyBundle();

                std::list<double> range;
                bundle.getValue(TEMPERATURE_PROP_RANGE, range);

                return *(range.begin());
            }

            void TemperatureResource::setMinRange(double temperature)
            {
                std::cout << "[TemperaturResource] setMinRange" << std::endl;

                double maxRange = getMaxRange();
                if (maxRange < temperature)
                {
                    throw CommonException("the temperature is higher than MAX range");
                    return;
                }

                std::list<double> newRange;
                newRange.push_back(temperature);
                newRange.push_back(maxRange);

                PropertyBundle bundle = getPropertyBundle();
                bundle.setValue(TEMPERATURE_PROP_RANGE, newRange);
                setPropertyBundle(bundle);
            }

            double TemperatureResource::getMaxRange()
            {
                std::cout << "[TemperaturResource] getMaxRange" << std::endl;

                PropertyBundle bundle = getPropertyBundle();

                std::list<double> range;
                bundle.getValue(TEMPERATURE_PROP_RANGE, range);

                return *(range.rbegin());
            }

            void TemperatureResource::setMaxRange(double temperature)
            {
                std::cout << "[TemperaturResource] setMaxRange" << std::endl;

                double minRange = getMinRange();
                if (minRange > temperature)
                {
                    throw CommonException("the temperature is lower than MIN range");
                    return;
                }

                std::list<double> newRange;
                newRange.push_back(minRange);
                newRange.push_back(temperature);

                PropertyBundle bundle = getPropertyBundle();
                bundle.setValue(TEMPERATURE_PROP_RANGE, newRange);
                setPropertyBundle(bundle);
            }

            double TemperatureResource::getTemperature()
            {
                std::cout << "[TemperaturResource] getTemperature" << std::endl;

                PropertyBundle bundle = getPropertyBundle();

                double temperature;
                bundle.getValue(TEMPERATURE_PROP_TEMPERATURE, temperature);

                return temperature;
            }

            void TemperatureResource::setTemperature(double temperature)
            {
                std::cout << "[TemperaturResource] setTemperature" << std::endl;

                if (getMinRange() > temperature || getMaxRange() < temperature)
                {
                    throw CommonException("temperature value is not proper.");
                    return;
                }

                PropertyBundle bundle = getPropertyBundle();
                bundle.setValue(TEMPERATURE_PROP_TEMPERATURE, temperature);
                setPropertyBundle(bundle);
            }

            std::string TemperatureResource::getTemperatureUnits()
            {
                std::cout << "[TemperaturResource] getTemperatureUnits" << std::endl;

                PropertyBundle bundle = getPropertyBundle();

                std::string units;
                bundle.getValue(TEMPERATURE_PROP_UNITS, units);

                return units;
            }

            double TemperatureResource::convertCelsiusToFahrenheit(double celsius)
            {
                return (celsius*1.8)+32.0;
            }

            double TemperatureResource::convertCelsiusToKelvin(double celsius)
            {
                return celsius + 273.15;
            }

            double TemperatureResource::convertFahrenheitToCelsius(double fahrenheit)
            {
                return (fahrenheit - 32.0)/1.8;
            }

            double TemperatureResource::convertFahrenheitToKelvin(double fahrenheit)
            {
                return convertCelsiusToKelvin(convertFahrenheitToCelsius(fahrenheit));
            }

            double TemperatureResource::convertKelvinToCelsius(double kelvin)
            {
                return kelvin - 273.15;
            }

            double TemperatureResource::convertKelvinToFahrenheit(double kelvin)
            {
                return convertCelsiusToFahrenheit(convertKelvinToCelsius(kelvin));
            }

            bool TemperatureResource::convertUnits(std::string units, PropertyBundle &outBundle)
            {
                PropertyBundle prevBundle = getPropertyBundle();

                std::string prevUnits = TEMPERATURE_DEFAULT_UNITS;
                double temperature = TEMPERATURE_DEFAULT_TEMP;
                std::list<double> range;

                prevBundle.getValue(TEMPERATURE_PROP_UNITS, prevUnits);
                prevBundle.getValue(TEMPERATURE_PROP_TEMPERATURE, temperature);
                prevBundle.getValue(TEMPERATURE_PROP_RANGE, range);

                double minRange = *(range.begin());
                double maxRange = *(range.rbegin());

                if (prevUnits == units)
                {
                    outBundle = prevBundle;
                    return true;
                }
                else if (prevUnits == TEMPERATURE_CELSIUS)
                {
                    if (units == TEMPERATURE_FAHRENHEIT)
                    {
                        temperature = convertCelsiusToFahrenheit(temperature);
                        minRange = convertCelsiusToFahrenheit(minRange);
                        maxRange = convertCelsiusToFahrenheit(maxRange);
                    }
                    else if (units == TEMPERATURE_KELVIN)
                    {
                        temperature = convertCelsiusToKelvin(temperature);
                        minRange = convertCelsiusToKelvin(minRange);
                        maxRange = convertCelsiusToKelvin(maxRange);
                    }
                    else
                    {
                        std::cout << "[TemperaturResource] unit[" << units <<
                            "] value is not supported value" << std::endl;
                        return false;
                    }
                }
                else if (prevUnits == TEMPERATURE_FAHRENHEIT)
                {
                    if (units == TEMPERATURE_CELSIUS)
                    {
                        temperature = convertFahrenheitToCelsius(temperature);
                        minRange = convertFahrenheitToCelsius(minRange);
                        maxRange = convertFahrenheitToCelsius(maxRange);
                    }
                    else if (units == TEMPERATURE_KELVIN)
                    {
                        temperature = convertFahrenheitToKelvin(temperature);
                        minRange = convertFahrenheitToKelvin(minRange);
                        maxRange = convertFahrenheitToKelvin(maxRange);
                    }
                    else
                    {
                        std::cout << "[TemperaturResource] unit[" << units <<
                            "] value is not supported value" << std::endl;
                        return false;
                    }
                }
                else if (prevUnits == TEMPERATURE_KELVIN)
                {
                    if (units == TEMPERATURE_CELSIUS)
                    {
                        temperature = convertKelvinToCelsius(temperature);
                        minRange = convertKelvinToCelsius(minRange);
                        maxRange = convertKelvinToCelsius(maxRange);
                    }
                    else if (units == TEMPERATURE_FAHRENHEIT)
                    {
                        temperature = convertKelvinToFahrenheit(temperature);
                        minRange = convertKelvinToFahrenheit(minRange);
                        maxRange = convertKelvinToFahrenheit(maxRange);
                    }
                    else
                    {
                        std::cout << "[TemperaturResource] unit[" << units <<
                            "] value is not supported value" << std::endl;
                        return false;
                    }
                }
                else
                {
                    std::cout << "[TemperaturResource] unit[" << prevUnits <<
                        "] value is not supported value" << std::endl;
                    return false;
                }

                std::list<double> newRange;
                newRange.push_back(minRange);
                newRange.push_back(maxRange);

                outBundle.setValue(TEMPERATURE_PROP_TEMPERATURE, temperature);
                outBundle.setValue(TEMPERATURE_PROP_UNITS, units);
                outBundle.setValue(TEMPERATURE_PROP_RANGE, newRange);

                return true;
            }

            void TemperatureResource::setTemperatureUnits(std::string units)
            {
                std::cout << "[TemperaturResource] setTemperatureUnits" << std::endl;

                if (TEMPERATURE_CELSIUS != units &&
                        TEMPERATURE_FAHRENHEIT != units &&
                        TEMPERATURE_KELVIN != units)
                {
                    throw CommonException("units value is not proper");
                    return;
                }

                PropertyBundle outBundle;
                bool ret = convertUnits(units, outBundle);
                if (true == ret)
                {
                    setPropertyBundle(outBundle);
                }
                else
                {
                    throw CommonException("units value set failed.");
                }
            }

            void TemperatureResource::setTemperatureResourceDelegate(
                    TemperatureResourceDelegate *temperatureDelegate)
            {
                std::cout << "[TemperaturResource] setTemperatureResourceDelegate" << std::endl;
                m_userDelegate = temperatureDelegate;
                setDelegate(this);
            }

            ResultCode TemperatureResource::onGet(RequestId requestId, const ResourceQuery &query)
            {
                std::cout << "[TemperaturResource] onGet" << std::endl;

                //query param: units = one of [C, F, K]
                //not supported units: 403 response
                //query param omitted : default units(C) or set value.

                std::string units = TEMPERATURE_DEFAULT_UNITS;
                if (query.hasContain(TEMPERATURE_PROP_UNITS))
                {
                    units = query.getQueryValue(TEMPERATURE_PROP_UNITS);
                }

                std::string curUnits = getTemperatureUnits();
                if (curUnits != units)
                {
                    return FAIL;
                }

                return SUCCESS;
            }

            ResultCode TemperatureResource::onSet(RequestId requestId,
                                                  const PropertyBundle &bundle,
                                                  const ResourceQuery &query)
            {
                std::cout << "[TemperaturResource] onSet" << std::endl;

                if (!this->m_userDelegate)
                {
                    std::cout << "[TemperaturResource] delegate is null" << std::endl;
                    return FAIL;
                }

                double temperature = TEMPERATURE_DEFAULT_TEMP;
                std::string units = TEMPERATURE_DEFAULT_UNITS;
                ResultCode retCode = FAIL;
                bool hasUnits = false;
                bundle.getValue(TEMPERATURE_PROP_TEMPERATURE, temperature);

                if (getMinRange() > temperature || getMaxRange() < temperature)
                {
                    std::cout << "[TemperaturResource] temperature value is not supported value!"
                        << std::endl;
                    return FAIL;
                }
                if (bundle.contains(TEMPERATURE_PROP_UNITS))
                {
                    bundle.getValue(TEMPERATURE_PROP_UNITS, units);
                    std::string curUnits = getTemperatureUnits();
                    if (curUnits != units)
                    {
                        std::cout <<
                            "[TemperaturResource] temperature units is not supported value!"
                            << std::endl;
                        return FAIL;
                    }
                    hasUnits = true;
                }
                else
                {
                    units = TEMPERATURE_DEFAULT_UNITS;
                }

                retCode = this->m_userDelegate->setTemperatureCallback(temperature, units);

                if (SUCCESS == retCode)
                {
                    PropertyBundle responseBundle;
                    responseBundle.setValue(TEMPERATURE_PROP_TEMPERATURE, temperature);
                    if (hasUnits)
                    {
                        responseBundle.setValue(TEMPERATURE_PROP_UNITS, units);
                    }
                    sendResponse(requestId, bundle);
                    return KEEP;
                }

                return retCode;
            }
        }
    }
}
