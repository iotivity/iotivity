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



#include "Subscriptions.h"


#include "Elements.h"

using namespace std;

Subscriptions::Subscriptions()
{
    setList(true);

}

Subscriptions::~Subscriptions()
{
    for ( std::list< ::Subscription * >::const_iterator itr = mpSubscription.begin();
          itr != mpSubscription.end(); itr++ )
    {
        delete (*itr);
    }
    mpSubscription.clear();

}

bool
Subscriptions::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::List::serializeContents(serializer)) { return false;}

    {
        bool bRet = true;

        if ( mpSubscription.size() > 0)
        {
            if (serializer.validateMandatory() == false) {serializer.setIDExcemption(true) ;}
            serializer.startListProperty("Subscriptions") ;
            for ( std::list< ::Subscription * >::const_iterator itr = mpSubscription.begin();
                  itr != mpSubscription.end(); itr++ )
            {
                (*itr)->setInstanceName("Subscriptions") ;
                if (false == (*itr)->serialize(serializer)) { bRet = false; break; }
            }
            serializer.endListProperty("Subscriptions") ;
            if (false == bRet) { if (serializer.isInFilteringMode("Subscriptions") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Subscriptions"); return false; }}
        }

    }


    return true ;
}

bool
Subscriptions::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::List::deSerializeContents(deserializer)) {return false;}

    {
        bool isArray = deserializer.getIsArray("Subscriptions");
        int sizeOfArray = 0;

        if (deserializer.validateMandatory() == false)
        {
            deserializer.setIDExcemption(true) ;
        }



        if (isArray)
        {
            sizeOfArray = deserializer.getSize("Subscriptions");





            if (sizeOfArray > 0)
            {
                for (int i = 0; i < sizeOfArray; i++)
                {
                    ::Subscription *m = new ::Subscription();

                    deserializer.setCurrentArrayIndex("Subscriptions", i);

                    m->setInstanceName("Subscriptions");

                    if (false == m->deSerialize(deserializer)) {return false;}

                    mpSubscription.push_back(m);
                }
            }
            else if (sizeOfArray == 0)
            {
                deserializer.removeParameter("Subscriptions") ;
            }
        }
        else
        {
            if (true == deserializer.hasProperty("Subscriptions"))
            {
                ::Subscription *m = new ::Subscription();

                m->deSerialize(deserializer);

                mpSubscription.push_back(m);
            }
            else
            {
                if ((deserializer.isInFilteringMode("Subscriptions") == false) ) { deserializer.setErrorMessage("Failed To deserialize : Subscriptions"); return false; }
            }
        }
    }




    return true ;
}

std::string
Subscriptions::getElementName()
{
    return EN_SUBSCRIPTIONS;
}

bool
Subscriptions::clone(::Subscriptions *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    {
        for ( std::list< ::Subscription * >::const_iterator itr = mpSubscription.begin();
              itr != mpSubscription.end(); itr++ )
        {
            delete (*itr);
        }
        mpSubscription.clear();
        for ( std::list< ::Subscription * >::const_iterator itr = pSrcObject->mpSubscription.begin();
              itr != pSrcObject->mpSubscription.end(); itr++ )
        {
            ::Subscription *pTemp = new ::Subscription();
            if (false == pTemp->clone((*itr)))
            {
                delete pTemp;
                return false;
            } ;
            mpSubscription.push_back(pTemp);
        }
    }


    return true ;
}
