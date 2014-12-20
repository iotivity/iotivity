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



#include "Events.h"


#include "Elements.h"

using namespace std;

Events::Events()
{

    mpUuid = NULL;
    mpResourceURI = NULL;
    mpEvent = NULL;
    mpEventTime = NULL;
    mpChangedResource = NULL ;
    mpChoiceItems.push_back("Capability");
    mpChoiceItems.push_back("Devices");
    mpChoiceItems.push_back("Device");
    mpChoiceItems.push_back("Operation");
    mpChoiceItems.push_back("Mode");
    mpChoiceItems.push_back("Movement");
    mpChoiceItems.push_back("Temperatures");
    mpChoiceItems.push_back("Temperature");
    mpChoiceItems.push_back("Doors");
    mpChoiceItems.push_back("Door");
    mpChoiceItems.push_back("humidity");
    mpChoiceItems.push_back("Wind");
    mpChoiceItems.push_back("Light");
    mpChoiceItems.push_back("Washer");
    mpChoiceItems.push_back("Fridge");
    mpChoiceItems.push_back("Oven");
    mpChoiceItems.push_back("Audio");
    mpChoiceItems.push_back("AVSources");
    mpChoiceItems.push_back("AVSource");
    mpChoiceItems.push_back("Subscriptions");
    mpChoiceItems.push_back("Subscription");
    mpChoiceItems.push_back("Information");
    mpChoiceItems.push_back("Versions");
    mpChoiceItems.push_back("Version");
    mpChoiceItems.push_back("Configuration");
    mpChoiceItems.push_back("Time");
    mpChoiceItems.push_back("Remote");
    mpChoiceItems.push_back("Networks");
    mpChoiceItems.push_back("Network");
    mpChoiceItems.push_back("WiFi");
    mpChoiceItems.push_back("AccessPoints");
    mpChoiceItems.push_back("AccessPoint");
    mpChoiceItems.push_back("EnergyConsumption");
    mpChoiceItems.push_back("Alarms");
    mpChoiceItems.push_back("Alarm");
    mpChoiceItems.push_back("Level");
    mpChoiceItems.push_back("Messages");
    mpChoiceItems.push_back("Message");
    mpChoiceItems.push_back("Detection");
    mpChoiceItems.push_back("Camera");
    mpChoiceItems.push_back("Captures");
    mpChoiceItems.push_back("Capture");
    mpChoiceItems.push_back("Groups");
    mpChoiceItems.push_back("Group");
    mpChoiceItems.push_back("Actions");
    mpChoiceItems.push_back("Action");
    mpChoiceItems.push_back("Patterns");
    mpChoiceItems.push_back("Pattern");
    mpChoiceItems.push_back("Records");
    mpChoiceItems.push_back("Record");
    mpChoiceItems.push_back("Energy");
    mpChoiceItems.push_back("UsageLimits");
    mpChoiceItems.push_back("UsageLimit");
    mpChoiceItems.push_back("FoodManager");
    mpChoiceItems.push_back("FoodList");
    mpChoiceItems.push_back("Food");
    mpChoiceItems.push_back("ShoppingList");
    mpChoiceItems.push_back("Shopping");
    mpChoiceItems.push_back("FoodImages");
    mpChoiceItems.push_back("FoodImage");

}

Events::~Events()
{
    if ( NULL != mpUuid) { delete mpUuid; mpUuid = NULL; }
    if ( NULL != mpResourceURI) { delete mpResourceURI; mpResourceURI = NULL; }
    if ( NULL != mpEvent) { delete mpEvent; mpEvent = NULL; }
    if ( NULL != mpEventTime) { delete mpEventTime; mpEventTime = NULL; }
    if ( NULL != mpChangedResource) { delete mpChangedResource; mpChangedResource = NULL; }

}

bool
Events::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
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
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpResourceURI )
        {
            if (mpResourceURI->length() > 0)
            {
                bRet = serializer.setPropertyValue("resourceURI", *mpResourceURI);
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("resourceURI") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : resourceURI"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpEvent )
        {
            if (mpEvent->value.length() > 0)
            {
                if (true == mpEvent->validateContent())
                {
                    bRet = serializer.setPropertyValue("event", mpEvent->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("event") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : event"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpEventTime )
        {
            if (mpEventTime->value.length() > 0)
            {
                if (true == mpEventTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("eventTime", mpEventTime->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("eventTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : eventTime"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpChangedResource )
        {
            bRet = false;
            for (std::list<std::string>::iterator itr = mpChoiceItems.begin(); itr != mpChoiceItems.end();
                 itr++  ) { if (mpChangedResource->getElementName().compare(*itr) == 0) {bRet = true; break;}} if (
                     bRet)
            {
                bRet = mpChangedResource->serialize(serializer);
            }
            else {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : null"); return false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("null") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : null"); return false; }}
    }


    return true ;
}

bool
Events::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (true == deserializer.hasProperty("uuid"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  uuid"); return false;}
        mpUuid = new OC::Cm::Serialization::Xsd::UuidType();
        if (mpUuid == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  uuid"); return false;}
        if (false == deserializer.getPropertyValue("uuid", &(mpUuid->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  uuid"); return false ;}
        if (false == mpUuid->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : uuid"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("uuid") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : uuid"); return false; }}
    if (true == deserializer.hasProperty("resourceURI"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  resourceURI"); return false;}
        mpResourceURI = new std::string();
        if (mpResourceURI == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  resourceURI"); return false;}
        if (false == deserializer.getPropertyValue("resourceURI", mpResourceURI)) { deserializer.setErrorMessage("Failed to deserialize parameter :  resourceURI"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("resourceURI") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : resourceURI"); return false; }}
    if (true == deserializer.hasProperty("event"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  event"); return false;}
        mpEvent = new ::NotificationEventType();
        if (mpEvent == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  event"); return false;}
        if (false == deserializer.getPropertyValue("event",
                &(mpEvent->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  event"); return false ;}
        if (false == mpEvent->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : event"); return false ;}
    }
    if (true == deserializer.hasProperty("eventTime"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  eventTime"); return false;}
        mpEventTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpEventTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  eventTime"); return false;}
        if (false == deserializer.getPropertyValue("eventTime",
                &(mpEventTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  eventTime"); return false ;}
        if (false == mpEventTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : eventTime"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("eventTime") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : eventTime"); return false; }}
    for (std::list<std::string>::iterator itr = mpChoiceItems.begin(); itr != mpChoiceItems.end();
         itr++  )
    {
        if (true == deserializer.hasGroupProperty(*itr))
        {
            mpChangedResource = deserializer.createElement(*itr);
            if (mpChangedResource == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter : "); return false;}
            if (false == mpChangedResource->deSerialize(deserializer)) {return false;}
            break;
        }
    }


    return true ;
}

std::string
Events::getElementName()
{
    return EN_EVENTS;
}

bool
Events::clone(::Events *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

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
        if ( NULL != mpResourceURI) { delete mpResourceURI; mpResourceURI = NULL; }

        if ( NULL != pSrcObject->mpResourceURI )
        {
            mpResourceURI = new std::string();
            if (mpResourceURI == NULL ) {return false;}
            *mpResourceURI = *(pSrcObject->mpResourceURI );
        }
    }
    {
        if ( NULL != mpEvent) { delete mpEvent; mpEvent = NULL; }

        if ( NULL != pSrcObject->mpEvent )
        {
            mpEvent = new ::NotificationEventType();
            if (mpEvent == NULL ) {return false;}
            mpEvent->value = (pSrcObject->mpEvent->value );
        }
    }
    {
        if ( NULL != mpEventTime) { delete mpEventTime; mpEventTime = NULL; }

        if ( NULL != pSrcObject->mpEventTime )
        {
            mpEventTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (mpEventTime == NULL ) {return false;}
            mpEventTime->value = (pSrcObject->mpEventTime->value );
        }
    }
    {}


    return true ;
}
