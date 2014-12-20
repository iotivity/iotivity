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

#include "Movement.h"


#include "Elements.h"

using namespace std;

Movement::Movement()
{

    mpBehavior = NULL;

}

Movement::~Movement()
{
    if ( NULL != mpBehavior) { delete mpBehavior; mpBehavior = NULL; }

}

bool
Movement::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    {
        bool bRet = false;

        if ( NULL != mpBehavior )
        {
            if (mpBehavior->value.length() > 0)
            {
                if (true == mpBehavior->validateContent())
                {
                    bRet = serializer.setPropertyValue("behavior", mpBehavior->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("behavior") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : behavior"); return false; }}
    }


    return true ;
}

bool
Movement::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("behavior"))
    {
        mpBehavior = new ::MovementType();
        if (mpBehavior == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  behavior"); return false;}
        if (false == deserializer.getPropertyValue("behavior", &(mpBehavior->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  behavior"); return false ;}
        if (false == mpBehavior->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : behavior"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("behavior") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : behavior"); return false; }}


    return true ;
}

std::string
Movement::getElementName()
{
    return EN_MOVEMENT;
}

bool
Movement::clone(::Movement *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpBehavior) { delete mpBehavior; mpBehavior = NULL; }

        if ( NULL != pSrcObject->mpBehavior )
        {
            mpBehavior = new ::MovementType();
            if (mpBehavior == NULL ) {return false;}
            mpBehavior->value = (pSrcObject->mpBehavior->value );
        }
    }


    return true ;
}
