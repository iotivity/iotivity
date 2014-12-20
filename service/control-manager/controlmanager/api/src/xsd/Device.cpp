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



#include "Device.h"


#include "Elements.h"

using namespace std;

Device::Device()
{

    mpUuid = NULL;
    mpName = NULL;
    mpType = NULL;
    mpResources = NULL;
    mpConnected = NULL;

    mpMode = NULL;

    mpInformationLink = NULL;
    mpMessagesLink = NULL;
    mpMovement = NULL;
    mpLight = NULL;
    mpOperation = NULL;
    mpHumidity = NULL;

    mpDoors = NULL;
    mpTemperatures = NULL;
    mpConfigurationLink = NULL;

    mpDetection = NULL;
    mpLevel = NULL;
    mpAlarms = NULL;
    mpEnergyConsumption = NULL;

    mpDRLC = NULL;
    mpThermostat = NULL;

}

Device::~Device()
{
    if ( NULL != mpUuid) { delete mpUuid; mpUuid = NULL; }
    if ( NULL != mpName) { delete mpName; mpName = NULL; }
    if ( NULL != mpType) { delete mpType; mpType = NULL; }
    if ( NULL != mpResources) { delete mpResources; mpResources = NULL; }
    if ( NULL != mpConnected) { delete mpConnected; mpConnected = NULL; }

    if ( NULL != mpMode) { delete mpMode; mpMode = NULL; }

    if ( NULL != mpInformationLink) { delete mpInformationLink; mpInformationLink = NULL; }
    if ( NULL != mpMessagesLink) { delete mpMessagesLink; mpMessagesLink = NULL; }
    if ( NULL != mpMovement) { delete mpMovement; mpMovement = NULL; }
    if ( NULL != mpLight) { delete mpLight; mpLight = NULL; }
    if ( NULL != mpOperation) { delete mpOperation; mpOperation = NULL; }
    if ( NULL != mpHumidity) { delete mpHumidity; mpHumidity = NULL; }

    if ( NULL != mpDoors) { delete mpDoors; mpDoors = NULL; }
    if ( NULL != mpTemperatures) { delete mpTemperatures; mpTemperatures = NULL; }
    if ( NULL != mpConfigurationLink) { delete mpConfigurationLink; mpConfigurationLink = NULL; }


    if ( NULL != mpDetection) { delete mpDetection; mpDetection = NULL; }
    if ( NULL != mpLevel) { delete mpLevel; mpLevel = NULL; }
    if ( NULL != mpAlarms) { delete mpAlarms; mpAlarms = NULL; }
    if ( NULL != mpEnergyConsumption) { delete mpEnergyConsumption; mpEnergyConsumption = NULL; }

    if ( NULL != mpDRLC) { delete mpDRLC; mpDRLC = NULL; }
    if ( NULL != mpThermostat) { delete mpThermostat; mpThermostat = NULL; }

}

bool
Device::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
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
        bool bRet = false;

        if ( NULL != mpType )
        {
            if (mpType->value.length() > 0)
            {
                if (true == mpType->validateContent())
                {
                    bRet = serializer.setPropertyValue("type", mpType->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("type") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : type"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpResources )
        {
            mpResources->setInstanceName("resources");
            bRet = mpResources->serializeContents(serializer) ;
        }
        if (false == bRet) { if (serializer.isInFilteringMode("resources") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : resources"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpConnected )
        {
            bRet = serializer.setPropertyValue("connected", *mpConnected);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("connected") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : connected"); return false; }}
    }


    if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpMode )
            {
                bRet = mpMode->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Mode") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Mode"); return false; }}
        }

    }       if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpInformationLink )
        {
            bRet = mpInformationLink->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("InformationLink") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : InformationLink"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpMessagesLink )
        {
            bRet = mpMessagesLink->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("MessagesLink") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : MessagesLink"); return false; }}
    }
    if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpMovement )
            {
                bRet = mpMovement->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Movement") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Movement"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpLight )
            {
                bRet = mpLight->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Light") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Light"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpOperation )
            {
                bRet = mpOperation->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Operation") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Operation"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpHumidity )
            {
                bRet = mpHumidity->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Humidity") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Humidity"); return false; }}
        }

    }

    if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpDoors )
            {
                bRet = mpDoors->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Doors") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Doors"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpTemperatures )
            {
                bRet = mpTemperatures->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Temperatures") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Temperatures"); return false; }}
        }

    }   if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpConfigurationLink )
        {
            bRet = mpConfigurationLink->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("ConfigurationLink") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : ConfigurationLink"); return false; }}
    }
    if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpDetection )
            {
                bRet = mpDetection->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Detection") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Detection"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpLevel )
            {
                bRet = mpLevel->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Level") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Level"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpAlarms )
            {
                bRet = mpAlarms->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Alarms") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Alarms"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpEnergyConsumption )
            {
                bRet = mpEnergyConsumption->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("EnergyConsumption") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : EnergyConsumption"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpDRLC )
            {
                bRet = mpDRLC->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("DRLC") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : DRLC"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpThermostat )
            {
                bRet = mpThermostat->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Thermostat") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Thermostat"); return false; }}
        }

    }

    return true ;
}

bool
Device::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
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
    if (true == deserializer.hasProperty("name"))
    {
        mpName = new ::String30();
        if (mpName == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  name"); return false;}
        if (false == deserializer.getPropertyValue("name", &(mpName->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  name"); return false ;}
        if (false == mpName->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : name"); return false ;}
    }
    if (true == deserializer.hasProperty("type"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  type"); return false;}
        mpType = new ::DeviceType();
        if (mpType == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  type"); return false;}
        if (false == deserializer.getPropertyValue("type", &(mpType->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  type"); return false ;}
        if (false == mpType->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : type"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("type") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : type"); return false; }}
    {
        if (true == deserializer.hasProperty("resources"))
        {
            if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  resources"); return false;}
            mpResources = new ::ArrayOfResourceType();
            if (mpResources == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  resources"); return false;}
            mpResources->setInstanceName("resources");
            if (false == mpResources->deSerializeContents(deserializer)) {  deserializer.setErrorMessage("Parameter Contains Invalid Value : resources"); return false ;}
        }
        else { if (deserializer.isInFilteringMode("resources") == false) { deserializer.setErrorMessage("Mandatory Parameter not found :  resources"); return false; }}
    }
    if (true == deserializer.hasProperty("connected"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  connected"); return false;}
        mpConnected = new bool();
        if (mpConnected == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  connected"); return false;}
        if (false == deserializer.getPropertyValue("connected", mpConnected)) { deserializer.setErrorMessage("Failed to deserialize parameter :  connected"); return false ;}
    }

    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Mode"))
        {
            mpMode = new ::Mode();
            if (mpMode == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Mode"); return false;}
            if (false == mpMode->deSerialize(deserializer)) {return false;}
        }

    }
    else
    {   if (true == deserializer.hasProperty("Mode")) {deserializer.setErrorMessage("Additional data to deserialize: Mode"); return false; } }

    if (true == deserializer.hasGroupProperty("InformationLink"))
    {
        if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  InformationLink"); return false;}
        mpInformationLink = new ::InformationLink();
        if (mpInformationLink == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  InformationLink"); return false;}
        if (false == mpInformationLink->deSerialize(deserializer)) {return false;}
    }
    if (true == deserializer.hasGroupProperty("MessagesLink"))
    {
        if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  MessagesLink"); return false;}
        mpMessagesLink = new ::MessagesLink();
        if (mpMessagesLink == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  MessagesLink"); return false;}
        if (false == mpMessagesLink->deSerialize(deserializer)) {return false;}
    }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Movement"))
        {
            mpMovement = new ::Movement();
            if (mpMovement == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Movement"); return false;}
            if (false == mpMovement->deSerialize(deserializer)) {return false;}
        }

    }
    else
    {   if (true == deserializer.hasProperty("Movement")) {deserializer.setErrorMessage("Additional data to deserialize: Movement"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Light"))
        {
            mpLight = new ::Light();
            if (mpLight == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Light"); return false;}
            if (false == mpLight->deSerialize(deserializer)) {return false;}
        }

    }
    else
    {   if (true == deserializer.hasProperty("Light")) {deserializer.setErrorMessage("Additional data to deserialize: Light"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Operation"))
        {
            mpOperation = new ::Operation();
            if (mpOperation == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Operation"); return false;}
            if (false == mpOperation->deSerialize(deserializer)) {return false;}
        }

    }
    else
    {   if (true == deserializer.hasProperty("Operation")) {deserializer.setErrorMessage("Additional data to deserialize: Operation"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Humidity"))
        {
            mpHumidity = new ::Humidity();
            if (mpHumidity == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Humidity"); return false;}
            if (false == mpHumidity->deSerialize(deserializer)) {return false;}
        }

    }
    else
    {   if (true == deserializer.hasProperty("Humidity")) {deserializer.setErrorMessage("Additional data to deserialize: Humidity"); return false; } }

    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Doors"))
        {
            mpDoors = new ::Doors();
            if (mpDoors == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Doors"); return false;}
            if (false == mpDoors->deSerialize(deserializer)) {return false;}
        }

    }
    else
    {   if (true == deserializer.hasProperty("Doors")) {deserializer.setErrorMessage("Additional data to deserialize: Doors"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Temperatures"))
        {
            mpTemperatures = new ::Temperatures();
            if (mpTemperatures == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Temperatures"); return false;}
            if (false == mpTemperatures->deSerialize(deserializer)) {return false;}
        }

    }
    else
    {   if (true == deserializer.hasProperty("Temperatures")) {deserializer.setErrorMessage("Additional data to deserialize: Temperatures"); return false; } }
    if (true == deserializer.hasGroupProperty("ConfigurationLink"))
    {
        if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  ConfigurationLink"); return false;}
        mpConfigurationLink = new ::ConfigurationLink();
        if (mpConfigurationLink == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  ConfigurationLink"); return false;}
        if (false == mpConfigurationLink->deSerialize(deserializer)) {return false;}
    }



    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Detection"))
        {
            mpDetection = new ::Detection();
            if (mpDetection == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Detection"); return false;}
            if (false == mpDetection->deSerialize(deserializer)) {return false;}
        }

    }
    else
    {   if (true == deserializer.hasProperty("Detection")) {deserializer.setErrorMessage("Additional data to deserialize: Detection"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Level"))
        {
            mpLevel = new ::Level();
            if (mpLevel == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Level"); return false;}
            if (false == mpLevel->deSerialize(deserializer)) {return false;}
        }

    }
    else
    {   if (true == deserializer.hasProperty("Level")) {deserializer.setErrorMessage("Additional data to deserialize: Level"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Alarms"))
        {
            mpAlarms = new ::Alarms();
            if (mpAlarms == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Alarms"); return false;}
            if (false == mpAlarms->deSerialize(deserializer)) {return false;}
        }

    }
    else
    {   if (true == deserializer.hasProperty("Alarms")) {deserializer.setErrorMessage("Additional data to deserialize: Alarms"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("EnergyConsumption"))
        {
            mpEnergyConsumption = new ::EnergyConsumption();
            if (mpEnergyConsumption == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  EnergyConsumption"); return false;}
            if (false == mpEnergyConsumption->deSerialize(deserializer)) {return false;}
        }

    }
    else
    {   if (true == deserializer.hasProperty("EnergyConsumption")) {deserializer.setErrorMessage("Additional data to deserialize: EnergyConsumption"); return false; } }

    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("DRLC"))
        {
            mpDRLC = new ::DRLC();
            if (mpDRLC == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  DRLC"); return false;}
            if (false == mpDRLC->deSerialize(deserializer)) {return false;}
        }

    }
    else
    {   if (true == deserializer.hasProperty("DRLC")) {deserializer.setErrorMessage("Additional data to deserialize: DRLC"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Thermostat"))
        {
            mpThermostat = new ::Thermostat();
            if (mpThermostat == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Thermostat"); return false;}
            if (false == mpThermostat->deSerialize(deserializer)) {return false;}
        }

    }
    else
    {   if (true == deserializer.hasProperty("Thermostat")) {deserializer.setErrorMessage("Additional data to deserialize: Thermostat"); return false; } }


    return true ;
}

std::string
Device::getElementName()
{
    return EN_DEVICE;
}

bool
Device::clone(::Device *pSrcObject)
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
        if ( NULL != mpName) { delete mpName; mpName = NULL; }

        if ( NULL != pSrcObject->mpName )
        {
            mpName = new ::String30();
            if (mpName == NULL ) {return false;}
            mpName->value = (pSrcObject->mpName->value );
        }
    }
    {
        if ( NULL != mpType) { delete mpType; mpType = NULL; }

        if ( NULL != pSrcObject->mpType )
        {
            mpType = new ::DeviceType();
            if (mpType == NULL ) {return false;}
            mpType->value = (pSrcObject->mpType->value );
        }
    }
    {
        if ( NULL != mpResources) { delete mpResources; mpResources = NULL; }

        if ( NULL != pSrcObject->mpResources )
        {
            mpResources = new ::ArrayOfResourceType();
            if (mpResources == NULL ) { return false;}
            mpResources->members = pSrcObject->mpResources->members;
        }
    }
    {
        if ( NULL != mpConnected) { delete mpConnected; mpConnected = NULL; }

        if ( NULL != pSrcObject->mpConnected )
        {
            mpConnected = new bool();
            if (mpConnected == NULL ) {return false;}
            *mpConnected = *(pSrcObject->mpConnected );
        }
    }

    {
        if ( NULL != mpMode) { delete mpMode; mpMode = NULL; }

        if ( NULL != pSrcObject->mpMode )
        {
            mpMode = new ::Mode();
            if (mpMode == NULL ) { return false;}
            if (false == mpMode->clone(pSrcObject->mpMode)) {return false;} ;
        }
    }

    {
        if ( NULL != mpInformationLink) { delete mpInformationLink; mpInformationLink = NULL; }

        if ( NULL != pSrcObject->mpInformationLink )
        {
            mpInformationLink = new ::InformationLink();
            if (mpInformationLink == NULL ) { return false;}
            if (false == mpInformationLink->clone(pSrcObject->mpInformationLink)) {return false;} ;
        }
    }
    {
        if ( NULL != mpMessagesLink) { delete mpMessagesLink; mpMessagesLink = NULL; }

        if ( NULL != pSrcObject->mpMessagesLink )
        {
            mpMessagesLink = new ::MessagesLink();
            if (mpMessagesLink == NULL ) { return false;}
            if (false == mpMessagesLink->clone(pSrcObject->mpMessagesLink)) {return false;} ;
        }
    }
    {
        if ( NULL != mpMovement) { delete mpMovement; mpMovement = NULL; }

        if ( NULL != pSrcObject->mpMovement )
        {
            mpMovement = new ::Movement();
            if (mpMovement == NULL ) { return false;}
            if (false == mpMovement->clone(pSrcObject->mpMovement)) {return false;} ;
        }
    }
    {
        if ( NULL != mpLight) { delete mpLight; mpLight = NULL; }

        if ( NULL != pSrcObject->mpLight )
        {
            mpLight = new ::Light();
            if (mpLight == NULL ) { return false;}
            if (false == mpLight->clone(pSrcObject->mpLight)) {return false;} ;
        }
    }
    {
        if ( NULL != mpOperation) { delete mpOperation; mpOperation = NULL; }

        if ( NULL != pSrcObject->mpOperation )
        {
            mpOperation = new ::Operation();
            if (mpOperation == NULL ) { return false;}
            if (false == mpOperation->clone(pSrcObject->mpOperation)) {return false;} ;
        }
    }
    {
        if ( NULL != mpHumidity) { delete mpHumidity; mpHumidity = NULL; }

        if ( NULL != pSrcObject->mpHumidity )
        {
            mpHumidity = new ::Humidity();
            if (mpHumidity == NULL ) { return false;}
            if (false == mpHumidity->clone(pSrcObject->mpHumidity)) {return false;} ;
        }
    }

    {
        if ( NULL != mpDoors) { delete mpDoors; mpDoors = NULL; }

        if ( NULL != pSrcObject->mpDoors )
        {
            mpDoors = new ::Doors();
            if (mpDoors == NULL ) { return false;}
            if (false == mpDoors->clone(pSrcObject->mpDoors)) {return false;} ;
        }
    }
    {
        if ( NULL != mpTemperatures) { delete mpTemperatures; mpTemperatures = NULL; }

        if ( NULL != pSrcObject->mpTemperatures )
        {
            mpTemperatures = new ::Temperatures();
            if (mpTemperatures == NULL ) { return false;}
            if (false == mpTemperatures->clone(pSrcObject->mpTemperatures)) {return false;} ;
        }
    }
    {
        if ( NULL != mpConfigurationLink) { delete mpConfigurationLink; mpConfigurationLink = NULL; }

        if ( NULL != pSrcObject->mpConfigurationLink )
        {
            mpConfigurationLink = new ::ConfigurationLink();
            if (mpConfigurationLink == NULL ) { return false;}
            if (false == mpConfigurationLink->clone(pSrcObject->mpConfigurationLink)) {return false;} ;
        }
    }



    {
        if ( NULL != mpDetection) { delete mpDetection; mpDetection = NULL; }

        if ( NULL != pSrcObject->mpDetection )
        {
            mpDetection = new ::Detection();
            if (mpDetection == NULL ) { return false;}
            if (false == mpDetection->clone(pSrcObject->mpDetection)) {return false;} ;
        }
    }
    {
        if ( NULL != mpLevel) { delete mpLevel; mpLevel = NULL; }

        if ( NULL != pSrcObject->mpLevel )
        {
            mpLevel = new ::Level();
            if (mpLevel == NULL ) { return false;}
            if (false == mpLevel->clone(pSrcObject->mpLevel)) {return false;} ;
        }
    }
    {
        if ( NULL != mpAlarms) { delete mpAlarms; mpAlarms = NULL; }

        if ( NULL != pSrcObject->mpAlarms )
        {
            mpAlarms = new ::Alarms();
            if (mpAlarms == NULL ) { return false;}
            if (false == mpAlarms->clone(pSrcObject->mpAlarms)) {return false;} ;
        }
    }
    {
        if ( NULL != mpEnergyConsumption) { delete mpEnergyConsumption; mpEnergyConsumption = NULL; }

        if ( NULL != pSrcObject->mpEnergyConsumption )
        {
            mpEnergyConsumption = new ::EnergyConsumption();
            if (mpEnergyConsumption == NULL ) { return false;}
            if (false == mpEnergyConsumption->clone(pSrcObject->mpEnergyConsumption)) {return false;} ;
        }
    }

    {
        if ( NULL != mpDRLC) { delete mpDRLC; mpDRLC = NULL; }

        if ( NULL != pSrcObject->mpDRLC )
        {
            mpDRLC = new ::DRLC();
            if (mpDRLC == NULL ) { return false;}
            if (false == mpDRLC->clone(pSrcObject->mpDRLC)) {return false;} ;
        }
    }
    {
        if ( NULL != mpThermostat) { delete mpThermostat; mpThermostat = NULL; }

        if ( NULL != pSrcObject->mpThermostat )
        {
            mpThermostat = new ::Thermostat();
            if (mpThermostat == NULL ) { return false;}
            if (false == mpThermostat->clone(pSrcObject->mpThermostat)) {return false;} ;
        }
    }


    return true ;
}
