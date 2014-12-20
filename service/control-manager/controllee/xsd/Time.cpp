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

#include "Time.h"


#include "Elements.h"

using namespace std;

Time::Time()
{

    mpCurrentTime = NULL;
    mpDstStartTime = NULL;
    mpDstOffset = NULL;
    mpDstEndTime = NULL;
    mpTzOffset = NULL;
    mpTzID = NULL;
    mpUtcTime = NULL;

}

Time::~Time()
{
    if ( NULL != mpCurrentTime) { delete mpCurrentTime; mpCurrentTime = NULL; }
    if ( NULL != mpDstStartTime) { delete mpDstStartTime; mpDstStartTime = NULL; }
    if ( NULL != mpDstOffset) { delete mpDstOffset; mpDstOffset = NULL; }
    if ( NULL != mpDstEndTime) { delete mpDstEndTime; mpDstEndTime = NULL; }
    if ( NULL != mpTzOffset) { delete mpTzOffset; mpTzOffset = NULL; }
    if ( NULL != mpTzID) { delete mpTzID; mpTzID = NULL; }
    if ( NULL != mpUtcTime) { delete mpUtcTime; mpUtcTime = NULL; }

}

bool
Time::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    {
        bool bRet = false;

        if ( NULL != mpCurrentTime )
        {
            if (mpCurrentTime->value.length() > 0)
            {
                if (true == mpCurrentTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("currentTime", mpCurrentTime->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("currentTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : currentTime"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpDstStartTime )
        {
            if (mpDstStartTime->value.length() > 0)
            {
                if (true == mpDstStartTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("dstStartTime", mpDstStartTime->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("dstStartTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : dstStartTime"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpDstOffset )
        {
            if (mpDstOffset->value.length() > 0)
            {
                if (true == mpDstOffset->validateContent())
                {
                    bRet = serializer.setPropertyValue("dstOffset", mpDstOffset->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("dstOffset") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : dstOffset"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpDstEndTime )
        {
            if (mpDstEndTime->value.length() > 0)
            {
                if (true == mpDstEndTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("dstEndTime", mpDstEndTime->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("dstEndTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : dstEndTime"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpTzOffset )
        {
            if (mpTzOffset->value.length() > 0)
            {
                if (true == mpTzOffset->validateContent())
                {
                    bRet = serializer.setPropertyValue("tzOffset", mpTzOffset->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("tzOffset") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : tzOffset"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpTzID )
        {
            if (mpTzID->value.length() > 0)
            {
                if (true == mpTzID->validateContent())
                {
                    bRet = serializer.setPropertyValue("tzID", mpTzID->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("tzID") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : tzID"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpUtcTime )
        {
            if (mpUtcTime->value.length() > 0)
            {
                if (true == mpUtcTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("utcTime", mpUtcTime->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("utcTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : utcTime"); return false; }}
    }


    return true ;
}

bool
Time::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("currentTime"))
    {
        mpCurrentTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpCurrentTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  currentTime"); return false;}
        if (false == deserializer.getPropertyValue("currentTime", &(mpCurrentTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  currentTime"); return false ;}
        if (false == mpCurrentTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : currentTime"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("currentTime") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : currentTime"); return false; }}
    if (true == deserializer.hasProperty("dstStartTime"))
    {
        mpDstStartTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpDstStartTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  dstStartTime"); return false;}
        if (false == deserializer.getPropertyValue("dstStartTime", &(mpDstStartTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  dstStartTime"); return false ;}
        if (false == mpDstStartTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : dstStartTime"); return false ;}
    }
    if (true == deserializer.hasProperty("dstOffset"))
    {
        mpDstOffset = new OC::Cm::Serialization::Xsd::TimeOffsetType();
        if (mpDstOffset == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  dstOffset"); return false;}
        if (false == deserializer.getPropertyValue("dstOffset", &(mpDstOffset->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  dstOffset"); return false ;}
        if (false == mpDstOffset->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : dstOffset"); return false ;}
    }
    if (true == deserializer.hasProperty("dstEndTime"))
    {
        mpDstEndTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpDstEndTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  dstEndTime"); return false;}
        if (false == deserializer.getPropertyValue("dstEndTime", &(mpDstEndTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  dstEndTime"); return false ;}
        if (false == mpDstEndTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : dstEndTime"); return false ;}
    }
    if (true == deserializer.hasProperty("tzOffset"))
    {
        mpTzOffset = new OC::Cm::Serialization::Xsd::TimeOffsetType();
        if (mpTzOffset == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  tzOffset"); return false;}
        if (false == deserializer.getPropertyValue("tzOffset", &(mpTzOffset->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  tzOffset"); return false ;}
        if (false == mpTzOffset->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : tzOffset"); return false ;}
    }
    if (true == deserializer.hasProperty("tzID"))
    {
        mpTzID = new ::TimeZoneType();
        if (mpTzID == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  tzID"); return false;}
        if (false == deserializer.getPropertyValue("tzID", &(mpTzID->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  tzID"); return false ;}
        if (false == mpTzID->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : tzID"); return false ;}
    }
    if (true == deserializer.hasProperty("utcTime"))
    {
        mpUtcTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpUtcTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  utcTime"); return false;}
        if (false == deserializer.getPropertyValue("utcTime", &(mpUtcTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  utcTime"); return false ;}
        if (false == mpUtcTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : utcTime"); return false ;}
    }


    return true ;
}

std::string
Time::getElementName()
{
    return EN_TIME;
}

bool
Time::clone(::Time *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpCurrentTime) { delete mpCurrentTime; mpCurrentTime = NULL; }

        if ( NULL != pSrcObject->mpCurrentTime )
        {
            mpCurrentTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (mpCurrentTime == NULL ) {return false;}
            mpCurrentTime->value = (pSrcObject->mpCurrentTime->value );
        }
    }
    {
        if ( NULL != mpDstStartTime) { delete mpDstStartTime; mpDstStartTime = NULL; }

        if ( NULL != pSrcObject->mpDstStartTime )
        {
            mpDstStartTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (mpDstStartTime == NULL ) {return false;}
            mpDstStartTime->value = (pSrcObject->mpDstStartTime->value );
        }
    }
    {
        if ( NULL != mpDstOffset) { delete mpDstOffset; mpDstOffset = NULL; }

        if ( NULL != pSrcObject->mpDstOffset )
        {
            mpDstOffset = new OC::Cm::Serialization::Xsd::TimeOffsetType();
            if (mpDstOffset == NULL ) {return false;}
            mpDstOffset->value = (pSrcObject->mpDstOffset->value );
        }
    }
    {
        if ( NULL != mpDstEndTime) { delete mpDstEndTime; mpDstEndTime = NULL; }

        if ( NULL != pSrcObject->mpDstEndTime )
        {
            mpDstEndTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (mpDstEndTime == NULL ) {return false;}
            mpDstEndTime->value = (pSrcObject->mpDstEndTime->value );
        }
    }
    {
        if ( NULL != mpTzOffset) { delete mpTzOffset; mpTzOffset = NULL; }

        if ( NULL != pSrcObject->mpTzOffset )
        {
            mpTzOffset = new OC::Cm::Serialization::Xsd::TimeOffsetType();
            if (mpTzOffset == NULL ) {return false;}
            mpTzOffset->value = (pSrcObject->mpTzOffset->value );
        }
    }
    {
        if ( NULL != mpTzID) { delete mpTzID; mpTzID = NULL; }

        if ( NULL != pSrcObject->mpTzID )
        {
            mpTzID = new ::TimeZoneType();
            if (mpTzID == NULL ) {return false;}
            mpTzID->value = (pSrcObject->mpTzID->value );
        }
    }
    {
        if ( NULL != mpUtcTime) { delete mpUtcTime; mpUtcTime = NULL; }

        if ( NULL != pSrcObject->mpUtcTime )
        {
            mpUtcTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (mpUtcTime == NULL ) {return false;}
            mpUtcTime->value = (pSrcObject->mpUtcTime->value );
        }
    }


    return true ;
}
