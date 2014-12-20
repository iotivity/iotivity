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

#include "Mode.h"


#include "Elements.h"

using namespace std;

Mode::Mode()
{

    mpSupportedModes = NULL;
    mpModes = NULL;

}

Mode::~Mode()
{
    if ( NULL != mpSupportedModes) { delete mpSupportedModes; mpSupportedModes = NULL; }
    if ( NULL != mpModes) { delete mpModes; mpModes = NULL; }

}

bool
Mode::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpSupportedModes )
        {
            mpSupportedModes->setInstanceName("supportedModes");
            bRet = mpSupportedModes->serializeContents(serializer) ;
        }
        if (false == bRet) { if (serializer.isInFilteringMode("supportedModes") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : supportedModes"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpModes )
        {
            mpModes->setInstanceName("modes");
            bRet = mpModes->serializeContents(serializer) ;
        }
        if (false == bRet) { if (serializer.isInFilteringMode("modes") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : modes"); return false; }}
    }


    return true ;
}

bool
Mode::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    {
        if (true == deserializer.hasProperty("supportedModes"))
        {
            if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  supportedModes"); return false;}
            mpSupportedModes = new ::ArrayOfString();
            if (mpSupportedModes == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  supportedModes"); return false;}
            mpSupportedModes->setInstanceName("supportedModes");
            if (false == mpSupportedModes->deSerializeContents(deserializer)) {  deserializer.setErrorMessage("Parameter Contains Invalid Value : supportedModes"); return false ;}
        }
        else { if (deserializer.isInFilteringMode("supportedModes") == false) { deserializer.setErrorMessage("Mandatory Parameter not found :  supportedModes"); return false; }}
    }
    {
        if (true == deserializer.hasProperty("modes"))
        {
            mpModes = new ::ArrayOfString();
            if (mpModes == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  modes"); return false;}
            mpModes->setInstanceName("modes");
            if (false == mpModes->deSerializeContents(deserializer)) {  deserializer.setErrorMessage("Parameter Contains Invalid Value : modes"); return false ;}
        }
        else { if (deserializer.isInFilteringMode("modes") == false) { deserializer.setErrorMessage("Mandatory Parameter not found :  modes"); return false; }}
    }


    return true ;
}

std::string
Mode::getElementName()
{
    return EN_MODE;
}

bool
Mode::clone(::Mode *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpSupportedModes) { delete mpSupportedModes; mpSupportedModes = NULL; }

        if ( NULL != pSrcObject->mpSupportedModes )
        {
            mpSupportedModes = new ::ArrayOfString();
            if (mpSupportedModes == NULL ) { return false;}
            mpSupportedModes->members = pSrcObject->mpSupportedModes->members;
        }
    }
    {
        if ( NULL != mpModes) { delete mpModes; mpModes = NULL; }

        if ( NULL != pSrcObject->mpModes )
        {
            mpModes = new ::ArrayOfString();
            if (mpModes == NULL ) { return false;}
            mpModes->members = pSrcObject->mpModes->members;
        }
    }


    return true ;
}
