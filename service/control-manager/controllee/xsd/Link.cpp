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

#include "Link.h"


#include "Elements.h"

using namespace std;

Link::Link()
{

    mpHref = NULL;

}

Link::~Link()
{
    if ( NULL != mpHref) { delete mpHref; mpHref = NULL; }

}

bool
Link::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpHref )
        {
            if (mpHref->length() > 0)
            {
                bRet = serializer.setPropertyValue("href", *mpHref);
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("href") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : href"); return false; }}
    }


    return true ;
}

bool
Link::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (true == deserializer.hasProperty("href"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  href"); return false;}
        mpHref = new std::string();
        if (mpHref == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  href"); return false;}
        if (false == deserializer.getPropertyValue("href", mpHref)) { deserializer.setErrorMessage("Failed to deserialize parameter :  href"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("href") == false) {deserializer.setErrorMessage("Mandatory Parameter not found : href"); return false; }}


    return true ;
}

std::string
Link::getElementName()
{
    return EN_LINK;
}

bool
Link::clone(::Link *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    {
        if ( NULL != mpHref) { delete mpHref; mpHref = NULL; }

        if ( NULL != pSrcObject->mpHref )
        {
            mpHref = new std::string();
            if (mpHref == NULL ) {return false;}
            *mpHref = *(pSrcObject->mpHref );
        }
    }


    return true ;
}
