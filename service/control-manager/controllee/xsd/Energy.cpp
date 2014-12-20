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

#include "Energy.h"


#include "Elements.h"

using namespace std;

Energy::Energy()
{

    mpUsages = NULL;
    mpUsageLimits = NULL;

}

Energy::~Energy()
{
    if ( NULL != mpUsages) { delete mpUsages; mpUsages = NULL; }
    if ( NULL != mpUsageLimits) { delete mpUsageLimits; mpUsageLimits = NULL; }

}

bool
Energy::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpUsages )
            {
                bRet = mpUsages->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("Usages") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Usages"); return false; }}
        }

    }   if (serializer.isRecursive())
    {
        {
            bool bRet = true;

            if ( NULL != mpUsageLimits )
            {
                bRet = mpUsageLimits->serialize(serializer);
            }
            if (false == bRet) { if (serializer.isInFilteringMode("UsageLimits") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : UsageLimits"); return false; }}
        }

    }

    return true ;
}

bool
Energy::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("Usages"))
        {
            mpUsages = new ::Usages();
            if (mpUsages == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  Usages"); return false;}
            if (false == mpUsages->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("Usages")) {deserializer.setErrorMessage("Additional data to deserialize: Usages"); return false; } }
    if (deserializer.isRecursive())
    {
        if (true == deserializer.hasGroupProperty("UsageLimits"))
        {
            mpUsageLimits = new ::UsageLimits();
            if (mpUsageLimits == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  UsageLimits"); return false;}
            if (false == mpUsageLimits->deSerialize(deserializer)) {return false;}
        }

    }
    else
        {   if (true == deserializer.hasProperty("UsageLimits")) {deserializer.setErrorMessage("Additional data to deserialize: UsageLimits"); return false; } }


    return true ;
}

std::string
Energy::getElementName()
{
    return EN_ENERGY;
}

bool
Energy::clone(::Energy *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    {
        if ( NULL != mpUsages) { delete mpUsages; mpUsages = NULL; }

        if ( NULL != pSrcObject->mpUsages )
        {
            mpUsages = new ::Usages();
            if (mpUsages == NULL ) { return false;}
            if (false == mpUsages->clone(pSrcObject->mpUsages)) {return false;} ;
        }
    }
    {
        if ( NULL != mpUsageLimits) { delete mpUsageLimits; mpUsageLimits = NULL; }

        if ( NULL != pSrcObject->mpUsageLimits )
        {
            mpUsageLimits = new ::UsageLimits();
            if (mpUsageLimits == NULL ) { return false;}
            if (false == mpUsageLimits->clone(pSrcObject->mpUsageLimits)) {return false;} ;
        }
    }


    return true ;
}
