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



#include "Information.h"


#include "Elements.h"

using namespace std;

Information::Information()
{

    mpDescription = NULL;
    mpManufacturer = NULL;
    mpDeviceSubType = NULL;
    mpModelID = NULL;
    mpSerialNumber = NULL;
    mpSalesLocation = NULL;
    mpSpecification = NULL;
    mpVersions = NULL;

}

Information::~Information()
{
    if ( NULL != mpDescription) { delete mpDescription; mpDescription = NULL; }
    if ( NULL != mpManufacturer) { delete mpManufacturer; mpManufacturer = NULL; }
    if ( NULL != mpDeviceSubType) { delete mpDeviceSubType; mpDeviceSubType = NULL; }
    if ( NULL != mpModelID) { delete mpModelID; mpModelID = NULL; }
    if ( NULL != mpSerialNumber) { delete mpSerialNumber; mpSerialNumber = NULL; }
    if ( NULL != mpSalesLocation) { delete mpSalesLocation; mpSalesLocation = NULL; }
    if ( NULL != mpSpecification) { delete mpSpecification; mpSpecification = NULL; }
    if ( NULL != mpVersions) { delete mpVersions; mpVersions = NULL; }

}

bool
Information::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpDescription )
        {
            if (mpDescription->value.length() > 0)
            {
                if (true == mpDescription->validateContent())
                {
                    bRet = serializer.setPropertyValue("description", mpDescription->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("description") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : description"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpManufacturer )
        {
            if (mpManufacturer->value.length() > 0)
            {
                if (true == mpManufacturer->validateContent())
                {
                    bRet = serializer.setPropertyValue("manufacturer", mpManufacturer->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("manufacturer") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : manufacturer"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpDeviceSubType )
        {
            if (mpDeviceSubType->value.length() > 0)
            {
                if (true == mpDeviceSubType->validateContent())
                {
                    bRet = serializer.setPropertyValue("deviceSubType", mpDeviceSubType->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("deviceSubType") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : deviceSubType"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpModelID )
        {
            if (mpModelID->value.length() > 0)
            {
                if (true == mpModelID->validateContent())
                {
                    bRet = serializer.setPropertyValue("modelID", mpModelID->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("modelID") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : modelID"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpSerialNumber )
        {
            if (mpSerialNumber->value.length() > 0)
            {
                if (true == mpSerialNumber->validateContent())
                {
                    bRet = serializer.setPropertyValue("serialNumber", mpSerialNumber->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("serialNumber") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : serialNumber"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpSalesLocation )
        {
            if (mpSalesLocation->value.length() > 0)
            {
                if (true == mpSalesLocation->validateContent())
                {
                    bRet = serializer.setPropertyValue("salesLocation", mpSalesLocation->value);
                }
                else { bRet = false ;}
            }
        }
        if (false == bRet) { if (serializer.isInFilteringMode("salesLocation") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : salesLocation"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpSpecification )
        {
            bRet = mpSpecification->serialize(serializer);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("Specification") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Specification"); return false; }}
    }
    if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpVersions )
            {
                bRet = mpVersions->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Versions") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Versions"); return false; }}
        }

    }

    return true ;
}

bool
Information::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("description"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  description"); return false;}
        mpDescription = new ::String100();
        if (mpDescription == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  description"); return false;}
        if (false == deserializer.getPropertyValue("description",
                &(mpDescription->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  description"); return false ;}
        if (false == mpDescription->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : description"); return false ;}
    }
    if (true == deserializer.hasProperty("manufacturer"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  manufacturer"); return false;}
        mpManufacturer = new ::String20();
        if (mpManufacturer == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  manufacturer"); return false;}
        if (false == deserializer.getPropertyValue("manufacturer",
                &(mpManufacturer->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  manufacturer"); return false ;}
        if (false == mpManufacturer->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : manufacturer"); return false ;}
    }
    if (true == deserializer.hasProperty("deviceSubType"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  deviceSubType"); return false;}
        mpDeviceSubType = new ::String30();
        if (mpDeviceSubType == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  deviceSubType"); return false;}
        if (false == deserializer.getPropertyValue("deviceSubType",
                &(mpDeviceSubType->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  deviceSubType"); return false ;}
        if (false == mpDeviceSubType->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : deviceSubType"); return false ;}
    }
    if (true == deserializer.hasProperty("modelID"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  modelID"); return false;}
        mpModelID = new ::String20();
        if (mpModelID == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  modelID"); return false;}
        if (false == deserializer.getPropertyValue("modelID",
                &(mpModelID->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  modelID"); return false ;}
        if (false == mpModelID->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : modelID"); return false ;}
    }
    if (true == deserializer.hasProperty("serialNumber"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  serialNumber"); return false;}
        mpSerialNumber = new ::String50();
        if (mpSerialNumber == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  serialNumber"); return false;}
        if (false == deserializer.getPropertyValue("serialNumber",
                &(mpSerialNumber->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  serialNumber"); return false ;}
        if (false == mpSerialNumber->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : serialNumber"); return false ;}
    }
    if (true == deserializer.hasProperty("salesLocation"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  salesLocation"); return false;}
        mpSalesLocation = new ::LocationType();
        if (mpSalesLocation == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  salesLocation"); return false;}
        if (false == deserializer.getPropertyValue("salesLocation",
                &(mpSalesLocation->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  salesLocation"); return false ;}
        if (false == mpSalesLocation->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : salesLocation"); return false ;}
    }
    if (true == deserializer.hasGroupProperty("Specification"))
    {
        mpSpecification = new ::Specification();
        if (mpSpecification == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Specification"); return false;}
        if (false == mpSpecification->deSerialize(deserializer)) {return false;}
    }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Versions"))
        {
            mpVersions = new ::Versions();
            if (mpVersions == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Versions"); return false;}
            if (false == mpVersions->deSerialize(deserializer)) {return false;}
        }

    }
    else
    {   if (true == deserializer.hasProperty("Versions")) {deserializer.setErrorMessage("Additional data to deserialize: Versions"); return false; } }


    return true ;
}

std::string
Information::getElementName()
{
    return EN_INFORMATION;
}

bool
Information::clone(::Information *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpDescription) { delete mpDescription; mpDescription = NULL; }

        if ( NULL != pSrcObject->mpDescription )
        {
            mpDescription = new ::String100();
            if (mpDescription == NULL ) {return false;}
            mpDescription->value = (pSrcObject->mpDescription->value );
        }
    }
    {
        if ( NULL != mpManufacturer) { delete mpManufacturer; mpManufacturer = NULL; }

        if ( NULL != pSrcObject->mpManufacturer )
        {
            mpManufacturer = new ::String20();
            if (mpManufacturer == NULL ) {return false;}
            mpManufacturer->value = (pSrcObject->mpManufacturer->value );
        }
    }
    {
        if ( NULL != mpDeviceSubType) { delete mpDeviceSubType; mpDeviceSubType = NULL; }

        if ( NULL != pSrcObject->mpDeviceSubType )
        {
            mpDeviceSubType = new ::String30();
            if (mpDeviceSubType == NULL ) {return false;}
            mpDeviceSubType->value = (pSrcObject->mpDeviceSubType->value );
        }
    }
    {
        if ( NULL != mpModelID) { delete mpModelID; mpModelID = NULL; }

        if ( NULL != pSrcObject->mpModelID )
        {
            mpModelID = new ::String20();
            if (mpModelID == NULL ) {return false;}
            mpModelID->value = (pSrcObject->mpModelID->value );
        }
    }
    {
        if ( NULL != mpSerialNumber) { delete mpSerialNumber; mpSerialNumber = NULL; }

        if ( NULL != pSrcObject->mpSerialNumber )
        {
            mpSerialNumber = new ::String50();
            if (mpSerialNumber == NULL ) {return false;}
            mpSerialNumber->value = (pSrcObject->mpSerialNumber->value );
        }
    }
    {
        if ( NULL != mpSalesLocation) { delete mpSalesLocation; mpSalesLocation = NULL; }

        if ( NULL != pSrcObject->mpSalesLocation )
        {
            mpSalesLocation = new ::LocationType();
            if (mpSalesLocation == NULL ) {return false;}
            mpSalesLocation->value = (pSrcObject->mpSalesLocation->value );
        }
    }
    {
        if ( NULL != mpSpecification) { delete mpSpecification; mpSpecification = NULL; }

        if ( NULL != pSrcObject->mpSpecification )
        {
            mpSpecification = new ::Specification();
            if (mpSpecification == NULL ) { return false;}
            if (false == mpSpecification->clone(pSrcObject->mpSpecification)) {return false;} ;
        }
    }
    {
        if ( NULL != mpVersions) { delete mpVersions; mpVersions = NULL; }

        if ( NULL != pSrcObject->mpVersions )
        {
            mpVersions = new ::Versions();
            if (mpVersions == NULL ) { return false;}
            if (false == mpVersions->clone(pSrcObject->mpVersions)) {return false;} ;
        }
    }


    return true ;
}
