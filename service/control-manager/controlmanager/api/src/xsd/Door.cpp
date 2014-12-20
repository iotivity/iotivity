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



#include "Door.h"


#include "Elements.h"

using namespace std;

Door::Door()
{

    mpName = NULL;
    mpOpenState = NULL;
    mpOpenElapsedTime = NULL;
    mpOpenTime = NULL;
    mpLock = NULL;
    mpPassword = NULL;
    mpOneTimePassword = NULL;
    mpMode = NULL;
    mpEmergencyAlarm = NULL;

}

Door::~Door()
{
    if ( NULL != mpName) { delete mpName; mpName = NULL; }
    if ( NULL != mpOpenState) { delete mpOpenState; mpOpenState = NULL; }
    if ( NULL != mpOpenElapsedTime) { delete mpOpenElapsedTime; mpOpenElapsedTime = NULL; }
    if ( NULL != mpOpenTime) { delete mpOpenTime; mpOpenTime = NULL; }
    if ( NULL != mpLock) { delete mpLock; mpLock = NULL; }
    if ( NULL != mpPassword) { delete mpPassword; mpPassword = NULL; }
    if ( NULL != mpOneTimePassword) { delete mpOneTimePassword; mpOneTimePassword = NULL; }
    if ( NULL != mpMode) { delete mpMode; mpMode = NULL; }
    if ( NULL != mpEmergencyAlarm) { delete mpEmergencyAlarm; mpEmergencyAlarm = NULL; }

}

bool
Door::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::IdentifiedObject::serializeContents(serializer)) { return false;}

    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpName )
        {
            if (mpName->value.length() > 0)
            {
                if (true == mpName->validateContent())
                {
                    bRet = serializer.setPropertyValue("name", mpName->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("name") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : name"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpOpenState )
        {
            if (mpOpenState->value.length() > 0)
            {
                if (true == mpOpenState->validateContent())
                {
                    bRet = serializer.setPropertyValue("openState", mpOpenState->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("openState") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : openState"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpOpenElapsedTime )
        {
            if (mpOpenElapsedTime->value.length() > 0)
            {
                if (true == mpOpenElapsedTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("openElapsedTime", mpOpenElapsedTime->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("openElapsedTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : openElapsedTime"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpOpenTime )
        {
            if (mpOpenTime->value.length() > 0)
            {
                if (true == mpOpenTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("openTime", mpOpenTime->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("openTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : openTime"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpLock )
        {
            if (mpLock->value.length() > 0)
            {
                if (true == mpLock->validateContent())
                {
                    bRet = serializer.setPropertyValue("lock", mpLock->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("lock") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : lock"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpPassword )
        {
            if (mpPassword->value.length() > 0)
            {
                if (true == mpPassword->validateContent())
                {
                    bRet = serializer.setPropertyValue("password", mpPassword->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("password") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : password"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpOneTimePassword )
        {
            if (mpOneTimePassword->value.length() > 0)
            {
                if (true == mpOneTimePassword->validateContent())
                {
                    bRet = serializer.setPropertyValue("oneTimePassword", mpOneTimePassword->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("oneTimePassword") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : oneTimePassword"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpMode )
        {
            if (mpMode->value.length() > 0)
            {
                if (true == mpMode->validateContent())
                {
                    bRet = serializer.setPropertyValue("mode", mpMode->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("mode") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : mode"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpEmergencyAlarm )
        {
            bRet = serializer.setPropertyValue("emergencyAlarm", *mpEmergencyAlarm);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("emergencyAlarm") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : emergencyAlarm"); return false; }}
    }


    return true ;
}

bool
Door::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::IdentifiedObject::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("name"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  name"); return false;}
        mpName = new ::String10();
        if (mpName == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  name"); return false;}
        if (false == deserializer.getPropertyValue("name", &(mpName->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  name"); return false ;}
        if (false == mpName->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : name"); return false ;}
    }
    if (true == deserializer.hasProperty("openState"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  openState"); return false;}
        mpOpenState = new ::OpenType();
        if (mpOpenState == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  openState"); return false;}
        if (false == deserializer.getPropertyValue("openState",
                &(mpOpenState->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  openState"); return false ;}
        if (false == mpOpenState->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : openState"); return false ;}
    }
    if (true == deserializer.hasProperty("openElapsedTime"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  openElapsedTime"); return false;}
        mpOpenElapsedTime = new OC::Cm::Serialization::Xsd::TimeType();
        if (mpOpenElapsedTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  openElapsedTime"); return false;}
        if (false == deserializer.getPropertyValue("openElapsedTime",
                &(mpOpenElapsedTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  openElapsedTime"); return false ;}
        if (false == mpOpenElapsedTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : openElapsedTime"); return false ;}
    }
    if (true == deserializer.hasProperty("openTime"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  openTime"); return false;}
        mpOpenTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpOpenTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  openTime"); return false;}
        if (false == deserializer.getPropertyValue("openTime",
                &(mpOpenTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  openTime"); return false ;}
        if (false == mpOpenTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : openTime"); return false ;}
    }
    if (true == deserializer.hasProperty("lock"))
    {
        mpLock = new ::LockType();
        if (mpLock == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  lock"); return false;}
        if (false == deserializer.getPropertyValue("lock", &(mpLock->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  lock"); return false ;}
        if (false == mpLock->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : lock"); return false ;}
    }
    if (true == deserializer.hasProperty("password"))
    {
        mpPassword = new ::String20();
        if (mpPassword == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  password"); return false;}
        if (false == deserializer.getPropertyValue("password",
                &(mpPassword->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  password"); return false ;}
        if (false == mpPassword->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : password"); return false ;}
    }
    if (true == deserializer.hasProperty("oneTimePassword"))
    {
        mpOneTimePassword = new ::String20();
        if (mpOneTimePassword == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  oneTimePassword"); return false;}
        if (false == deserializer.getPropertyValue("oneTimePassword",
                &(mpOneTimePassword->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  oneTimePassword"); return false ;}
        if (false == mpOneTimePassword->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : oneTimePassword"); return false ;}
    }
    if (true == deserializer.hasProperty("mode"))
    {
        mpMode = new ::DoorLockModeType();
        if (mpMode == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  mode"); return false;}
        if (false == deserializer.getPropertyValue("mode", &(mpMode->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  mode"); return false ;}
        if (false == mpMode->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : mode"); return false ;}
    }
    if (true == deserializer.hasProperty("emergencyAlarm"))
    {
        mpEmergencyAlarm = new bool();
        if (mpEmergencyAlarm == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  emergencyAlarm"); return false;}
        if (false == deserializer.getPropertyValue("emergencyAlarm", mpEmergencyAlarm)) { deserializer.setErrorMessage("Failed to deserialize parameter :  emergencyAlarm"); return false ;}
    }


    return true ;
}

std::string
Door::getElementName()
{
    return EN_DOOR;
}

bool
Door::clone(::Door *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::IdentifiedObject::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpName) { delete mpName; mpName = NULL; }

        if ( NULL != pSrcObject->mpName )
        {
            mpName = new ::String10();
            if (mpName == NULL ) {return false;}
            mpName->value = (pSrcObject->mpName->value );
        }
    }
    {
        if ( NULL != mpOpenState) { delete mpOpenState; mpOpenState = NULL; }

        if ( NULL != pSrcObject->mpOpenState )
        {
            mpOpenState = new ::OpenType();
            if (mpOpenState == NULL ) {return false;}
            mpOpenState->value = (pSrcObject->mpOpenState->value );
        }
    }
    {
        if ( NULL != mpOpenElapsedTime) { delete mpOpenElapsedTime; mpOpenElapsedTime = NULL; }

        if ( NULL != pSrcObject->mpOpenElapsedTime )
        {
            mpOpenElapsedTime = new OC::Cm::Serialization::Xsd::TimeType();
            if (mpOpenElapsedTime == NULL ) {return false;}
            mpOpenElapsedTime->value = (pSrcObject->mpOpenElapsedTime->value );
        }
    }
    {
        if ( NULL != mpOpenTime) { delete mpOpenTime; mpOpenTime = NULL; }

        if ( NULL != pSrcObject->mpOpenTime )
        {
            mpOpenTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (mpOpenTime == NULL ) {return false;}
            mpOpenTime->value = (pSrcObject->mpOpenTime->value );
        }
    }
    {
        if ( NULL != mpLock) { delete mpLock; mpLock = NULL; }

        if ( NULL != pSrcObject->mpLock )
        {
            mpLock = new ::LockType();
            if (mpLock == NULL ) {return false;}
            mpLock->value = (pSrcObject->mpLock->value );
        }
    }
    {
        if ( NULL != mpPassword) { delete mpPassword; mpPassword = NULL; }

        if ( NULL != pSrcObject->mpPassword )
        {
            mpPassword = new ::String20();
            if (mpPassword == NULL ) {return false;}
            mpPassword->value = (pSrcObject->mpPassword->value );
        }
    }
    {
        if ( NULL != mpOneTimePassword) { delete mpOneTimePassword; mpOneTimePassword = NULL; }

        if ( NULL != pSrcObject->mpOneTimePassword )
        {
            mpOneTimePassword = new ::String20();
            if (mpOneTimePassword == NULL ) {return false;}
            mpOneTimePassword->value = (pSrcObject->mpOneTimePassword->value );
        }
    }
    {
        if ( NULL != mpMode) { delete mpMode; mpMode = NULL; }

        if ( NULL != pSrcObject->mpMode )
        {
            mpMode = new ::DoorLockModeType();
            if (mpMode == NULL ) {return false;}
            mpMode->value = (pSrcObject->mpMode->value );
        }
    }
    {
        if ( NULL != mpEmergencyAlarm) { delete mpEmergencyAlarm; mpEmergencyAlarm = NULL; }

        if ( NULL != pSrcObject->mpEmergencyAlarm )
        {
            mpEmergencyAlarm = new bool();
            if (mpEmergencyAlarm == NULL ) {return false;}
            *mpEmergencyAlarm = *(pSrcObject->mpEmergencyAlarm );
        }
    }


    return true ;
}
