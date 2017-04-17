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
#include <RemoteTemperatureResource.h>
#include <PropertyBundle.h>
#include <ResourceQuery.h>
#include <logger.h>

#define TAG "OIC_SH_CLIENT_REMOTETEMPERATURE"

const static std::string TEMPERATURE_KEY = "temperature";
const static std::string TEMPERATURE_UNITS_KEY = "units";
const static std::string TEMPERATURE_RANGE_KEY = "range";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            RemoteTemperatureResource::RemoteTemperatureResource() :
                m_delegate(NULL)
            {
            }

            RemoteTemperatureResource::~RemoteTemperatureResource()
            {
            }

            void RemoteTemperatureResource::setRemoteTemperatureResourceDelegate(
                    RemoteTemperatureResourceDelegate *delegate)
            {
                setDelegate(this);

                this->m_delegate = delegate;
            }

            void RemoteTemperatureResource::onGet(PropertyBundle bundle, ResultCode ret)
            {
                double temperature;
                std::string units;
                std::list<double> range;

                if (NULL == m_delegate)
                {
                    std::cout << "delegate is not set yet" << std::endl;
                    return;
                }

                if (bundle.contains(TEMPERATURE_KEY) &&
                        bundle.contains(TEMPERATURE_UNITS_KEY) &&
                        bundle.contains(TEMPERATURE_RANGE_KEY))
                {
                    bundle.getValue(TEMPERATURE_KEY, temperature);
                    bundle.getValue(TEMPERATURE_UNITS_KEY, units);
                    bundle.getValue(TEMPERATURE_RANGE_KEY, range);
                    m_delegate->onGetTemperature(temperature, units, range, ret);
                }
            }

            void RemoteTemperatureResource::onSet(PropertyBundle bundle, ResultCode ret)
            {
                double temperature;

                if (NULL == m_delegate)
                {
                    std::cout << "delegate is not set yet" << std::endl;
                    return;
                }

                if (bundle.contains(TEMPERATURE_KEY))
                {
                    std::string units;
                    std::list<double> range;

                    bundle.getValue(TEMPERATURE_KEY, temperature);
                    if (SUCCESS == ret)
                    {
                        if (!bundle.contains(TEMPERATURE_UNITS_KEY))
                        {
                            units = "C";
                        }
                        else
                        {
                            bundle.getValue(TEMPERATURE_UNITS_KEY, units);
                        }
                        m_delegate->onSetTemperature(temperature, units, range, ret);
                    }
                    else
                    {
                        bundle.getValue(TEMPERATURE_UNITS_KEY, units);
                        bundle.getValue(TEMPERATURE_RANGE_KEY, range);
                        m_delegate->onSetTemperature(temperature, units, range, ret);
                    }
                }
            }

            void RemoteTemperatureResource::onObserve(PropertyBundle bundle,
                        const ObserveResponse obsType,
                        ResultCode ret)
            {
                OIC_LOG(DEBUG, TAG, "Entered onObserve");
                return;
            }


            void RemoteTemperatureResource::getTemperature()
            {
                std::cout << "[RemoteTemperatureResource] getTemperature Requested.." << std::endl;
                getPropertyBundle();
            }

            void RemoteTemperatureResource::getTemperatureWithUnits(std::string units)
            {
                std::cout << "[RemoteTemperatureResource] getTemperatureWithUnits Requested.."
                    << std::endl;

                ResourceQuery query;
                query.setQuery(TEMPERATURE_UNITS_KEY, units);
                getPropertyBundle(query);
            }

            void RemoteTemperatureResource::setTemperature(double temperature)
            {
                std::cout << "[RemoteTemperatureResource] setTemperature Requested.." << std::endl;

                PropertyBundle bundle;

                bundle.setValue(TEMPERATURE_KEY, temperature);
                setPropertyBundle(bundle);
            }

            void RemoteTemperatureResource::setTemperatureWithUnits(double temperature,
                                                                    std::string units)
            {
                std::cout << "[RemoteTemperatureResource] setTemperatureWithUnits Requested.."
                    << std::endl;

                PropertyBundle bundle;

                bundle.setValue(TEMPERATURE_KEY, temperature);
                bundle.setValue(TEMPERATURE_UNITS_KEY, units);
                setPropertyBundle(bundle);
            }
        }
    }
}
