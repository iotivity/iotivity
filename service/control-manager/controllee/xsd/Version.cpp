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

#include "Version.h"


#include "Elements.h"

using namespace std;

Version::Version()
{

    mpType = NULL;
    mpName = NULL;
    mpNumber = NULL;
    mpNewVersionAvailable = NULL;
    mpUserAgreement = NULL;

}

Version::~Version()
{
    if ( NULL != mpType) { delete mpType; mpType = NULL; }
    if ( NULL != mpName) { delete mpName; mpName = NULL; }
    if ( NULL != mpNumber) { delete mpNumber; mpNumber = NULL; }
    if ( NULL != mpNewVersionAvailable) { delete mpNewVersionAvailable; mpNewVersionAvailable = NULL; }
    if ( NULL != mpUserAgreement) { delete mpUserAgreement; mpUserAgreement = NULL; }

}

bool
Version::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::IdentifiedObject::serializeContents(serializer)) { return false;}

    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpType )
        {
            if (mpType->value.length() > 0)
            {
                if (true == mpType->validateContent())
                {
                    bRet = serializer.setPropertyValue("type", mpType->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("type") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : type"); return false; }}
    }
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
        bool bRet = false;

        if ( NULL != mpNumber )
        {
            if (mpNumber->value.length() > 0)
            {
                if (true == mpNumber->validateContent())
                {
                    bRet = serializer.setPropertyValue("number", mpNumber->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("number") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : number"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpNewVersionAvailable )
        {
            bRet = serializer.setPropertyValue("newVersionAvailable", *mpNewVersionAvailable);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("newVersionAvailable") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : newVersionAvailable"); return false; }}
    }
    {
        bool bRet = true;

        if ( NULL != mpUserAgreement )
        {
            bRet = serializer.setPropertyValue("userAgreement", *mpUserAgreement);
        }
        if (false == bRet) { if (serializer.isInFilteringMode("userAgreement") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : userAgreement"); return false; }}
    }


    return true ;
}

bool
Version::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::IdentifiedObject::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("type"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  type"); return false;}
        mpType = new ::VersionType();
        if (mpType == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  type"); return false;}
        if (false == deserializer.getPropertyValue("type", &(mpType->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  type"); return false ;}
        if (false == mpType->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : type"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("type") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : type"); return false; }}
    if (true == deserializer.hasProperty("name"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  name"); return false;}
        mpName = new ::String50();
        if (mpName == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  name"); return false;}
        if (false == deserializer.getPropertyValue("name", &(mpName->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  name"); return false ;}
        if (false == mpName->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : name"); return false ;}
    }
    if (true == deserializer.hasProperty("number"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  number"); return false;}
        mpNumber = new ::String20();
        if (mpNumber == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  number"); return false;}
        if (false == deserializer.getPropertyValue("number", &(mpNumber->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  number"); return false ;}
        if (false == mpNumber->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : number"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("number") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : number"); return false; }}
    if (true == deserializer.hasProperty("newVersionAvailable"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  newVersionAvailable"); return false;}
        mpNewVersionAvailable = new bool();
        if (mpNewVersionAvailable == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  newVersionAvailable"); return false;}
        if (false == deserializer.getPropertyValue("newVersionAvailable", mpNewVersionAvailable)) { deserializer.setErrorMessage("Failed to deserialize parameter :  newVersionAvailable"); return false ;}
    }
    if (true == deserializer.hasProperty("userAgreement"))
    {
        mpUserAgreement = new bool();
        if (mpUserAgreement == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  userAgreement"); return false;}
        if (false == deserializer.getPropertyValue("userAgreement", mpUserAgreement)) { deserializer.setErrorMessage("Failed to deserialize parameter :  userAgreement"); return false ;}
    }


    return true ;
}

std::string
Version::getElementName()
{
    return EN_VERSION;
}

bool
Version::clone(::Version *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::IdentifiedObject::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpType) { delete mpType; mpType = NULL; }

        if ( NULL != pSrcObject->mpType )
        {
            mpType = new ::VersionType();
            if (mpType == NULL ) {return false;}
            mpType->value = (pSrcObject->mpType->value );
        }
    }
    {
        if ( NULL != mpName) { delete mpName; mpName = NULL; }

        if ( NULL != pSrcObject->mpName )
        {
            mpName = new ::String50();
            if (mpName == NULL ) {return false;}
            mpName->value = (pSrcObject->mpName->value );
        }
    }
    {
        if ( NULL != mpNumber) { delete mpNumber; mpNumber = NULL; }

        if ( NULL != pSrcObject->mpNumber )
        {
            mpNumber = new ::String20();
            if (mpNumber == NULL ) {return false;}
            mpNumber->value = (pSrcObject->mpNumber->value );
        }
    }
    {
        if ( NULL != mpNewVersionAvailable) { delete mpNewVersionAvailable; mpNewVersionAvailable = NULL; }

        if ( NULL != pSrcObject->mpNewVersionAvailable )
        {
            mpNewVersionAvailable = new bool();
            if (mpNewVersionAvailable == NULL ) {return false;}
            *mpNewVersionAvailable = *(pSrcObject->mpNewVersionAvailable );
        }
    }
    {
        if ( NULL != mpUserAgreement) { delete mpUserAgreement; mpUserAgreement = NULL; }

        if ( NULL != pSrcObject->mpUserAgreement )
        {
            mpUserAgreement = new bool();
            if (mpUserAgreement == NULL ) {return false;}
            *mpUserAgreement = *(pSrcObject->mpUserAgreement );
        }
    }


    return true ;
}
