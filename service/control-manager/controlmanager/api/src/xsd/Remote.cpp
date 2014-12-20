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



#include "Remote.h"


#include "Elements.h"

using namespace std;

Remote::Remote()
{

    mpAuthCode = NULL;
    mpAccountID = NULL;
    mpPeerID = NULL;
    mpPeerGroupID = NULL;
    mpCountryCode = NULL;

}

Remote::~Remote()
{
    if ( NULL != mpAuthCode) { delete mpAuthCode; mpAuthCode = NULL; }
    if ( NULL != mpAccountID) { delete mpAccountID; mpAccountID = NULL; }
    if ( NULL != mpPeerID) { delete mpPeerID; mpPeerID = NULL; }
    if ( NULL != mpPeerGroupID) { delete mpPeerGroupID; mpPeerGroupID = NULL; }
    if ( NULL != mpCountryCode) { delete mpCountryCode; mpCountryCode = NULL; }

}

bool
Remote::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer)) { return false;}

    {
        bool bRet = false;

        if ( NULL != mpAuthCode )
        {
            if (mpAuthCode->value.length() > 0)
            {
                if (true == mpAuthCode->validateContent())
                {
                    bRet = serializer.setPropertyValue("authCode", mpAuthCode->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("authCode") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : authCode"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpAccountID )
        {
            if (mpAccountID->value.length() > 0)
            {
                if (true == mpAccountID->validateContent())
                {
                    bRet = serializer.setPropertyValue("accountID", mpAccountID->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("accountID") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : accountID"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpPeerID )
        {
            if (mpPeerID->value.length() > 0)
            {
                if (true == mpPeerID->validateContent())
                {
                    bRet = serializer.setPropertyValue("peerID", mpPeerID->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("peerID") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : peerID"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpPeerGroupID )
        {
            if (mpPeerGroupID->value.length() > 0)
            {
                if (true == mpPeerGroupID->validateContent())
                {
                    bRet = serializer.setPropertyValue("peerGroupID", mpPeerGroupID->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("peerGroupID") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : peerGroupID"); return false; }}
    }
    {
        bool bRet = false;

        if ( NULL != mpCountryCode )
        {
            if (mpCountryCode->value.length() > 0)
            {
                if (true == mpCountryCode->validateContent())
                {
                    bRet = serializer.setPropertyValue("countryCode", mpCountryCode->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("countryCode") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : countryCode"); return false; }}
    }


    return true ;
}

bool
Remote::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer)) {return false;}

    if (true == deserializer.hasProperty("authCode"))
    {
        mpAuthCode = new ::String30();
        if (mpAuthCode == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  authCode"); return false;}
        if (false == deserializer.getPropertyValue("authCode",
                &(mpAuthCode->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  authCode"); return false ;}
        if (false == mpAuthCode->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : authCode"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("authCode") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : authCode"); return false; }}
    if (true == deserializer.hasProperty("accountID"))
    {
        mpAccountID = new ::String30();
        if (mpAccountID == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  accountID"); return false;}
        if (false == deserializer.getPropertyValue("accountID",
                &(mpAccountID->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  accountID"); return false ;}
        if (false == mpAccountID->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : accountID"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("accountID") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : accountID"); return false; }}
    if (true == deserializer.hasProperty("peerID"))
    {
        mpPeerID = new ::String50();
        if (mpPeerID == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  peerID"); return false;}
        if (false == deserializer.getPropertyValue("peerID",
                &(mpPeerID->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  peerID"); return false ;}
        if (false == mpPeerID->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : peerID"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("peerID") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : peerID"); return false; }}
    if (true == deserializer.hasProperty("peerGroupID"))
    {
        mpPeerGroupID = new ::String30();
        if (mpPeerGroupID == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  peerGroupID"); return false;}
        if (false == deserializer.getPropertyValue("peerGroupID",
                &(mpPeerGroupID->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  peerGroupID"); return false ;}
        if (false == mpPeerGroupID->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : peerGroupID"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("peerGroupID") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : peerGroupID"); return false; }}
    if (true == deserializer.hasProperty("countryCode"))
    {
        mpCountryCode = new ::String10();
        if (mpCountryCode == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  countryCode"); return false;}
        if (false == deserializer.getPropertyValue("countryCode",
                &(mpCountryCode->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  countryCode"); return false ;}
        if (false == mpCountryCode->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : countryCode"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("countryCode") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : countryCode"); return false; }}


    return true ;
}

std::string
Remote::getElementName()
{
    return EN_REMOTE;
}

bool
Remote::clone(::Remote *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::Resource::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpAuthCode) { delete mpAuthCode; mpAuthCode = NULL; }

        if ( NULL != pSrcObject->mpAuthCode )
        {
            mpAuthCode = new ::String30();
            if (mpAuthCode == NULL ) {return false;}
            mpAuthCode->value = (pSrcObject->mpAuthCode->value );
        }
    }
    {
        if ( NULL != mpAccountID) { delete mpAccountID; mpAccountID = NULL; }

        if ( NULL != pSrcObject->mpAccountID )
        {
            mpAccountID = new ::String30();
            if (mpAccountID == NULL ) {return false;}
            mpAccountID->value = (pSrcObject->mpAccountID->value );
        }
    }
    {
        if ( NULL != mpPeerID) { delete mpPeerID; mpPeerID = NULL; }

        if ( NULL != pSrcObject->mpPeerID )
        {
            mpPeerID = new ::String50();
            if (mpPeerID == NULL ) {return false;}
            mpPeerID->value = (pSrcObject->mpPeerID->value );
        }
    }
    {
        if ( NULL != mpPeerGroupID) { delete mpPeerGroupID; mpPeerGroupID = NULL; }

        if ( NULL != pSrcObject->mpPeerGroupID )
        {
            mpPeerGroupID = new ::String30();
            if (mpPeerGroupID == NULL ) {return false;}
            mpPeerGroupID->value = (pSrcObject->mpPeerGroupID->value );
        }
    }
    {
        if ( NULL != mpCountryCode) { delete mpCountryCode; mpCountryCode = NULL; }

        if ( NULL != pSrcObject->mpCountryCode )
        {
            mpCountryCode = new ::String10();
            if (mpCountryCode == NULL ) {return false;}
            mpCountryCode->value = (pSrcObject->mpCountryCode->value );
        }
    }


    return true ;
}
