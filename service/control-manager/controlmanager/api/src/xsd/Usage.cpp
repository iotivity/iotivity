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



#include "Usage.h"


#include "Elements.h"

using namespace std;

Usage::Usage()
{

    mpEnergy = NULL;
    mpUsageTime = NULL;
    mpStartTime = NULL;
    mpEndTime = NULL;

}

Usage::~Usage()
{
    if ( NULL != mpEnergy) { delete mpEnergy; mpEnergy = NULL; }
    if ( NULL != mpUsageTime) { delete mpUsageTime; mpUsageTime = NULL; }
    if ( NULL != mpStartTime) { delete mpStartTime; mpStartTime = NULL; }
    if ( NULL != mpEndTime) { delete mpEndTime; mpEndTime = NULL; }

}

bool
Usage::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpEnergy )
        {
            bRet = serializer.setPropertyValue("energy", *mpEnergy);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("energy") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : energy"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpUsageTime )
        {
            bRet = serializer.setPropertyValue("usageTime", *mpUsageTime);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("usageTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : usageTime"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpStartTime )
        {
            if (mpStartTime->value.length() > 0)
            {
                if (true == mpStartTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("startTime", mpStartTime->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("startTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : startTime"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpEndTime )
        {
            if (mpEndTime->value.length() > 0)
            {
                if (true == mpEndTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("endTime", mpEndTime->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("endTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : endTime"); return false; }}
    }


    return true ;
}

bool
Usage::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (true == deserializer.hasProperty("energy"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  energy"); return false;}
        mpEnergy = new uint16_t();
        if (mpEnergy == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  energy"); return false;}
        if (false == deserializer.getPropertyValue("energy", mpEnergy)) { deserializer.setErrorMessage("Failed to deserialize parameter :  energy"); return false ;}
    }
    if (true == deserializer.hasProperty("usageTime"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  usageTime"); return false;}
        mpUsageTime = new uint32_t();
        if (mpUsageTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  usageTime"); return false;}
        if (false == deserializer.getPropertyValue("usageTime", mpUsageTime)) { deserializer.setErrorMessage("Failed to deserialize parameter :  usageTime"); return false ;}
    }
    if (true == deserializer.hasProperty("startTime"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  startTime"); return false;}
        mpStartTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpStartTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  startTime"); return false;}
        if (false == deserializer.getPropertyValue("startTime",
                &(mpStartTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  startTime"); return false ;}
        if (false == mpStartTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : startTime"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("startTime") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : startTime"); return false; }}
    if (true == deserializer.hasProperty("endTime"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  endTime"); return false;}
        mpEndTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpEndTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  endTime"); return false;}
        if (false == deserializer.getPropertyValue("endTime",
                &(mpEndTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  endTime"); return false ;}
        if (false == mpEndTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : endTime"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("endTime") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : endTime"); return false; }}


    return true ;
}

std::string
Usage::getElementName()
{
    return EN_USAGE;
}

bool
Usage::clone(::Usage *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    {
        if ( NULL != mpEnergy) { delete mpEnergy; mpEnergy = NULL; }

        if ( NULL != pSrcObject->mpEnergy )
        {
            mpEnergy = new uint16_t();
            if (mpEnergy == NULL ) {return false;}
            *mpEnergy = *(pSrcObject->mpEnergy );
        }
    }
    {
        if ( NULL != mpUsageTime) { delete mpUsageTime; mpUsageTime = NULL; }

        if ( NULL != pSrcObject->mpUsageTime )
        {
            mpUsageTime = new uint32_t();
            if (mpUsageTime == NULL ) {return false;}
            *mpUsageTime = *(pSrcObject->mpUsageTime );
        }
    }
    {
        if ( NULL != mpStartTime) { delete mpStartTime; mpStartTime = NULL; }

        if ( NULL != pSrcObject->mpStartTime )
        {
            mpStartTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (mpStartTime == NULL ) {return false;}
            mpStartTime->value = (pSrcObject->mpStartTime->value );
        }
    }
    {
        if ( NULL != mpEndTime) { delete mpEndTime; mpEndTime = NULL; }

        if ( NULL != pSrcObject->mpEndTime )
        {
            mpEndTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (mpEndTime == NULL ) {return false;}
            mpEndTime->value = (pSrcObject->mpEndTime->value );
        }
    }


    return true ;
}
