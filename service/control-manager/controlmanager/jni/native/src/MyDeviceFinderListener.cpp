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




#include "MyDeviceFinderListener.h"
#include "CMDeviceFinder.h"
#include "JNICMDev.h"
//#include "Log.h"

using namespace std ;

MyDeviceFinderListener::MyDeviceFinderListener(jobject mylistener)
{

    listener = JNIGlobal::getGlobleRef(mylistener) ;
}

MyDeviceFinderListener::~MyDeviceFinderListener()
{
    JNIGlobal::releaseGlobleRef(listener) ;
}

void
MyDeviceFinderListener::OnDeviceAdded( OC::Cm::Device &device)
{
    JNICMDevice *jniDevice = (JNICMDevice *) (&device) ;
    jobject javaDeviceObject = jniDevice->m_device ;
    JNIGlobal::executeVoidMethod("OC/Cm/DeviceFinder/IDeviceFinderListener", "OnDeviceAdded",
                                 "(LOC/Cm/Device;)V", listener, javaDeviceObject);
}

void
MyDeviceFinderListener::OnDeviceRemoved( OC::Cm::Device &device)
{
    JNICMDevice *jniDevice = (JNICMDevice *) (&device) ;
    jobject javaDeviceObject = jniDevice->m_device ;
    JNIGlobal::executeVoidMethod("OC/Cm/DeviceFinder/IDeviceFinderListener", "OnDeviceRemoved",
                                 "(LOC/Cm/Device;)V", listener, javaDeviceObject);
}

void
MyDeviceFinderListener::OnDeviceUpdated( OC::Cm::Device &device)
{
    JNICMDevice *jniDevice = (JNICMDevice *) (&device) ;
    jobject javaDeviceObject = jniDevice->m_device ;
    JNIGlobal::executeVoidMethod("OC/Cm/DeviceFinder/IDeviceFinderListener", "OnDeviceUpdated",
                                 "(LOC/Cm/Device;)V", listener, javaDeviceObject);
}

void
MyDeviceFinderListener::OnDeviceError( OC::Cm::Device &device )
{
    JNICMDevice *jniDevice = (JNICMDevice *) (&device) ;
    jobject javaDeviceObject = jniDevice->m_device ;
    JNIGlobal::executeVoidMethod("OC/Cm/DeviceFinder/IDeviceFinderListener", "OnDeviceError",
                                 "(LOC/Cm/Device;)V", listener, javaDeviceObject);
}
