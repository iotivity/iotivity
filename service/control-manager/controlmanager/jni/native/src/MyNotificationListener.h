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


/*
 * MyNotificationListener.h
 *
 *  Created on: Jul 4, 2013
 *      Author: kirtigupta
 */

#ifndef MYNOTIFICATIONLISTENER_H_
#define MYNOTIFICATIONLISTENER_H_

#include "Notification/INotificationListener.h"
#include "JNIGlobal.h"
#include <jni.h>

using namespace std ;

class MyNotificationListener : public OC::Cm::Notification::INotificationListener
{
    public:
        MyNotificationListener(jobject myListener);

        virtual ~MyNotificationListener();

        void onNotificationReceived( std::string &uuid, std::string &resource,
                                     std::string &elementType, OC::Cm::Serialization::ISerializable *notification,
                                     std::string &subscriptionURI,  OC::Cm::Serialization::Xsd::DateTimeType *eventTime) ;

        void onMulticastedNotifcationReceived( const OC::Cm::Device &device, const std::string &elementType,
                                               const OC::Cm::Serialization::ISerializable *notification) ;

    private :
        jobject listener;
};


#endif /* MYNOTIFICATIONLISTENER_H_ */
