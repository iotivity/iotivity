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



#include "Alarms.h"


#include "Elements.h"

using namespace std;

Alarms::Alarms()
{
    setList(true);

}

Alarms::~Alarms()
{
    for ( std::list< ::Alarm * >::const_iterator itr = mpAlarm.begin(); itr != mpAlarm.end(); itr++ )
    {
        delete (*itr);
    }
    mpAlarm.clear();

}

bool
Alarms::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::List::serializeContents(serializer)) { return false;}

    {
        bool bRet = true;

        if ( mpAlarm.size() > 0)
        {
            if (serializer.validateMandatory() == false) {serializer.setIDExcemption(true) ;}
            serializer.startListProperty("Alarms") ;
            for ( std::list< ::Alarm * >::const_iterator itr = mpAlarm.begin(); itr != mpAlarm.end(); itr++ )
            {
                (*itr)->setInstanceName("Alarms") ;
                if (false == (*itr)->serialize(serializer)) { bRet = false; break; }
            }
            serializer.endListProperty("Alarms") ;
            if (false == bRet) { if (serializer.isInFilteringMode("Alarms") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Alarms"); return false; }}
        }

    }


    return true ;
}

bool
Alarms::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::List::deSerializeContents(deserializer)) {return false;}

    {
        bool isArray = deserializer.getIsArray("Alarms");
        int sizeOfArray = 0;

        if (deserializer.validateMandatory() == false)
        {
            deserializer.setIDExcemption(true) ;
        }



        if (isArray)
        {
            sizeOfArray = deserializer.getSize("Alarms");





            if (sizeOfArray > 0)
            {
                for (int i = 0; i < sizeOfArray; i++)
                {
                    ::Alarm *m = new ::Alarm();

                    deserializer.setCurrentArrayIndex("Alarms", i);

                    m->setInstanceName("Alarms");

                    if (false == m->deSerialize(deserializer)) {return false;}

                    mpAlarm.push_back(m);
                }
            }
            else if (sizeOfArray == 0)
            {
                deserializer.removeParameter("Alarms") ;
            }
        }
        else
        {
            if (true == deserializer.hasProperty("Alarms"))
            {
                ::Alarm *m = new ::Alarm();

                m->deSerialize(deserializer);

                mpAlarm.push_back(m);
            }
            else
            {
                if ((deserializer.isInFilteringMode("Alarms") == false) ) { deserializer.setErrorMessage("Failed To deserialize : Alarms"); return false; }
            }
        }
    }




    return true ;
}

std::string
Alarms::getElementName()
{
    return EN_ALARMS;
}

bool
Alarms::clone(::Alarms *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    {
        for ( std::list< ::Alarm * >::const_iterator itr = mpAlarm.begin(); itr != mpAlarm.end(); itr++ )
        {
            delete (*itr);
        }
        mpAlarm.clear();
        for ( std::list< ::Alarm * >::const_iterator itr = pSrcObject->mpAlarm.begin();
              itr != pSrcObject->mpAlarm.end(); itr++ )
        {
            ::Alarm *pTemp = new ::Alarm();
            if (false == pTemp->clone((*itr)))
            {
                delete pTemp;
                return false;
            } ;
            mpAlarm.push_back(pTemp);
        }
    }


    return true ;
}
