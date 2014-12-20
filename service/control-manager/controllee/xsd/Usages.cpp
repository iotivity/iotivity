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

#include "Usages.h"

#include "Elements.h"

using namespace std;

Usages::Usages()
{

    mpStartTime = NULL;
    mpEndTime = NULL;
    mpPeriodType = NULL;
    mpUnit = NULL;
    mpDeviceID = NULL;
    mpGroupID = NULL;

}

Usages::~Usages()
{
    if ( NULL != mpStartTime)
    {
        delete mpStartTime;
        mpStartTime = NULL;
    }
    if ( NULL != mpEndTime)
    {
        delete mpEndTime;
        mpEndTime = NULL;
    }
    if ( NULL != mpPeriodType)
    {
        delete mpPeriodType;
        mpPeriodType = NULL;
    }
    if ( NULL != mpUnit)
    {
        delete mpUnit;
        mpUnit = NULL;
    }
    if ( NULL != mpDeviceID)
    {
        delete mpDeviceID;
        mpDeviceID = NULL;
    }
    if ( NULL != mpGroupID)
    {
        delete mpGroupID;
        mpGroupID = NULL;
    }
    for (std::list< ::Usage *>::const_iterator itr = mpUsage.begin();
         itr != mpUsage.end(); itr++)
    {
        delete (*itr);
    }
    mpUsage.clear();

}

bool Usages::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer))
    {
        return false;
    }

    {
        bool bRet = false;

        if ( NULL != mpStartTime)
        {
            if (mpStartTime->value.length() > 0)
            {
                if (true == mpStartTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("startTime",
                                                       mpStartTime->value);
                }
                else
                {
                    bRet = false;
                }
            }
            else
            {
                bRet = false;
            }
        }
        if (false == bRet)
        {
            if (serializer.isInFilteringMode("startTime") == false)
            {
                serializer.setErrorMessage(
                    "Failed To Serialize parameter (Invalid content) : startTime");
                return false;
            }
        }
    }
    {
        bool bRet = false;

        if ( NULL != mpEndTime)
        {
            if (mpEndTime->value.length() > 0)
            {
                if (true == mpEndTime->validateContent())
                {
                    bRet = serializer.setPropertyValue("endTime",
                                                       mpEndTime->value);
                }
                else
                {
                    bRet = false;
                }
            }
            else
            {
                bRet = false;
            }
        }
        if (false == bRet)
        {
            if (serializer.isInFilteringMode("endTime") == false)
            {
                serializer.setErrorMessage(
                    "Failed To Serialize parameter (Invalid content) : endTime");
                return false;
            }
        }
    }
    {
        bool bRet = false;

        if ( NULL != mpPeriodType)
        {
            if (mpPeriodType->value.length() > 0)
            {
                if (true == mpPeriodType->validateContent())
                {
                    bRet = serializer.setPropertyValue("periodType",
                                                       mpPeriodType->value);
                }
                else
                {
                    bRet = false;
                }
            }
            else
            {
                bRet = false;
            }
        }
        if (false == bRet)
        {
            if (serializer.isInFilteringMode("periodType") == false)
            {
                serializer.setErrorMessage(
                    "Failed To Serialize parameter (Invalid content) : periodType");
                return false;
            }
        }
    }
    {
        bool bRet = true;

        if ( NULL != mpUnit)
        {
            if (mpUnit->value.length() > 0)
            {
                if (true == mpUnit->validateContent())
                {
                    bRet = serializer.setPropertyValue("unit", mpUnit->value);
                }
                else
                {
                    bRet = false;
                }
            }
        }
        if (false == bRet)
        {
            if (serializer.isInFilteringMode("unit") == false)
            {
                serializer.setErrorMessage(
                    "Failed To Serialize parameter (Invalid content) : unit");
                return false;
            }
        }
    }
    {
        bool bRet = true;

        if ( NULL != mpDeviceID)
        {
            if (mpDeviceID->value.length() > 0)
            {
                if (true == mpDeviceID->validateContent())
                {
                    bRet = serializer.setPropertyValue("deviceID",
                                                       mpDeviceID->value);
                }
                else
                {
                    bRet = false;
                }
            }
        }
        if (false == bRet)
        {
            if (serializer.isInFilteringMode("deviceID") == false)
            {
                serializer.setErrorMessage(
                    "Failed To Serialize parameter (Invalid content) : deviceID");
                return false;
            }
        }
    }
    {
        bool bRet = true;

        if ( NULL != mpGroupID)
        {
            if (mpGroupID->value.length() > 0)
            {
                if (true == mpGroupID->validateContent())
                {
                    bRet = serializer.setPropertyValue("groupID",
                                                       mpGroupID->value);
                }
                else
                {
                    bRet = false;
                }
            }
        }
        if (false == bRet)
        {
            if (serializer.isInFilteringMode("groupID") == false)
            {
                serializer.setErrorMessage(
                    "Failed To Serialize parameter (Invalid content) : groupID");
                return false;
            }
        }
    }
    {
        bool bRet = true;
        if (serializer.validateMandatory() == false)
        {
            serializer.setIDExcemption(true);
        }

        if (mpUsage.size() > 0)
        {
            serializer.startListProperty("Usage");
            for (std::list< ::Usage *>::const_iterator itr = mpUsage.begin();
                 itr != mpUsage.end(); itr++)
            {
                if (false == (*itr)->serialize(serializer))
                {
                    bRet = false;
                    break;
                }
            }
            serializer.endListProperty("Usage");
            if (false == bRet)
            {
                if (serializer.isInFilteringMode("Usage") == false)
                {
                    serializer.setErrorMessage(
                        "Failed To Serialize parameter (Invalid content) : Usage");
                    return false;
                }
            }

        }
    }

    return true;
}

bool Usages::deSerializeContents(
    OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer))
    {
        return false;
    }

    if (true == deserializer.hasProperty("startTime"))
    {
        mpStartTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpStartTime == NULL)
        {
            deserializer.setErrorMessage(
                "Failed to allocate memrory for parameter :  startTime");
            return false;
        }
        if (false
            == deserializer.getPropertyValue("startTime",
                                             &(mpStartTime->value)))
        {
            deserializer.setErrorMessage(
                "Failed to deserialize parameter :  startTime");
            return false;
        }
        if (false == mpStartTime->validateContent())
        {
            deserializer.setErrorMessage(
                "Parameter Contains Invalid Value : startTime");
            return false;
        }
    }
    else
    {
        if (deserializer.isInFilteringMode("startTime") == false)
        {
            deserializer.setErrorMessage(
                "Mandatory Parameter not found : startTime");
            return false;
        }
    }
    if (true == deserializer.hasProperty("endTime"))
    {
        mpEndTime = new OC::Cm::Serialization::Xsd::DateTimeType();
        if (mpEndTime == NULL)
        {
            deserializer.setErrorMessage(
                "Failed to allocate memrory for parameter :  endTime");
            return false;
        }
        if (false
            == deserializer.getPropertyValue("endTime",
                                             &(mpEndTime->value)))
        {
            deserializer.setErrorMessage(
                "Failed to deserialize parameter :  endTime");
            return false;
        }
        if (false == mpEndTime->validateContent())
        {
            deserializer.setErrorMessage(
                "Parameter Contains Invalid Value : endTime");
            return false;
        }
    }
    else
    {
        if (deserializer.isInFilteringMode("endTime") == false)
        {
            deserializer.setErrorMessage(
                "Mandatory Parameter not found : endTime");
            return false;
        }
    }
    if (true == deserializer.hasProperty("periodType"))
    {
        mpPeriodType = new ::PeriodType();
        if (mpPeriodType == NULL)
        {
            deserializer.setErrorMessage(
                "Failed to allocate memrory for parameter :  periodType");
            return false;
        }
        if (false
            == deserializer.getPropertyValue("periodType",
                                             &(mpPeriodType->value)))
        {
            deserializer.setErrorMessage(
                "Failed to deserialize parameter :  periodType");
            return false;
        }
        if (false == mpPeriodType->validateContent())
        {
            deserializer.setErrorMessage(
                "Parameter Contains Invalid Value : periodType");
            return false;
        }
    }
    else
    {
        if (deserializer.isInFilteringMode("periodType") == false)
        {
            deserializer.setErrorMessage(
                "Mandatory Parameter not found : periodType");
            return false;
        }
    }
    if (true == deserializer.hasProperty("unit"))
    {
        mpUnit = new ::PowerUnitType();
        if (mpUnit == NULL)
        {
            deserializer.setErrorMessage(
                "Failed to allocate memrory for parameter :  unit");
            return false;
        }
        if (false == deserializer.getPropertyValue("unit", &(mpUnit->value)))
        {
            deserializer.setErrorMessage(
                "Failed to deserialize parameter :  unit");
            return false;
        }
        if (false == mpUnit->validateContent())
        {
            deserializer.setErrorMessage(
                "Parameter Contains Invalid Value : unit");
            return false;
        }
    }
    if (true == deserializer.hasProperty("deviceID"))
    {
        mpDeviceID = new OC::Cm::Serialization::Xsd::UuidType();
        if (mpDeviceID == NULL)
        {
            deserializer.setErrorMessage(
                "Failed to allocate memrory for parameter :  deviceID");
            return false;
        }
        if (false
            == deserializer.getPropertyValue("deviceID",
                                             &(mpDeviceID->value)))
        {
            deserializer.setErrorMessage(
                "Failed to deserialize parameter :  deviceID");
            return false;
        }
        if (false == mpDeviceID->validateContent())
        {
            deserializer.setErrorMessage(
                "Parameter Contains Invalid Value : deviceID");
            return false;
        }
    }
    if (true == deserializer.hasProperty("groupID"))
    {
        mpGroupID = new ::String50();
        if (mpGroupID == NULL)
        {
            deserializer.setErrorMessage(
                "Failed to allocate memrory for parameter :  groupID");
            return false;
        }
        if (false
            == deserializer.getPropertyValue("groupID",
                                             &(mpGroupID->value)))
        {
            deserializer.setErrorMessage(
                "Failed to deserialize parameter :  groupID");
            return false;
        }
        if (false == mpGroupID->validateContent())
        {
            deserializer.setErrorMessage(
                "Parameter Contains Invalid Value : groupID");
            return false;
        }
    }
    {
        bool isArray = deserializer.getIsArray("Usage");
        int sizeOfArray = 0;

        if (deserializer.validateMandatory() == false)
        {
            deserializer.setIDExcemption(true);
        }

        if (isArray)
        {
            sizeOfArray = deserializer.getSize("Usage");

            if (sizeOfArray > 0)
            {
                for (int i = 0; i < sizeOfArray; i++)
                {
                    ::Usage *m = new ::Usage();

                    deserializer.setCurrentArrayIndex("Usage", i);

                    m->setInstanceName("Usage");

                    if (false == m->deSerialize(deserializer))
                    {
                        return false;
                    }

                    mpUsage.push_back(m);
                }
            }
            else if (sizeOfArray == 0)
            {
                deserializer.removeParameter("Usage");
            }
        }
        else
        {
            if (true == deserializer.hasProperty("Usage"))
            {
                ::Usage *m = new ::Usage();

                if (false == m->deSerialize(deserializer))
                {
                    return false;
                }

                mpUsage.push_back(m);
            }
            else
            {

            }
        }
    }

    return true;
}

std::string Usages::getElementName()
{
    return EN_USAGES;
}

bool Usages::clone(::Usages *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false;
    }

    if (false == ::Resource::clone(pSrcObject))
    {
        return false;
    }

    {
        if ( NULL != mpStartTime)
        {
            delete mpStartTime;
            mpStartTime = NULL;
        }

        if ( NULL != pSrcObject->mpStartTime)
        {
            mpStartTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (mpStartTime == NULL)
            {
                return false;
            }
            mpStartTime->value = (pSrcObject->mpStartTime->value);
        }
    }
    {
        if ( NULL != mpEndTime)
        {
            delete mpEndTime;
            mpEndTime = NULL;
        }

        if ( NULL != pSrcObject->mpEndTime)
        {
            mpEndTime = new OC::Cm::Serialization::Xsd::DateTimeType();
            if (mpEndTime == NULL)
            {
                return false;
            }
            mpEndTime->value = (pSrcObject->mpEndTime->value);
        }
    }
    {
        if ( NULL != mpPeriodType)
        {
            delete mpPeriodType;
            mpPeriodType = NULL;
        }

        if ( NULL != pSrcObject->mpPeriodType)
        {
            mpPeriodType = new ::PeriodType();
            if (mpPeriodType == NULL)
            {
                return false;
            }
            mpPeriodType->value = (pSrcObject->mpPeriodType->value);
        }
    }
    {
        if ( NULL != mpUnit)
        {
            delete mpUnit;
            mpUnit = NULL;
        }

        if ( NULL != pSrcObject->mpUnit)
        {
            mpUnit = new ::PowerUnitType();
            if (mpUnit == NULL)
            {
                return false;
            }
            mpUnit->value = (pSrcObject->mpUnit->value);
        }
    }
    {
        if ( NULL != mpDeviceID)
        {
            delete mpDeviceID;
            mpDeviceID = NULL;
        }

        if ( NULL != pSrcObject->mpDeviceID)
        {
            mpDeviceID = new OC::Cm::Serialization::Xsd::UuidType();
            if (mpDeviceID == NULL)
            {
                return false;
            }
            mpDeviceID->value = (pSrcObject->mpDeviceID->value);
        }
    }
    {
        if ( NULL != mpGroupID)
        {
            delete mpGroupID;
            mpGroupID = NULL;
        }

        if ( NULL != pSrcObject->mpGroupID)
        {
            mpGroupID = new ::String50();
            if (mpGroupID == NULL)
            {
                return false;
            }
            mpGroupID->value = (pSrcObject->mpGroupID->value);
        }
    }
    {
        for (std::list< ::Usage *>::const_iterator itr = mpUsage.begin();
             itr != mpUsage.end(); itr++)
        {
            delete (*itr);
        }
        mpUsage.clear();
        for (std::list< ::Usage *>::const_iterator itr =
                 pSrcObject->mpUsage.begin(); itr != pSrcObject->mpUsage.end();
             itr++)
        {
            ::Usage *pTemp = new ::Usage();
            if (false == pTemp->clone((*itr)))
            {
                delete pTemp;
                return false;
            };
            mpUsage.push_back(pTemp);
        }
    }

    return true;
}
