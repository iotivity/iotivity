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

#include "Detection.h"


#include "Elements.h"

using namespace std;

Detection::Detection()
{

    mpMotionDetectionInUse = NULL;
    mpMotionDetected = NULL;
    mpMotionSensitivity = NULL;
    mpMaxMotionSensitivity = NULL;
    mpSoundDetectionInUse = NULL;
    mpSoundDetected = NULL;
    mpSoundSensitivity = NULL;
    mpMaxSoundSensitivity = NULL;
    mpBarrierDetectionInUse = NULL;
    mpBarrierDetected = NULL;

}

Detection::~Detection()
{
    if ( NULL != mpMotionDetectionInUse) { delete mpMotionDetectionInUse; mpMotionDetectionInUse = NULL; }
    if ( NULL != mpMotionDetected) { delete mpMotionDetected; mpMotionDetected = NULL; }
    if ( NULL != mpMotionSensitivity) { delete mpMotionSensitivity; mpMotionSensitivity = NULL; }
    if ( NULL != mpMaxMotionSensitivity) { delete mpMaxMotionSensitivity; mpMaxMotionSensitivity = NULL; }
    if ( NULL != mpSoundDetectionInUse) { delete mpSoundDetectionInUse; mpSoundDetectionInUse = NULL; }
    if ( NULL != mpSoundDetected) { delete mpSoundDetected; mpSoundDetected = NULL; }
    if ( NULL != mpSoundSensitivity) { delete mpSoundSensitivity; mpSoundSensitivity = NULL; }
    if ( NULL != mpMaxSoundSensitivity) { delete mpMaxSoundSensitivity; mpMaxSoundSensitivity = NULL; }
    if ( NULL != mpBarrierDetectionInUse) { delete mpBarrierDetectionInUse; mpBarrierDetectionInUse = NULL; }
    if ( NULL != mpBarrierDetected) { delete mpBarrierDetected; mpBarrierDetected = NULL; }

}

bool
Detection::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    {
        bool bRet = true;

        if ( NULL != mpMotionDetectionInUse )
        {
            bRet = serializer.setPropertyValue("motionDetectionInUse", *mpMotionDetectionInUse);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("motionDetectionInUse") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : motionDetectionInUse"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpMotionDetected )
        {
            bRet = serializer.setPropertyValue("motionDetected", *mpMotionDetected);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("motionDetected") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : motionDetected"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpMotionSensitivity )
        {
            bRet = serializer.setPropertyValue("motionSensitivity", *mpMotionSensitivity);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("motionSensitivity") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : motionSensitivity"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpMaxMotionSensitivity )
        {
            bRet = serializer.setPropertyValue("maxMotionSensitivity", *mpMaxMotionSensitivity);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("maxMotionSensitivity") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : maxMotionSensitivity"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpSoundDetectionInUse )
        {
            bRet = serializer.setPropertyValue("soundDetectionInUse", *mpSoundDetectionInUse);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("soundDetectionInUse") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : soundDetectionInUse"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpSoundDetected )
        {
            bRet = serializer.setPropertyValue("soundDetected", *mpSoundDetected);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("soundDetected") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : soundDetected"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpSoundSensitivity )
        {
            bRet = serializer.setPropertyValue("soundSensitivity", *mpSoundSensitivity);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("soundSensitivity") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : soundSensitivity"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpMaxSoundSensitivity )
        {
            bRet = serializer.setPropertyValue("maxSoundSensitivity", *mpMaxSoundSensitivity);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("maxSoundSensitivity") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : maxSoundSensitivity"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpBarrierDetectionInUse )
        {
            bRet = serializer.setPropertyValue("barrierDetectionInUse", *mpBarrierDetectionInUse);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("barrierDetectionInUse") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : barrierDetectionInUse"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpBarrierDetected )
        {
            bRet = serializer.setPropertyValue("barrierDetected", *mpBarrierDetected);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("barrierDetected") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : barrierDetected"); return false; }}
    }


    return true ;
}

bool
Detection::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("motionDetectionInUse"))
    {
        mpMotionDetectionInUse = new bool();
        if (mpMotionDetectionInUse == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  motionDetectionInUse"); return false;}
        if (false == deserializer.getPropertyValue("motionDetectionInUse", mpMotionDetectionInUse)) { deserializer.setErrorMessage("Failed to deserialize parameter :  motionDetectionInUse"); return false ;}
    }
    if (true == deserializer.hasProperty("motionDetected"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  motionDetected"); return false;}
        mpMotionDetected = new bool();
        if (mpMotionDetected == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  motionDetected"); return false;}
        if (false == deserializer.getPropertyValue("motionDetected", mpMotionDetected)) { deserializer.setErrorMessage("Failed to deserialize parameter :  motionDetected"); return false ;}
    }
    if (true == deserializer.hasProperty("motionSensitivity"))
    {
        mpMotionSensitivity = new uint8_t();
        if (mpMotionSensitivity == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  motionSensitivity"); return false;}
        if (false == deserializer.getPropertyValue("motionSensitivity", mpMotionSensitivity)) { deserializer.setErrorMessage("Failed to deserialize parameter :  motionSensitivity"); return false ;}
    }
    if (true == deserializer.hasProperty("maxMotionSensitivity"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  maxMotionSensitivity"); return false;}
        mpMaxMotionSensitivity = new uint8_t();
        if (mpMaxMotionSensitivity == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  maxMotionSensitivity"); return false;}
        if (false == deserializer.getPropertyValue("maxMotionSensitivity", mpMaxMotionSensitivity)) { deserializer.setErrorMessage("Failed to deserialize parameter :  maxMotionSensitivity"); return false ;}
    }
    if (true == deserializer.hasProperty("soundDetectionInUse"))
    {
        mpSoundDetectionInUse = new bool();
        if (mpSoundDetectionInUse == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  soundDetectionInUse"); return false;}
        if (false == deserializer.getPropertyValue("soundDetectionInUse", mpSoundDetectionInUse)) { deserializer.setErrorMessage("Failed to deserialize parameter :  soundDetectionInUse"); return false ;}
    }
    if (true == deserializer.hasProperty("soundDetected"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  soundDetected"); return false;}
        mpSoundDetected = new bool();
        if (mpSoundDetected == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  soundDetected"); return false;}
        if (false == deserializer.getPropertyValue("soundDetected", mpSoundDetected)) { deserializer.setErrorMessage("Failed to deserialize parameter :  soundDetected"); return false ;}
    }
    if (true == deserializer.hasProperty("soundSensitivity"))
    {
        mpSoundSensitivity = new uint8_t();
        if (mpSoundSensitivity == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  soundSensitivity"); return false;}
        if (false == deserializer.getPropertyValue("soundSensitivity", mpSoundSensitivity)) { deserializer.setErrorMessage("Failed to deserialize parameter :  soundSensitivity"); return false ;}
    }
    if (true == deserializer.hasProperty("maxSoundSensitivity"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  maxSoundSensitivity"); return false;}
        mpMaxSoundSensitivity = new uint8_t();
        if (mpMaxSoundSensitivity == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  maxSoundSensitivity"); return false;}
        if (false == deserializer.getPropertyValue("maxSoundSensitivity", mpMaxSoundSensitivity)) { deserializer.setErrorMessage("Failed to deserialize parameter :  maxSoundSensitivity"); return false ;}
    }
    if (true == deserializer.hasProperty("barrierDetectionInUse"))
    {
        mpBarrierDetectionInUse = new bool();
        if (mpBarrierDetectionInUse == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  barrierDetectionInUse"); return false;}
        if (false == deserializer.getPropertyValue("barrierDetectionInUse", mpBarrierDetectionInUse)) { deserializer.setErrorMessage("Failed to deserialize parameter :  barrierDetectionInUse"); return false ;}
    }
    if (true == deserializer.hasProperty("barrierDetected"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  barrierDetected"); return false;}
        mpBarrierDetected = new bool();
        if (mpBarrierDetected == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  barrierDetected"); return false;}
        if (false == deserializer.getPropertyValue("barrierDetected", mpBarrierDetected)) { deserializer.setErrorMessage("Failed to deserialize parameter :  barrierDetected"); return false ;}
    }


    return true ;
}

std::string
Detection::getElementName()
{
    return EN_DETECTION;
}

bool
Detection::clone(::Detection *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpMotionDetectionInUse) { delete mpMotionDetectionInUse; mpMotionDetectionInUse = NULL; }

        if ( NULL != pSrcObject->mpMotionDetectionInUse )
        {
            mpMotionDetectionInUse = new bool();
            if (mpMotionDetectionInUse == NULL ) {return false;}
            *mpMotionDetectionInUse = *(pSrcObject->mpMotionDetectionInUse );
        }
    }
    {
        if ( NULL != mpMotionDetected) { delete mpMotionDetected; mpMotionDetected = NULL; }

        if ( NULL != pSrcObject->mpMotionDetected )
        {
            mpMotionDetected = new bool();
            if (mpMotionDetected == NULL ) {return false;}
            *mpMotionDetected = *(pSrcObject->mpMotionDetected );
        }
    }
    {
        if ( NULL != mpMotionSensitivity) { delete mpMotionSensitivity; mpMotionSensitivity = NULL; }

        if ( NULL != pSrcObject->mpMotionSensitivity )
        {
            mpMotionSensitivity = new uint8_t();
            if (mpMotionSensitivity == NULL ) {return false;}
            *mpMotionSensitivity = *(pSrcObject->mpMotionSensitivity );
        }
    }
    {
        if ( NULL != mpMaxMotionSensitivity) { delete mpMaxMotionSensitivity; mpMaxMotionSensitivity = NULL; }

        if ( NULL != pSrcObject->mpMaxMotionSensitivity )
        {
            mpMaxMotionSensitivity = new uint8_t();
            if (mpMaxMotionSensitivity == NULL ) {return false;}
            *mpMaxMotionSensitivity = *(pSrcObject->mpMaxMotionSensitivity );
        }
    }
    {
        if ( NULL != mpSoundDetectionInUse) { delete mpSoundDetectionInUse; mpSoundDetectionInUse = NULL; }

        if ( NULL != pSrcObject->mpSoundDetectionInUse )
        {
            mpSoundDetectionInUse = new bool();
            if (mpSoundDetectionInUse == NULL ) {return false;}
            *mpSoundDetectionInUse = *(pSrcObject->mpSoundDetectionInUse );
        }
    }
    {
        if ( NULL != mpSoundDetected) { delete mpSoundDetected; mpSoundDetected = NULL; }

        if ( NULL != pSrcObject->mpSoundDetected )
        {
            mpSoundDetected = new bool();
            if (mpSoundDetected == NULL ) {return false;}
            *mpSoundDetected = *(pSrcObject->mpSoundDetected );
        }
    }
    {
        if ( NULL != mpSoundSensitivity) { delete mpSoundSensitivity; mpSoundSensitivity = NULL; }

        if ( NULL != pSrcObject->mpSoundSensitivity )
        {
            mpSoundSensitivity = new uint8_t();
            if (mpSoundSensitivity == NULL ) {return false;}
            *mpSoundSensitivity = *(pSrcObject->mpSoundSensitivity );
        }
    }
    {
        if ( NULL != mpMaxSoundSensitivity) { delete mpMaxSoundSensitivity; mpMaxSoundSensitivity = NULL; }

        if ( NULL != pSrcObject->mpMaxSoundSensitivity )
        {
            mpMaxSoundSensitivity = new uint8_t();
            if (mpMaxSoundSensitivity == NULL ) {return false;}
            *mpMaxSoundSensitivity = *(pSrcObject->mpMaxSoundSensitivity );
        }
    }
    {
        if ( NULL != mpBarrierDetectionInUse) { delete mpBarrierDetectionInUse; mpBarrierDetectionInUse = NULL; }

        if ( NULL != pSrcObject->mpBarrierDetectionInUse )
        {
            mpBarrierDetectionInUse = new bool();
            if (mpBarrierDetectionInUse == NULL ) {return false;}
            *mpBarrierDetectionInUse = *(pSrcObject->mpBarrierDetectionInUse );
        }
    }
    {
        if ( NULL != mpBarrierDetected) { delete mpBarrierDetected; mpBarrierDetected = NULL; }

        if ( NULL != pSrcObject->mpBarrierDetected )
        {
            mpBarrierDetected = new bool();
            if (mpBarrierDetected == NULL ) {return false;}
            *mpBarrierDetected = *(pSrcObject->mpBarrierDetected );
        }
    }


    return true ;
}
