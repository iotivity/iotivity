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
 * CMResourceHandler.h
 *
 *  Created on: Jul 8, 2013
 *      Author: kirtigupta
 */

#ifndef CMRESOURCEHANDLER_H_
#define CMRESOURCEHANDLER_H_

#include "string"
#include <jni.h>

#include "CMServerResourceHandler.h"

using namespace std ;

class CMResourceHandler : public OC::Cm::Server::ResourceHandler::SyncResourceHandler
{
    public:
        CMResourceHandler(jlong context, jobject handler);

        virtual ~CMResourceHandler();

//  virtual bool onInit() ;
//
//  virtual bool onDeinit() ;
//
//  virtual bool handleResource( OC::Cm::Connector::Server::ServerSession& session,std::string methodId,Sec::Shp::Serialization::ISerializable* request,
//                              OC::Cm::Serialization::ISerializable* response) ;


        virtual bool handleSyncResource( OC::Cm::Connector::Server::ServerSession &session,
                                         std::string methodId,
                                         int &statusCode,
                                         OC::Cm::Serialization::ISerializable  *request,
                                         OC::Cm::Serialization::ISerializable  *response) ;


        jobject res_handler ;
};

#endif /* CMRESOURCEHANDLER_H_ */
