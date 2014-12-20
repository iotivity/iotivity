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



#include "Subscription.h"


#include "Elements.h"

using namespace std;

Subscription::Subscription()
{

    mpUuid = NULL;
    mpResourceURIs = NULL;
    mpNotificationURI = NULL;
    mpTimeout = NULL;
    mpDuration = NULL;

}

Subscription::~Subscription()
{
    if ( NULL != mpUuid) { delete mpUuid; mpUuid = NULL; }
    if ( NULL != mpResourceURIs) { delete mpResourceURIs; mpResourceURIs = NULL; }
    if ( NULL != mpNotificationURI) { delete mpNotificationURI; mpNotificationURI = NULL; }
    if ( NULL != mpTimeout) { delete mpTimeout; mpTimeout = NULL; }
    if ( NULL != mpDuration) { delete mpDuration; mpDuration = NULL; }

}

bool
Subscription::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::IdentifiedObject::serializeContents(serializer)) { return false;}

    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpUuid )
        {
            if (mpUuid->value.length() > 0)
            {
                if (true == mpUuid->validateContent())
                {
                    bRet = serializer.setPropertyValue("uuid", mpUuid->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("uuid") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : uuid"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpResourceURIs )
        {
            mpResourceURIs->setInstanceName("resourceURIs");
            bRet = mpResourceURIs->serializeContents(serializer) ;
        }
        if (false == bRet) { if (serializer.isInFilteringMode("resourceURIs") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : resourceURIs"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpNotificationURI )
        {
            if (mpNotificationURI->length() > 0)
            {
                bRet = serializer.setPropertyValue("notificationURI", *mpNotificationURI);
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("notificationURI") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : notificationURI"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpTimeout )
        {
            bRet = serializer.setPropertyValue("timeout", *mpTimeout);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("timeout") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : timeout"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpDuration )
        {
            bRet = serializer.setPropertyValue("duration", *mpDuration);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("duration") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : duration"); return false; }}
    }


    return true ;
}

bool
Subscription::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::IdentifiedObject::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("uuid"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  uuid"); return false;}
        mpUuid = new OC::Cm::Serialization::Xsd::UuidType();
        if (mpUuid == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  uuid"); return false;}
        if (false == deserializer.getPropertyValue("uuid", &(mpUuid->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  uuid"); return false ;}
        if (false == mpUuid->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : uuid"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("uuid") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : uuid"); return false; }}
    {
        if (true == deserializer.hasProperty("resourceURIs"))
        {
            mpResourceURIs = new ::ArrayOfAnyURI();
            if (mpResourceURIs == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  resourceURIs"); return false;}
            mpResourceURIs->setInstanceName("resourceURIs");
            if (false == mpResourceURIs->deSerializeContents(deserializer)) {  deserializer.setErrorMessage("Parameter Contains Invalid Value : resourceURIs"); return false ;}
        }
        else { if (deserializer.isInFilteringMode("resourceURIs") == false) { deserializer.setErrorMessage("Mandatory Parameter not found :  resourceURIs"); return false; }}
    }
    if (true == deserializer.hasProperty("notificationURI"))
    {
        mpNotificationURI = new std::string();
        if (mpNotificationURI == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  notificationURI"); return false;}
        if (false == deserializer.getPropertyValue("notificationURI", mpNotificationURI)) { deserializer.setErrorMessage("Failed to deserialize parameter :  notificationURI"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("notificationURI") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : notificationURI"); return false; }}
    if (true == deserializer.hasProperty("timeout"))
    {
        mpTimeout = new uint8_t();
        if (mpTimeout == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  timeout"); return false;}
        if (false == deserializer.getPropertyValue("timeout", mpTimeout)) { deserializer.setErrorMessage("Failed to deserialize parameter :  timeout"); return false ;}
    }
    if (true == deserializer.hasProperty("duration"))
    {
        mpDuration = new uint8_t();
        if (mpDuration == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  duration"); return false;}
        if (false == deserializer.getPropertyValue("duration", mpDuration)) { deserializer.setErrorMessage("Failed to deserialize parameter :  duration"); return false ;}
    }


    return true ;
}

std::string
Subscription::getElementName()
{
    return EN_SUBSCRIPTION;
}

bool
Subscription::clone(::Subscription *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::IdentifiedObject::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpUuid) { delete mpUuid; mpUuid = NULL; }

        if ( NULL != pSrcObject->mpUuid )
        {
            mpUuid = new OC::Cm::Serialization::Xsd::UuidType();
            if (mpUuid == NULL ) {return false;}
            mpUuid->value = (pSrcObject->mpUuid->value );
        }
    }
    {
        if ( NULL != mpResourceURIs) { delete mpResourceURIs; mpResourceURIs = NULL; }

        if ( NULL != pSrcObject->mpResourceURIs )
        {
            mpResourceURIs = new ::ArrayOfAnyURI();
            if (mpResourceURIs == NULL ) { return false;}
            mpResourceURIs->members = pSrcObject->mpResourceURIs->members;
        }
    }
    {
        if ( NULL != mpNotificationURI) { delete mpNotificationURI; mpNotificationURI = NULL; }

        if ( NULL != pSrcObject->mpNotificationURI )
        {
            mpNotificationURI = new std::string();
            if (mpNotificationURI == NULL ) {return false;}
            *mpNotificationURI = *(pSrcObject->mpNotificationURI );
        }
    }
    {
        if ( NULL != mpTimeout) { delete mpTimeout; mpTimeout = NULL; }

        if ( NULL != pSrcObject->mpTimeout )
        {
            mpTimeout = new uint8_t();
            if (mpTimeout == NULL ) {return false;}
            *mpTimeout = *(pSrcObject->mpTimeout );
        }
    }
    {
        if ( NULL != mpDuration) { delete mpDuration; mpDuration = NULL; }

        if ( NULL != pSrcObject->mpDuration )
        {
            mpDuration = new uint8_t();
            if (mpDuration == NULL ) {return false;}
            *mpDuration = *(pSrcObject->mpDuration );
        }
    }


    return true ;
}
