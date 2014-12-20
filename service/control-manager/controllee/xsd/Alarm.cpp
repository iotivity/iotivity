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

#include "Alarm.h"


#include "Elements.h"

using namespace std;

Alarm::Alarm()
{

    mpTriggeredTime = NULL;
    mpAlarmType = NULL;
    mpCode = NULL;
    mpMulticastEnabled = NULL;

}

Alarm::~Alarm()
{
    if ( NULL != mpTriggeredTime) { delete mpTriggeredTime; mpTriggeredTime = NULL; }
    if ( NULL != mpAlarmType) { delete mpAlarmType; mpAlarmType = NULL; }
    if ( NULL != mpCode) { delete mpCode; mpCode = NULL; }
    if ( NULL != mpMulticastEnabled) { delete mpMulticastEnabled; mpMulticastEnabled = NULL; }

}

bool
Alarm::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::IdentifiedObject::serializeContents(serializer)) { return false;}

    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpTriggeredTime )
        {
            if (mpTriggeredTime->value.length() > 0)
            {
                if (true == mpTriggeredTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("triggeredTime", mpTriggeredTime->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("triggeredTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : triggeredTime"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpAlarmType )
        {
            if (mpAlarmType->value.length() > 0)
            {
                if (true == mpAlarmType->validateContent())
                {
                    bRet = serializer.setPropertyValue("alarmType", mpAlarmType->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("alarmType") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : alarmType"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpCode )
        {
            if (mpCode->value.length() > 0)
            {
                if (true == mpCode->validateContent())
                {
                    bRet = serializer.setPropertyValue("code", mpCode->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("code") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : code"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpMulticastEnabled )
        {
            bRet = serializer.setPropertyValue("multicastEnabled", *mpMulticastEnabled);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("multicastEnabled") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : multicastEnabled"); return false; }}
    }


    return true ;
}

bool
Alarm::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::IdentifiedObject::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("triggeredTime"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  triggeredTime"); return false;}
        mpTriggeredTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpTriggeredTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  triggeredTime"); return false;}
        if (false == deserializer.getPropertyValue("triggeredTime", &(mpTriggeredTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  triggeredTime"); return false ;}
        if (false == mpTriggeredTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : triggeredTime"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("triggeredTime") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : triggeredTime"); return false; }}
    if (true == deserializer.hasProperty("alarmType"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  alarmType"); return false;}
        mpAlarmType = new ::AlarmCodeType();
        if (mpAlarmType == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  alarmType"); return false;}
        if (false == deserializer.getPropertyValue("alarmType", &(mpAlarmType->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  alarmType"); return false ;}
        if (false == mpAlarmType->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : alarmType"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("alarmType") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : alarmType"); return false; }}
    if (true == deserializer.hasProperty("code"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  code"); return false;}
        mpCode = new ::String20();
        if (mpCode == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  code"); return false;}
        if (false == deserializer.getPropertyValue("code", &(mpCode->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  code"); return false ;}
        if (false == mpCode->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : code"); return false ;}
    }
    if (true == deserializer.hasProperty("multicastEnabled"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  multicastEnabled"); return false;}
        mpMulticastEnabled = new bool();
        if (mpMulticastEnabled == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  multicastEnabled"); return false;}
        if (false == deserializer.getPropertyValue("multicastEnabled", mpMulticastEnabled)) { deserializer.setErrorMessage("Failed to deserialize parameter :  multicastEnabled"); return false ;}
    }


    return true ;
}

std::string
Alarm::getElementName()
{
    return EN_ALARM;
}

bool
Alarm::clone(::Alarm *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::IdentifiedObject::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpTriggeredTime) { delete mpTriggeredTime; mpTriggeredTime = NULL; }

        if ( NULL != pSrcObject->mpTriggeredTime )
        {
            mpTriggeredTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (mpTriggeredTime == NULL ) {return false;}
            mpTriggeredTime->value = (pSrcObject->mpTriggeredTime->value );
        }
    }
    {
        if ( NULL != mpAlarmType) { delete mpAlarmType; mpAlarmType = NULL; }

        if ( NULL != pSrcObject->mpAlarmType )
        {
            mpAlarmType = new ::AlarmCodeType();
            if (mpAlarmType == NULL ) {return false;}
            mpAlarmType->value = (pSrcObject->mpAlarmType->value );
        }
    }
    {
        if ( NULL != mpCode) { delete mpCode; mpCode = NULL; }

        if ( NULL != pSrcObject->mpCode )
        {
            mpCode = new ::String20();
            if (mpCode == NULL ) {return false;}
            mpCode->value = (pSrcObject->mpCode->value );
        }
    }
    {
        if ( NULL != mpMulticastEnabled) { delete mpMulticastEnabled; mpMulticastEnabled = NULL; }

        if ( NULL != pSrcObject->mpMulticastEnabled )
        {
            mpMulticastEnabled = new bool();
            if (mpMulticastEnabled == NULL ) {return false;}
            *mpMulticastEnabled = *(pSrcObject->mpMulticastEnabled );
        }
    }


    return true ;
}
