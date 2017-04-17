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
            class RemoteTemperatureResourceDelegate
            {
            public:
                virtual void onGetTemperature(double temperature,
                                              std::string units,
                                              std::list<double> range,
                                              ResultCode ret) = 0;
                virtual void onSetTemperature(double temperature,
                                              std::string units,
                                              std::list<double> range,
                                              ResultCode ret) = 0;
            };

            class RemoteTemperatureResource: public SHBaseRemoteResource,
                    public SHBaseRemoteResourceDelegate
            {
            public:
                virtual ~RemoteTemperatureResource();
                void setRemoteTemperatureResourceDelegate(
                        RemoteTemperatureResourceDelegate *delegate);
                void getTemperature();
                void getTemperatureWithUnits(std::string units);
                void setTemperature(double temperature);
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
