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



#include "Record.h"


#include "Elements.h"

using namespace std;

Record::Record()
{

    mpDeviceIDs = NULL;
    mpGroupIDs = NULL;
    mpEventTime = NULL;
    mpEventDescription = NULL;

}

Record::~Record()
{
    if ( NULL != mpDeviceIDs) { delete mpDeviceIDs; mpDeviceIDs = NULL; }
    if ( NULL != mpGroupIDs) { delete mpGroupIDs; mpGroupIDs = NULL; }
    if ( NULL != mpEventTime) { delete mpEventTime; mpEventTime = NULL; }
    if ( NULL != mpEventDescription) { delete mpEventDescription; mpEventDescription = NULL; }

}

bool
Record::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::IdentifiedObject::serializeContents(serializer)) { return false;}

    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpDeviceIDs )
        {
            mpDeviceIDs->setInstanceName("deviceIDs");
            bRet = mpDeviceIDs->serializeContents(serializer) ;
        }
        if (false == bRet) { if (serializer.isInFilteringMode("deviceIDs") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : deviceIDs"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpGroupIDs )
        {
            mpGroupIDs->setInstanceName("groupIDs");
            bRet = mpGroupIDs->serializeContents(serializer) ;
        }
        if (false == bRet) { if (serializer.isInFilteringMode("groupIDs") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : groupIDs"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpEventTime )
        {
            if (mpEventTime->value.length() > 0)
            {
                if (true == mpEventTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("eventTime", mpEventTime->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("eventTime") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : eventTime"); return false; }}
    }
    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpEventDescription )
        {
            if (mpEventDescription->value.length() > 0)
            {
                if (true == mpEventDescription->validateContent())
                {
                    bRet = serializer.setPropertyValue("eventDescription", mpEventDescription->value);
                }
                else { bRet = false ;}
            }
            else { bRet = false;}
        }
        if (false == bRet) { if (serializer.isInFilteringMode("eventDescription") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : eventDescription"); return false; }}
    }


    return true ;
}

bool
Record::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::IdentifiedObject::deSerializeContents(deserializer)) {return false;}

    {
        if (true == deserializer.hasProperty("deviceIDs"))
        {
            if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  deviceIDs"); return false;}
            mpDeviceIDs = new ::ArrayOfUuid();
            if (mpDeviceIDs == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  deviceIDs"); return false;}
            mpDeviceIDs->setInstanceName("deviceIDs");
            if (false == mpDeviceIDs->deSerializeContents(deserializer)) {  deserializer.setErrorMessage("Parameter Contains Invalid Value : deviceIDs"); return false ;}
        }
    }
    {
        if (true == deserializer.hasProperty("groupIDs"))
        {
            if (deserializer.validateMandatory() == false) {deserializer.setErrorMessage("trying to set protected field :  groupIDs"); return false;}
            mpGroupIDs = new ::ArrayOfString();
            if (mpGroupIDs == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  groupIDs"); return false;}
            mpGroupIDs->setInstanceName("groupIDs");
            if (false == mpGroupIDs->deSerializeContents(deserializer)) {  deserializer.setErrorMessage("Parameter Contains Invalid Value : groupIDs"); return false ;}
        }
    }
    if (true == deserializer.hasProperty("eventTime"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  eventTime"); return false;}
        mpEventTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpEventTime == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  eventTime"); return false;}
        if (false == deserializer.getPropertyValue("eventTime",
                &(mpEventTime->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  eventTime"); return false ;}
        if (false == mpEventTime->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : eventTime"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("eventTime") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : eventTime"); return false; }}
    if (true == deserializer.hasProperty("eventDescription"))
    {
        if (deserializer.validateMandatory() == false) { deserializer.setErrorMessage("trying to set protected field :  eventDescription"); return false;}
        mpEventDescription = new ::String200();
        if (mpEventDescription == NULL ) { deserializer.setErrorMessage("Failed to allocate memrory for parameter :  eventDescription"); return false;}
        if (false == deserializer.getPropertyValue("eventDescription",
                &(mpEventDescription->value))) {deserializer.setErrorMessage("Failed to deserialize parameter :  eventDescription"); return false ;}
        if (false == mpEventDescription->validateContent()) { deserializer.setErrorMessage("Parameter Contains Invalid Value : eventDescription"); return false ;}
    }
    else { if (deserializer.isInFilteringMode("eventDescription") == false) { deserializer.setErrorMessage("Mandatory Parameter not found : eventDescription"); return false; }}


    return true ;
}

std::string
Record::getElementName()
{
    return EN_RECORD;
}

bool
Record::clone(::Record *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    if (false == ::IdentifiedObject::clone(pSrcObject)) {return false;}

    {
        if ( NULL != mpDeviceIDs) { delete mpDeviceIDs; mpDeviceIDs = NULL; }

        if ( NULL != pSrcObject->mpDeviceIDs )
        {
            mpDeviceIDs = new ::ArrayOfUuid();
            if (mpDeviceIDs == NULL ) { return false;}
            mpDeviceIDs->members = pSrcObject->mpDeviceIDs->members;
        }
    }
    {
        if ( NULL != mpGroupIDs) { delete mpGroupIDs; mpGroupIDs = NULL; }

        if ( NULL != pSrcObject->mpGroupIDs )
        {
            mpGroupIDs = new ::ArrayOfString();
            if (mpGroupIDs == NULL ) { return false;}
            mpGroupIDs->members = pSrcObject->mpGroupIDs->members;
        }
    }
    {
        if ( NULL != mpEventTime) { delete mpEventTime; mpEventTime = NULL; }

        if ( NULL != pSrcObject->mpEventTime )
        {
            mpEventTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (mpEventTime == NULL ) {return false;}
            mpEventTime->value = (pSrcObject->mpEventTime->value );
        }
    }
    {
        if ( NULL != mpEventDescription) { delete mpEventDescription; mpEventDescription = NULL; }

        if ( NULL != pSrcObject->mpEventDescription )
        {
            mpEventDescription = new ::String200();
            if (mpEventDescription == NULL ) {return false;}
            mpEventDescription->value = (pSrcObject->mpEventDescription->value );
        }
    }


    return true ;
}
