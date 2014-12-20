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

#include "LongPollingNotification.h"

#include "Elements.h"

using namespace std;

LongPollingNotification::LongPollingNotification()
{

    mpSubscriptionID = NULL;

}

LongPollingNotification::~LongPollingNotification()
{
    if ( NULL != mpSubscriptionID)
    {
        delete mpSubscriptionID;
        mpSubscriptionID = NULL;
    }
    for (std::list< ::Events *>::const_iterator itr = mpEvents.begin();
         itr != mpEvents.end(); itr++)
    {
        delete (*itr);
    }
    mpEvents.clear();

}

bool LongPollingNotification::serializeContents(
    OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::Resource::serializeContents(serializer))
    {
        return false;
    }

    if (serializer.validateMandatory() == true)
    {
        bool bRet = false;

        if ( NULL != mpSubscriptionID)
        {
            if (mpSubscriptionID->value.length() > 0)
            {
                if (true == mpSubscriptionID->validateContent())
                {
                    bRet = serializer.setPropertyValue("subscriptionID",
                                                       mpSubscriptionID->value);
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
            if (serializer.isInFilteringMode("subscriptionID") == false)
            {
                serializer.setErrorMessage(
                    "Failed To Serialize parameter (Invalid content) : subscriptionID");
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
        if (mpEvents.size() < 1)
        {
            if (serializer.isInFilteringMode("Events") == false)
            {
                serializer.setErrorMessage(
                    " parameter item count is less than minOccures value specified in xsd : Events");
                return false;
            }
        }
        serializer.startListProperty("Events");
        for (std::list< ::Events *>::const_iterator itr = mpEvents.begin();
             itr != mpEvents.end(); itr++)
        {
            if (false == (*itr)->serialize(serializer))
            {
                bRet = false;
                break;
            }
        }
        serializer.endListProperty("Events");
        if (false == bRet)
        {
            if (serializer.isInFilteringMode("Events") == false)
            {
                serializer.setErrorMessage(
                    "Failed To Serialize parameter (Invalid content) : Events");
                return false;
            }
        }
    }

    return true;
}

bool LongPollingNotification::deSerializeContents(
    OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::Resource::deSerializeContents(deserializer))
    {
        return false;
    }

    if (true == deserializer.hasProperty("subscriptionID"))
    {
        if (deserializer.validateMandatory() == false)
        {
            deserializer.setErrorMessage(
                "trying to set protected field :  subscriptionID");
            return false;
        }
        mpSubscriptionID = new ::String50();
        if (mpSubscriptionID == NULL)
        {
            deserializer.setErrorMessage(
                "Failed to allocate memrory for parameter :  subscriptionID");
            return false;
        }
        if (false
            == deserializer.getPropertyValue("subscriptionID",
                                             &(mpSubscriptionID->value)))
        {
            deserializer.setErrorMessage(
                "Failed to deserialize parameter :  subscriptionID");
            return false;
        }
        if (false == mpSubscriptionID->validateContent())
        {
            deserializer.setErrorMessage(
                "Parameter Contains Invalid Value : subscriptionID");
            return false;
        }
    }
    else
    {
        if (deserializer.isInFilteringMode("subscriptionID") == false)
        {
            deserializer.setErrorMessage(
                "Mandatory Parameter not found : subscriptionID");
            return false;
        }
    }
    {
        bool isArray = deserializer.getIsArray("Events");
        int sizeOfArray = 0;

        if (deserializer.validateMandatory() == false)
        {
            deserializer.setIDExcemption(true);
        }

        if (isArray)
        {
            sizeOfArray = deserializer.getSize("Events");

            if (sizeOfArray < 1)
            {
                if ((deserializer.isInFilteringMode("Events") == false))
                {
                    deserializer.setErrorMessage(
                        " parameter item count is less than minOccures value specified in xsd : Events");
                    return false;
                }
            }

            if (sizeOfArray > 0)
            {
                for (int i = 0; i < sizeOfArray; i++)
                {
                    ::Events *m = new ::Events();

                    deserializer.setCurrentArrayIndex("Events", i);

                    m->setInstanceName("Events");

                    if (false == m->deSerialize(deserializer))
                    {
                        return false;
                    }

                    mpEvents.push_back(m);
                }
            }
            else if (sizeOfArray == 0)
            {
                deserializer.removeParameter("Events");
            }
        }
        else
        {
            if (true == deserializer.hasProperty("Events"))
            {
                ::Events *m = new ::Events();

                m->deSerialize(deserializer);

                mpEvents.push_back(m);
            }
            else
            {
                if ((deserializer.isInFilteringMode("Events") == false))
                {
                    deserializer.setErrorMessage(
                        "Failed To deserialize : Events");
                    return false;
                }
            }
        }
    }

    return true;
}

std::string LongPollingNotification::getElementName()
{
    return EN_LONGPOLLINGNOTIFICATION;
}

bool LongPollingNotification::clone(::LongPollingNotification *pSrcObject)
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
        if ( NULL != mpSubscriptionID)
        {
            delete mpSubscriptionID;
            mpSubscriptionID = NULL;
        }

        if ( NULL != pSrcObject->mpSubscriptionID)
        {
            mpSubscriptionID = new ::String50();
            if (mpSubscriptionID == NULL)
            {
                return false;
            }
            mpSubscriptionID->value = (pSrcObject->mpSubscriptionID->value);
        }
    }
    {
        for (std::list< ::Events *>::const_iterator itr = mpEvents.begin();
             itr != mpEvents.end(); itr++)
        {
            delete (*itr);
        }
        mpEvents.clear();
        for (std::list< ::Events *>::const_iterator itr =
                 pSrcObject->mpEvents.begin(); itr != pSrcObject->mpEvents.end();
             itr++)
        {
            ::Events *pTemp = new ::Events();
            if (false == pTemp->clone((*itr)))
            {
                delete pTemp;
                return false;
            };
            mpEvents.push_back(pTemp);
        }
    }

    return true;
}
