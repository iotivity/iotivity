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

#include "Doors.h"

#include "Elements.h"

using namespace std;

Doors::Doors()
{
    setList(true);

}

Doors::~Doors()
{
    for (std::list< ::Door *>::const_iterator itr = mpDoor.begin();
         itr != mpDoor.end(); itr++)
    {
        delete (*itr);
    }
    mpDoor.clear();

}

bool Doors::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::List::serializeContents(serializer))
    {
        return false;
    }

    {
        bool bRet = true;

        if (mpDoor.size() > 0)
        {
            if (serializer.validateMandatory() == false)
            {
                serializer.setIDExcemption(true);
            }
            serializer.startListProperty("Doors");
            for (std::list< ::Door *>::const_iterator itr = mpDoor.begin();
                 itr != mpDoor.end(); itr++)
            {
                (*itr)->setInstanceName("Doors");
                if (false == (*itr)->serialize(serializer))
                {
                    bRet = false;
                    break;
                }
            }
            serializer.endListProperty("Doors");
            if (false == bRet)
            {
                if (serializer.isInFilteringMode("Doors") == false)
                {
                    serializer.setErrorMessage(
                        "Failed To Serialize parameter (Invalid content) : Doors");
                    return false;
                }
            }
        }

    }

    return true;
}

bool Doors::deSerializeContents(
    OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::List::deSerializeContents(deserializer))
    {
        return false;
    }

    {
        bool isArray = deserializer.getIsArray("Doors");
        int sizeOfArray = 0;

        if (deserializer.validateMandatory() == false)
        {
            deserializer.setIDExcemption(true);
        }

        if (isArray)
        {
            sizeOfArray = deserializer.getSize("Doors");

            if (sizeOfArray > 0)
            {
                for (int i = 0; i < sizeOfArray; i++)
                {
                    ::Door *m = new ::Door();

                    deserializer.setCurrentArrayIndex("Doors", i);

                    m->setInstanceName("Doors");

                    if (false == m->deSerialize(deserializer))
                    {
                        return false;
                    }

                    mpDoor.push_back(m);
                }
            }
            else if (sizeOfArray == 0)
            {
                deserializer.removeParameter("Doors");
            }
        }
        else
        {
            if (true == deserializer.hasProperty("Doors"))
            {
                ::Door *m = new ::Door();

                m->deSerialize(deserializer);

                mpDoor.push_back(m);
            }
            else
            {
                if ((deserializer.isInFilteringMode("Doors") == false))
                {
                    deserializer.setErrorMessage(
                        "Failed To deserialize : Doors");
                    return false;
                }
            }
        }
    }

    return true;
}

std::string Doors::getElementName()
{
    return EN_DOORS;
}

bool Doors::clone(::Doors *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false;
    }

    {
        for (std::list< ::Door *>::const_iterator itr = mpDoor.begin();
             itr != mpDoor.end(); itr++)
        {
            delete (*itr);
        }
        mpDoor.clear();
        for (std::list< ::Door *>::const_iterator itr =
                 pSrcObject->mpDoor.begin(); itr != pSrcObject->mpDoor.end();
             itr++)
        {
            ::Door *pTemp = new ::Door();
            if (false == pTemp->clone((*itr)))
            {
                delete pTemp;
                return false;
            };
            mpDoor.push_back(pTemp);
        }
    }

    return true;
}
