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

#include "Message.h"


#include "Elements.h"

using namespace std;

Message::Message()
{

    mpStartTime = NULL;
    mpDuration = NULL;
    mpPriority = NULL;
    mpWriter = NULL;
    mpTitle = NULL;
    mpMessageType = NULL;
    mpText = NULL;
    mpImage = NULL;

}

Message::~Message()
{
    if ( NULL != mpStartTime) { delete mpStartTime; mpStartTime = NULL; }
    if ( NULL != mpDuration) { delete mpDuration; mpDuration = NULL; }
    if ( NULL != mpPriority) { delete mpPriority; mpPriority = NULL; }
    if ( NULL != mpWriter) { delete mpWriter; mpWriter = NULL; }
    if ( NULL != mpTitle) { delete mpTitle; mpTitle = NULL; }
    if ( NULL != mpMessageType) { delete mpMessageType; mpMessageType = NULL; }
    if ( NULL != mpText) { delete mpText; mpText = NULL; }
    if ( NULL != mpImage) { delete mpImage; mpImage = NULL; }

}

bool
Message::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::IdentifiedObject::serializeContents(serializer)) { return false;}

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
        bool bRet = true;

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
        }
        if (false == bRet) { if (serializer.isInFilteringMode("duration") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : duration"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpPriority )
        {
            bRet = serializer.setPropertyValue("priority", *mpPriority);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("priority") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : priority"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpWriter )
        {
            if (mpWriter->value.length() > 0)
            {
                if (true == mpWriter->validateContent())
                {
                    bRet = serializer.setPropertyValue("writer", mpWriter->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("writer") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : writer"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpTitle )
        {
            if (mpTitle->value.length() > 0)
            {
                if (true == mpTitle->validateContent())
                {
                    bRet = serializer.setPropertyValue("title", mpTitle->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("title") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : title"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpMessageType )
        {
            if (mpMessageType->value.length() > 0)
            {
                if (true == mpMessageType->validateContent())
                {
                    bRet = serializer.setPropertyValue("messageType", mpMessageType->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("messageType") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : messageType"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpText )
        {
            if (mpText->value.length() > 0)
            {
                if (true == mpText->validateContent())
                {
                    bRet = serializer.setPropertyValue("text", mpText->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("text") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : text"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpImage )
        {
            bRet = mpImage->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("Image") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Image"); return false; }}
    }


    return true ;
}

bool
Message::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::IdentifiedObject::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("startTime"))
    {
        mpStartTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpStartTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  startTime"); return false;}
        if (false == deserializer.getPropertyValue("startTime", &(mpStartTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  startTime"); return false ;}
        if (false == mpStartTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : startTime"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("startTime") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : startTime"); return false; }}
    if (true == deserializer.hasProperty("duration"))
    {
        mpDuration = new OC::Cm::Serialization::Xsd::TimeType();
        if (mpDuration == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  duration"); return false;}
        if (false == deserializer.getPropertyValue("duration", &(mpDuration->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  duration"); return false ;}
        if (false == mpDuration->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : duration"); return false ;}
    }
    if (true == deserializer.hasProperty("priority"))
    {
        mpPriority = new uint8_t();
        if (mpPriority == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  priority"); return false;}
        if (false == deserializer.getPropertyValue("priority", mpPriority)) { deserializer.setErrorMessage("Failed to deserialize parameter :  priority"); return false ;}
    }
    if (true == deserializer.hasProperty("writer"))
    {
        mpWriter = new ::String20();
        if (mpWriter == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  writer"); return false;}
        if (false == deserializer.getPropertyValue("writer", &(mpWriter->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  writer"); return false ;}
        if (false == mpWriter->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : writer"); return false ;}
    }
    if (true == deserializer.hasProperty("title"))
    {
        mpTitle = new ::String20();
        if (mpTitle == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  title"); return false;}
        if (false == deserializer.getPropertyValue("title", &(mpTitle->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  title"); return false ;}
        if (false == mpTitle->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : title"); return false ;}
    }
    if (true == deserializer.hasProperty("messageType"))
    {
        mpMessageType = new ::MessageType();
        if (mpMessageType == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  messageType"); return false;}
        if (false == deserializer.getPropertyValue("messageType", &(mpMessageType->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  messageType"); return false ;}
        if (false == mpMessageType->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : messageType"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("messageType") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : messageType"); return false; }}
    if (true == deserializer.hasProperty("text"))
    {
        mpText = new ::String500();
        if (mpText == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  text"); return false;}
        if (false == deserializer.getPropertyValue("text", &(mpText->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  text"); return false ;}
        if (false == mpText->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : text"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("text") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : text"); return false; }}
    if (true == deserializer.hasGroupProperty("Image"))
    {
        mpImage = new ::Image();
        if (mpImage == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Image"); return false;}
        if (false == mpImage->deSerialize(deserializer)) {return false;}
    }


    return true ;
}

std::string
Message::getElementName()
{
    return EN_MESSAGE;
}

bool
Message::clone(::Message *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::IdentifiedObject::clone(pSrcObject)) {return false;}

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
        if ( NULL != mpDuration) { delete mpDuration; mpDuration = NULL; }

        if ( NULL != pSrcObject->mpDuration )
        {
            mpDuration = new OC::Cm::Serialization::Xsd::TimeType();
            if (mpDuration == NULL ) {return false;}
            mpDuration->value = (pSrcObject->mpDuration->value );
        }
    }
    {
        if ( NULL != mpPriority) { delete mpPriority; mpPriority = NULL; }

        if ( NULL != pSrcObject->mpPriority )
        {
            mpPriority = new uint8_t();
            if (mpPriority == NULL ) {return false;}
            *mpPriority = *(pSrcObject->mpPriority );
        }
    }
    {
        if ( NULL != mpWriter) { delete mpWriter; mpWriter = NULL; }

        if ( NULL != pSrcObject->mpWriter )
        {
            mpWriter = new ::String20();
            if (mpWriter == NULL ) {return false;}
            mpWriter->value = (pSrcObject->mpWriter->value );
        }
    }
    {
        if ( NULL != mpTitle) { delete mpTitle; mpTitle = NULL; }

        if ( NULL != pSrcObject->mpTitle )
        {
            mpTitle = new ::String20();
            if (mpTitle == NULL ) {return false;}
            mpTitle->value = (pSrcObject->mpTitle->value );
        }
    }
    {
        if ( NULL != mpMessageType) { delete mpMessageType; mpMessageType = NULL; }

        if ( NULL != pSrcObject->mpMessageType )
        {
            mpMessageType = new ::MessageType();
            if (mpMessageType == NULL ) {return false;}
            mpMessageType->value = (pSrcObject->mpMessageType->value );
        }
    }
    {
        if ( NULL != mpText) { delete mpText; mpText = NULL; }

        if ( NULL != pSrcObject->mpText )
        {
            mpText = new ::String500();
            if (mpText == NULL ) {return false;}
            mpText->value = (pSrcObject->mpText->value );
        }
    }
    {
        if ( NULL != mpImage) { delete mpImage; mpImage = NULL; }

        if ( NULL != pSrcObject->mpImage )
        {
            mpImage = new ::Image();
            if (mpImage == NULL ) { return false;}
            if (false == mpImage->clone(pSrcObject->mpImage)) {return false;} ;
        }
    }


    return true ;
}
