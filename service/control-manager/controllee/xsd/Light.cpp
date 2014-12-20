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

#include "Light.h"


#include "Elements.h"

using namespace std;

Light::Light()
{

    mpLightPower = NULL;
    mpDimmingLevel = NULL;
    mpMaxDimmingLevel = NULL;
    mpRelativeDimmingLevel = NULL;
    mpTransitionTime = NULL;
    mpActivatePatternID = NULL;
    mpOnPatternID = NULL;
    mpColor = NULL;

}

Light::~Light()
{
    if ( NULL != mpLightPower) { delete mpLightPower; mpLightPower = NULL; }
    if ( NULL != mpDimmingLevel) { delete mpDimmingLevel; mpDimmingLevel = NULL; }
    if ( NULL != mpMaxDimmingLevel) { delete mpMaxDimmingLevel; mpMaxDimmingLevel = NULL; }
    if ( NULL != mpRelativeDimmingLevel) { delete mpRelativeDimmingLevel; mpRelativeDimmingLevel = NULL; }
    if ( NULL != mpTransitionTime) { delete mpTransitionTime; mpTransitionTime = NULL; }
    if ( NULL != mpActivatePatternID) { delete mpActivatePatternID; mpActivatePatternID = NULL; }
    if ( NULL != mpOnPatternID) { delete mpOnPatternID; mpOnPatternID = NULL; }
    if ( NULL != mpColor) { delete mpColor; mpColor = NULL; }

}

bool
Light::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    {
        bool bRet = true;

        if ( NULL != mpLightPower )
        {
            if (mpLightPower->value.length() > 0)
            {
                if (true == mpLightPower->validateContent())
                {
                    bRet = serializer.setPropertyValue("lightPower", mpLightPower->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("lightPower") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : lightPower"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpDimmingLevel )
        {
            bRet = serializer.setPropertyValue("dimmingLevel", *mpDimmingLevel);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("dimmingLevel") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : dimmingLevel"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpMaxDimmingLevel )
        {
            bRet = serializer.setPropertyValue("maxDimmingLevel", *mpMaxDimmingLevel);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("maxDimmingLevel") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : maxDimmingLevel"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpRelativeDimmingLevel )
        {
            bRet = serializer.setPropertyValue("relativeDimmingLevel", *mpRelativeDimmingLevel);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("relativeDimmingLevel") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : relativeDimmingLevel"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpTransitionTime )
        {
            bRet = serializer.setPropertyValue("transitionTime", *mpTransitionTime);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("transitionTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : transitionTime"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpActivatePatternID )
        {
            if (mpActivatePatternID->value.length() > 0)
            {
                if (true == mpActivatePatternID->validateContent())
                {
                    bRet = serializer.setPropertyValue("activatePatternID", mpActivatePatternID->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("activatePatternID") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : activatePatternID"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpOnPatternID )
        {
            if (mpOnPatternID->value.length() > 0)
            {
                if (true == mpOnPatternID->validateContent())
                {
                    bRet = serializer.setPropertyValue("onPatternID", mpOnPatternID->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("onPatternID") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : onPatternID"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpColor )
        {
            bRet = mpColor->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("Color") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Color"); return false; }}
    }


    return true ;
}

bool
Light::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("lightPower"))
    {
        mpLightPower = new ::OnType();
        if (mpLightPower == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  lightPower"); return false;}
        if (false == deserializer.getPropertyValue("lightPower", &(mpLightPower->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  lightPower"); return false ;}
        if (false == mpLightPower->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : lightPower"); return false ;}
    }
    if (true == deserializer.hasProperty("dimmingLevel"))
    {
        mpDimmingLevel = new uint8_t();
        if (mpDimmingLevel == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  dimmingLevel"); return false;}
        if (false == deserializer.getPropertyValue("dimmingLevel", mpDimmingLevel)) { deserializer.setErrorMessage("Failed to deserialize parameter :  dimmingLevel"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("dimmingLevel") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : dimmingLevel"); return false; }}
    if (true == deserializer.hasProperty("maxDimmingLevel"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  maxDimmingLevel"); return false;}
        mpMaxDimmingLevel = new uint8_t();
        if (mpMaxDimmingLevel == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  maxDimmingLevel"); return false;}
        if (false == deserializer.getPropertyValue("maxDimmingLevel", mpMaxDimmingLevel)) { deserializer.setErrorMessage("Failed to deserialize parameter :  maxDimmingLevel"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("maxDimmingLevel") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : maxDimmingLevel"); return false; }}
    if (true == deserializer.hasProperty("relativeDimmingLevel"))
    {
        mpRelativeDimmingLevel = new int16_t();
        if (mpRelativeDimmingLevel == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  relativeDimmingLevel"); return false;}
        if (false == deserializer.getPropertyValue("relativeDimmingLevel", mpRelativeDimmingLevel)) { deserializer.setErrorMessage("Failed to deserialize parameter :  relativeDimmingLevel"); return false ;}
    }
    if (true == deserializer.hasProperty("transitionTime"))
    {
        mpTransitionTime = new uint16_t();
        if (mpTransitionTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  transitionTime"); return false;}
        if (false == deserializer.getPropertyValue("transitionTime", mpTransitionTime)) { deserializer.setErrorMessage("Failed to deserialize parameter :  transitionTime"); return false ;}
    }
    if (true == deserializer.hasProperty("activatePatternID"))
    {
        mpActivatePatternID = new ::String50();
        if (mpActivatePatternID == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  activatePatternID"); return false;}
        if (false == deserializer.getPropertyValue("activatePatternID", &(mpActivatePatternID->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  activatePatternID"); return false ;}
        if (false == mpActivatePatternID->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : activatePatternID"); return false ;}
    }
    if (true == deserializer.hasProperty("onPatternID"))
    {
        mpOnPatternID = new ::String50();
        if (mpOnPatternID == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  onPatternID"); return false;}
        if (false == deserializer.getPropertyValue("onPatternID", &(mpOnPatternID->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  onPatternID"); return false ;}
        if (false == mpOnPatternID->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : onPatternID"); return false ;}
    }
    if (true == deserializer.hasGroupProperty("Color"))
    {
        mpColor = new ::Color();
        if (mpColor == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Color"); return false;}
        if (false == mpColor->deSerialize(deserializer)) {return false;}
    }


    return true ;
}

std::string
Light::getElementName()
{
    return EN_LIGHT;
}

bool
Light::clone(::Light *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpLightPower) { delete mpLightPower; mpLightPower = NULL; }

        if ( NULL != pSrcObject->mpLightPower )
        {
            mpLightPower = new ::OnType();
            if (mpLightPower == NULL ) {return false;}
            mpLightPower->value = (pSrcObject->mpLightPower->value );
        }
    }
    {
        if ( NULL != mpDimmingLevel) { delete mpDimmingLevel; mpDimmingLevel = NULL; }

        if ( NULL != pSrcObject->mpDimmingLevel )
        {
            mpDimmingLevel = new uint8_t();
            if (mpDimmingLevel == NULL ) {return false;}
            *mpDimmingLevel = *(pSrcObject->mpDimmingLevel );
        }
    }
    {
        if ( NULL != mpMaxDimmingLevel) { delete mpMaxDimmingLevel; mpMaxDimmingLevel = NULL; }

        if ( NULL != pSrcObject->mpMaxDimmingLevel )
        {
            mpMaxDimmingLevel = new uint8_t();
            if (mpMaxDimmingLevel == NULL ) {return false;}
            *mpMaxDimmingLevel = *(pSrcObject->mpMaxDimmingLevel );
        }
    }
    {
        if ( NULL != mpRelativeDimmingLevel) { delete mpRelativeDimmingLevel; mpRelativeDimmingLevel = NULL; }

        if ( NULL != pSrcObject->mpRelativeDimmingLevel )
        {
            mpRelativeDimmingLevel = new int16_t();
            if (mpRelativeDimmingLevel == NULL ) {return false;}
            *mpRelativeDimmingLevel = *(pSrcObject->mpRelativeDimmingLevel );
        }
    }
    {
        if ( NULL != mpTransitionTime) { delete mpTransitionTime; mpTransitionTime = NULL; }

        if ( NULL != pSrcObject->mpTransitionTime )
        {
            mpTransitionTime = new uint16_t();
            if (mpTransitionTime == NULL ) {return false;}
            *mpTransitionTime = *(pSrcObject->mpTransitionTime );
        }
    }
    {
        if ( NULL != mpActivatePatternID) { delete mpActivatePatternID; mpActivatePatternID = NULL; }

        if ( NULL != pSrcObject->mpActivatePatternID )
        {
            mpActivatePatternID = new ::String50();
            if (mpActivatePatternID == NULL ) {return false;}
            mpActivatePatternID->value = (pSrcObject->mpActivatePatternID->value );
        }
    }
    {
        if ( NULL != mpOnPatternID) { delete mpOnPatternID; mpOnPatternID = NULL; }

        if ( NULL != pSrcObject->mpOnPatternID )
        {
            mpOnPatternID = new ::String50();
            if (mpOnPatternID == NULL ) {return false;}
            mpOnPatternID->value = (pSrcObject->mpOnPatternID->value );
        }
    }
    {
        if ( NULL != mpColor) { delete mpColor; mpColor = NULL; }

        if ( NULL != pSrcObject->mpColor )
        {
            mpColor = new ::Color();
            if (mpColor == NULL ) { return false;}
            if (false == mpColor->clone(pSrcObject->mpColor)) {return false;} ;
        }
    }


    return true ;
}
