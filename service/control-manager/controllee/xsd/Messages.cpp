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

#include "Messages.h"

#include "Elements.h"

using namespace std;

Messages::Messages()
{
    setList(true);

}

Messages::~Messages()
{
    for (std::list< ::Message *>::const_iterator itr = mpMessage.begin();
         itr != mpMessage.end(); itr++)
    {
        delete (*itr);
    }
    mpMessage.clear();

}

bool Messages::serializeContents(
    OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::List::serializeContents(serializer))
    {
        return false;
    }

    {
        bool bRet = true;

        if (mpMessage.size() > 0)
        {
            if (serializer.validateMandatory() == false)
            {
                serializer.setIDExcemption(true);
            }
            serializer.startListProperty("Messages");
            for (std::list< ::Message *>::const_iterator itr = mpMessage.begin();
                 itr != mpMessage.end(); itr++)
            {
                (*itr)->setInstanceName("Messages");
                if (false == (*itr)->serialize(serializer))
                {
                    bRet = false;
                    break;
                }
            }
            serializer.endListProperty("Messages");
            if (false == bRet)
            {
                if (serializer.isInFilteringMode("Messages") == false)
                {
                    serializer.setErrorMessage(
                        "Failed To Serialize parameter (Invalid content) : Messages");
                    return false;
                }
            }
        }

    }

    return true;
}

bool Messages::deSerializeContents(
    OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::List::deSerializeContents(deserializer))
    {
        return false;
    }

    {
        bool isArray = deserializer.getIsArray("Messages");
        int sizeOfArray = 0;

        if (deserializer.validateMandatory() == false)
        {
            deserializer.setIDExcemption(true);
        }

        if (isArray)
        {
            sizeOfArray = deserializer.getSize("Messages");

            if (sizeOfArray > 0)
            {
                for (int i = 0; i < sizeOfArray; i++)
                {
                    ::Message *m = new ::Message();

                    deserializer.setCurrentArrayIndex("Messages", i);

                    m->setInstanceName("Messages");

                    if (false == m->deSerialize(deserializer))
                    {
                        return false;
                    }

                    mpMessage.push_back(m);
                }
            }
            else if (sizeOfArray == 0)
            {
                deserializer.removeParameter("Messages");
            }
        }
        else
        {
            if (true == deserializer.hasProperty("Messages"))
            {
                ::Message *m = new ::Message();

                m->deSerialize(deserializer);

                mpMessage.push_back(m);
            }
            else
            {
                if ((deserializer.isInFilteringMode("Messages") == false))
                {
                    deserializer.setErrorMessage(
                        "Failed To deserialize : Messages");
                    return false;
                }
            }
        }
    }

    return true;
}

std::string Messages::getElementName()
{
    return EN_MESSAGES;
}

bool Messages::clone(::Messages *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false;
    }

    {
        for (std::list< ::Message *>::const_iterator itr = mpMessage.begin();
             itr != mpMessage.end(); itr++)
        {
            delete (*itr);
        }
        mpMessage.clear();
        for (std::list< ::Message *>::const_iterator itr =
                 pSrcObject->mpMessage.begin();
             itr != pSrcObject->mpMessage.end(); itr++)
        {
            ::Message *pTemp = new ::Message();
            if (false == pTemp->clone((*itr)))
            {
                delete pTemp;
                return false;
            };
            mpMessage.push_back(pTemp);
        }
    }

    return true;
}
