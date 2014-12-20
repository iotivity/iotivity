//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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




#ifndef CMDEVICEFACTORY_H_
#define CMDEVICEFACTORY_H_

#include <jni.h>
#include "CMConfig.h"
#include "DeviceType.h"
#include "DeviceDomain.h"
#include "Device.h"
#include "CMDeviceFinder.h"
#include "DeviceFinder/DeviceFactory.h"

using namespace std ;

class CMDeviceFactory : public OC::Cm::DeviceFinder::DeviceFactory
{
    public:

        //CMDeviceFactory();

        CMDeviceFactory(jobject obj);

        virtual ~CMDeviceFactory();

        virtual  OC::Cm::Device *createDevice(OC::Cm::DeviceType type, OC::Cm::DeviceDomain domain);

        jobject m_deviceFactoryObj;
};

#endif /* CMDEVICEFACTORY_H_ */
