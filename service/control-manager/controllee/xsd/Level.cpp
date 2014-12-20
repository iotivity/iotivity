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

#include "Level.h"


#include "Elements.h"

using namespace std;

Level::Level()
{

    mpCurrent = NULL;
    mpDesired = NULL;
    mpIncrement = NULL;
    mpMaximum = NULL;
    mpMinimum = NULL;

}

Level::~Level()
{
    if ( NULL != mpCurrent) { delete mpCurrent; mpCurrent = NULL; }
    if ( NULL != mpDesired) { delete mpDesired; mpDesired = NULL; }
    if ( NULL != mpIncrement) { delete mpIncrement; mpIncrement = NULL; }
    if ( NULL != mpMaximum) { delete mpMaximum; mpMaximum = NULL; }
    if ( NULL != mpMinimum) { delete mpMinimum; mpMinimum = NULL; }

}

bool
Level::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpCurrent )
        {
            bRet = serializer.setPropertyValue("current", *mpCurrent);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("current") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : current"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpDesired )
        {
            bRet = serializer.setPropertyValue("desired", *mpDesired);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("desired") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : desired"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpIncrement )
        {
            bRet = serializer.setPropertyValue("increment", *mpIncrement);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("increment") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : increment"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpMaximum )
        {
            bRet = serializer.setPropertyValue("maximum", *mpMaximum);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("maximum") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : maximum"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpMinimum )
        {
            bRet = serializer.setPropertyValue("minimum", *mpMinimum);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("minimum") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : minimum"); return false; }}
    }


    return true ;
}

bool
Level::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("current"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  current"); return false;}
        mpCurrent = new uint8_t();
        if (mpCurrent == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  current"); return false;}
        if (false == deserializer.getPropertyValue("current", mpCurrent)) { deserializer.setErrorMessage("Failed to deserialize parameter :  current"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("current") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : current"); return false; }}
    if (true == deserializer.hasProperty("desired"))
    {
        mpDesired = new uint8_t();
        if (mpDesired == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  desired"); return false;}
        if (false == deserializer.getPropertyValue("desired", mpDesired)) { deserializer.setErrorMessage("Failed to deserialize parameter :  desired"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("desired") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : desired"); return false; }}
    if (true == deserializer.hasProperty("increment"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  increment"); return false;}
        mpIncrement = new uint8_t();
        if (mpIncrement == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  increment"); return false;}
        if (false == deserializer.getPropertyValue("increment", mpIncrement)) { deserializer.setErrorMessage("Failed to deserialize parameter :  increment"); return false ;}
    }
    if (true == deserializer.hasProperty("maximum"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  maximum"); return false;}
        mpMaximum = new uint8_t();
        if (mpMaximum == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  maximum"); return false;}
        if (false == deserializer.getPropertyValue("maximum", mpMaximum)) { deserializer.setErrorMessage("Failed to deserialize parameter :  maximum"); return false ;}
    }
    if (true == deserializer.hasProperty("minimum"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  minimum"); return false;}
        mpMinimum = new uint8_t();
        if (mpMinimum == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  minimum"); return false;}
        if (false == deserializer.getPropertyValue("minimum", mpMinimum)) { deserializer.setErrorMessage("Failed to deserialize parameter :  minimum"); return false ;}
    }


    return true ;
}

std::string
Level::getElementName()
{
    return EN_LEVEL;
}

bool
Level::clone(::Level *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpCurrent) { delete mpCurrent; mpCurrent = NULL; }

        if ( NULL != pSrcObject->mpCurrent )
        {
            mpCurrent = new uint8_t();
            if (mpCurrent == NULL ) {return false;}
            *mpCurrent = *(pSrcObject->mpCurrent );
        }
    }
    {
        if ( NULL != mpDesired) { delete mpDesired; mpDesired = NULL; }

        if ( NULL != pSrcObject->mpDesired )
        {
            mpDesired = new uint8_t();
            if (mpDesired == NULL ) {return false;}
            *mpDesired = *(pSrcObject->mpDesired );
        }
    }
    {
        if ( NULL != mpIncrement) { delete mpIncrement; mpIncrement = NULL; }

        if ( NULL != pSrcObject->mpIncrement )
        {
            mpIncrement = new uint8_t();
            if (mpIncrement == NULL ) {return false;}
            *mpIncrement = *(pSrcObject->mpIncrement );
        }
    }
    {
        if ( NULL != mpMaximum) { delete mpMaximum; mpMaximum = NULL; }

        if ( NULL != pSrcObject->mpMaximum )
        {
            mpMaximum = new uint8_t();
            if (mpMaximum == NULL ) {return false;}
            *mpMaximum = *(pSrcObject->mpMaximum );
        }
    }
    {
        if ( NULL != mpMinimum) { delete mpMinimum; mpMinimum = NULL; }

        if ( NULL != pSrcObject->mpMinimum )
        {
            mpMinimum = new uint8_t();
            if (mpMinimum == NULL ) {return false;}
            *mpMinimum = *(pSrcObject->mpMinimum );
        }
    }


    return true ;
}
