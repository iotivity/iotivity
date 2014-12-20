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
 * MyResourceResponseListener.h
 *
 *  Created on: Jun 11, 2013
 *      Author: kirtigupta
 */

#ifndef MYRESOURCERESPONSELISTENER_H_
#define MYRESOURCERESPONSELISTENER_H_

#include "CMClientResource.h"
//#include "Log.h"
#include "JNIGlobal.h"
#include <jni.h>

using namespace std ;

class MyResourceResponseListener : public OC::Cm::Client::Resource::IResourceResponseListener
{

    public:

        MyResourceResponseListener(jobject myListener);

        virtual ~MyResourceResponseListener();

        virtual void OnGetResponseReceived(int clientSessionId, int status,
                                           OC::Cm::Connector::HeadersList &headers,  OC::Cm::Serialization::ISerializable *response) ;

        virtual void OnPutResponseReceived(int clientSessionId, int status,
                                           OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response) ;

        virtual void OnPostResponseReceived(int clientSessionId, int status,
                                            OC::Cm::Connector::HeadersList &headers,  OC::Cm::Serialization::ISerializable *response) ;

        virtual void onDeleteResponseReceived(int clientSessionId, int status,
                                              OC::Cm::Connector::HeadersList &headers, OC::Cm::Serialization::ISerializable *response);

        virtual void onSubscribeResponseReceived( int clientSessionId, int status,
                std::string &subscriptionUri ) ;

        virtual void onUnSubscribeResponseReceived(int clientSessionId, int status );

        virtual void onErrorReceived( int clientSessionId, int status, OC::Cm::CMError error,
                                      OC::Cm::Serialization::ISerializable *errorMessage) ;

        virtual void onRequestComplete(int requestId, std::string requestPayload,
                                       std::string responsePayload);

        void setRequestCount(int reqCount);

        bool forceDeleteOnError ;

    private:

        jobject listener;

        int     m_reqCount;

};

#endif /* MYRESOURCERESPONSELISTENER_H_ */
