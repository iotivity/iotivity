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

#include "ControlDevice.h"
#include "DeviceType.h"
#include "DeviceDomain.h"
#include "Resource.h"
#include "Elements.h"
#include "log.h"


ControlDevice::ControlDevice(   OC::Cm::DeviceType type,
                                OC::Cm::DeviceDomain domain )
    : OC::Cm::Device(type, domain)
{
    setDeviceType(type);
    setDomain(domain);
}


list<std::string>
ControlDevice::getSupportedResourceTypes()
{
    return m_resourceTypes ;
}

/**
OC::Cm::Client::Resource::Resource*
ControlDevice::getResource( std::string resourceType )
{
    for( std::list<std::string>::const_iterator itr = m_resourceTypes.begin(); itr != m_resourceTypes.end(); itr++ )
    {
        if ( resourceType.compare(*itr) == 0)
        {
            return createResource(resourceType);
        }
    }

    return NULL ;
}
**/

bool
ControlDevice::getDeviceDescription( OC::Cm::Serialization::ISerializable **serializable )
{
    return false;
}

std::string
ControlDevice::getSubscriptionsLink()
{
    return m_subscriptionsLink ;
}

bool
ControlDevice::loadDeviceDescription(OC::Cm::Serialization::ISerializable &serializable)
{
    CE(cout << "ControlDevice::loadDeviceDescription => called" << endl);
    std::string elementName = serializable.getElementName();

    if (0 == elementName.compare(EN_CAPABILITY))
    {
        m_capabilities = (Capability &) serializable ;

        if ((m_capabilities.mpDevicesLink == NULL) || (m_capabilities.mpDevicesLink->mpHref == NULL))
        {
            return false ;
        }

        setDeviceDescriptionUri(m_capabilities.mpDevicesLink->mpHref->c_str());

        if ((m_capabilities.mpSubscriptionsLink == NULL) || (m_capabilities.mpSubscriptionsLink->mpHref == NULL))
        {
            return false ;
        }

        if (m_capabilities.mpSubscriptionsLink->mpHref->empty() == false)
        {
            m_subscriptionsLink = *(m_capabilities.mpSubscriptionsLink->mpHref) ;
        }

        m_device_description_loaded = false ;

        return true ;
    }
    else if (0 == elementName.compare(EN_DEVICES))
    {
        ::Devices devices = (::Devices &) serializable ;

        // load  Device[0]....
        if ((devices.mpDevice.size() > 0))
        {
            ::Device *device = devices.mpDevice.front();
            if (( NULL != device ) && ( NULL != device->mpResources))
            {
                for ( std::list< ::ResourceType >::const_iterator itr = device->mpResources->members.begin(); itr != device->mpResources->members.end(); itr++ )
                {
                    m_resourceTypes.push_back( (*itr).value );
                }
            }

            m_device_description_loaded = true ;
            return true ;
        }
    }

    return false;
}

/**
OC::Cm::Client::Resource::Resource*
ControlDevice::createResource(std::string& resourceType)
{
    std::string address     = getAddress() ;
    std::string protocol    = getProtocol() ;

if (resourceType.compare("AccessPoint")){
return new ::AccessPointResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("AccessPoints")){
return new ::AccessPointsResource(getContext() , protocol, address) ;
}

#if 0 // OIC IGNORE

else if (resourceType.compare("AVSource")){
return new ::AVSourceResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("AVSources")){
return new ::AVSourcesResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Action")){
return new ::ActionResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Actions")){
return new ::ActionsResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Audio")){
return new ::AudioResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Camera")){
return new ::CameraResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Capture")){
return new ::CaptureResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Captures")){
return new ::CapturesResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("CustomRecipe")){
return new ::CustomRecipeResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("CustomRecipes")){
return new ::CustomRecipesResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("DefrostReservation")){
return new ::DefrostReservationResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("DefrostReservations")){
return new ::DefrostReservationsResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Food")){
return new ::FoodResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("FoodImage")){
return new ::FoodImageResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("FoodImages")){
return new ::FoodImagesResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("FoodList")){
return new ::FoodListResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("FoodManager")){
return new ::FoodManagerResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Fridge")){
return new ::FridgeResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Group")){
return new ::GroupResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Groups")){
return new ::GroupsResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Oven")){
return new ::OvenResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Pattern")){
return new ::PatternResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Patterns")){
return new ::PatternsResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Shopping")){
return new ::ShoppingResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("ShoppingList")){
return new ::ShoppingListResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Washer")){
return new ::WasherResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Wind")){
return new ::WindResource(getContext() , protocol, address) ;
}
#endif

else if (resourceType.compare("Alarm")){
return new ::AlarmResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Alarms")){
return new ::AlarmsResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Capability")){
return new ::CapabilityResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Configuration")){
return new ::ConfigurationResource(getContext() , protocol, address) ;
}

else if (resourceType.compare("DRLC")){
return new ::DRLCResource(getContext() , protocol, address) ;
}

else if (resourceType.compare("Detection")){
return new ::DetectionResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Device")){
return new ::DeviceResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Devices")){
return new ::DevicesResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Door")){
return new ::DoorResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Doors")){
return new ::DoorsResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Energy")){
return new ::EnergyResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("EnergyConsumption")){
return new ::EnergyConsumptionResource(getContext() , protocol, address) ;
}

else if (resourceType.compare("Humidity")){
return new ::HumidityResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Information")){
return new ::InformationResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Level")){
return new ::LevelResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Light")){
return new ::LightResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("LongPollingNotification")){
return new ::LongPollingNotificationResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Message")){
return new ::MessageResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Messages")){
return new ::MessagesResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Mode")){
return new ::ModeResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Movement")){
return new ::MovementResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Network")){
return new ::NetworkResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Networks")){
return new ::NetworksResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Notification")){
return new ::NotificationResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Notifications")){
return new ::NotificationsResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Operation")){
return new ::OperationResource(getContext() , protocol, address) ;
}

else if (resourceType.compare("Record")){
return new ::RecordResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Records")){
return new ::RecordsResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Remote")){
return new ::RemoteResource(getContext() , protocol, address) ;
}

else if (resourceType.compare("Subscription")){
return new ::SubscriptionResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Subscriptions")){
return new ::SubscriptionsResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Temperature")){
return new ::TemperatureResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Temperatures")){
return new ::TemperaturesResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Thermostat")){
return new ::ThermostatResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Time")){
return new ::TimeResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("UsageLimit")){
return new ::UsageLimitResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("UsageLimits")){
return new ::UsageLimitsResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Usages")){
return new ::UsagesResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Version")){
return new ::VersionResource(getContext() , protocol, address) ;
}
else if (resourceType.compare("Versions")){
return new ::VersionsResource(getContext() , protocol, address) ;
}

else if (resourceType.compare("WiFi")){
return new ::WiFiResource(getContext() , protocol, address) ;
}



    return NULL ;
}

**/
