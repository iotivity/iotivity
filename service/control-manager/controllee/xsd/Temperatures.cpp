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

#include "Temperatures.h"

#include "Elements.h"

using namespace std;

Temperatures::Temperatures()
{
    setList(true);

}

Temperatures::~Temperatures()
{
    for (std::list< ::Temperature *>::const_iterator itr = mpTemperature.begin();
         itr != mpTemperature.end(); itr++)
    {
        delete (*itr);
    }
    mpTemperature.clear();

}

bool Temperatures::serializeContents(
    OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::List::serializeContents(serializer))
    {
        return false;
    }

    {
        bool bRet = true;

        if (mpTemperature.size() > 0)
        {
            if (serializer.validateMandatory() == false)
            {
                serializer.setIDExcemption(true);
            }
            serializer.startListProperty("Temperatures");
            for (std::list< ::Temperature *>::const_iterator itr =
                     mpTemperature.begin(); itr != mpTemperature.end(); itr++)
            {
                (*itr)->setInstanceName("Temperatures");
                if (false == (*itr)->serialize(serializer))
                {
                    bRet = false;
                    break;
                }
            }
            serializer.endListProperty("Temperatures");
            if (false == bRet)
            {
                if (serializer.isInFilteringMode("Temperatures") == false)
                {
                    serializer.setErrorMessage(
                        "Failed To Serialize parameter (Invalid content) : Temperatures");
                    return false;
                }
            }
        }

    }

    return true;
}

bool Temperatures::deSerializeContents(
    OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::List::deSerializeContents(deserializer))
    {
        return false;
    }

    {
        bool isArray = deserializer.getIsArray("Temperatures");
        int sizeOfArray = 0;

        if (deserializer.validateMandatory() == false)
        {
            deserializer.setIDExcemption(true);
        }

        if (isArray)
        {
            sizeOfArray = deserializer.getSize("Temperatures");

            if (sizeOfArray > 0)
            {
                for (int i = 0; i < sizeOfArray; i++)
                {
                    ::Temperature *m = new ::Temperature();

                    deserializer.setCurrentArrayIndex("Temperatures", i);

                    m->setInstanceName("Temperatures");

                    if (false == m->deSerialize(deserializer))
                    {
                        return false;
                    }

                    mpTemperature.push_back(m);
                }
            }
            else if (sizeOfArray == 0)
            {
                deserializer.removeParameter("Temperatures");
            }
        }
        else
        {
            if (true == deserializer.hasProperty("Temperatures"))
            {
                ::Temperature *m = new ::Temperature();

                m->deSerialize(deserializer);

                mpTemperature.push_back(m);
            }
            else
            {
                if ((deserializer.isInFilteringMode("Temperatures") == false))
                {
                    deserializer.setErrorMessage(
                        "Failed To deserialize : Temperatures");
                    return false;
                }
            }
        }
    }

    return true;
}

std::string Temperatures::getElementName()
{
    return EN_TEMPERATURES;
}

bool Temperatures::clone(::Temperatures *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false;
    }

    {
        for (std::list< ::Temperature *>::const_iterator itr =
                 mpTemperature.begin(); itr != mpTemperature.end(); itr++)
        {
            delete (*itr);
        }
        mpTemperature.clear();
        for (std::list< ::Temperature *>::const_iterator itr =
                 pSrcObject->mpTemperature.begin();
             itr != pSrcObject->mpTemperature.end(); itr++)
        {
            ::Temperature *pTemp = new ::Temperature();
            if (false == pTemp->clone((*itr)))
            {
                delete pTemp;
                return false;
            };
            mpTemperature.push_back(pTemp);
        }
    }

    return true;
}
