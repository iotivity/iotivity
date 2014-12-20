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



#include "Versions.h"


#include "Elements.h"

using namespace std;

Versions::Versions()
{
    setList(true);

}

Versions::~Versions()
{
    for ( std::list< ::Version * >::const_iterator itr = mpVersion.begin(); itr != mpVersion.end();
          itr++ )
    {
        delete (*itr);
    }
    mpVersion.clear();

}

bool
Versions::serializeContents(OC::Cm::Serialization::ISerializer &serializer)
{
    if (false == ::List::serializeContents(serializer)) { return false;}

    {
        bool bRet = true;

        if ( mpVersion.size() > 0)
        {
            if (serializer.validateMandatory() == false) {serializer.setIDExcemption(true) ;}
            serializer.startListProperty("Versions") ;
            for ( std::list< ::Version * >::const_iterator itr = mpVersion.begin(); itr != mpVersion.end();
                  itr++ )
            {
                (*itr)->setInstanceName("Versions") ;
                if (false == (*itr)->serialize(serializer)) { bRet = false; break; }
            }
            serializer.endListProperty("Versions") ;
            if (false == bRet) { if (serializer.isInFilteringMode("Versions") == false) {serializer.setErrorMessage("Failed To Serialize parameter (Invalid content) : Versions"); return false; }}
        }

    }


    return true ;
}

bool
Versions::deSerializeContents(OC::Cm::Serialization::IDeserializer &deserializer)
{
    if (false == ::List::deSerializeContents(deserializer)) {return false;}

    {
        bool isArray = deserializer.getIsArray("Versions");
        int sizeOfArray = 0;

        if (deserializer.validateMandatory() == false)
        {
            deserializer.setIDExcemption(true) ;
        }



        if (isArray)
        {
            sizeOfArray = deserializer.getSize("Versions");





            if (sizeOfArray > 0)
            {
                for (int i = 0; i < sizeOfArray; i++)
                {
                    ::Version *m = new ::Version();

                    deserializer.setCurrentArrayIndex("Versions", i);

                    m->setInstanceName("Versions");

                    if (false == m->deSerialize(deserializer)) {return false;}

                    mpVersion.push_back(m);
                }
            }
            else if (sizeOfArray == 0)
            {
                deserializer.removeParameter("Versions") ;
            }
        }
        else
        {
            if (true == deserializer.hasProperty("Versions"))
            {
                ::Version *m = new ::Version();

                m->deSerialize(deserializer);

                mpVersion.push_back(m);
            }
            else
            {
                if ((deserializer.isInFilteringMode("Versions") == false) ) { deserializer.setErrorMessage("Failed To deserialize : Versions"); return false; }
            }
        }
    }




    return true ;
}

std::string
Versions::getElementName()
{
    return EN_VERSIONS;
}

bool
Versions::clone(::Versions *pSrcObject)
{
    if (pSrcObject == NULL)
    {
        return false ;
    }

    {
        for ( std::list< ::Version * >::const_iterator itr = mpVersion.begin(); itr != mpVersion.end();
              itr++ )
        {
            delete (*itr);
        }
        mpVersion.clear();
        for ( std::list< ::Version * >::const_iterator itr = pSrcObject->mpVersion.begin();
              itr != pSrcObject->mpVersion.end(); itr++ )
        {
            ::Version *pTemp = new ::Version();
            if (false == pTemp->clone((*itr)))
            {
                delete pTemp;
                return false;
            } ;
            mpVersion.push_back(pTemp);
        }
    }


    return true ;
}
