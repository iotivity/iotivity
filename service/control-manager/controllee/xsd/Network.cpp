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

#include "Network.h"


#include "Elements.h"

using namespace std;

Network::Network()
{

    mpEnabled = NULL;
    mpConnected = NULL;
    mpPriority = NULL;
    mpDhcpEnabled = NULL;
    mpIpAddress = NULL;
    mpSubnetMask = NULL;
    mpGateway = NULL;
    mpDnsPrimary = NULL;
    mpDnsSecondary = NULL;
    mpMacAddress = NULL;
    mpWiFi = NULL;

}

Network::~Network()
{
    if ( NULL != mpEnabled) { delete mpEnabled; mpEnabled = NULL; }
    if ( NULL != mpConnected) { delete mpConnected; mpConnected = NULL; }
    if ( NULL != mpPriority) { delete mpPriority; mpPriority = NULL; }
    if ( NULL != mpDhcpEnabled) { delete mpDhcpEnabled; mpDhcpEnabled = NULL; }
    if ( NULL != mpIpAddress) { delete mpIpAddress; mpIpAddress = NULL; }
    if ( NULL != mpSubnetMask) { delete mpSubnetMask; mpSubnetMask = NULL; }
    if ( NULL != mpGateway) { delete mpGateway; mpGateway = NULL; }
    if ( NULL != mpDnsPrimary) { delete mpDnsPrimary; mpDnsPrimary = NULL; }
    if ( NULL != mpDnsSecondary) { delete mpDnsSecondary; mpDnsSecondary = NULL; }
    if ( NULL != mpMacAddress) { delete mpMacAddress; mpMacAddress = NULL; }
    if ( NULL != mpWiFi) { delete mpWiFi; mpWiFi = NULL; }

}

bool
Network::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::IdentifiedObject::serializeContents(serializer)) { return false;}

    {
        bool bRet = false;

        if ( NULL != mpEnabled )
        {
            bRet = serializer.setPropertyValue("enabled", *mpEnabled);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("enabled") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : enabled"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpConnected )
        {
            bRet = serializer.setPropertyValue("connected", *mpConnected);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("connected") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : connected"); return false; }}
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
        bool bRet = false;

        if ( NULL != mpDhcpEnabled )
        {
            bRet = serializer.setPropertyValue("dhcpEnabled", *mpDhcpEnabled);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("dhcpEnabled") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : dhcpEnabled"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpIpAddress )
        {
            if (mpIpAddress->value.length() > 0)
            {
                if (true == mpIpAddress->validateContent())
                {
                    bRet = serializer.setPropertyValue("ipAddress", mpIpAddress->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("ipAddress") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : ipAddress"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpSubnetMask )
        {
            if (mpSubnetMask->value.length() > 0)
            {
                if (true == mpSubnetMask->validateContent())
                {
                    bRet = serializer.setPropertyValue("subnetMask", mpSubnetMask->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("subnetMask") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : subnetMask"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpGateway )
        {
            if (mpGateway->value.length() > 0)
            {
                if (true == mpGateway->validateContent())
                {
                    bRet = serializer.setPropertyValue("gateway", mpGateway->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("gateway") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : gateway"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpDnsPrimary )
        {
            if (mpDnsPrimary->value.length() > 0)
            {
                if (true == mpDnsPrimary->validateContent())
                {
                    bRet = serializer.setPropertyValue("dnsPrimary", mpDnsPrimary->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("dnsPrimary") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : dnsPrimary"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpDnsSecondary )
        {
            if (mpDnsSecondary->value.length() > 0)
            {
                if (true == mpDnsSecondary->validateContent())
                {
                    bRet = serializer.setPropertyValue("dnsSecondary", mpDnsSecondary->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("dnsSecondary") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : dnsSecondary"); return false; }}
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
    if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpWiFi )
            {
                bRet = mpWiFi->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("WiFi") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : WiFi"); return false; }}
        }

    }

    return true ;
}

bool
Network::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::IdentifiedObject::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("enabled"))
    {
        mpEnabled = new bool();
        if (mpEnabled == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  enabled"); return false;}
        if (false == deserializer.getPropertyValue("enabled", mpEnabled)) { deserializer.setErrorMessage("Failed to deserialize parameter :  enabled"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("enabled") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : enabled"); return false; }}
    if (true == deserializer.hasProperty("connected"))
    {
        mpConnected = new bool();
        if (mpConnected == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  connected"); return false;}
        if (false == deserializer.getPropertyValue("connected", mpConnected)) { deserializer.setErrorMessage("Failed to deserialize parameter :  connected"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("connected") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : connected"); return false; }}
    if (true == deserializer.hasProperty("priority"))
    {
        mpPriority = new uint8_t();
        if (mpPriority == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  priority"); return false;}
        if (false == deserializer.getPropertyValue("priority", mpPriority)) { deserializer.setErrorMessage("Failed to deserialize parameter :  priority"); return false ;}
    }
    if (true == deserializer.hasProperty("dhcpEnabled"))
    {
        mpDhcpEnabled = new bool();
        if (mpDhcpEnabled == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  dhcpEnabled"); return false;}
        if (false == deserializer.getPropertyValue("dhcpEnabled", mpDhcpEnabled)) { deserializer.setErrorMessage("Failed to deserialize parameter :  dhcpEnabled"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("dhcpEnabled") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : dhcpEnabled"); return false; }}
    if (true == deserializer.hasProperty("ipAddress"))
    {
        mpIpAddress = new ::String50();
        if (mpIpAddress == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  ipAddress"); return false;}
        if (false == deserializer.getPropertyValue("ipAddress", &(mpIpAddress->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  ipAddress"); return false ;}
        if (false == mpIpAddress->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : ipAddress"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("ipAddress") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : ipAddress"); return false; }}
    if (true == deserializer.hasProperty("subnetMask"))
    {
        mpSubnetMask = new ::String20();
        if (mpSubnetMask == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  subnetMask"); return false;}
        if (false == deserializer.getPropertyValue("subnetMask", &(mpSubnetMask->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  subnetMask"); return false ;}
        if (false == mpSubnetMask->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : subnetMask"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("subnetMask") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : subnetMask"); return false; }}
    if (true == deserializer.hasProperty("gateway"))
    {
        mpGateway = new ::String50();
        if (mpGateway == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  gateway"); return false;}
        if (false == deserializer.getPropertyValue("gateway", &(mpGateway->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  gateway"); return false ;}
        if (false == mpGateway->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : gateway"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("gateway") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : gateway"); return false; }}
    if (true == deserializer.hasProperty("dnsPrimary"))
    {
        mpDnsPrimary = new ::String50();
        if (mpDnsPrimary == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  dnsPrimary"); return false;}
        if (false == deserializer.getPropertyValue("dnsPrimary", &(mpDnsPrimary->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  dnsPrimary"); return false ;}
        if (false == mpDnsPrimary->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : dnsPrimary"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("dnsPrimary") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : dnsPrimary"); return false; }}
    if (true == deserializer.hasProperty("dnsSecondary"))
    {
        mpDnsSecondary = new ::String50();
        if (mpDnsSecondary == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  dnsSecondary"); return false;}
        if (false == deserializer.getPropertyValue("dnsSecondary", &(mpDnsSecondary->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  dnsSecondary"); return false ;}
        if (false == mpDnsSecondary->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : dnsSecondary"); return false ;}
    }
    if (true == deserializer.hasProperty("macAddress"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  macAddress"); return false;}
        mpMacAddress = new ::String20();
        if (mpMacAddress == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  macAddress"); return false;}
        if (false == deserializer.getPropertyValue("macAddress", &(mpMacAddress->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  macAddress"); return false ;}
        if (false == mpMacAddress->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : macAddress"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("macAddress") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : macAddress"); return false; }}
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("WiFi"))
        {
            mpWiFi = new ::WiFi();
            if (mpWiFi == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  WiFi"); return false;}
            if (false == mpWiFi->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("WiFi")) {deserializer.setErrorMessage("Additional data to deserialize: WiFi"); return false; } }


    return true ;
}

std::string
Network::getElementName()
{
    return EN_NETWORK;
}

bool
Network::clone(::Network *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::IdentifiedObject::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpEnabled) { delete mpEnabled; mpEnabled = NULL; }

        if ( NULL != pSrcObject->mpEnabled )
        {
            mpEnabled = new bool();
            if (mpEnabled == NULL ) {return false;}
            *mpEnabled = *(pSrcObject->mpEnabled );
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
        if ( NULL != mpPriority) { delete mpPriority; mpPriority = NULL; }

        if ( NULL != pSrcObject->mpPriority )
        {
            mpPriority = new uint8_t();
            if (mpPriority == NULL ) {return false;}
            *mpPriority = *(pSrcObject->mpPriority );
        }
    }
    {
        if ( NULL != mpDhcpEnabled) { delete mpDhcpEnabled; mpDhcpEnabled = NULL; }

        if ( NULL != pSrcObject->mpDhcpEnabled )
        {
            mpDhcpEnabled = new bool();
            if (mpDhcpEnabled == NULL ) {return false;}
            *mpDhcpEnabled = *(pSrcObject->mpDhcpEnabled );
        }
    }
    {
        if ( NULL != mpIpAddress) { delete mpIpAddress; mpIpAddress = NULL; }

        if ( NULL != pSrcObject->mpIpAddress )
        {
            mpIpAddress = new ::String50();
            if (mpIpAddress == NULL ) {return false;}
            mpIpAddress->value = (pSrcObject->mpIpAddress->value );
        }
    }
    {
        if ( NULL != mpSubnetMask) { delete mpSubnetMask; mpSubnetMask = NULL; }

        if ( NULL != pSrcObject->mpSubnetMask )
        {
            mpSubnetMask = new ::String20();
            if (mpSubnetMask == NULL ) {return false;}
            mpSubnetMask->value = (pSrcObject->mpSubnetMask->value );
        }
    }
    {
        if ( NULL != mpGateway) { delete mpGateway; mpGateway = NULL; }

        if ( NULL != pSrcObject->mpGateway )
        {
            mpGateway = new ::String50();
            if (mpGateway == NULL ) {return false;}
            mpGateway->value = (pSrcObject->mpGateway->value );
        }
    }
    {
        if ( NULL != mpDnsPrimary) { delete mpDnsPrimary; mpDnsPrimary = NULL; }

        if ( NULL != pSrcObject->mpDnsPrimary )
        {
            mpDnsPrimary = new ::String50();
            if (mpDnsPrimary == NULL ) {return false;}
            mpDnsPrimary->value = (pSrcObject->mpDnsPrimary->value );
        }
    }
    {
        if ( NULL != mpDnsSecondary) { delete mpDnsSecondary; mpDnsSecondary = NULL; }

        if ( NULL != pSrcObject->mpDnsSecondary )
        {
            mpDnsSecondary = new ::String50();
            if (mpDnsSecondary == NULL ) {return false;}
            mpDnsSecondary->value = (pSrcObject->mpDnsSecondary->value );
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
        if ( NULL != mpWiFi) { delete mpWiFi; mpWiFi = NULL; }

        if ( NULL != pSrcObject->mpWiFi )
        {
            mpWiFi = new ::WiFi();
            if (mpWiFi == NULL ) { return false;}
            if (false == mpWiFi->clone(pSrcObject->mpWiFi)) {return false;} ;
        }
    }


    return true ;
}
