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

#include "Humidity.h"


#include "Elements.h"

using namespace std;

Humidity::Humidity()
{

    mpCurrent = NULL;
    mpDesired = NULL;
    mpUnit = NULL;

}

Humidity::~Humidity()
{
    if ( NULL != mpCurrent) { delete mpCurrent; mpCurrent = NULL; }
    if ( NULL != mpDesired) { delete mpDesired; mpDesired = NULL; }
    if ( NULL != mpUnit) { delete mpUnit; mpUnit = NULL; }

}

bool
Humidity::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpCurrent )
        {
            if (true == mpCurrent->validateContent())
            {
                bRet = serializer.setPropertyValue("current", mpCurrent->value);
            }
            else { bRet = false ;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("current") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : current"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpDesired )
        {
            if (true == mpDesired->validateContent())
            {
                bRet = serializer.setPropertyValue("desired", mpDesired->value);
            }
            else { bRet = false ;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("desired") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : desired"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpUnit )
        {
            if (mpUnit->value.length() > 0)
            {
                if (true == mpUnit->validateContent())
                {
                    bRet = serializer.setPropertyValue("unit", mpUnit->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("unit") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : unit"); return false; }}
    }


    return true ;
}

bool
Humidity::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("current"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  current"); return false;}
        mpCurrent = new ::Float32();
        if (mpCurrent == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  current"); return false;}
        if (false == deserializer.getPropertyValue("current", &(mpCurrent->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  current"); return false ;}
        if (false == mpCurrent->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : current"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("current") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : current"); return false; }}
    if (true == deserializer.hasProperty("desired"))
    {
        mpDesired = new ::Float32();
        if (mpDesired == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  desired"); return false;}
        if (false == deserializer.getPropertyValue("desired", &(mpDesired->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  desired"); return false ;}
        if (false == mpDesired->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : desired"); return false ;}
    }
    if (true == deserializer.hasProperty("unit"))
    {
        mpUnit = new ::HumidityUnitType();
        if (mpUnit == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  unit"); return false;}
        if (false == deserializer.getPropertyValue("unit", &(mpUnit->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  unit"); return false ;}
        if (false == mpUnit->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : unit"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("unit") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : unit"); return false; }}


    return true ;
}

std::string
Humidity::getElementName()
{
    return EN_HUMIDITY;
}

bool
Humidity::clone(::Humidity *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpCurrent) { delete mpCurrent; mpCurrent = NULL; }

        if ( NULL != pSrcObject->mpCurrent )
        {
            mpCurrent = new ::Float32();
            if (mpCurrent == NULL ) {return false;}
            mpCurrent->value = (pSrcObject->mpCurrent->value );
        }
    }
    {
        if ( NULL != mpDesired) { delete mpDesired; mpDesired = NULL; }

        if ( NULL != pSrcObject->mpDesired )
        {
            mpDesired = new ::Float32();
            if (mpDesired == NULL ) {return false;}
            mpDesired->value = (pSrcObject->mpDesired->value );
        }
    }
    {
        if ( NULL != mpUnit) { delete mpUnit; mpUnit = NULL; }

        if ( NULL != pSrcObject->mpUnit )
        {
            mpUnit = new ::HumidityUnitType();
            if (mpUnit == NULL ) {return false;}
            mpUnit->value = (pSrcObject->mpUnit->value );
        }
    }


    return true ;
}
