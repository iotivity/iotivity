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



#include "Operation.h"


#include "Elements.h"

using namespace std;

Operation::Operation()
{

    mpPower = NULL;
    mpState = NULL;
    mpSupportedProgress = NULL;
    mpProgress = NULL;
    mpOperationTime = NULL;
    mpOperationTimeMin = NULL;
    mpOperationTimeMax = NULL;
    mpRemainingTime = NULL;
    mpRunningTime = NULL;
    mpDelayStartTime = NULL;
    mpDelayEndTime = NULL;
    mpProgressPercentage = NULL;
    mpKidsLock = NULL;
    mpReset = NULL;

}

Operation::~Operation()
{
    if ( NULL != mpPower) { delete mpPower; mpPower = NULL; }
    if ( NULL != mpState) { delete mpState; mpState = NULL; }
    if ( NULL != mpSupportedProgress) { delete mpSupportedProgress; mpSupportedProgress = NULL; }
    if ( NULL != mpProgress) { delete mpProgress; mpProgress = NULL; }
    if ( NULL != mpOperationTime) { delete mpOperationTime; mpOperationTime = NULL; }
    if ( NULL != mpOperationTimeMin) { delete mpOperationTimeMin; mpOperationTimeMin = NULL; }
    if ( NULL != mpOperationTimeMax) { delete mpOperationTimeMax; mpOperationTimeMax = NULL; }
    if ( NULL != mpRemainingTime) { delete mpRemainingTime; mpRemainingTime = NULL; }
    if ( NULL != mpRunningTime) { delete mpRunningTime; mpRunningTime = NULL; }
    if ( NULL != mpDelayStartTime) { delete mpDelayStartTime; mpDelayStartTime = NULL; }
    if ( NULL != mpDelayEndTime) { delete mpDelayEndTime; mpDelayEndTime = NULL; }
    if ( NULL != mpProgressPercentage) { delete mpProgressPercentage; mpProgressPercentage = NULL; }
    if ( NULL != mpKidsLock) { delete mpKidsLock; mpKidsLock = NULL; }
    if ( NULL != mpReset) { delete mpReset; mpReset = NULL; }

}

bool
Operation::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    {
        bool bRet = false;

        if ( NULL != mpPower )
        {
            if (mpPower->value.length() > 0)
            {
                if (true == mpPower->validateContent())
                {
                    bRet = serializer.setPropertyValue("power", mpPower->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("power") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : power"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpState )
        {
            if (mpState->value.length() > 0)
            {
                if (true == mpState->validateContent())
                {
                    bRet = serializer.setPropertyValue("state", mpState->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("state") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : state"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpSupportedProgress )
        {
            mpSupportedProgress->setInstanceName("supportedProgress");
            bRet = mpSupportedProgress->serializeContents(serializer) ;
        }
        if (false == bRet) { if (serializer.isInFilteringMode("supportedProgress") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : supportedProgress"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpProgress )
        {
            if (mpProgress->value.length() > 0)
            {
                if (true == mpProgress->validateContent())
                {
                    bRet = serializer.setPropertyValue("progress", mpProgress->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("progress") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : progress"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpOperationTime )
        {
            if (mpOperationTime->value.length() > 0)
            {
                if (true == mpOperationTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("operationTime", mpOperationTime->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("operationTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : operationTime"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpOperationTimeMin )
        {
            if (mpOperationTimeMin->value.length() > 0)
            {
                if (true == mpOperationTimeMin->validateContent())
                {
                    bRet = serializer.setPropertyValue("operationTimeMin", mpOperationTimeMin->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("operationTimeMin") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : operationTimeMin"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpOperationTimeMax )
        {
            if (mpOperationTimeMax->value.length() > 0)
            {
                if (true == mpOperationTimeMax->validateContent())
                {
                    bRet = serializer.setPropertyValue("operationTimeMax", mpOperationTimeMax->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("operationTimeMax") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : operationTimeMax"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpRemainingTime )
        {
            if (mpRemainingTime->value.length() > 0)
            {
                if (true == mpRemainingTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("remainingTime", mpRemainingTime->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("remainingTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : remainingTime"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpRunningTime )
        {
            if (mpRunningTime->value.length() > 0)
            {
                if (true == mpRunningTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("runningTime", mpRunningTime->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("runningTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : runningTime"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpDelayStartTime )
        {
            if (mpDelayStartTime->value.length() > 0)
            {
                if (true == mpDelayStartTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("delayStartTime", mpDelayStartTime->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("delayStartTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : delayStartTime"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpDelayEndTime )
        {
            if (mpDelayEndTime->value.length() > 0)
            {
                if (true == mpDelayEndTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("delayEndTime", mpDelayEndTime->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("delayEndTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : delayEndTime"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpProgressPercentage )
        {
            bRet = serializer.setPropertyValue("progressPercentage", *mpProgressPercentage);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("progressPercentage") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : progressPercentage"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpKidsLock )
        {
            if (mpKidsLock->value.length() > 0)
            {
                if (true == mpKidsLock->validateContent())
                {
                    bRet = serializer.setPropertyValue("kidsLock", mpKidsLock->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("kidsLock") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : kidsLock"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpReset )
        {
            if (mpReset->value.length() > 0)
            {
                if (true == mpReset->validateContent())
                {
                    bRet = serializer.setPropertyValue("reset", mpReset->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("reset") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : reset"); return false; }}
    }


    return true ;
}

bool
Operation::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("power"))
    {
        mpPower = new ::OnType();
        if (mpPower == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  power"); return false;}
        if (false == deserializer.getPropertyValue("power",
                &(mpPower->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  power"); return false ;}
        if (false == mpPower->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : power"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("power") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : power"); return false; }}
    if (true == deserializer.hasProperty("state"))
    {
        mpState = new ::StateType();
        if (mpState == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  state"); return false;}
        if (false == deserializer.getPropertyValue("state",
                &(mpState->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  state"); return false ;}
        if (false == mpState->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : state"); return false ;}
    }
    {
        if (true == deserializer.hasProperty("supportedProgress"))
        {
            if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  supportedProgress"); return false;}
            mpSupportedProgress = new ::ArrayOfString();
            if (mpSupportedProgress == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  supportedProgress"); return false;}
            mpSupportedProgress->setInstanceName("supportedProgress");
            if (false == mpSupportedProgress->deSerializeContents(deserializer)) {  deserializer.setErrorMessage("Parameter Contains Invalid Value : supportedProgress"); return false ;}
        }
    }
    if (true == deserializer.hasProperty("progress"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  progress"); return false;}
        mpProgress = new ::String20();
        if (mpProgress == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  progress"); return false;}
        if (false == deserializer.getPropertyValue("progress",
                &(mpProgress->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  progress"); return false ;}
        if (false == mpProgress->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : progress"); return false ;}
    }
    if (true == deserializer.hasProperty("operationTime"))
    {
        mpOperationTime = new OC::Cm::Serialization::Xsd::TimeType();
        if (mpOperationTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  operationTime"); return false;}
        if (false == deserializer.getPropertyValue("operationTime",
                &(mpOperationTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  operationTime"); return false ;}
        if (false == mpOperationTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : operationTime"); return false ;}
    }
    if (true == deserializer.hasProperty("operationTimeMin"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  operationTimeMin"); return false;}
        mpOperationTimeMin = new OC::Cm::Serialization::Xsd::TimeType();
        if (mpOperationTimeMin == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  operationTimeMin"); return false;}
        if (false == deserializer.getPropertyValue("operationTimeMin",
                &(mpOperationTimeMin->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  operationTimeMin"); return false ;}
        if (false == mpOperationTimeMin->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : operationTimeMin"); return false ;}
    }
    if (true == deserializer.hasProperty("operationTimeMax"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  operationTimeMax"); return false;}
        mpOperationTimeMax = new OC::Cm::Serialization::Xsd::TimeType();
        if (mpOperationTimeMax == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  operationTimeMax"); return false;}
        if (false == deserializer.getPropertyValue("operationTimeMax",
                &(mpOperationTimeMax->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  operationTimeMax"); return false ;}
        if (false == mpOperationTimeMax->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : operationTimeMax"); return false ;}
    }
    if (true == deserializer.hasProperty("remainingTime"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  remainingTime"); return false;}
        mpRemainingTime = new OC::Cm::Serialization::Xsd::TimeType();
        if (mpRemainingTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  remainingTime"); return false;}
        if (false == deserializer.getPropertyValue("remainingTime",
                &(mpRemainingTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  remainingTime"); return false ;}
        if (false == mpRemainingTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : remainingTime"); return false ;}
    }
    if (true == deserializer.hasProperty("runningTime"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  runningTime"); return false;}
        mpRunningTime = new OC::Cm::Serialization::Xsd::TimeType();
        if (mpRunningTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  runningTime"); return false;}
        if (false == deserializer.getPropertyValue("runningTime",
                &(mpRunningTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  runningTime"); return false ;}
        if (false == mpRunningTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : runningTime"); return false ;}
    }
    if (true == deserializer.hasProperty("delayStartTime"))
    {
        mpDelayStartTime = new OC::Cm::Serialization::Xsd::TimeType();
        if (mpDelayStartTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  delayStartTime"); return false;}
        if (false == deserializer.getPropertyValue("delayStartTime",
                &(mpDelayStartTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  delayStartTime"); return false ;}
        if (false == mpDelayStartTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : delayStartTime"); return false ;}
    }
    if (true == deserializer.hasProperty("delayEndTime"))
    {
        mpDelayEndTime = new OC::Cm::Serialization::Xsd::TimeType();
        if (mpDelayEndTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  delayEndTime"); return false;}
        if (false == deserializer.getPropertyValue("delayEndTime",
                &(mpDelayEndTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  delayEndTime"); return false ;}
        if (false == mpDelayEndTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : delayEndTime"); return false ;}
    }
    if (true == deserializer.hasProperty("progressPercentage"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  progressPercentage"); return false;}
        mpProgressPercentage = new uint8_t();
        if (mpProgressPercentage == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  progressPercentage"); return false;}
        if (false == deserializer.getPropertyValue("progressPercentage", mpProgressPercentage)) { deserializer.setErrorMessage("Failed to deserialize parameter :  progressPercentage"); return false ;}
    }
    if (true == deserializer.hasProperty("kidsLock"))
    {
        mpKidsLock = new ::StateType();
        if (mpKidsLock == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  kidsLock"); return false;}
        if (false == deserializer.getPropertyValue("kidsLock",
                &(mpKidsLock->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  kidsLock"); return false ;}
        if (false == mpKidsLock->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : kidsLock"); return false ;}
    }
    if (true == deserializer.hasProperty("reset"))
    {
        mpReset = new ::OnType();
        if (mpReset == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  reset"); return false;}
        if (false == deserializer.getPropertyValue("reset",
                &(mpReset->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  reset"); return false ;}
        if (false == mpReset->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : reset"); return false ;}
    }


    return true ;
}

std::string
Operation::getElementName()
{
    return EN_OPERATION;
}

bool
Operation::clone(::Operation *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpPower) { delete mpPower; mpPower = NULL; }

        if ( NULL != pSrcObject->mpPower )
        {
            mpPower = new ::OnType();
            if (mpPower == NULL ) {return false;}
            mpPower->value = (pSrcObject->mpPower->value );
        }
    }
    {
        if ( NULL != mpState) { delete mpState; mpState = NULL; }

        if ( NULL != pSrcObject->mpState )
        {
            mpState = new ::StateType();
            if (mpState == NULL ) {return false;}
            mpState->value = (pSrcObject->mpState->value );
        }
    }
    {
        if ( NULL != mpSupportedProgress) { delete mpSupportedProgress; mpSupportedProgress = NULL; }

        if ( NULL != pSrcObject->mpSupportedProgress )
        {
            mpSupportedProgress = new ::ArrayOfString();
            if (mpSupportedProgress == NULL ) { return false;}
            mpSupportedProgress->members = pSrcObject->mpSupportedProgress->members;
        }
    }
    {
        if ( NULL != mpProgress) { delete mpProgress; mpProgress = NULL; }

        if ( NULL != pSrcObject->mpProgress )
        {
            mpProgress = new ::String20();
            if (mpProgress == NULL ) {return false;}
            mpProgress->value = (pSrcObject->mpProgress->value );
        }
    }
    {
        if ( NULL != mpOperationTime) { delete mpOperationTime; mpOperationTime = NULL; }

        if ( NULL != pSrcObject->mpOperationTime )
        {
            mpOperationTime = new OC::Cm::Serialization::Xsd::TimeType();
            if (mpOperationTime == NULL ) {return false;}
            mpOperationTime->value = (pSrcObject->mpOperationTime->value );
        }
    }
    {
        if ( NULL != mpOperationTimeMin) { delete mpOperationTimeMin; mpOperationTimeMin = NULL; }

        if ( NULL != pSrcObject->mpOperationTimeMin )
        {
            mpOperationTimeMin = new OC::Cm::Serialization::Xsd::TimeType();
            if (mpOperationTimeMin == NULL ) {return false;}
            mpOperationTimeMin->value = (pSrcObject->mpOperationTimeMin->value );
        }
    }
    {
        if ( NULL != mpOperationTimeMax) { delete mpOperationTimeMax; mpOperationTimeMax = NULL; }

        if ( NULL != pSrcObject->mpOperationTimeMax )
        {
            mpOperationTimeMax = new OC::Cm::Serialization::Xsd::TimeType();
            if (mpOperationTimeMax == NULL ) {return false;}
            mpOperationTimeMax->value = (pSrcObject->mpOperationTimeMax->value );
        }
    }
    {
        if ( NULL != mpRemainingTime) { delete mpRemainingTime; mpRemainingTime = NULL; }

        if ( NULL != pSrcObject->mpRemainingTime )
        {
            mpRemainingTime = new OC::Cm::Serialization::Xsd::TimeType();
            if (mpRemainingTime == NULL ) {return false;}
            mpRemainingTime->value = (pSrcObject->mpRemainingTime->value );
        }
    }
    {
        if ( NULL != mpRunningTime) { delete mpRunningTime; mpRunningTime = NULL; }

        if ( NULL != pSrcObject->mpRunningTime )
        {
            mpRunningTime = new OC::Cm::Serialization::Xsd::TimeType();
            if (mpRunningTime == NULL ) {return false;}
            mpRunningTime->value = (pSrcObject->mpRunningTime->value );
        }
    }
    {
        if ( NULL != mpDelayStartTime) { delete mpDelayStartTime; mpDelayStartTime = NULL; }

        if ( NULL != pSrcObject->mpDelayStartTime )
        {
            mpDelayStartTime = new OC::Cm::Serialization::Xsd::TimeType();
            if (mpDelayStartTime == NULL ) {return false;}
            mpDelayStartTime->value = (pSrcObject->mpDelayStartTime->value );
        }
    }
    {
        if ( NULL != mpDelayEndTime) { delete mpDelayEndTime; mpDelayEndTime = NULL; }

        if ( NULL != pSrcObject->mpDelayEndTime )
        {
            mpDelayEndTime = new OC::Cm::Serialization::Xsd::TimeType();
            if (mpDelayEndTime == NULL ) {return false;}
            mpDelayEndTime->value = (pSrcObject->mpDelayEndTime->value );
        }
    }
    {
        if ( NULL != mpProgressPercentage) { delete mpProgressPercentage; mpProgressPercentage = NULL; }

        if ( NULL != pSrcObject->mpProgressPercentage )
        {
            mpProgressPercentage = new uint8_t();
            if (mpProgressPercentage == NULL ) {return false;}
            *mpProgressPercentage = *(pSrcObject->mpProgressPercentage );
        }
    }
    {
        if ( NULL != mpKidsLock) { delete mpKidsLock; mpKidsLock = NULL; }

        if ( NULL != pSrcObject->mpKidsLock )
        {
            mpKidsLock = new ::StateType();
            if (mpKidsLock == NULL ) {return false;}
            mpKidsLock->value = (pSrcObject->mpKidsLock->value );
        }
    }
    {
        if ( NULL != mpReset) { delete mpReset; mpReset = NULL; }

        if ( NULL != pSrcObject->mpReset )
        {
            mpReset = new ::OnType();
            if (mpReset == NULL ) {return false;}
            mpReset->value = (pSrcObject->mpReset->value );
        }
    }


    return true ;
}
