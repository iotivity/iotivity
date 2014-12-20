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



#include "AccessPoint.h"


#include "Elements.h"

using namespace std;

AccessPoint::AccessPoint()
{

    mpChannel = NULL;
    mpSsid = NULL;
    mpMacAddress = NULL;
    mpMaxRate = NULL;
    mpRssi = NULL;
    mpSecurityType = NULL;
    mpEncryptionType = NULL;

}

AccessPoint::~AccessPoint()
{
    if ( NULL != mpChannel) { delete mpChannel; mpChannel = NULL; }
    if ( NULL != mpSsid) { delete mpSsid; mpSsid = NULL; }
    if ( NULL != mpMacAddress) { delete mpMacAddress; mpMacAddress = NULL; }
    if ( NULL != mpMaxRate) { delete mpMaxRate; mpMaxRate = NULL; }
    if ( NULL != mpRssi) { delete mpRssi; mpRssi = NULL; }
    if ( NULL != mpSecurityType) { delete mpSecurityType; mpSecurityType = NULL; }
    if ( NULL != mpEncryptionType) { delete mpEncryptionType; mpEncryptionType = NULL; }

}

bool
AccessPoint::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::IdentifiedObject::serializeContents(serializer)) { return false;}

    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpChannel )
        {
            bRet = serializer.setPropertyValue("channel", *mpChannel);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("channel") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : channel"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

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
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("ssid") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : ssid"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpMacAddress )
        {
            if (mpMacAddress->value.length() > 0)
            {
                if (true == mpMacAddress->validateContent())
                {
                    bRet = serializer.setPropertyValue("macAddress", mpMacAddress->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("macAddress") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : macAddress"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpMaxRate )
        {
            bRet = serializer.setPropertyValue("maxRate", *mpMaxRate);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("maxRate") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : maxRate"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpRssi )
        {
            bRet = serializer.setPropertyValue("rssi", *mpRssi);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("rssi") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : rssi"); return false; }}
    }
    if (serializer.validateMandatory() == true)
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
    if (serializer.validateMandatory() == true)
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


    return true ;
}

bool
AccessPoint::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::IdentifiedObject::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("channel"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  channel"); return false;}
        mpChannel = new uint8_t();
        if (mpChannel == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  channel"); return false;}
        if (false == deserializer.getPropertyValue("channel", mpChannel)) { deserializer.setErrorMessage("Failed to deserialize parameter :  channel"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("channel") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : channel"); return false; }}
    if (true == deserializer.hasProperty("ssid"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  ssid"); return false;}
        mpSsid = new ::String50();
        if (mpSsid == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  ssid"); return false;}
        if (false == deserializer.getPropertyValue("ssid", &(mpSsid->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  ssid"); return false ;}
        if (false == mpSsid->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : ssid"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("ssid") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : ssid"); return false; }}
    if (true == deserializer.hasProperty("macAddress"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  macAddress"); return false;}
        mpMacAddress = new ::String20();
        if (mpMacAddress == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  macAddress"); return false;}
        if (false == deserializer.getPropertyValue("macAddress",
                &(mpMacAddress->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  macAddress"); return false ;}
        if (false == mpMacAddress->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : macAddress"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("macAddress") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : macAddress"); return false; }}
    if (true == deserializer.hasProperty("maxRate"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  maxRate"); return false;}
        mpMaxRate = new uint8_t();
        if (mpMaxRate == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  maxRate"); return false;}
        if (false == deserializer.getPropertyValue("maxRate", mpMaxRate)) { deserializer.setErrorMessage("Failed to deserialize parameter :  maxRate"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("maxRate") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : maxRate"); return false; }}
    if (true == deserializer.hasProperty("rssi"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  rssi"); return false;}
        mpRssi = new int8_t();
        if (mpRssi == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  rssi"); return false;}
        if (false == deserializer.getPropertyValue("rssi", mpRssi)) { deserializer.setErrorMessage("Failed to deserialize parameter :  rssi"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("rssi") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : rssi"); return false; }}
    if (true == deserializer.hasProperty("securityType"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  securityType"); return false;}
        mpSecurityType = new ::String20();
        if (mpSecurityType == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  securityType"); return false;}
        if (false == deserializer.getPropertyValue("securityType",
                &(mpSecurityType->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  securityType"); return false ;}
        if (false == mpSecurityType->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : securityType"); return false ;}
    }
    if (true == deserializer.hasProperty("encryptionType"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  encryptionType"); return false;}
        mpEncryptionType = new ::String20();
        if (mpEncryptionType == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  encryptionType"); return false;}
        if (false == deserializer.getPropertyValue("encryptionType",
                &(mpEncryptionType->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  encryptionType"); return false ;}
        if (false == mpEncryptionType->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : encryptionType"); return false ;}
    }


    return true ;
}

std::string
AccessPoint::getElementName()
{
    return EN_ACCESSPOINT;
}

bool
AccessPoint::clone(::AccessPoint *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::IdentifiedObject::clone(pSrcObject)) {return false;}

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
        if ( NULL != mpMacAddress) { delete mpMacAddress; mpMacAddress = NULL; }

        if ( NULL != pSrcObject->mpMacAddress )
        {
            mpMacAddress = new ::String20();
            if (mpMacAddress == NULL ) {return false;}
            mpMacAddress->value = (pSrcObject->mpMacAddress->value );
        }
    }
    {
        if ( NULL != mpMaxRate) { delete mpMaxRate; mpMaxRate = NULL; }

        if ( NULL != pSrcObject->mpMaxRate )
        {
            mpMaxRate = new uint8_t();
            if (mpMaxRate == NULL ) {return false;}
            *mpMaxRate = *(pSrcObject->mpMaxRate );
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


    return true ;
}
