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

#include "DRLC.h"


#include "Elements.h"

using namespace std;

DRLC::DRLC()
{

    mpDrlcLevel = NULL;
    mpDuration = NULL;
    mpDrlcStartTime = NULL;
    mpOverride = NULL;

}

DRLC::~DRLC()
{
    if ( NULL != mpDrlcLevel) { delete mpDrlcLevel; mpDrlcLevel = NULL; }
    if ( NULL != mpDuration) { delete mpDuration; mpDuration = NULL; }
    if ( NULL != mpDrlcStartTime) { delete mpDrlcStartTime; mpDrlcStartTime = NULL; }
    if ( NULL != mpOverride) { delete mpOverride; mpOverride = NULL; }

}

bool
DRLC::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    {
        bool bRet = false;

        if ( NULL != mpDrlcLevel )
        {
            bRet = serializer.setPropertyValue("drlcLevel", *mpDrlcLevel);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("drlcLevel") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : drlcLevel"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpDuration )
        {
            if (mpDuration->value.length() > 0)
            {
                if (true == mpDuration->validateContent())
                {
                    bRet = serializer.setPropertyValue("duration", mpDuration->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("duration") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : duration"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpDrlcStartTime )
        {
            if (mpDrlcStartTime->value.length() > 0)
            {
                if (true == mpDrlcStartTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("drlcStartTime", mpDrlcStartTime->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("drlcStartTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : drlcStartTime"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpOverride )
        {
            if (mpOverride->value.length() > 0)
            {
                if (true == mpOverride->validateContent())
                {
                    bRet = serializer.setPropertyValue("override", mpOverride->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("override") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : override"); return false; }}
    }


    return true ;
}

bool
DRLC::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("drlcLevel"))
    {
        mpDrlcLevel = new uint8_t();
        if (mpDrlcLevel == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  drlcLevel"); return false;}
        if (false == deserializer.getPropertyValue("drlcLevel", mpDrlcLevel)) { deserializer.setErrorMessage("Failed to deserialize parameter :  drlcLevel"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("drlcLevel") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : drlcLevel"); return false; }}
    if (true == deserializer.hasProperty("duration"))
    {
        mpDuration = new OC::Cm::Serialization::Xsd::TimeType();
        if (mpDuration == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  duration"); return false;}
        if (false == deserializer.getPropertyValue("duration", &(mpDuration->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  duration"); return false ;}
        if (false == mpDuration->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : duration"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("duration") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : duration"); return false; }}
    if (true == deserializer.hasProperty("drlcStartTime"))
    {
        mpDrlcStartTime = new OC::Cm::Serialization::Xsd::TimeType();
        if (mpDrlcStartTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  drlcStartTime"); return false;}
        if (false == deserializer.getPropertyValue("drlcStartTime", &(mpDrlcStartTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  drlcStartTime"); return false ;}
        if (false == mpDrlcStartTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : drlcStartTime"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("drlcStartTime") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : drlcStartTime"); return false; }}
    if (true == deserializer.hasProperty("override"))
    {
        mpOverride = new ::OnType();
        if (mpOverride == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  override"); return false;}
        if (false == deserializer.getPropertyValue("override", &(mpOverride->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  override"); return false ;}
        if (false == mpOverride->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : override"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("override") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : override"); return false; }}


    return true ;
}

std::string
DRLC::getElementName()
{
    return EN_DRLC;
}

bool
DRLC::clone(::DRLC *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpDrlcLevel) { delete mpDrlcLevel; mpDrlcLevel = NULL; }

        if ( NULL != pSrcObject->mpDrlcLevel )
        {
            mpDrlcLevel = new uint8_t();
            if (mpDrlcLevel == NULL ) {return false;}
            *mpDrlcLevel = *(pSrcObject->mpDrlcLevel );
        }
    }
    {
        if ( NULL != mpDuration) { delete mpDuration; mpDuration = NULL; }

        if ( NULL != pSrcObject->mpDuration )
        {
            mpDuration = new OC::Cm::Serialization::Xsd::TimeType();
            if (mpDuration == NULL ) {return false;}
            mpDuration->value = (pSrcObject->mpDuration->value );
        }
    }
    {
        if ( NULL != mpDrlcStartTime) { delete mpDrlcStartTime; mpDrlcStartTime = NULL; }

        if ( NULL != pSrcObject->mpDrlcStartTime )
        {
            mpDrlcStartTime = new OC::Cm::Serialization::Xsd::TimeType();
            if (mpDrlcStartTime == NULL ) {return false;}
            mpDrlcStartTime->value = (pSrcObject->mpDrlcStartTime->value );
        }
    }
    {
        if ( NULL != mpOverride) { delete mpOverride; mpOverride = NULL; }

        if ( NULL != pSrcObject->mpOverride )
        {
            mpOverride = new ::OnType();
            if (mpOverride == NULL ) {return false;}
            mpOverride->value = (pSrcObject->mpOverride->value );
        }
    }


    return true ;
}
