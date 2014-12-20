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



#include "AccessPointsLink.h"


#include "Elements.h"

using namespace std;

AccessPointsLink::AccessPointsLink()
{


}

AccessPointsLink::~AccessPointsLink()
{

}

bool
AccessPointsLink::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Link::serializeContents(serializer)) { return false;}



    return true ;
}

bool
AccessPointsLink::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Link::deSerializeContents(deserializer)) {return false;}



    return true ;
}

std::string
AccessPointsLink::getElementName()
{
    return EN_ACCESSPOINTSLINK;
}

bool
AccessPointsLink::clone(::AccessPointsLink *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Link::clone(pSrcObject)) {return false;}



    return true ;
}
