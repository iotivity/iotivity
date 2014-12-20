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

#include "ISerializer.h"
#include "IDeserializer.h"
#include "SerializableDataFactory.h"
#include "CDError.h"
#include "IContext.h"
#include "CDContext.h"
#include "JsonSerializer.h"

namespace OC
{
    namespace Cm
    {
        namespace Serialization
        {

            ISerializable::ISerializable()
            {
                m_pSchema_instance_data = NULL ;
                mIsList = false ;
            }


            ISerializable::~ISerializable()
            {
                if ( NULL != m_pSchema_instance_data )
                {
                    delete m_pSchema_instance_data ;
                    m_pSchema_instance_data = NULL ;
                }
            }

            bool
            ISerializable::serialize(OC::Cm::Serialization::ISerializer  &serializer)
            {
                if (mIsList == false)
                {
                    std::string eName ;

                    if ( mInstanceName.length() > 0 )
                    {
                        eName.assign( mInstanceName ) ;
                    }
                    else
                    {
                        eName.assign( this->getElementName() ) ;
                    }

                    if (false == serializer.startGroupProperty(eName))
                    {
                        return false ;
                    }

                    if ( m_schema_instance_type.empty() == true )
                    {
                        if (false == this->serializeContents(serializer))
                        {
                            return false ;
                        }
                    }
                    else
                    {
                        if ( m_pSchema_instance_data != NULL)
                        {
                            serializer.setSchemaInstanceType( this->m_schema_instance_type );

                            if ( false == m_pSchema_instance_data->serializeContents( serializer ) )
                            {
                                return false ;
                            }
                        }
                    }

                    if (false == serializer.endGroupProperty(eName))
                    {
                        return false ;
                    }
                }
                else
                {
                    if (false == this->serializeContents(serializer))
                    {
                        return false ;
                    }
                }

                return true ;
            }

            bool
            ISerializable::deSerialize(OC::Cm::Serialization::IDeserializer  &deserializer)
            {
                if (mIsList == false)
                {
                    std::string eName ;
                    bool bRet = true ;

                    if ( mInstanceName.length() > 0 )
                    {
                        eName.assign( mInstanceName ) ;
                    }
                    else
                    {
                        eName.assign( this->getElementName() ) ;
                    }

                    if (false == deserializer.getGroupProperty(eName))
                    {
                        std::string errorMsg ;
                        errorMsg.append("Failed to find group parameter : ");
                        errorMsg.append(eName);
                        deserializer.setErrorMessage(errorMsg) ;
                        bRet = false ;
                    }

                    if (bRet)
                    {
                        if ( false == deserializer.getSchemaInstanceType( this->m_schema_instance_type ) )
                        {
                            if (false == this->deSerializeContents(deserializer))
                            {
                                bRet = false ;
                            }
                        }
                        else
                        {
                            OC::CDContext *pContext = (OC::CDContext *)deserializer.getContext() ;

                            OC::Cm::Serialization::SerializableDataFactory *factory = pContext->p_server->getSerializableDataFactory() ;
                            if ( NULL == factory )
                            {
                                return false ;
                            }
                            else
                            {
                                if ( false == factory->createSerializable( this->m_schema_instance_type, &( this->m_pSchema_instance_data )))
                                {
                                    return false ;
                                }

                                if ( NULL == this->m_pSchema_instance_data)
                                {
                                    return false;
                                }

                                if (false == this->m_pSchema_instance_data->deSerializeContents(deserializer))
                                {
                                    return false ;
                                }
                            }
                        }

                        if (false == deserializer.endGroupProperty(eName))
                        {
                            std::string errorMsg ;
                            errorMsg.append("failed to deserialize parameter : ");
                            errorMsg.append(eName);
                            //deserializer.setErrorMessage(errorMsg) ;
                            bRet = false ;
                        }
                    }

                    return bRet ;
                }
                else
                {
                    if (false == this->deSerializeContents(deserializer))
                    {
                        return false ;
                    }
                }

                return true ;
            }

            CDError
            ISerializable::validateContent()
            {
                OC::Cm::CDError error ;

                OC::Cm::Serialization::ISerializer *p_serializer = new OC::Cm::Core::Serialization::Json::JsonSerializer();
                if (p_serializer == NULL)
                {
                    return error.setErrorCode(SHP_MEMORY_ALLOCATION_FAILED);
                }

                if (false == this->serialize(*p_serializer))
                {
                    std::string errorMessage = p_serializer->getErrorMessage();

                    delete p_serializer ;
                    return error.setErrorCode(SHP_SERIALIZATION_FAILED).setErrorMessage(errorMessage);
                }

                delete p_serializer ;
                return error.setErrorCode(SHP_SUCCESS);
            }

            bool
            ISerializable::setInstanceName( std::string name )
            {
                if ( name.empty() )
                {
                    return false;
                }

                mInstanceName.assign(name);
                return true;
            }

            std::string
            ISerializable::getInstanceName()
            {
                return mInstanceName ;
            }

            void
            ISerializable::setList(bool isList)
            {
                mIsList = isList ;
            }


        }
    }
}


