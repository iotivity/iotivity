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

#include "Temperature.h"


#include "Elements.h"

using namespace std;

Temperature::Temperature()
{

    mpName = NULL;
    mpCurrent = NULL;
    mpDesired = NULL;
    mpIncrement = NULL;
    mpMaximum = NULL;
    mpMinimum = NULL;
    mpUnit = NULL;

}

Temperature::~Temperature()
{
    if ( NULL != mpName) { delete mpName; mpName = NULL; }
    if ( NULL != mpCurrent) { delete mpCurrent; mpCurrent = NULL; }
    if ( NULL != mpDesired) { delete mpDesired; mpDesired = NULL; }
    if ( NULL != mpIncrement) { delete mpIncrement; mpIncrement = NULL; }
    if ( NULL != mpMaximum) { delete mpMaximum; mpMaximum = NULL; }
    if ( NULL != mpMinimum) { delete mpMinimum; mpMinimum = NULL; }
    if ( NULL != mpUnit) { delete mpUnit; mpUnit = NULL; }

}

bool
Temperature::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::IdentifiedObject::serializeContents(serializer)) { return false;}

    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpName )
        {
            if (mpName->value.length() > 0)
            {
                if (true == mpName->validateContent())
                {
                    bRet = serializer.setPropertyValue("name", mpName->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("name") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : name"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpCurrent )
        {
            if (true == mpCurrent->validateContent())
            {
                bRet = serializer.setPropertyValue("current", mpCurrent->value);
            }
            else { bRet = false ;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("current") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : current"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpDesired )
        {
            if (true == mpDesired->validateContent())
            {
                bRet = serializer.setPropertyValue("desired", mpDesired->value);
            }
            else { bRet = false ;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("desired") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : desired"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpIncrement )
        {
            if (true == mpIncrement->validateContent())
            {
                bRet = serializer.setPropertyValue("increment", mpIncrement->value);
            }
            else { bRet = false ;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("increment") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : increment"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpMaximum )
        {
            if (true == mpMaximum->validateContent())
            {
                bRet = serializer.setPropertyValue("maximum", mpMaximum->value);
            }
            else { bRet = false ;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("maximum") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : maximum"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpMinimum )
        {
            if (true == mpMinimum->validateContent())
            {
                bRet = serializer.setPropertyValue("minimum", mpMinimum->value);
            }
            else { bRet = false ;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("minimum") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : minimum"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpUnit )
        {
            if (mpUnit->value.length() > 0)
            {
                if (true == mpUnit->validateContent())
                {
                    bRet = serializer.setPropertyValue("unit", mpUnit->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("unit") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : unit"); return false; }}
    }


    return true ;
}

bool
Temperature::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::IdentifiedObject::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("name"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  name"); return false;}
        mpName = new ::String10();
        if (mpName == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  name"); return false;}
        if (false == deserializer.getPropertyValue("name", &(mpName->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  name"); return false ;}
        if (false == mpName->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : name"); return false ;}
    }
    if (true == deserializer.hasProperty("current"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  current"); return false;}
        mpCurrent = new ::Float32();
        if (mpCurrent == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  current"); return false;}
        if (false == deserializer.getPropertyValue("current", &(mpCurrent->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  current"); return false ;}
        if (false == mpCurrent->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : current"); return false ;}
    }
    if (true == deserializer.hasProperty("desired"))
    {
        mpDesired = new ::Float32();
        if (mpDesired == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  desired"); return false;}
        if (false == deserializer.getPropertyValue("desired", &(mpDesired->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  desired"); return false ;}
        if (false == mpDesired->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : desired"); return false ;}
    }
    if (true == deserializer.hasProperty("increment"))
    {
        mpIncrement = new ::Float32();
        if (mpIncrement == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  increment"); return false;}
        if (false == deserializer.getPropertyValue("increment", &(mpIncrement->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  increment"); return false ;}
        if (false == mpIncrement->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : increment"); return false ;}
    }
    if (true == deserializer.hasProperty("maximum"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  maximum"); return false;}
        mpMaximum = new ::Float32();
        if (mpMaximum == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  maximum"); return false;}
        if (false == deserializer.getPropertyValue("maximum", &(mpMaximum->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  maximum"); return false ;}
        if (false == mpMaximum->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : maximum"); return false ;}
    }
    if (true == deserializer.hasProperty("minimum"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  minimum"); return false;}
        mpMinimum = new ::Float32();
        if (mpMinimum == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  minimum"); return false;}
        if (false == deserializer.getPropertyValue("minimum", &(mpMinimum->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  minimum"); return false ;}
        if (false == mpMinimum->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : minimum"); return false ;}
    }
    if (true == deserializer.hasProperty("unit"))
    {
        mpUnit = new ::TemperatureUnitType();
        if (mpUnit == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  unit"); return false;}
        if (false == deserializer.getPropertyValue("unit", &(mpUnit->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  unit"); return false ;}
        if (false == mpUnit->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : unit"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("unit") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : unit"); return false; }}


    return true ;
}

std::string
Temperature::getElementName()
{
    return EN_TEMPERATURE;
}

bool
Temperature::clone(::Temperature *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::IdentifiedObject::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpName) { delete mpName; mpName = NULL; }

        if ( NULL != pSrcObject->mpName )
        {
            mpName = new ::String10();
            if (mpName == NULL ) {return false;}
            mpName->value = (pSrcObject->mpName->value );
        }
    }
    {
        if ( NULL != mpCurrent) { delete mpCurrent; mpCurrent = NULL; }

        if ( NULL != pSrcObject->mpCurrent )
        {
            mpCurrent = new ::Float32();
            if (mpCurrent == NULL ) {return false;}
            mpCurrent->value = (pSrcObject->mpCurrent->value );
        }
    }
    {
        if ( NULL != mpDesired) { delete mpDesired; mpDesired = NULL; }

        if ( NULL != pSrcObject->mpDesired )
        {
            mpDesired = new ::Float32();
            if (mpDesired == NULL ) {return false;}
            mpDesired->value = (pSrcObject->mpDesired->value );
        }
    }
    {
        if ( NULL != mpIncrement) { delete mpIncrement; mpIncrement = NULL; }

        if ( NULL != pSrcObject->mpIncrement )
        {
            mpIncrement = new ::Float32();
            if (mpIncrement == NULL ) {return false;}
            mpIncrement->value = (pSrcObject->mpIncrement->value );
        }
    }
    {
        if ( NULL != mpMaximum) { delete mpMaximum; mpMaximum = NULL; }

        if ( NULL != pSrcObject->mpMaximum )
        {
            mpMaximum = new ::Float32();
            if (mpMaximum == NULL ) {return false;}
            mpMaximum->value = (pSrcObject->mpMaximum->value );
        }
    }
    {
        if ( NULL != mpMinimum) { delete mpMinimum; mpMinimum = NULL; }

        if ( NULL != pSrcObject->mpMinimum )
        {
            mpMinimum = new ::Float32();
            if (mpMinimum == NULL ) {return false;}
            mpMinimum->value = (pSrcObject->mpMinimum->value );
        }
    }
    {
        if ( NULL != mpUnit) { delete mpUnit; mpUnit = NULL; }

        if ( NULL != pSrcObject->mpUnit )
        {
            mpUnit = new ::TemperatureUnitType();
            if (mpUnit == NULL ) {return false;}
            mpUnit->value = (pSrcObject->mpUnit->value );
        }
    }


    return true ;
}
