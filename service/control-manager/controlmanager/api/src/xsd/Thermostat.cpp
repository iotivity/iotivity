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



#include "Thermostat.h"


#include "Elements.h"

using namespace std;

Thermostat::Thermostat()
{

    mpSupportedThermostatModes = NULL;
    mpThermostatMode = NULL;
    mpSupportedFanModes = NULL;
    mpFanMode = NULL;
    mpSupportedOperatingStates = NULL;
    mpOperatingState = NULL;

}

Thermostat::~Thermostat()
{
    if ( NULL != mpSupportedThermostatModes) { delete mpSupportedThermostatModes; mpSupportedThermostatModes = NULL; }
    if ( NULL != mpThermostatMode) { delete mpThermostatMode; mpThermostatMode = NULL; }
    if ( NULL != mpSupportedFanModes) { delete mpSupportedFanModes; mpSupportedFanModes = NULL; }
    if ( NULL != mpFanMode) { delete mpFanMode; mpFanMode = NULL; }
    if ( NULL != mpSupportedOperatingStates) { delete mpSupportedOperatingStates; mpSupportedOperatingStates = NULL; }
    if ( NULL != mpOperatingState) { delete mpOperatingState; mpOperatingState = NULL; }

}

bool
Thermostat::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpSupportedThermostatModes )
        {
            mpSupportedThermostatModes->setInstanceName("supportedThermostatModes");
            bRet = mpSupportedThermostatModes->serializeContents(serializer) ;
        }
        if (false == bRet) { if (serializer.isInFilteringMode("supportedThermostatModes") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : supportedThermostatModes"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpThermostatMode )
        {
            if (mpThermostatMode->value.length() > 0)
            {
                if (true == mpThermostatMode->validateContent())
                {
                    bRet = serializer.setPropertyValue("thermostatMode", mpThermostatMode->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("thermostatMode") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : thermostatMode"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpSupportedFanModes )
        {
            mpSupportedFanModes->setInstanceName("supportedFanModes");
            bRet = mpSupportedFanModes->serializeContents(serializer) ;
        }
        if (false == bRet) { if (serializer.isInFilteringMode("supportedFanModes") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : supportedFanModes"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpFanMode )
        {
            if (mpFanMode->value.length() > 0)
            {
                if (true == mpFanMode->validateContent())
                {
                    bRet = serializer.setPropertyValue("fanMode", mpFanMode->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("fanMode") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : fanMode"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpSupportedOperatingStates )
        {
            mpSupportedOperatingStates->setInstanceName("supportedOperatingStates");
            bRet = mpSupportedOperatingStates->serializeContents(serializer) ;
        }
        if (false == bRet) { if (serializer.isInFilteringMode("supportedOperatingStates") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : supportedOperatingStates"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpOperatingState )
        {
            if (mpOperatingState->value.length() > 0)
            {
                if (true == mpOperatingState->validateContent())
                {
                    bRet = serializer.setPropertyValue("operatingState", mpOperatingState->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("operatingState") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : operatingState"); return false; }}
    }


    return true ;
}

bool
Thermostat::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    {
        if (true == deserializer.hasProperty("supportedThermostatModes"))
        {
            if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  supportedThermostatModes"); return false;}
            mpSupportedThermostatModes = new ::ArrayOfString();
            if (mpSupportedThermostatModes == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  supportedThermostatModes"); return false;}
            mpSupportedThermostatModes->setInstanceName("supportedThermostatModes");
            if (false == mpSupportedThermostatModes->deSerializeContents(deserializer)) {  deserializer.setErrorMessage("Parameter Contains Invalid Value : supportedThermostatModes"); return false ;}
        }
        else { if (deserializer.isInFilteringMode("supportedThermostatModes") == false) { deserializer.setErrorMessage("Mandatory Parameter not found :  supportedThermostatModes"); return false; }}
    }
    if (true == deserializer.hasProperty("thermostatMode"))
    {
        mpThermostatMode = new ::String20();
        if (mpThermostatMode == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  thermostatMode"); return false;}
        if (false == deserializer.getPropertyValue("thermostatMode",
                &(mpThermostatMode->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  thermostatMode"); return false ;}
        if (false == mpThermostatMode->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : thermostatMode"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("thermostatMode") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : thermostatMode"); return false; }}
    {
        if (true == deserializer.hasProperty("supportedFanModes"))
        {
            if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  supportedFanModes"); return false;}
            mpSupportedFanModes = new ::ArrayOfString();
            if (mpSupportedFanModes == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  supportedFanModes"); return false;}
            mpSupportedFanModes->setInstanceName("supportedFanModes");
            if (false == mpSupportedFanModes->deSerializeContents(deserializer)) {  deserializer.setErrorMessage("Parameter Contains Invalid Value : supportedFanModes"); return false ;}
        }
        else { if (deserializer.isInFilteringMode("supportedFanModes") == false) { deserializer.setErrorMessage("Mandatory Parameter not found :  supportedFanModes"); return false; }}
    }
    if (true == deserializer.hasProperty("fanMode"))
    {
        mpFanMode = new ::String20();
        if (mpFanMode == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  fanMode"); return false;}
        if (false == deserializer.getPropertyValue("fanMode",
                &(mpFanMode->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  fanMode"); return false ;}
        if (false == mpFanMode->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : fanMode"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("fanMode") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : fanMode"); return false; }}
    {
        if (true == deserializer.hasProperty("supportedOperatingStates"))
        {
            if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  supportedOperatingStates"); return false;}
            mpSupportedOperatingStates = new ::ArrayOfString();
            if (mpSupportedOperatingStates == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  supportedOperatingStates"); return false;}
            mpSupportedOperatingStates->setInstanceName("supportedOperatingStates");
            if (false == mpSupportedOperatingStates->deSerializeContents(deserializer)) {  deserializer.setErrorMessage("Parameter Contains Invalid Value : supportedOperatingStates"); return false ;}
        }
        else { if (deserializer.isInFilteringMode("supportedOperatingStates") == false) { deserializer.setErrorMessage("Mandatory Parameter not found :  supportedOperatingStates"); return false; }}
    }
    if (true == deserializer.hasProperty("operatingState"))
    {
        mpOperatingState = new ::String20();
        if (mpOperatingState == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  operatingState"); return false;}
        if (false == deserializer.getPropertyValue("operatingState",
                &(mpOperatingState->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  operatingState"); return false ;}
        if (false == mpOperatingState->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : operatingState"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("operatingState") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : operatingState"); return false; }}


    return true ;
}

std::string
Thermostat::getElementName()
{
    return EN_THERMOSTAT;
}

bool
Thermostat::clone(::Thermostat *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpSupportedThermostatModes) { delete mpSupportedThermostatModes; mpSupportedThermostatModes = NULL; }

        if ( NULL != pSrcObject->mpSupportedThermostatModes )
        {
            mpSupportedThermostatModes = new ::ArrayOfString();
            if (mpSupportedThermostatModes == NULL ) { return false;}
            mpSupportedThermostatModes->members = pSrcObject->mpSupportedThermostatModes->members;
        }
    }
    {
        if ( NULL != mpThermostatMode) { delete mpThermostatMode; mpThermostatMode = NULL; }

        if ( NULL != pSrcObject->mpThermostatMode )
        {
            mpThermostatMode = new ::String20();
            if (mpThermostatMode == NULL ) {return false;}
            mpThermostatMode->value = (pSrcObject->mpThermostatMode->value );
        }
    }
    {
        if ( NULL != mpSupportedFanModes) { delete mpSupportedFanModes; mpSupportedFanModes = NULL; }

        if ( NULL != pSrcObject->mpSupportedFanModes )
        {
            mpSupportedFanModes = new ::ArrayOfString();
            if (mpSupportedFanModes == NULL ) { return false;}
            mpSupportedFanModes->members = pSrcObject->mpSupportedFanModes->members;
        }
    }
    {
        if ( NULL != mpFanMode) { delete mpFanMode; mpFanMode = NULL; }

        if ( NULL != pSrcObject->mpFanMode )
        {
            mpFanMode = new ::String20();
            if (mpFanMode == NULL ) {return false;}
            mpFanMode->value = (pSrcObject->mpFanMode->value );
        }
    }
    {
        if ( NULL != mpSupportedOperatingStates) { delete mpSupportedOperatingStates; mpSupportedOperatingStates = NULL; }

        if ( NULL != pSrcObject->mpSupportedOperatingStates )
        {
            mpSupportedOperatingStates = new ::ArrayOfString();
            if (mpSupportedOperatingStates == NULL ) { return false;}
            mpSupportedOperatingStates->members = pSrcObject->mpSupportedOperatingStates->members;
        }
    }
    {
        if ( NULL != mpOperatingState) { delete mpOperatingState; mpOperatingState = NULL; }

        if ( NULL != pSrcObject->mpOperatingState )
        {
            mpOperatingState = new ::String20();
            if (mpOperatingState == NULL ) {return false;}
            mpOperatingState->value = (pSrcObject->mpOperatingState->value );
        }
    }


    return true ;
}
