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

#include "UsageLimit.h"


#include "Elements.h"

using namespace std;

UsageLimit::UsageLimit()
{

    mpThreshold = NULL;
    mpStartTime = NULL;
    mpEndTime = NULL;
    mpEfficiency = NULL;
    mpDeviceID = NULL;
    mpGroupID = NULL;

}

UsageLimit::~UsageLimit()
{
    if ( NULL != mpThreshold) { delete mpThreshold; mpThreshold = NULL; }
    if ( NULL != mpStartTime) { delete mpStartTime; mpStartTime = NULL; }
    if ( NULL != mpEndTime) { delete mpEndTime; mpEndTime = NULL; }
    if ( NULL != mpEfficiency) { delete mpEfficiency; mpEfficiency = NULL; }
    if ( NULL != mpDeviceID) { delete mpDeviceID; mpDeviceID = NULL; }
    if ( NULL != mpGroupID) { delete mpGroupID; mpGroupID = NULL; }

}

bool
UsageLimit::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::IdentifiedObject::serializeContents(serializer)) { return false;}

    {
        bool bRet = false;

        if ( NULL != mpThreshold )
        {
            bRet = serializer.setPropertyValue("threshold", *mpThreshold);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("threshold") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : threshold"); return false; }}
    }
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
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpEfficiency )
        {
            if (mpEfficiency->value.length() > 0)
            {
                if (true == mpEfficiency->validateContent())
                {
                    bRet = serializer.setPropertyValue("efficiency", mpEfficiency->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("efficiency") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : efficiency"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpDeviceID )
        {
            if (mpDeviceID->value.length() > 0)
            {
                if (true == mpDeviceID->validateContent())
                {
                    bRet = serializer.setPropertyValue("deviceID", mpDeviceID->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("deviceID") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : deviceID"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpGroupID )
        {
            if (mpGroupID->value.length() > 0)
            {
                if (true == mpGroupID->validateContent())
                {
                    bRet = serializer.setPropertyValue("groupID", mpGroupID->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("groupID") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : groupID"); return false; }}
    }


    return true ;
}

bool
UsageLimit::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::IdentifiedObject::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("threshold"))
    {
        mpThreshold = new uint16_t();
        if (mpThreshold == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  threshold"); return false;}
        if (false == deserializer.getPropertyValue("threshold", mpThreshold)) { deserializer.setErrorMessage("Failed to deserialize parameter :  threshold"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("threshold") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : threshold"); return false; }}
    if (true == deserializer.hasProperty("startTime"))
    {
        mpStartTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpStartTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  startTime"); return false;}
        if (false == deserializer.getPropertyValue("startTime", &(mpStartTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  startTime"); return false ;}
        if (false == mpStartTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : startTime"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("startTime") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : startTime"); return false; }}
    if (true == deserializer.hasProperty("endTime"))
    {
        mpEndTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpEndTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  endTime"); return false;}
        if (false == deserializer.getPropertyValue("endTime", &(mpEndTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  endTime"); return false ;}
        if (false == mpEndTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : endTime"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("endTime") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : endTime"); return false; }}
    if (true == deserializer.hasProperty("efficiency"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  efficiency"); return false;}
        mpEfficiency = new ::String200();
        if (mpEfficiency == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  efficiency"); return false;}
        if (false == deserializer.getPropertyValue("efficiency", &(mpEfficiency->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  efficiency"); return false ;}
        if (false == mpEfficiency->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : efficiency"); return false ;}
    }
    if (true == deserializer.hasProperty("deviceID"))
    {
        mpDeviceID = new OC::Cm::Serialization::Xsd::UuidType();
        if (mpDeviceID == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  deviceID"); return false;}
        if (false == deserializer.getPropertyValue("deviceID", &(mpDeviceID->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  deviceID"); return false ;}
        if (false == mpDeviceID->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : deviceID"); return false ;}
    }
    if (true == deserializer.hasProperty("groupID"))
    {
        mpGroupID = new ::String50();
        if (mpGroupID == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  groupID"); return false;}
        if (false == deserializer.getPropertyValue("groupID", &(mpGroupID->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  groupID"); return false ;}
        if (false == mpGroupID->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : groupID"); return false ;}
    }


    return true ;
}

std::string
UsageLimit::getElementName()
{
    return EN_USAGELIMIT;
}

bool
UsageLimit::clone(::UsageLimit *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::IdentifiedObject::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpThreshold) { delete mpThreshold; mpThreshold = NULL; }

        if ( NULL != pSrcObject->mpThreshold )
        {
            mpThreshold = new uint16_t();
            if (mpThreshold == NULL ) {return false;}
            *mpThreshold = *(pSrcObject->mpThreshold );
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
    {
        if ( NULL != mpEfficiency) { delete mpEfficiency; mpEfficiency = NULL; }

        if ( NULL != pSrcObject->mpEfficiency )
        {
            mpEfficiency = new ::String200();
            if (mpEfficiency == NULL ) {return false;}
            mpEfficiency->value = (pSrcObject->mpEfficiency->value );
        }
    }
    {
        if ( NULL != mpDeviceID) { delete mpDeviceID; mpDeviceID = NULL; }

        if ( NULL != pSrcObject->mpDeviceID )
        {
            mpDeviceID = new OC::Cm::Serialization::Xsd::UuidType();
            if (mpDeviceID == NULL ) {return false;}
            mpDeviceID->value = (pSrcObject->mpDeviceID->value );
        }
    }
    {
        if ( NULL != mpGroupID) { delete mpGroupID; mpGroupID = NULL; }

        if ( NULL != pSrcObject->mpGroupID )
        {
            mpGroupID = new ::String50();
            if (mpGroupID == NULL ) {return false;}
            mpGroupID->value = (pSrcObject->mpGroupID->value );
        }
    }


    return true ;
}
