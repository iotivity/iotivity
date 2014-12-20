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



#include "Resource.h"


#include "Elements.h"

using namespace std;

Resource::Resource()
{


}

Resource::~Resource()
{

}

bool
Resource::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{


    return true ;
}

bool
Resource::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{


    return true ;
}

std::string
Resource::getElementName()
{
    return EN_RESOURCE;
}

bool
Resource::clone(::Resource *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }



    return true ;
}
