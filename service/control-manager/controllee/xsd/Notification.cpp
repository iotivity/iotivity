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

#include "Notification.h"

#include "Elements.h"

using namespace std;

Notification::Notification()
{

    mpSubscriptionURI = NULL;

}

Notification::~Notification()
{
    if ( NULL != mpSubscriptionURI)
    {
        delete mpSubscriptionURI;
        mpSubscriptionURI = NULL;
    }
    for (std::list< ::Events *>::const_iterator itr = mpEvents.begin();
         itr != mpEvents.end(); itr++)
    {
        delete (*itr);
    }
    mpEvents.clear();

}

bool Notification::serializeContents(
    OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::IdentifiedObject::serializeContents(serializer))
    {
        return false;
    }

    if (serializer.validateMandatory() == true)
    {
        bool bRet = true;

        if ( NULL != mpSubscriptionURI)
        {
            if (mpSubscriptionURI->length() > 0)
            {
                bRet = serializer.setPropertyValue("subscriptionURI",
                                                   *mpSubscriptionURI);
            }
        }
        if (false == bRet)
        {
            if (serializer.isInFilteringMode("subscriptionURI") == false)
            {
                serializer.setErrorMessage(
                    "Failed To Serialize parameter (Invalid content) : subscriptionURI");
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

bool Notification::deSerializeContents(
    OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::IdentifiedObject::deSerializeContents(deserializer))
    {
        return false;
    }

    if (true == deserializer.hasProperty("subscriptionURI"))
    {
        if (deserializer.validateMandatory() == false)
        {
            deserializer.setErrorMessage(
                "trying to set protected field :  subscriptionURI");
            return false;
        }
        mpSubscriptionURI = new std::string();
        if (mpSubscriptionURI == NULL)
        {
            deserializer.setErrorMessage(
                "Failed to allocate memrory for parameter :  subscriptionURI");
            return false;
        }
        if (false
            == deserializer.getPropertyValue("subscriptionURI",
                                             mpSubscriptionURI))
        {
            deserializer.setErrorMessage(
                "Failed to deserialize parameter :  subscriptionURI");
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

std::string Notification::getElementName()
{
    return EN_NOTIFICATION;
}

bool Notification::clone(::Notification *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false;
    }

    if (false == ::IdentifiedObject::clone(pSrcObject))
    {
        return false;
    }

    {
        if ( NULL != mpSubscriptionURI)
        {
            delete mpSubscriptionURI;
            mpSubscriptionURI = NULL;
        }

        if ( NULL != pSrcObject->mpSubscriptionURI)
        {
            mpSubscriptionURI = new std::string();
            if (mpSubscriptionURI == NULL)
            {
                return false;
            }
            *mpSubscriptionURI = *(pSrcObject->mpSubscriptionURI);
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
