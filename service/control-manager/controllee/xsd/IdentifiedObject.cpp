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

#include "IdentifiedObject.h"


#include "Elements.h"

using namespace std;

IdentifiedObject::IdentifiedObject()
{

    mpId = NULL;
    mpDescription = NULL;

}

IdentifiedObject::~IdentifiedObject()
{
    if ( NULL != mpId) { delete mpId; mpId = NULL; }
    if ( NULL != mpDescription) { delete mpDescription; mpDescription = NULL; }

}

bool
IdentifiedObject::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    if ((serializer.validateMandatory() == true) || ((serializer.validateMandatory() == false) && (serializer.getIDExcemption())))
    {
        bool bRet = false;

        if ( NULL != mpId )
        {
            if (mpId->value.length() > 0)
            {
                if (true == mpId->validateContent())
                {
                    bRet = serializer.setPropertyValue("id", mpId->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("id") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : id"); return false; }}
    }
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


    return true ;
}

bool
IdentifiedObject::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("id"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  id"); return false;}
        mpId = new ::String50();
        if (mpId == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  id"); return false;}
        if (false == deserializer.getPropertyValue("id", &(mpId->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  id"); return false ;}
        if (false == mpId->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : id"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("id") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : id"); return false; }}
    if (true == deserializer.hasProperty("description"))
    {
        mpDescription = new ::String50();
        if (mpDescription == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  description"); return false;}
        if (false == deserializer.getPropertyValue("description", &(mpDescription->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  description"); return false ;}
        if (false == mpDescription->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : description"); return false ;}
    }


    return true ;
}

std::string
IdentifiedObject::getElementName()
{
    return EN_IDENTIFIEDOBJECT;
}

bool
IdentifiedObject::clone(::IdentifiedObject *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpId) { delete mpId; mpId = NULL; }

        if ( NULL != pSrcObject->mpId )
        {
            mpId = new ::String50();
            if (mpId == NULL ) {return false;}
            mpId->value = (pSrcObject->mpId->value );
        }
    }
    {
        if ( NULL != mpDescription) { delete mpDescription; mpDescription = NULL; }

        if ( NULL != pSrcObject->mpDescription )
        {
            mpDescription = new ::String50();
            if (mpDescription == NULL ) {return false;}
            mpDescription->value = (pSrcObject->mpDescription->value );
        }
    }


    return true ;
}
