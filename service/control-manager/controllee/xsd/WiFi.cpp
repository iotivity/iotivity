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

#include "WiFi.h"


#include "Elements.h"

using namespace std;

WiFi::WiFi()
{

    mpOperationType = NULL;
    mpChannel = NULL;
    mpSsid = NULL;
    mpRssi = NULL;
    mpSecurityType = NULL;
    mpEncryptionType = NULL;
    mpSecurityKey = NULL;
    mpWifiDirectStatus = NULL;
    mpWpsEnabled = NULL;
    mpWpsStatus = NULL;
    mpAccessPointsLink = NULL;

}

WiFi::~WiFi()
{
    if ( NULL != mpOperationType) { delete mpOperationType; mpOperationType = NULL; }
    if ( NULL != mpChannel) { delete mpChannel; mpChannel = NULL; }
    if ( NULL != mpSsid) { delete mpSsid; mpSsid = NULL; }
    if ( NULL != mpRssi) { delete mpRssi; mpRssi = NULL; }
    if ( NULL != mpSecurityType) { delete mpSecurityType; mpSecurityType = NULL; }
    if ( NULL != mpEncryptionType) { delete mpEncryptionType; mpEncryptionType = NULL; }
    if ( NULL != mpSecurityKey) { delete mpSecurityKey; mpSecurityKey = NULL; }
    if ( NULL != mpWifiDirectStatus) { delete mpWifiDirectStatus; mpWifiDirectStatus = NULL; }
    if ( NULL != mpWpsEnabled) { delete mpWpsEnabled; mpWpsEnabled = NULL; }
    if ( NULL != mpWpsStatus) { delete mpWpsStatus; mpWpsStatus = NULL; }
    if ( NULL != mpAccessPointsLink) { delete mpAccessPointsLink; mpAccessPointsLink = NULL; }

}

bool
WiFi::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    {
        bool bRet = true;

        if ( NULL != mpOperationType )
        {
            if (mpOperationType->value.length() > 0)
            {
                if (true == mpOperationType->validateContent())
                {
                    bRet = serializer.setPropertyValue("operationType", mpOperationType->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("operationType") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : operationType"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpChannel )
        {
            bRet = serializer.setPropertyValue("channel", *mpChannel);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("channel") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : channel"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpSsid )
        {
            if (mpSsid->value.length() > 0)
            {
                if (true == mpSsid->validateContent())
                {
                    bRet = serializer.setPropertyValue("ssid", mpSsid->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("ssid") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : ssid"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpRssi )
        {
            bRet = serializer.setPropertyValue("rssi", *mpRssi);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("rssi") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : rssi"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpSecurityType )
        {
            if (mpSecurityType->value.length() > 0)
            {
                if (true == mpSecurityType->validateContent())
                {
                    bRet = serializer.setPropertyValue("securityType", mpSecurityType->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("securityType") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : securityType"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpEncryptionType )
        {
            if (mpEncryptionType->value.length() > 0)
            {
                if (true == mpEncryptionType->validateContent())
                {
                    bRet = serializer.setPropertyValue("encryptionType", mpEncryptionType->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("encryptionType") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : encryptionType"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpSecurityKey )
        {
            if (mpSecurityKey->value.length() > 0)
            {
                if (true == mpSecurityKey->validateContent())
                {
                    bRet = serializer.setPropertyValue("securityKey", mpSecurityKey->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("securityKey") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : securityKey"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpWifiDirectStatus )
        {
            if (mpWifiDirectStatus->value.length() > 0)
            {
                if (true == mpWifiDirectStatus->validateContent())
                {
                    bRet = serializer.setPropertyValue("wifiDirectStatus", mpWifiDirectStatus->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("wifiDirectStatus") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : wifiDirectStatus"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpWpsEnabled )
        {
            bRet = serializer.setPropertyValue("wpsEnabled", *mpWpsEnabled);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("wpsEnabled") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : wpsEnabled"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpWpsStatus )
        {
            if (mpWpsStatus->value.length() > 0)
            {
                if (true == mpWpsStatus->validateContent())
                {
                    bRet = serializer.setPropertyValue("wpsStatus", mpWpsStatus->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("wpsStatus") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : wpsStatus"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpAccessPointsLink )
        {
            bRet = mpAccessPointsLink->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("AccessPointsLink") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : AccessPointsLink"); return false; }}
    }


    return true ;
}

bool
WiFi::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("operationType"))
    {
        mpOperationType = new ::WifiOperationType();
        if (mpOperationType == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  operationType"); return false;}
        if (false == deserializer.getPropertyValue("operationType", &(mpOperationType->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  operationType"); return false ;}
        if (false == mpOperationType->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : operationType"); return false ;}
    }
    if (true == deserializer.hasProperty("channel"))
    {
        mpChannel = new uint8_t();
        if (mpChannel == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  channel"); return false;}
        if (false == deserializer.getPropertyValue("channel", mpChannel)) { deserializer.setErrorMessage("Failed to deserialize parameter :  channel"); return false ;}
    }
    if (true == deserializer.hasProperty("ssid"))
    {
        mpSsid = new ::String50();
        if (mpSsid == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  ssid"); return false;}
        if (false == deserializer.getPropertyValue("ssid", &(mpSsid->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  ssid"); return false ;}
        if (false == mpSsid->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : ssid"); return false ;}
    }
    if (true == deserializer.hasProperty("rssi"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  rssi"); return false;}
        mpRssi = new int8_t();
        if (mpRssi == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  rssi"); return false;}
        if (false == deserializer.getPropertyValue("rssi", mpRssi)) { deserializer.setErrorMessage("Failed to deserialize parameter :  rssi"); return false ;}
    }
    if (true == deserializer.hasProperty("securityType"))
    {
        mpSecurityType = new ::String20();
        if (mpSecurityType == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  securityType"); return false;}
        if (false == deserializer.getPropertyValue("securityType", &(mpSecurityType->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  securityType"); return false ;}
        if (false == mpSecurityType->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : securityType"); return false ;}
    }
    if (true == deserializer.hasProperty("encryptionType"))
    {
        mpEncryptionType = new ::String20();
        if (mpEncryptionType == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  encryptionType"); return false;}
        if (false == deserializer.getPropertyValue("encryptionType", &(mpEncryptionType->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  encryptionType"); return false ;}
        if (false == mpEncryptionType->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : encryptionType"); return false ;}
    }
    if (true == deserializer.hasProperty("securityKey"))
    {
        mpSecurityKey = new ::String100();
        if (mpSecurityKey == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  securityKey"); return false;}
        if (false == deserializer.getPropertyValue("securityKey", &(mpSecurityKey->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  securityKey"); return false ;}
        if (false == mpSecurityKey->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : securityKey"); return false ;}
    }
    if (true == deserializer.hasProperty("wifiDirectStatus"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  wifiDirectStatus"); return false;}
        mpWifiDirectStatus = new ::WifiEventType();
        if (mpWifiDirectStatus == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  wifiDirectStatus"); return false;}
        if (false == deserializer.getPropertyValue("wifiDirectStatus", &(mpWifiDirectStatus->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  wifiDirectStatus"); return false ;}
        if (false == mpWifiDirectStatus->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : wifiDirectStatus"); return false ;}
    }
    if (true == deserializer.hasProperty("wpsEnabled"))
    {
        mpWpsEnabled = new bool();
        if (mpWpsEnabled == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  wpsEnabled"); return false;}
        if (false == deserializer.getPropertyValue("wpsEnabled", mpWpsEnabled)) { deserializer.setErrorMessage("Failed to deserialize parameter :  wpsEnabled"); return false ;}
    }
    if (true == deserializer.hasProperty("wpsStatus"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  wpsStatus"); return false;}
        mpWpsStatus = new ::WifiEventType();
        if (mpWpsStatus == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  wpsStatus"); return false;}
        if (false == deserializer.getPropertyValue("wpsStatus", &(mpWpsStatus->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  wpsStatus"); return false ;}
        if (false == mpWpsStatus->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : wpsStatus"); return false ;}
    }
    if (true == deserializer.hasGroupProperty("AccessPointsLink"))
    {
        if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  AccessPointsLink"); return false;}
        mpAccessPointsLink = new ::AccessPointsLink();
        if (mpAccessPointsLink == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  AccessPointsLink"); return false;}
        if (false == mpAccessPointsLink->deSerialize(deserializer)) {return false;}
    }


    return true ;
}

std::string
WiFi::getElementName()
{
    return EN_WIFI;
}

bool
WiFi::clone(::WiFi *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpOperationType) { delete mpOperationType; mpOperationType = NULL; }

        if ( NULL != pSrcObject->mpOperationType )
        {
            mpOperationType = new ::WifiOperationType();
            if (mpOperationType == NULL ) {return false;}
            mpOperationType->value = (pSrcObject->mpOperationType->value );
        }
    }
    {
        if ( NULL != mpChannel) { delete mpChannel; mpChannel = NULL; }

        if ( NULL != pSrcObject->mpChannel )
        {
            mpChannel = new uint8_t();
            if (mpChannel == NULL ) {return false;}
            *mpChannel = *(pSrcObject->mpChannel );
        }
    }
    {
        if ( NULL != mpSsid) { delete mpSsid; mpSsid = NULL; }

        if ( NULL != pSrcObject->mpSsid )
        {
            mpSsid = new ::String50();
            if (mpSsid == NULL ) {return false;}
            mpSsid->value = (pSrcObject->mpSsid->value );
        }
    }
    {
        if ( NULL != mpRssi) { delete mpRssi; mpRssi = NULL; }

        if ( NULL != pSrcObject->mpRssi )
        {
            mpRssi = new int8_t();
            if (mpRssi == NULL ) {return false;}
            *mpRssi = *(pSrcObject->mpRssi );
        }
    }
    {
        if ( NULL != mpSecurityType) { delete mpSecurityType; mpSecurityType = NULL; }

        if ( NULL != pSrcObject->mpSecurityType )
        {
            mpSecurityType = new ::String20();
            if (mpSecurityType == NULL ) {return false;}
            mpSecurityType->value = (pSrcObject->mpSecurityType->value );
        }
    }
    {
        if ( NULL != mpEncryptionType) { delete mpEncryptionType; mpEncryptionType = NULL; }

        if ( NULL != pSrcObject->mpEncryptionType )
        {
            mpEncryptionType = new ::String20();
            if (mpEncryptionType == NULL ) {return false;}
            mpEncryptionType->value = (pSrcObject->mpEncryptionType->value );
        }
    }
    {
        if ( NULL != mpSecurityKey) { delete mpSecurityKey; mpSecurityKey = NULL; }

        if ( NULL != pSrcObject->mpSecurityKey )
        {
            mpSecurityKey = new ::String100();
            if (mpSecurityKey == NULL ) {return false;}
            mpSecurityKey->value = (pSrcObject->mpSecurityKey->value );
        }
    }
    {
        if ( NULL != mpWifiDirectStatus) { delete mpWifiDirectStatus; mpWifiDirectStatus = NULL; }

        if ( NULL != pSrcObject->mpWifiDirectStatus )
        {
            mpWifiDirectStatus = new ::WifiEventType();
            if (mpWifiDirectStatus == NULL ) {return false;}
            mpWifiDirectStatus->value = (pSrcObject->mpWifiDirectStatus->value );
        }
    }
    {
        if ( NULL != mpWpsEnabled) { delete mpWpsEnabled; mpWpsEnabled = NULL; }

        if ( NULL != pSrcObject->mpWpsEnabled )
        {
            mpWpsEnabled = new bool();
            if (mpWpsEnabled == NULL ) {return false;}
            *mpWpsEnabled = *(pSrcObject->mpWpsEnabled );
        }
    }
    {
        if ( NULL != mpWpsStatus) { delete mpWpsStatus; mpWpsStatus = NULL; }

        if ( NULL != pSrcObject->mpWpsStatus )
        {
            mpWpsStatus = new ::WifiEventType();
            if (mpWpsStatus == NULL ) {return false;}
            mpWpsStatus->value = (pSrcObject->mpWpsStatus->value );
        }
    }
    {
        if ( NULL != mpAccessPointsLink) { delete mpAccessPointsLink; mpAccessPointsLink = NULL; }

        if ( NULL != pSrcObject->mpAccessPointsLink )
        {
            mpAccessPointsLink = new ::AccessPointsLink();
            if (mpAccessPointsLink == NULL ) { return false;}
            if (false == mpAccessPointsLink->clone(pSrcObject->mpAccessPointsLink)) {return false;} ;
        }
    }


    return true ;
}
