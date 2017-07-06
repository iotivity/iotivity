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
 * This file contains the declaration of classes and its members related to
 * RemoteTemperatureResource.
 */

#ifndef SMARTHOME_API_CLIENT_REMOTETEMPERATURE_H_
#define SMARTHOME_API_CLIENT_REMOTETEMPERATURE_H_

#include <list>
#include <SHBaseRemoteResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions for callback of
             * RemoteTemperatureResource class.
             */
            class RemoteTemperatureResourceDelegate
            {
            public:
                /**
                 * API to handle a response about Temperature Get request.
                 *
                 * @param[in] temperature temperature
                 * @param[in] unit unit of temperature
                 * @param[in] range range values
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onGetTemperature(double temperature,
                                              std::string units,
                                              std::list<double> range,
                                              ResultCode ret) = 0;

                /**
                 * API to handle a response about Temperature Set request.
                 *
                 * @param[in] temperature temperature
                 * @param[in] unit unit of temperature
                 * @param[in] range range value
                 * @param[in] ret ResultCode about response.(SUCCESS OR FAIL)
                 */
                virtual void onSetTemperature(double temperature,
                                              std::string units,
                                              std::list<double> range,
                                              ResultCode ret) = 0;
            };

            /**
             * This class contains a set of functions to control RemoteTemperatureResource.
             */
            class RemoteTemperatureResource: public SHBaseRemoteResource,
                    public SHBaseRemoteResourceDelegate
            {
            public:
                virtual ~RemoteTemperatureResource();

                /**
                 * API to set remote delegate.
                 *
                 * @param[in] delegate instance of delegate class
                 */
                void setRemoteTemperatureResourceDelegate(
                        RemoteTemperatureResourceDelegate *delegate);

                /**
                 * API to get temperature.
                 */
                void getTemperature();

                /**
                 * API to get temperature with unit.
                 *
                 * @param[in] unit unit of temperature
                 */
                void getTemperatureWithUnits(std::string units);

                /**
                 * API to set temperature.
                 *
                 * @param[in] temperature temperature
                 */
                void setTemperature(double temperature);

                /**
                 * API to set temperature with unit.
                 *
                 * @param[in] temperature temperature
                 * @param[in] units unit of temperature
                 */
                void setTemperatureWithUnits(double temperature, std::string units);

            protected:
                RemoteTemperatureResource();

            private:
                virtual void onGet(PropertyBundle bundle, ResultCode ret);
                virtual void onSet(PropertyBundle bundle, ResultCode ret);
                virtual void onObserve(PropertyBundle bundle,
                        const ObserveResponse obsType,
                        ResultCode ret);

                RemoteTemperatureResourceDelegate *m_delegate;
            };
        }
    }
}
#endif /* SMARTHOME_API_CLIENT_REMOTETEMPERATURE_H_ */
