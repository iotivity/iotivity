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
    mpChoiceItenms.push_back("Capability");
    mpChoiceItenms.push_back("Devices");
    mpChoiceItenms.push_back("Device");
    mpChoiceItenms.push_back("Operation");
    mpChoiceItenms.push_back("Mode");
    mpChoiceItenms.push_back("Movement");
    mpChoiceItenms.push_back("Temperatures");
    mpChoiceItenms.push_back("Temperature");
    mpChoiceItenms.push_back("Doors");
    mpChoiceItenms.push_back("Door");
    mpChoiceItenms.push_back("humidity");
    mpChoiceItenms.push_back("Wind");
    mpChoiceItenms.push_back("Light");
    mpChoiceItenms.push_back("Washer");
    mpChoiceItenms.push_back("Fridge");
    mpChoiceItenms.push_back("Oven");
    mpChoiceItenms.push_back("Audio");
    mpChoiceItenms.push_back("AVSources");
    mpChoiceItenms.push_back("AVSource");
    mpChoiceItenms.push_back("Subscriptions");
    mpChoiceItenms.push_back("Subscription");
    mpChoiceItenms.push_back("Information");
    mpChoiceItenms.push_back("Versions");
    mpChoiceItenms.push_back("Version");
    mpChoiceItenms.push_back("Configuration");
    mpChoiceItenms.push_back("Time");
    mpChoiceItenms.push_back("Remote");
    mpChoiceItenms.push_back("Networks");
    mpChoiceItenms.push_back("Network");
    mpChoiceItenms.push_back("WiFi");
    mpChoiceItenms.push_back("AccessPoints");
    mpChoiceItenms.push_back("AccessPoint");
    mpChoiceItenms.push_back("EnergyConsumption");
    mpChoiceItenms.push_back("Alarms");
    mpChoiceItenms.push_back("Alarm");
    mpChoiceItenms.push_back("Level");
    mpChoiceItenms.push_back("Messages");
    mpChoiceItenms.push_back("Message");
    mpChoiceItenms.push_back("Detection");
    mpChoiceItenms.push_back("Camera");
    mpChoiceItenms.push_back("Captures");
    mpChoiceItenms.push_back("Capture");
    mpChoiceItenms.push_back("Groups");
    mpChoiceItenms.push_back("Group");
    mpChoiceItenms.push_back("Actions");
    mpChoiceItenms.push_back("Action");
    mpChoiceItenms.push_back("Patterns");
    mpChoiceItenms.push_back("Pattern");
    mpChoiceItenms.push_back("Records");
    mpChoiceItenms.push_back("Record");
    mpChoiceItenms.push_back("Energy");
    mpChoiceItenms.push_back("UsageLimits");
    mpChoiceItenms.push_back("UsageLimit");
    mpChoiceItenms.push_back("FoodManager");
    mpChoiceItenms.push_back("FoodList");
    mpChoiceItenms.push_back("Food");
    mpChoiceItenms.push_back("ShoppingList");
    mpChoiceItenms.push_back("Shopping");
    mpChoiceItenms.push_back("FoodImages");
    mpChoiceItenms.push_back("FoodImage");

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
            bRet = false; for (std::list<std::string>::iterator itr = mpChoiceItenms.begin(); itr != mpChoiceItenms.end(); itr++  ) { if (mpChangedResource->getElementName().compare(*itr) == 0) {bRet = true; break;}} if (bRet)
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
        if (false == deserializer.getPropertyValue("event", &(mpEvent->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  event"); return false ;}
        if (false == mpEvent->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : event"); return false ;}
    }
    if (true == deserializer.hasProperty("eventTime"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  eventTime"); return false;}
        mpEventTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpEventTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  eventTime"); return false;}
        if (false == deserializer.getPropertyValue("eventTime", &(mpEventTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  eventTime"); return false ;}
        if (false == mpEventTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : eventTime"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("eventTime") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : eventTime"); return false; }}
    for (std::list<std::string>::iterator itr = mpChoiceItenms.begin(); itr != mpChoiceItenms.end(); itr++  )
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
