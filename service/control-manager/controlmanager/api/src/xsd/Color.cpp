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



#include "Color.h"


#include "Elements.h"

using namespace std;

Color::Color()
{

    mpHue = NULL;
    mpRelativeHue = NULL;
    mpSaturation = NULL;
    mpRelativeSaturation = NULL;
    mpChromaticityX = NULL;
    mpChromaticityY = NULL;
    mpColorTemperature = NULL;

}

Color::~Color()
{
    if ( NULL != mpHue) { delete mpHue; mpHue = NULL; }
    if ( NULL != mpRelativeHue) { delete mpRelativeHue; mpRelativeHue = NULL; }
    if ( NULL != mpSaturation) { delete mpSaturation; mpSaturation = NULL; }
    if ( NULL != mpRelativeSaturation) { delete mpRelativeSaturation; mpRelativeSaturation = NULL; }
    if ( NULL != mpChromaticityX) { delete mpChromaticityX; mpChromaticityX = NULL; }
    if ( NULL != mpChromaticityY) { delete mpChromaticityY; mpChromaticityY = NULL; }
    if ( NULL != mpColorTemperature) { delete mpColorTemperature; mpColorTemperature = NULL; }

}

bool
Color::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    {
        bool bRet = false;

        if ( NULL != mpHue )
        {
            bRet = serializer.setPropertyValue("hue", *mpHue);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("hue") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : hue"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpRelativeHue )
        {
            bRet = serializer.setPropertyValue("relativeHue", *mpRelativeHue);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("relativeHue") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : relativeHue"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpSaturation )
        {
            bRet = serializer.setPropertyValue("saturation", *mpSaturation);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("saturation") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : saturation"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpRelativeSaturation )
        {
            bRet = serializer.setPropertyValue("relativeSaturation", *mpRelativeSaturation);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("relativeSaturation") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : relativeSaturation"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpChromaticityX )
        {
            bRet = serializer.setPropertyValue("chromaticityX", *mpChromaticityX);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("chromaticityX") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : chromaticityX"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpChromaticityY )
        {
            bRet = serializer.setPropertyValue("chromaticityY", *mpChromaticityY);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("chromaticityY") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : chromaticityY"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpColorTemperature )
        {
            bRet = serializer.setPropertyValue("colorTemperature", *mpColorTemperature);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("colorTemperature") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : colorTemperature"); return false; }}
    }


    return true ;
}

bool
Color::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (true == deserializer.hasProperty("hue"))
    {
        mpHue = new uint8_t();
        if (mpHue == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  hue"); return false;}
        if (false == deserializer.getPropertyValue("hue", mpHue)) { deserializer.setErrorMessage("Failed to deserialize parameter :  hue"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("hue") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : hue"); return false; }}
    if (true == deserializer.hasProperty("relativeHue"))
    {
        mpRelativeHue = new int16_t();
        if (mpRelativeHue == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  relativeHue"); return false;}
        if (false == deserializer.getPropertyValue("relativeHue", mpRelativeHue)) { deserializer.setErrorMessage("Failed to deserialize parameter :  relativeHue"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("relativeHue") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : relativeHue"); return false; }}
    if (true == deserializer.hasProperty("saturation"))
    {
        mpSaturation = new uint8_t();
        if (mpSaturation == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  saturation"); return false;}
        if (false == deserializer.getPropertyValue("saturation", mpSaturation)) { deserializer.setErrorMessage("Failed to deserialize parameter :  saturation"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("saturation") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : saturation"); return false; }}
    if (true == deserializer.hasProperty("relativeSaturation"))
    {
        mpRelativeSaturation = new int16_t();
        if (mpRelativeSaturation == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  relativeSaturation"); return false;}
        if (false == deserializer.getPropertyValue("relativeSaturation", mpRelativeSaturation)) { deserializer.setErrorMessage("Failed to deserialize parameter :  relativeSaturation"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("relativeSaturation") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : relativeSaturation"); return false; }}
    if (true == deserializer.hasProperty("chromaticityX"))
    {
        mpChromaticityX = new uint16_t();
        if (mpChromaticityX == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  chromaticityX"); return false;}
        if (false == deserializer.getPropertyValue("chromaticityX", mpChromaticityX)) { deserializer.setErrorMessage("Failed to deserialize parameter :  chromaticityX"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("chromaticityX") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : chromaticityX"); return false; }}
    if (true == deserializer.hasProperty("chromaticityY"))
    {
        mpChromaticityY = new uint16_t();
        if (mpChromaticityY == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  chromaticityY"); return false;}
        if (false == deserializer.getPropertyValue("chromaticityY", mpChromaticityY)) { deserializer.setErrorMessage("Failed to deserialize parameter :  chromaticityY"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("chromaticityY") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : chromaticityY"); return false; }}
    if (true == deserializer.hasProperty("colorTemperature"))
    {
        mpColorTemperature = new uint16_t();
        if (mpColorTemperature == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  colorTemperature"); return false;}
        if (false == deserializer.getPropertyValue("colorTemperature", mpColorTemperature)) { deserializer.setErrorMessage("Failed to deserialize parameter :  colorTemperature"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("colorTemperature") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : colorTemperature"); return false; }}


    return true ;
}

std::string
Color::getElementName()
{
    return EN_COLOR;
}

bool
Color::clone(::Color *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    {
        if ( NULL != mpHue) { delete mpHue; mpHue = NULL; }

        if ( NULL != pSrcObject->mpHue )
        {
            mpHue = new uint8_t();
            if (mpHue == NULL ) {return false;}
            *mpHue = *(pSrcObject->mpHue );
        }
    }
    {
        if ( NULL != mpRelativeHue) { delete mpRelativeHue; mpRelativeHue = NULL; }

        if ( NULL != pSrcObject->mpRelativeHue )
        {
            mpRelativeHue = new int16_t();
            if (mpRelativeHue == NULL ) {return false;}
            *mpRelativeHue = *(pSrcObject->mpRelativeHue );
        }
    }
    {
        if ( NULL != mpSaturation) { delete mpSaturation; mpSaturation = NULL; }

        if ( NULL != pSrcObject->mpSaturation )
        {
            mpSaturation = new uint8_t();
            if (mpSaturation == NULL ) {return false;}
            *mpSaturation = *(pSrcObject->mpSaturation );
        }
    }
    {
        if ( NULL != mpRelativeSaturation) { delete mpRelativeSaturation; mpRelativeSaturation = NULL; }

        if ( NULL != pSrcObject->mpRelativeSaturation )
        {
            mpRelativeSaturation = new int16_t();
            if (mpRelativeSaturation == NULL ) {return false;}
            *mpRelativeSaturation = *(pSrcObject->mpRelativeSaturation );
        }
    }
    {
        if ( NULL != mpChromaticityX) { delete mpChromaticityX; mpChromaticityX = NULL; }

        if ( NULL != pSrcObject->mpChromaticityX )
        {
            mpChromaticityX = new uint16_t();
            if (mpChromaticityX == NULL ) {return false;}
            *mpChromaticityX = *(pSrcObject->mpChromaticityX );
        }
    }
    {
        if ( NULL != mpChromaticityY) { delete mpChromaticityY; mpChromaticityY = NULL; }

        if ( NULL != pSrcObject->mpChromaticityY )
        {
            mpChromaticityY = new uint16_t();
            if (mpChromaticityY == NULL ) {return false;}
            *mpChromaticityY = *(pSrcObject->mpChromaticityY );
        }
    }
    {
        if ( NULL != mpColorTemperature) { delete mpColorTemperature; mpColorTemperature = NULL; }

        if ( NULL != pSrcObject->mpColorTemperature )
        {
            mpColorTemperature = new uint16_t();
            if (mpColorTemperature == NULL ) {return false;}
            *mpColorTemperature = *(pSrcObject->mpColorTemperature );
        }
    }


    return true ;
}
