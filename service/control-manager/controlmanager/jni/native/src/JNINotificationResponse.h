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




#ifndef JNINOTIFICATIONRESPONSE_H_
#define JNINOTIFICATIONRESPONSE_H_

#include "Notification/NotificationResponse.h"
#include <jni.h>

class JNINotificationResponse: public OC::Cm::Notification::NotificationResponse
{
    public:

        JNINotificationResponse(jobject jnotificationresponse,  std::string resourcePath ,
                                std::string notificationType , std::string uuid,
                                OC::Cm::Serialization::ISerializable *notificationData );

        ~JNINotificationResponse() ;

        jobject m_notificationResponse ;

};



#endif /* JNINOTIFICATIONRESPONSE_H_ */
