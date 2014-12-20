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

#include "Networks.h"

#include "Elements.h"

using namespace std;

Networks::Networks()
{
    setList(true);

}

Networks::~Networks()
{
    for (std::list< ::Network *>::const_iterator itr = mpNetwork.begin();
         itr != mpNetwork.end(); itr++)
    {
        delete (*itr);
    }
    mpNetwork.clear();

}

bool Networks::serializeContents(
    OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::List::serializeContents(serializer))
    {
        return false;
    }

    {
        bool bRet = true;

        if (mpNetwork.size() > 0)
        {
            if (serializer.validateMandatory() == false)
            {
                serializer.setIDExcemption(true);
            }
            serializer.startListProperty("Networks");
            for (std::list< ::Network *>::const_iterator itr = mpNetwork.begin();
                 itr != mpNetwork.end(); itr++)
            {
                (*itr)->setInstanceName("Networks");
                if (false == (*itr)->serialize(serializer))
                {
                    bRet = false;
                    break;
                }
            }
            serializer.endListProperty("Networks");
            if (false == bRet)
            {
                if (serializer.isInFilteringMode("Networks") == false)
                {
                    serializer.setErrorMessage(
                        "Failed To Serialize parameter (Invalid content) : Networks");
                    return false;
                }
            }
        }

    }

    return true;
}

bool Networks::deSerializeContents(
    OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::List::deSerializeContents(deserializer))
    {
        return false;
    }

    {
        bool isArray = deserializer.getIsArray("Networks");
        int sizeOfArray = 0;

        if (deserializer.validateMandatory() == false)
        {
            deserializer.setIDExcemption(true);
        }

        if (isArray)
        {
            sizeOfArray = deserializer.getSize("Networks");

            if (sizeOfArray > 0)
            {
                for (int i = 0; i < sizeOfArray; i++)
                {
                    ::Network *m = new ::Network();

                    deserializer.setCurrentArrayIndex("Networks", i);

                    m->setInstanceName("Networks");

                    if (false == m->deSerialize(deserializer))
                    {
                        return false;
                    }

                    mpNetwork.push_back(m);
                }
            }
            else if (sizeOfArray == 0)
            {
                deserializer.removeParameter("Networks");
            }
        }
        else
        {
            if (true == deserializer.hasProperty("Networks"))
            {
                ::Network *m = new ::Network();

                m->deSerialize(deserializer);

                mpNetwork.push_back(m);
            }
            else
            {
                if ((deserializer.isInFilteringMode("Networks") == false))
                {
                    deserializer.setErrorMessage(
                        "Failed To deserialize : Networks");
                    return false;
                }
            }
        }
    }

    return true;
}

std::string Networks::getElementName()
{
    return EN_NETWORKS;
}

bool Networks::clone(::Networks *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false;
    }

    {
        for (std::list< ::Network *>::const_iterator itr = mpNetwork.begin();
             itr != mpNetwork.end(); itr++)
        {
            delete (*itr);
        }
        mpNetwork.clear();
        for (std::list< ::Network *>::const_iterator itr =
                 pSrcObject->mpNetwork.begin();
             itr != pSrcObject->mpNetwork.end(); itr++)
        {
            ::Network *pTemp = new ::Network();
            if (false == pTemp->clone((*itr)))
            {
                delete pTemp;
                return false;
            };
            mpNetwork.push_back(pTemp);
        }
    }

    return true;
}
