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



#include "Capability.h"


#include "Elements.h"

using namespace std;

Capability::Capability()
{

    mpSubscriptionsLink = NULL;
    mpGroupsLink = NULL;
    mpRecordsLink = NULL;
    mpPatternsLink = NULL;
    mpEnergyLink = NULL;
    mpDevicesLink = NULL;
    mpActionsLink = NULL;
    mpFoodManagerLink = NULL;
    mpNotificationsLink = NULL;
    mpLongPollingNotificationLink = NULL;

}

Capability::~Capability()
{
    if ( NULL != mpSubscriptionsLink) { delete mpSubscriptionsLink; mpSubscriptionsLink = NULL; }
    if ( NULL != mpGroupsLink) { delete mpGroupsLink; mpGroupsLink = NULL; }
    if ( NULL != mpRecordsLink) { delete mpRecordsLink; mpRecordsLink = NULL; }
    if ( NULL != mpPatternsLink) { delete mpPatternsLink; mpPatternsLink = NULL; }
    if ( NULL != mpEnergyLink) { delete mpEnergyLink; mpEnergyLink = NULL; }
    if ( NULL != mpDevicesLink) { delete mpDevicesLink; mpDevicesLink = NULL; }
    if ( NULL != mpActionsLink) { delete mpActionsLink; mpActionsLink = NULL; }
    if ( NULL != mpFoodManagerLink) { delete mpFoodManagerLink; mpFoodManagerLink = NULL; }
    if ( NULL != mpNotificationsLink) { delete mpNotificationsLink; mpNotificationsLink = NULL; }
    if ( NULL != mpLongPollingNotificationLink) { delete mpLongPollingNotificationLink; mpLongPollingNotificationLink = NULL; }

}

bool
Capability::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpSubscriptionsLink )
        {
            bRet = mpSubscriptionsLink->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("SubscriptionsLink") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : SubscriptionsLink"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpGroupsLink )
        {
            bRet = mpGroupsLink->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("GroupsLink") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : GroupsLink"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpRecordsLink )
        {
            bRet = mpRecordsLink->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("RecordsLink") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : RecordsLink"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpPatternsLink )
        {
            bRet = mpPatternsLink->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("PatternsLink") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : PatternsLink"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpEnergyLink )
        {
            bRet = mpEnergyLink->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("EnergyLink") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : EnergyLink"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpDevicesLink )
        {
            bRet = mpDevicesLink->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("DevicesLink") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : DevicesLink"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpActionsLink )
        {
            bRet = mpActionsLink->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("ActionsLink") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : ActionsLink"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpFoodManagerLink )
        {
            bRet = mpFoodManagerLink->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("FoodManagerLink") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : FoodManagerLink"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpNotificationsLink )
        {
            bRet = mpNotificationsLink->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("NotificationsLink") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : NotificationsLink"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpLongPollingNotificationLink )
        {
            bRet = mpLongPollingNotificationLink->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("LongPollingNotificationLink") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : LongPollingNotificationLink"); return false; }}
    }


    return true ;
}

bool
Capability::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasGroupProperty("SubscriptionsLink"))
    {
        if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  SubscriptionsLink"); return false;}
        mpSubscriptionsLink = new ::SubscriptionsLink();
        if (mpSubscriptionsLink == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  SubscriptionsLink"); return false;}
        if (false == mpSubscriptionsLink->deSerialize(deserializer)) {return false;}
    }
    else { if ((deserializer.isInFilteringMode("SubscriptionsLink") == false) ) { deserializer.setErrorMessage("Failed To deserialize :  SubscriptionsLink"); return false; }}
    if (true == deserializer.hasGroupProperty("GroupsLink"))
    {
        if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  GroupsLink"); return false;}
        mpGroupsLink = new ::GroupsLink();
        if (mpGroupsLink == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  GroupsLink"); return false;}
        if (false == mpGroupsLink->deSerialize(deserializer)) {return false;}
    }
    if (true == deserializer.hasGroupProperty("RecordsLink"))
    {
        if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  RecordsLink"); return false;}
        mpRecordsLink = new ::RecordsLink();
        if (mpRecordsLink == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  RecordsLink"); return false;}
        if (false == mpRecordsLink->deSerialize(deserializer)) {return false;}
    }
    if (true == deserializer.hasGroupProperty("PatternsLink"))
    {
        if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  PatternsLink"); return false;}
        mpPatternsLink = new ::PatternsLink();
        if (mpPatternsLink == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  PatternsLink"); return false;}
        if (false == mpPatternsLink->deSerialize(deserializer)) {return false;}
    }
    if (true == deserializer.hasGroupProperty("EnergyLink"))
    {
        if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  EnergyLink"); return false;}
        mpEnergyLink = new ::EnergyLink();
        if (mpEnergyLink == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  EnergyLink"); return false;}
        if (false == mpEnergyLink->deSerialize(deserializer)) {return false;}
    }
    if (true == deserializer.hasGroupProperty("DevicesLink"))
    {
        if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  DevicesLink"); return false;}
        mpDevicesLink = new ::DevicesLink();
        if (mpDevicesLink == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  DevicesLink"); return false;}
        if (false == mpDevicesLink->deSerialize(deserializer)) {return false;}
    }
    else { if ((deserializer.isInFilteringMode("DevicesLink") == false) ) { deserializer.setErrorMessage("Failed To deserialize :  DevicesLink"); return false; }}
    if (true == deserializer.hasGroupProperty("ActionsLink"))
    {
        if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  ActionsLink"); return false;}
        mpActionsLink = new ::ActionsLink();
        if (mpActionsLink == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  ActionsLink"); return false;}
        if (false == mpActionsLink->deSerialize(deserializer)) {return false;}
    }
    if (true == deserializer.hasGroupProperty("FoodManagerLink"))
    {
        if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  FoodManagerLink"); return false;}
        mpFoodManagerLink = new ::FoodManagerLink();
        if (mpFoodManagerLink == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  FoodManagerLink"); return false;}
        if (false == mpFoodManagerLink->deSerialize(deserializer)) {return false;}
    }
    if (true == deserializer.hasGroupProperty("NotificationsLink"))
    {
        if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  NotificationsLink"); return false;}
        mpNotificationsLink = new ::NotificationsLink();
        if (mpNotificationsLink == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  NotificationsLink"); return false;}
        if (false == mpNotificationsLink->deSerialize(deserializer)) {return false;}
    }
    if (true == deserializer.hasGroupProperty("LongPollingNotificationLink"))
    {
        if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  LongPollingNotificationLink"); return false;}
        mpLongPollingNotificationLink = new ::LongPollingNotificationLink();
        if (mpLongPollingNotificationLink == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  LongPollingNotificationLink"); return false;}
        if (false == mpLongPollingNotificationLink->deSerialize(deserializer)) {return false;}
    }


    return true ;
}

std::string
Capability::getElementName()
{
    return EN_CAPABILITY;
}

bool
Capability::clone(::Capability *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpSubscriptionsLink) { delete mpSubscriptionsLink; mpSubscriptionsLink = NULL; }

        if ( NULL != pSrcObject->mpSubscriptionsLink )
        {
            mpSubscriptionsLink = new ::SubscriptionsLink();
            if (mpSubscriptionsLink == NULL ) { return false;}
            if (false == mpSubscriptionsLink->clone(pSrcObject->mpSubscriptionsLink)) {return false;} ;
        }
    }
    {
        if ( NULL != mpGroupsLink) { delete mpGroupsLink; mpGroupsLink = NULL; }

        if ( NULL != pSrcObject->mpGroupsLink )
        {
            mpGroupsLink = new ::GroupsLink();
            if (mpGroupsLink == NULL ) { return false;}
            if (false == mpGroupsLink->clone(pSrcObject->mpGroupsLink)) {return false;} ;
        }
    }
    {
        if ( NULL != mpRecordsLink) { delete mpRecordsLink; mpRecordsLink = NULL; }

        if ( NULL != pSrcObject->mpRecordsLink )
        {
            mpRecordsLink = new ::RecordsLink();
            if (mpRecordsLink == NULL ) { return false;}
            if (false == mpRecordsLink->clone(pSrcObject->mpRecordsLink)) {return false;} ;
        }
    }
    {
        if ( NULL != mpPatternsLink) { delete mpPatternsLink; mpPatternsLink = NULL; }

        if ( NULL != pSrcObject->mpPatternsLink )
        {
            mpPatternsLink = new ::PatternsLink();
            if (mpPatternsLink == NULL ) { return false;}
            if (false == mpPatternsLink->clone(pSrcObject->mpPatternsLink)) {return false;} ;
        }
    }
    {
        if ( NULL != mpEnergyLink) { delete mpEnergyLink; mpEnergyLink = NULL; }

        if ( NULL != pSrcObject->mpEnergyLink )
        {
            mpEnergyLink = new ::EnergyLink();
            if (mpEnergyLink == NULL ) { return false;}
            if (false == mpEnergyLink->clone(pSrcObject->mpEnergyLink)) {return false;} ;
        }
    }
    {
        if ( NULL != mpDevicesLink) { delete mpDevicesLink; mpDevicesLink = NULL; }

        if ( NULL != pSrcObject->mpDevicesLink )
        {
            mpDevicesLink = new ::DevicesLink();
            if (mpDevicesLink == NULL ) { return false;}
            if (false == mpDevicesLink->clone(pSrcObject->mpDevicesLink)) {return false;} ;
        }
    }
    {
        if ( NULL != mpActionsLink) { delete mpActionsLink; mpActionsLink = NULL; }

        if ( NULL != pSrcObject->mpActionsLink )
        {
            mpActionsLink = new ::ActionsLink();
            if (mpActionsLink == NULL ) { return false;}
            if (false == mpActionsLink->clone(pSrcObject->mpActionsLink)) {return false;} ;
        }
    }
    {
        if ( NULL != mpFoodManagerLink) { delete mpFoodManagerLink; mpFoodManagerLink = NULL; }

        if ( NULL != pSrcObject->mpFoodManagerLink )
        {
            mpFoodManagerLink = new ::FoodManagerLink();
            if (mpFoodManagerLink == NULL ) { return false;}
            if (false == mpFoodManagerLink->clone(pSrcObject->mpFoodManagerLink)) {return false;} ;
        }
    }
    {
        if ( NULL != mpNotificationsLink) { delete mpNotificationsLink; mpNotificationsLink = NULL; }

        if ( NULL != pSrcObject->mpNotificationsLink )
        {
            mpNotificationsLink = new ::NotificationsLink();
            if (mpNotificationsLink == NULL ) { return false;}
            if (false == mpNotificationsLink->clone(pSrcObject->mpNotificationsLink)) {return false;} ;
        }
    }
    {
        if ( NULL != mpLongPollingNotificationLink) { delete mpLongPollingNotificationLink; mpLongPollingNotificationLink = NULL; }

        if ( NULL != pSrcObject->mpLongPollingNotificationLink )
        {
            mpLongPollingNotificationLink = new ::LongPollingNotificationLink();
            if (mpLongPollingNotificationLink == NULL ) { return false;}
            if (false == mpLongPollingNotificationLink->clone(
                    pSrcObject->mpLongPollingNotificationLink)) {return false;} ;
        }
    }


    return true ;
}
