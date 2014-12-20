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

#include "Specification.h"


#include "Elements.h"

using namespace std;

Specification::Specification()
{

    mpPower = NULL;
    mpDisplay = NULL;
    mpSound = NULL;
    mpOtaSupported = NULL;
    mpResetDeviceSupported = NULL;
    mpResetFactoryDefaultSupported = NULL;
    mpSupportedConnectivity = NULL;

}

Specification::~Specification()
{
    if ( NULL != mpPower) { delete mpPower; mpPower = NULL; }
    if ( NULL != mpDisplay) { delete mpDisplay; mpDisplay = NULL; }
    if ( NULL != mpSound) { delete mpSound; mpSound = NULL; }
    if ( NULL != mpOtaSupported) { delete mpOtaSupported; mpOtaSupported = NULL; }
    if ( NULL != mpResetDeviceSupported) { delete mpResetDeviceSupported; mpResetDeviceSupported = NULL; }
    if ( NULL != mpResetFactoryDefaultSupported) { delete mpResetFactoryDefaultSupported; mpResetFactoryDefaultSupported = NULL; }
    if ( NULL != mpSupportedConnectivity) { delete mpSupportedConnectivity; mpSupportedConnectivity = NULL; }

}

bool
Specification::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpPower )
        {
            if (mpPower->value.length() > 0)
            {
                if (true == mpPower->validateContent())
                {
                    bRet = serializer.setPropertyValue("power", mpPower->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("power") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : power"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpDisplay )
        {
            if (mpDisplay->value.length() > 0)
            {
                if (true == mpDisplay->validateContent())
                {
                    bRet = serializer.setPropertyValue("display", mpDisplay->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("display") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : display"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpSound )
        {
            if (mpSound->value.length() > 0)
            {
                if (true == mpSound->validateContent())
                {
                    bRet = serializer.setPropertyValue("sound", mpSound->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("sound") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : sound"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpOtaSupported )
        {
            bRet = serializer.setPropertyValue("otaSupported", *mpOtaSupported);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("otaSupported") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : otaSupported"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpResetDeviceSupported )
        {
            bRet = serializer.setPropertyValue("resetDeviceSupported", *mpResetDeviceSupported);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("resetDeviceSupported") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : resetDeviceSupported"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpResetFactoryDefaultSupported )
        {
            bRet = serializer.setPropertyValue("resetFactoryDefaultSupported", *mpResetFactoryDefaultSupported);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("resetFactoryDefaultSupported") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : resetFactoryDefaultSupported"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpSupportedConnectivity )
        {
            mpSupportedConnectivity->setInstanceName("supportedConnectivity");
            bRet = mpSupportedConnectivity->serializeContents(serializer) ;
        }
        if (false == bRet) { if (serializer.isInFilteringMode("supportedConnectivity") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : supportedConnectivity"); return false; }}
    }


    return true ;
}

bool
Specification::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (true == deserializer.hasProperty("power"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  power"); return false;}
        mpPower = new ::String20();
        if (mpPower == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  power"); return false;}
        if (false == deserializer.getPropertyValue("power", &(mpPower->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  power"); return false ;}
        if (false == mpPower->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : power"); return false ;}
    }
    if (true == deserializer.hasProperty("display"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  display"); return false;}
        mpDisplay = new ::DisplayType();
        if (mpDisplay == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  display"); return false;}
        if (false == deserializer.getPropertyValue("display", &(mpDisplay->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  display"); return false ;}
        if (false == mpDisplay->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : display"); return false ;}
    }
    if (true == deserializer.hasProperty("sound"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  sound"); return false;}
        mpSound = new ::SoundType();
        if (mpSound == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  sound"); return false;}
        if (false == deserializer.getPropertyValue("sound", &(mpSound->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  sound"); return false ;}
        if (false == mpSound->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : sound"); return false ;}
    }
    if (true == deserializer.hasProperty("otaSupported"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  otaSupported"); return false;}
        mpOtaSupported = new bool();
        if (mpOtaSupported == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  otaSupported"); return false;}
        if (false == deserializer.getPropertyValue("otaSupported", mpOtaSupported)) { deserializer.setErrorMessage("Failed to deserialize parameter :  otaSupported"); return false ;}
    }
    if (true == deserializer.hasProperty("resetDeviceSupported"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  resetDeviceSupported"); return false;}
        mpResetDeviceSupported = new bool();
        if (mpResetDeviceSupported == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  resetDeviceSupported"); return false;}
        if (false == deserializer.getPropertyValue("resetDeviceSupported", mpResetDeviceSupported)) { deserializer.setErrorMessage("Failed to deserialize parameter :  resetDeviceSupported"); return false ;}
    }
    if (true == deserializer.hasProperty("resetFactoryDefaultSupported"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  resetFactoryDefaultSupported"); return false;}
        mpResetFactoryDefaultSupported = new bool();
        if (mpResetFactoryDefaultSupported == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  resetFactoryDefaultSupported"); return false;}
        if (false == deserializer.getPropertyValue("resetFactoryDefaultSupported", mpResetFactoryDefaultSupported)) { deserializer.setErrorMessage("Failed to deserialize parameter :  resetFactoryDefaultSupported"); return false ;}
    }
    {
        if (true == deserializer.hasProperty("supportedConnectivity"))
        {
            if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  supportedConnectivity"); return false;}
            mpSupportedConnectivity = new ::ArrayOfConnectivityType();
            if (mpSupportedConnectivity == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  supportedConnectivity"); return false;}
            mpSupportedConnectivity->setInstanceName("supportedConnectivity");
            if (false == mpSupportedConnectivity->deSerializeContents(deserializer)) {  deserializer.setErrorMessage("Parameter Contains Invalid Value : supportedConnectivity"); return false ;}
        }
    }


    return true ;
}

std::string
Specification::getElementName()
{
    return EN_SPECIFICATION;
}

bool
Specification::clone(::Specification *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    {
        if ( NULL != mpPower) { delete mpPower; mpPower = NULL; }

        if ( NULL != pSrcObject->mpPower )
        {
            mpPower = new ::String20();
            if (mpPower == NULL ) {return false;}
            mpPower->value = (pSrcObject->mpPower->value );
        }
    }
    {
        if ( NULL != mpDisplay) { delete mpDisplay; mpDisplay = NULL; }

        if ( NULL != pSrcObject->mpDisplay )
        {
            mpDisplay = new ::DisplayType();
            if (mpDisplay == NULL ) {return false;}
            mpDisplay->value = (pSrcObject->mpDisplay->value );
        }
    }
    {
        if ( NULL != mpSound) { delete mpSound; mpSound = NULL; }

        if ( NULL != pSrcObject->mpSound )
        {
            mpSound = new ::SoundType();
            if (mpSound == NULL ) {return false;}
            mpSound->value = (pSrcObject->mpSound->value );
        }
    }
    {
        if ( NULL != mpOtaSupported) { delete mpOtaSupported; mpOtaSupported = NULL; }

        if ( NULL != pSrcObject->mpOtaSupported )
        {
            mpOtaSupported = new bool();
            if (mpOtaSupported == NULL ) {return false;}
            *mpOtaSupported = *(pSrcObject->mpOtaSupported );
        }
    }
    {
        if ( NULL != mpResetDeviceSupported) { delete mpResetDeviceSupported; mpResetDeviceSupported = NULL; }

        if ( NULL != pSrcObject->mpResetDeviceSupported )
        {
            mpResetDeviceSupported = new bool();
            if (mpResetDeviceSupported == NULL ) {return false;}
            *mpResetDeviceSupported = *(pSrcObject->mpResetDeviceSupported );
        }
    }
    {
        if ( NULL != mpResetFactoryDefaultSupported) { delete mpResetFactoryDefaultSupported; mpResetFactoryDefaultSupported = NULL; }

        if ( NULL != pSrcObject->mpResetFactoryDefaultSupported )
        {
            mpResetFactoryDefaultSupported = new bool();
            if (mpResetFactoryDefaultSupported == NULL ) {return false;}
            *mpResetFactoryDefaultSupported = *(pSrcObject->mpResetFactoryDefaultSupported );
        }
    }
    {
        if ( NULL != mpSupportedConnectivity) { delete mpSupportedConnectivity; mpSupportedConnectivity = NULL; }

        if ( NULL != pSrcObject->mpSupportedConnectivity )
        {
            mpSupportedConnectivity = new ::ArrayOfConnectivityType();
            if (mpSupportedConnectivity == NULL ) { return false;}
            mpSupportedConnectivity->members = pSrcObject->mpSupportedConnectivity->members;
        }
    }


    return true ;
}
