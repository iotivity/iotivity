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




#ifndef JNICMDEV_H_
#define JNICMDEV_H_

#include "Device.h"
#include <jni.h>
#include "Log.h"


class JNICMDevice: public OC::Cm::Device

{
    public:

        JNICMDevice( jobject jdevice , jint devType, jint devDomain );

        ~JNICMDevice() ;

        virtual bool loadDeviceDescription(OC::Cm::Serialization::ISerializable &serializable);

        virtual bool getDeviceDescription(OC::Cm::Serialization::ISerializable **serializable);

        virtual std::string getSubscriptionsLink();

        jobject m_device ;
};




#endif /* JNICMDEV_H_ */
