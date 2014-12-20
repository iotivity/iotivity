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

#include "ArrayOfDayType.h"
#include "Elements.h"

using namespace std;

ArrayOfDayType::ArrayOfDayType()
{

}

ArrayOfDayType::~ArrayOfDayType()
{

}

bool
ArrayOfDayType::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    bool bRet = true ;

    serializer.startListProperty(this->getInstanceName()) ;

    for ( std::list< ::DayType >::iterator itr = members.begin(); itr != members.end(); itr++)
    {
        bRet = (*itr).validateContent();
        if (bRet)
        {
            bRet = serializer.setPropertyValue( this->getInstanceName(), (*itr).value ) ;
        }


        if (bRet == false)
        {
            serializer.setErrorMessage( this->getInstanceName() + " Parameter Contains Invalid Value : " ) ;
            break;
        }
    }

    serializer.endListProperty(this->getInstanceName()) ;

    return bRet ;
}

bool
ArrayOfDayType::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    bool bRet = true ;
    bool isArray = deserializer.getIsArray(this->getInstanceName());
    int sizeOfArray = 0;

    if (isArray)
    {
        sizeOfArray = deserializer.getSize(this->getInstanceName());
        for (int i = 0; i < sizeOfArray; i++)
        {
            deserializer.setCurrentArrayIndex(this->getInstanceName(), i);

            ::DayType el ;

            bRet = deserializer.getPropertyValue(this->getInstanceName(), &(el.value)) ;
            if (bRet)
            {
                bRet = el.validateContent();
            }


            if (bRet == false)
            {
                deserializer.setErrorMessage( this->getInstanceName() + " Parameter Contains Invalid Value : " ) ;
                break ;
            }

            members.push_back(el);
        }
    }
    else
    {
        if (true == deserializer.hasProperty(this->getInstanceName()))
        {
            ::DayType el ;

            bRet = deserializer.getPropertyValue(this->getInstanceName(), &(el.value)) ;
            if (bRet)
            {
                bRet = el.validateContent();
            }


            if (bRet == false)
            {
                deserializer.setErrorMessage( this->getInstanceName() + " Parameter Contains Invalid Value : " ) ;
                return false;
            }

            members.push_back(el);
        }
    }

    return bRet ;
}

std::string
ArrayOfDayType::getElementName()
{
    return "XS:LIST";
}


