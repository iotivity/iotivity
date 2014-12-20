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



#ifndef CMCoreSerializableDataFactory_H_
#define CMCoreSerializableDataFactory_H_

#include "Serialization/SerializableDataFactory.h"

using namespace std ;

class CMCoreSerializableDataFactory : public OC::Cm::Serialization::SerializableDataFactory
{

    public:

        CMCoreSerializableDataFactory();

        virtual ~CMCoreSerializableDataFactory();

        virtual bool createSerializable( std::string &elementType,
                                         OC::Cm::Serialization::ISerializable **pp_output ) ;

        virtual bool createMethodSerializables( int resourceType, std::string &methodId,
                                                OC::Cm::Serialization::ISerializable **pp_request,
                                                OC::Cm::Serialization::ISerializable **pp_response ) ;

        OC::Cm::Serialization::SerializableDataFactory  *pApplicationFactory ;

        OC::Cm::Serialization::SerializableDataFactory  *pSubscriptionFactory ;
};

#endif /* Control ManagerSERIALIZABLEDATAFACTORY_H_ */
