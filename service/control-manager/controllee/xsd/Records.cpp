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

#include "Records.h"

#include "Elements.h"

using namespace std;

Records::Records()
{
    setList(true);

}

Records::~Records()
{
    for (std::list< ::Record *>::const_iterator itr = mpRecord.begin();
         itr != mpRecord.end(); itr++)
    {
        delete (*itr);
    }
    mpRecord.clear();

}

bool Records::serializeContents(
    OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::List::serializeContents(serializer))
    {
        return false;
    }

    {
        bool bRet = true;

        if (mpRecord.size() > 0)
        {
            if (serializer.validateMandatory() == false)
            {
                serializer.setIDExcemption(true);
            }
            serializer.startListProperty("Records");
            for (std::list< ::Record *>::const_iterator itr = mpRecord.begin();
                 itr != mpRecord.end(); itr++)
            {
                (*itr)->setInstanceName("Records");
                if (false == (*itr)->serialize(serializer))
                {
                    bRet = false;
                    break;
                }
            }
            serializer.endListProperty("Records");
            if (false == bRet)
            {
                if (serializer.isInFilteringMode("Records") == false)
                {
                    serializer.setErrorMessage(
                        "Failed To Serialize parameter (Invalid content) : Records");
                    return false;
                }
            }
        }

    }

    return true;
}

bool Records::deSerializeContents(
    OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::List::deSerializeContents(deserializer))
    {
        return false;
    }

    {
        bool isArray = deserializer.getIsArray("Records");
        int sizeOfArray = 0;

        if (deserializer.validateMandatory() == false)
        {
            deserializer.setIDExcemption(true);
        }

        if (isArray)
        {
            sizeOfArray = deserializer.getSize("Records");

            if (sizeOfArray > 0)
            {
                for (int i = 0; i < sizeOfArray; i++)
                {
                    ::Record *m = new ::Record();

                    deserializer.setCurrentArrayIndex("Records", i);

                    m->setInstanceName("Records");

                    if (false == m->deSerialize(deserializer))
                    {
                        return false;
                    }

                    mpRecord.push_back(m);
                }
            }
            else if (sizeOfArray == 0)
            {
                deserializer.removeParameter("Records");
            }
        }
        else
        {
            if (true == deserializer.hasProperty("Records"))
            {
                ::Record *m = new ::Record();

                m->deSerialize(deserializer);

                mpRecord.push_back(m);
            }
            else
            {
                if ((deserializer.isInFilteringMode("Records") == false))
                {
                    deserializer.setErrorMessage(
                        "Failed To deserialize : Records");
                    return false;
                }
            }
        }
    }

    return true;
}

std::string Records::getElementName()
{
    return EN_RECORDS;
}

bool Records::clone(::Records *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false;
    }

    {
        for (std::list< ::Record *>::const_iterator itr = mpRecord.begin();
             itr != mpRecord.end(); itr++)
        {
            delete (*itr);
        }
        mpRecord.clear();
        for (std::list< ::Record *>::const_iterator itr =
                 pSrcObject->mpRecord.begin(); itr != pSrcObject->mpRecord.end();
             itr++)
        {
            ::Record *pTemp = new ::Record();
            if (false == pTemp->clone((*itr)))
            {
                delete pTemp;
                return false;
            };
            mpRecord.push_back(pTemp);
        }
    }

    return true;
}
