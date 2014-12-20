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




#include "CMDeviceFactory.h"
#include "OC_Cm_Device.h"
#include "JNICMDev.h"
#include "JNIGlobal.h"

using namespace std ;

CMDeviceFactory::CMDeviceFactory(jobject obj)
{
    this->m_deviceFactoryObj = JNIGlobal::getGlobleRef(obj) ;
}

CMDeviceFactory::~CMDeviceFactory()
{
    JNIGlobal::executeVoidMethod("OC/Cm/DeviceFinder/DeviceFactory", "onNativeDelete", "()V",
                                 m_deviceFactoryObj);
    JNIGlobal::releaseGlobleRef(m_deviceFactoryObj) ;
}

OC::Cm::Device *
CMDeviceFactory::createDevice(OC::Cm::DeviceType type, OC::Cm::DeviceDomain domain)
{
    jobject jdomain = JNIGlobal::getEnum("OC/Cm/DeviceDomain", "getDeviceDomain",
                                         "(I)LOC/Cm/DeviceDomain;" , (int)domain);
    jobject jdeviceType = JNIGlobal::getEnum("OC/Cm/DeviceType", "getDeviceType",
                          "(I)LOC/Cm/DeviceType;" , (int)type);

    jlong device = JNIGlobal::executeLongMethod("OC/Cm/DeviceFinder/DeviceFactory", "createDevice",
                   "(LOC/Cm/DeviceType;LOC/Cm/DeviceDomain;)J", m_deviceFactoryObj , jdeviceType, jdomain);

    JNIGlobal::releaseLocalRef(jdomain);
    JNIGlobal::releaseLocalRef(jdeviceType);

    return (OC::Cm::Device *)device ;
}
