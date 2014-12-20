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



#include "Devices.h"


#include "Elements.h"

using namespace std;

Devices::Devices()
{
    setList(true);

}

Devices::~Devices()
{
    for ( std::list< ::Device * >::const_iterator itr = mpDevice.begin(); itr != mpDevice.end();
          itr++ )
    {
        delete (*itr);
    }
    mpDevice.clear();

}

bool
Devices::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::List::serializeContents(serializer)) { return false;}

    {
        bool bRet = true;
        if (serializer.validateMandatory() == false) {serializer.setIDExcemption(true) ;}
        if ( mpDevice.size() < 1) { if (serializer.isInFilteringMode("Devices") == false) { serializer.setErrorMessage(" parameter item count is less than minOccures value specified in xsd : Device");  return false; } }
        serializer.startListProperty("Devices") ;
        for ( std::list< ::Device * >::const_iterator itr = mpDevice.begin(); itr != mpDevice.end();
              itr++ )
        {
            (*itr)->setInstanceName("Devices") ;
            if (false == (*itr)->serialize(serializer)) { bRet = false; break; }
        }
        serializer.endListProperty("Devices") ;
        if (false == bRet) { if (serializer.isInFilteringMode("Devices") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Devices"); return false; }}
    }


    return true ;
}

bool
Devices::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::List::deSerializeContents(deserializer)) {return false;}

    {
        bool isArray = deserializer.getIsArray("Devices");
        int sizeOfArray = 0;

        if (deserializer.validateMandatory() == false)
        {
            deserializer.setIDExcemption(true) ;
        }



        if (isArray)
        {
            sizeOfArray = deserializer.getSize("Devices");

            if ( sizeOfArray < 1) {if ((deserializer.isInFilteringMode("Devices") == false) ) { deserializer.setErrorMessage(" parameter item count is less than minOccures value specified in xsd : Devices"); return false; }}




            if (sizeOfArray > 0)
            {
                for (int i = 0; i < sizeOfArray; i++)
                {
                    ::Device *m = new ::Device();

                    deserializer.setCurrentArrayIndex("Devices", i);

                    m->setInstanceName("Devices");

                    if (false == m->deSerialize(deserializer)) {return false;}

                    mpDevice.push_back(m);
                }
            }
            else if (sizeOfArray == 0)
            {
                deserializer.removeParameter("Devices") ;
            }
        }
        else
        {
            if (true == deserializer.hasProperty("Devices"))
            {
                ::Device *m = new ::Device();

                m->deSerialize(deserializer);

                mpDevice.push_back(m);
            }
            else
            {
                return false;
            }
        }
    }




    return true ;
}

std::string
Devices::getElementName()
{
    return EN_DEVICES;
}

bool
Devices::clone(::Devices *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    {
        for ( std::list< ::Device * >::const_iterator itr = mpDevice.begin(); itr != mpDevice.end();
              itr++ )
        {
            delete (*itr);
        }
        mpDevice.clear();
        for ( std::list< ::Device * >::const_iterator itr = pSrcObject->mpDevice.begin();
              itr != pSrcObject->mpDevice.end(); itr++ )
        {
            ::Device *pTemp = new ::Device();
            if (false == pTemp->clone((*itr))) {return false;} ;
            mpDevice.push_back(pTemp);
        }
    }


    return true ;
}
