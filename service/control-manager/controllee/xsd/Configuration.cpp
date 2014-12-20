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

#include "Configuration.h"


#include "Elements.h"

using namespace std;

Configuration::Configuration()
{

    mpInstalledLocation = NULL;
    mpRegion = NULL;
    mpCurrency = NULL;
    mpRemoteControlEnabled = NULL;
    mpPairingStart = NULL;
    mpUnpairingStart = NULL;
    mpPairingTimeout = NULL;
    mpSubDeviceIDs = NULL;
    mpIsSubDevice = NULL;
    mpConnectedDeviceType = NULL;
    mpNetworks = NULL;
    mpTime = NULL;
    mpRemote = NULL;

}

Configuration::~Configuration()
{
    if ( NULL != mpInstalledLocation) { delete mpInstalledLocation; mpInstalledLocation = NULL; }
    if ( NULL != mpRegion) { delete mpRegion; mpRegion = NULL; }
    if ( NULL != mpCurrency) { delete mpCurrency; mpCurrency = NULL; }
    if ( NULL != mpRemoteControlEnabled) { delete mpRemoteControlEnabled; mpRemoteControlEnabled = NULL; }
    if ( NULL != mpPairingStart) { delete mpPairingStart; mpPairingStart = NULL; }
    if ( NULL != mpUnpairingStart) { delete mpUnpairingStart; mpUnpairingStart = NULL; }
    if ( NULL != mpPairingTimeout) { delete mpPairingTimeout; mpPairingTimeout = NULL; }
    if ( NULL != mpSubDeviceIDs) { delete mpSubDeviceIDs; mpSubDeviceIDs = NULL; }
    if ( NULL != mpIsSubDevice) { delete mpIsSubDevice; mpIsSubDevice = NULL; }
    if ( NULL != mpConnectedDeviceType) { delete mpConnectedDeviceType; mpConnectedDeviceType = NULL; }
    if ( NULL != mpNetworks) { delete mpNetworks; mpNetworks = NULL; }
    if ( NULL != mpTime) { delete mpTime; mpTime = NULL; }
    if ( NULL != mpRemote) { delete mpRemote; mpRemote = NULL; }

}

bool
Configuration::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    {
        bool bRet = true;

        if ( NULL != mpInstalledLocation )
        {
            if (mpInstalledLocation->value.length() > 0)
            {
                if (true == mpInstalledLocation->validateContent())
                {
                    bRet = serializer.setPropertyValue("installedLocation", mpInstalledLocation->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("installedLocation") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : installedLocation"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpRegion )
        {
            if (mpRegion->value.length() > 0)
            {
                if (true == mpRegion->validateContent())
                {
                    bRet = serializer.setPropertyValue("region", mpRegion->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("region") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : region"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpCurrency )
        {
            if (mpCurrency->value.length() > 0)
            {
                if (true == mpCurrency->validateContent())
                {
                    bRet = serializer.setPropertyValue("currency", mpCurrency->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("currency") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : currency"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpRemoteControlEnabled )
        {
            bRet = serializer.setPropertyValue("remoteControlEnabled", *mpRemoteControlEnabled);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("remoteControlEnabled") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : remoteControlEnabled"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpPairingStart )
        {
            if (mpPairingStart->value.length() > 0)
            {
                if (true == mpPairingStart->validateContent())
                {
                    bRet = serializer.setPropertyValue("pairingStart", mpPairingStart->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("pairingStart") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : pairingStart"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpUnpairingStart )
        {
            if (mpUnpairingStart->value.length() > 0)
            {
                if (true == mpUnpairingStart->validateContent())
                {
                    bRet = serializer.setPropertyValue("unpairingStart", mpUnpairingStart->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("unpairingStart") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : unpairingStart"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpPairingTimeout )
        {
            bRet = serializer.setPropertyValue("pairingTimeout", *mpPairingTimeout);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("pairingTimeout") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : pairingTimeout"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpSubDeviceIDs )
        {
            mpSubDeviceIDs->setInstanceName("subDeviceIDs");
            bRet = mpSubDeviceIDs->serializeContents(serializer) ;
        }
        if (false == bRet) { if (serializer.isInFilteringMode("subDeviceIDs") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : subDeviceIDs"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpIsSubDevice )
        {
            bRet = serializer.setPropertyValue("isSubDevice", *mpIsSubDevice);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("isSubDevice") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : isSubDevice"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpConnectedDeviceType )
        {
            if (mpConnectedDeviceType->value.length() > 0)
            {
                if (true == mpConnectedDeviceType->validateContent())
                {
                    bRet = serializer.setPropertyValue("connectedDeviceType", mpConnectedDeviceType->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("connectedDeviceType") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : connectedDeviceType"); return false; }}
    }
    if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpNetworks )
            {
                bRet = mpNetworks->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Networks") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Networks"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpTime )
            {
                bRet = mpTime->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Time") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Time"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpRemote )
            {
                bRet = mpRemote->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Remote") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Remote"); return false; }}
        }

    }

    return true ;
}

bool
Configuration::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("installedLocation"))
    {
        mpInstalledLocation = new ::InternalLocationType();
        if (mpInstalledLocation == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  installedLocation"); return false;}
        if (false == deserializer.getPropertyValue("installedLocation", &(mpInstalledLocation->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  installedLocation"); return false ;}
        if (false == mpInstalledLocation->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : installedLocation"); return false ;}
    }
    if (true == deserializer.hasProperty("region"))
    {
        mpRegion = new ::String20();
        if (mpRegion == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  region"); return false;}
        if (false == deserializer.getPropertyValue("region", &(mpRegion->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  region"); return false ;}
        if (false == mpRegion->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : region"); return false ;}
    }
    if (true == deserializer.hasProperty("currency"))
    {
        mpCurrency = new ::CurrencyType();
        if (mpCurrency == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  currency"); return false;}
        if (false == deserializer.getPropertyValue("currency", &(mpCurrency->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  currency"); return false ;}
        if (false == mpCurrency->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : currency"); return false ;}
    }
    if (true == deserializer.hasProperty("remoteControlEnabled"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  remoteControlEnabled"); return false;}
        mpRemoteControlEnabled = new bool();
        if (mpRemoteControlEnabled == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  remoteControlEnabled"); return false;}
        if (false == deserializer.getPropertyValue("remoteControlEnabled", mpRemoteControlEnabled)) { deserializer.setErrorMessage("Failed to deserialize parameter :  remoteControlEnabled"); return false ;}
    }
    if (true == deserializer.hasProperty("pairingStart"))
    {
        mpPairingStart = new ::OnType();
        if (mpPairingStart == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  pairingStart"); return false;}
        if (false == deserializer.getPropertyValue("pairingStart", &(mpPairingStart->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  pairingStart"); return false ;}
        if (false == mpPairingStart->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : pairingStart"); return false ;}
    }
    if (true == deserializer.hasProperty("unpairingStart"))
    {
        mpUnpairingStart = new ::OnType();
        if (mpUnpairingStart == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  unpairingStart"); return false;}
        if (false == deserializer.getPropertyValue("unpairingStart", &(mpUnpairingStart->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  unpairingStart"); return false ;}
        if (false == mpUnpairingStart->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : unpairingStart"); return false ;}
    }
    if (true == deserializer.hasProperty("pairingTimeout"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  pairingTimeout"); return false;}
        mpPairingTimeout = new uint16_t();
        if (mpPairingTimeout == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  pairingTimeout"); return false;}
        if (false == deserializer.getPropertyValue("pairingTimeout", mpPairingTimeout)) { deserializer.setErrorMessage("Failed to deserialize parameter :  pairingTimeout"); return false ;}
    }
    {
        if (true == deserializer.hasProperty("subDeviceIDs"))
        {
            mpSubDeviceIDs = new ::ArrayOfString();
            if (mpSubDeviceIDs == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  subDeviceIDs"); return false;}
            mpSubDeviceIDs->setInstanceName("subDeviceIDs");
            if (false == mpSubDeviceIDs->deSerializeContents(deserializer)) {  deserializer.setErrorMessage("Parameter Contains Invalid Value : subDeviceIDs"); return false ;}
        }
    }
    if (true == deserializer.hasProperty("isSubDevice"))
    {
        mpIsSubDevice = new bool();
        if (mpIsSubDevice == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  isSubDevice"); return false;}
        if (false == deserializer.getPropertyValue("isSubDevice", mpIsSubDevice)) { deserializer.setErrorMessage("Failed to deserialize parameter :  isSubDevice"); return false ;}
    }
    if (true == deserializer.hasProperty("connectedDeviceType"))
    {
        mpConnectedDeviceType = new ::DeviceType();
        if (mpConnectedDeviceType == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  connectedDeviceType"); return false;}
        if (false == deserializer.getPropertyValue("connectedDeviceType", &(mpConnectedDeviceType->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  connectedDeviceType"); return false ;}
        if (false == mpConnectedDeviceType->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : connectedDeviceType"); return false ;}
    }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Networks"))
        {
            mpNetworks = new ::Networks();
            if (mpNetworks == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Networks"); return false;}
            if (false == mpNetworks->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Networks")) {deserializer.setErrorMessage("Additional data to deserialize: Networks"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Time"))
        {
            mpTime = new ::Time();
            if (mpTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Time"); return false;}
            if (false == mpTime->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Time")) {deserializer.setErrorMessage("Additional data to deserialize: Time"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Remote"))
        {
            mpRemote = new ::Remote();
            if (mpRemote == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Remote"); return false;}
            if (false == mpRemote->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Remote")) {deserializer.setErrorMessage("Additional data to deserialize: Remote"); return false; } }


    return true ;
}

std::string
Configuration::getElementName()
{
    return EN_CONFIGURATION;
}

bool
Configuration::clone(::Configuration *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpInstalledLocation) { delete mpInstalledLocation; mpInstalledLocation = NULL; }

        if ( NULL != pSrcObject->mpInstalledLocation )
        {
            mpInstalledLocation = new ::InternalLocationType();
            if (mpInstalledLocation == NULL ) {return false;}
            mpInstalledLocation->value = (pSrcObject->mpInstalledLocation->value );
        }
    }
    {
        if ( NULL != mpRegion) { delete mpRegion; mpRegion = NULL; }

        if ( NULL != pSrcObject->mpRegion )
        {
            mpRegion = new ::String20();
            if (mpRegion == NULL ) {return false;}
            mpRegion->value = (pSrcObject->mpRegion->value );
        }
    }
    {
        if ( NULL != mpCurrency) { delete mpCurrency; mpCurrency = NULL; }

        if ( NULL != pSrcObject->mpCurrency )
        {
            mpCurrency = new ::CurrencyType();
            if (mpCurrency == NULL ) {return false;}
            mpCurrency->value = (pSrcObject->mpCurrency->value );
        }
    }
    {
        if ( NULL != mpRemoteControlEnabled) { delete mpRemoteControlEnabled; mpRemoteControlEnabled = NULL; }

        if ( NULL != pSrcObject->mpRemoteControlEnabled )
        {
            mpRemoteControlEnabled = new bool();
            if (mpRemoteControlEnabled == NULL ) {return false;}
            *mpRemoteControlEnabled = *(pSrcObject->mpRemoteControlEnabled );
        }
    }
    {
        if ( NULL != mpPairingStart) { delete mpPairingStart; mpPairingStart = NULL; }

        if ( NULL != pSrcObject->mpPairingStart )
        {
            mpPairingStart = new ::OnType();
            if (mpPairingStart == NULL ) {return false;}
            mpPairingStart->value = (pSrcObject->mpPairingStart->value );
        }
    }
    {
        if ( NULL != mpUnpairingStart) { delete mpUnpairingStart; mpUnpairingStart = NULL; }

        if ( NULL != pSrcObject->mpUnpairingStart )
        {
            mpUnpairingStart = new ::OnType();
            if (mpUnpairingStart == NULL ) {return false;}
            mpUnpairingStart->value = (pSrcObject->mpUnpairingStart->value );
        }
    }
    {
        if ( NULL != mpPairingTimeout) { delete mpPairingTimeout; mpPairingTimeout = NULL; }

        if ( NULL != pSrcObject->mpPairingTimeout )
        {
            mpPairingTimeout = new uint16_t();
            if (mpPairingTimeout == NULL ) {return false;}
            *mpPairingTimeout = *(pSrcObject->mpPairingTimeout );
        }
    }
    {
        if ( NULL != mpSubDeviceIDs) { delete mpSubDeviceIDs; mpSubDeviceIDs = NULL; }

        if ( NULL != pSrcObject->mpSubDeviceIDs )
        {
            mpSubDeviceIDs = new ::ArrayOfString();
            if (mpSubDeviceIDs == NULL ) { return false;}
            mpSubDeviceIDs->members = pSrcObject->mpSubDeviceIDs->members;
        }
    }
    {
        if ( NULL != mpIsSubDevice) { delete mpIsSubDevice; mpIsSubDevice = NULL; }

        if ( NULL != pSrcObject->mpIsSubDevice )
        {
            mpIsSubDevice = new bool();
            if (mpIsSubDevice == NULL ) {return false;}
            *mpIsSubDevice = *(pSrcObject->mpIsSubDevice );
        }
    }
    {
        if ( NULL != mpConnectedDeviceType) { delete mpConnectedDeviceType; mpConnectedDeviceType = NULL; }

        if ( NULL != pSrcObject->mpConnectedDeviceType )
        {
            mpConnectedDeviceType = new ::DeviceType();
            if (mpConnectedDeviceType == NULL ) {return false;}
            mpConnectedDeviceType->value = (pSrcObject->mpConnectedDeviceType->value );
        }
    }
    {
        if ( NULL != mpNetworks) { delete mpNetworks; mpNetworks = NULL; }

        if ( NULL != pSrcObject->mpNetworks )
        {
            mpNetworks = new ::Networks();
            if (mpNetworks == NULL ) { return false;}
            if (false == mpNetworks->clone(pSrcObject->mpNetworks)) {return false;} ;
        }
    }
    {
        if ( NULL != mpTime) { delete mpTime; mpTime = NULL; }

        if ( NULL != pSrcObject->mpTime )
        {
            mpTime = new ::Time();
            if (mpTime == NULL ) { return false;}
            if (false == mpTime->clone(pSrcObject->mpTime)) {return false;} ;
        }
    }
    {
        if ( NULL != mpRemote) { delete mpRemote; mpRemote = NULL; }

        if ( NULL != pSrcObject->mpRemote )
        {
            mpRemote = new ::Remote();
            if (mpRemote == NULL ) { return false;}
            if (false == mpRemote->clone(pSrcObject->mpRemote)) {return false;} ;
        }
    }


    return true ;
}
