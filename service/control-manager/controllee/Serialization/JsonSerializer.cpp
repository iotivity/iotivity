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

#include "JsonSerializer.h"
#include "JsonDeserializer.h"
#include "IContext.h"
#include "CDContext.h"
#include "SerializableDataFactory.h"
#include "ISerializable.h"
#include <list>

using namespace std;

namespace OC
{
    namespace Cm
    {
        namespace Core
        {
            namespace Serialization
            {
                namespace Json
                {

                    JsonSerializer::JsonSerializer()
                    {
                        bValidateMandatory = true;
                        inFilteringMode = false ;
                        isFilteringDone = false;
                        recursive = true ;
                        currentLevel = 0 ;
                        totalLevel = 0;
                        bIdExcemtion = false ;

                        //m_currentNodes.push_back(nodeName);
                        m_values.push_back(new ::Json::Value(::Json::objectValue));
                    }

                    JsonSerializer::~JsonSerializer()
                    {
                        for ( std::list< ::Json::Value *>::iterator itr = m_values.begin(); itr != m_values.end(); itr++ )
                        {
                            delete (*itr) ;
                        }

                        m_values.clear();
                    }
                    /**
                     * @fn          : startGroupProperty()
                     * @brief       : Start of a new node.
                     * @Assumptions :
                     * @input       : string nodeName
                     * @output      : NA
                     * @logic       : NA
                     * @return      : bool
                     * @since       : 5/15/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/

                    bool JsonSerializer::startGroupProperty(string nodeName)
                    {
                        if ((inFilteringMode) && (0 == filter.compare(nodeName)))
                        {
                            inFilteringMode = false ;
                            isFilteringDone = false ;
                        }

                        if (inFilteringMode == false)
                        {
                            currentLevel++ ;
                        }

                        totalLevel++ ;

                        //if (isFilteringDone == false)
                        //{
                        m_currentNodes.push_back(nodeName);
                        m_values.push_back(new ::Json::Value(::Json::objectValue));
                        //}
                        //else
                        //{
                        //  m_filteredNodes.push_back(nodeName);
                        //  m_filteredNodeValues.push_back(new ::Json::Value());
                        //}

                        return true;
                    }

                    bool JsonSerializer::setPropertyValue(string propertyName, ::Json::Value value)
                    {
                        if (inFilteringMode)
                        {
                            if (0 != filter.compare(propertyName))
                            {
                                return true ;
                            }
                        }

                        if (propertyName.empty())
                            return false;

                        ::Json::Value *JsonValue = m_values.back();

                        if ( ((*JsonValue)[propertyName].isNull() == false) && ((*JsonValue)[propertyName].isArray()))
                        {
                            (*JsonValue)[propertyName].append(value) ;
                        }
                        else
                        {
                            if ((*JsonValue)[propertyName].empty())
                            {
                                (*JsonValue)[propertyName] = value;
                            }
                            else
                            {
//          if( (*JsonValue)[propertyName].isArray())
//          {
//              (*JsonValue)[propertyName].append(value) ;
//          }
//          else
//          {

                                ::Json::Value arrayValue(::Json::arrayValue);
                                arrayValue.append((*JsonValue)[propertyName]);
                                arrayValue.append(value);

                                (*JsonValue)[propertyName] = arrayValue ;

//          }
                            }
                        }

                        return true;
                    }

                    /**
                     * @fn          : setStringPropertyValue()
                     * @brief       : Sets a string value for a property inside a node.
                     * @Assumptions :
                     * @input       : string propertyName, string value
                     * @output      : NA
                     * @logic       : NA
                     * @return      : bool
                     * @since       : 5/15/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/
                    bool JsonSerializer::setStringPropertyValue(string propertyName, string value)
                    {
                        ::Json::Value jsonValue(value) ;
                        return setPropertyValue(propertyName, jsonValue);
                    }

                    bool
                    JsonSerializer::setSchemaInstanceType(std::string type)
                    {
                        return this->setStringPropertyValue( "xsi_type", type ) ;
                    }

                    /*
                    *
                     * @fn          : setBoolPropertyValue()
                     * @brief       : Sets a boolean value for a property inside a node.
                     * @Assumptions :
                     * @input       : string propertyName, string value
                     * @output      : NA
                     * @logic       : NA
                     * @return      : bool
                     * @since       : 5/15/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/

                    bool JsonSerializer::setBoolPropertyValue(string propertyName, bool value)
                    {
                        ::Json::Value jsonValue(value) ;
                        return setPropertyValue(propertyName, jsonValue);
                    }

                    /**
                     * @fn          : setIntPropertyValue()
                     * @brief       : Sets an integer value for a property inside a node.
                     * @Assumptions :
                     * @input       : string propertyName,int value
                     * @output      : NA
                     * @logic       : NA
                     * @return      : bool
                     * @since       : 5/15/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/


                    bool JsonSerializer::setIntPropertyValue(string propertyName, int value)
                    {
                        ::Json::Value jsonValue(value) ;
                        return setPropertyValue(propertyName, jsonValue);
                    }

                    bool JsonSerializer::setUIntPropertyValue(string propertyName, unsigned int value)
                    {
                        ::Json::Value jsonValue(value) ;
                        return setPropertyValue(propertyName, jsonValue);
                    }

                    /**
                     * @fn          : setDoublePropertyValue()
                     * @brief       : Sets an double value for a property inside a node.
                     * @Assumptions :
                     * @input       : string propertyName,double value
                     * @output      : NA
                     * @logic       : NA
                     * @return      : bool
                     * @since       : 5/15/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/

                    bool JsonSerializer::setDoublePropertyValue(string propertyName, double value)
                    {
                        ::Json::Value jsonValue(value) ;
                        return setPropertyValue(propertyName, jsonValue);
                    }


                    bool JsonSerializer::setPropertyValue(std::string propertyName, std::string value)
                    {
                        return setStringPropertyValue(propertyName, value);
                    }

                    bool JsonSerializer::setPropertyValue(std::string propertyName, bool value)
                    {
                        return setBoolPropertyValue(propertyName, value);
                    }

//bool JsonSerializer::setPropertyValue(std::string propertyName, int value)
//{
//  return setIntPropertyValue(propertyName,value);
//}

                    bool JsonSerializer::setPropertyValue(std::string propertyName, double value)
                    {
                        return setDoublePropertyValue(propertyName, value);
                    }

                    bool
                    JsonSerializer::setPropertyValue(std::string propertyName, int8_t value)
                    {
                        return setIntPropertyValue(propertyName, (int)value);
                    }

                    bool
                    JsonSerializer::setPropertyValue(std::string propertyName, int16_t value)
                    {
                        return setIntPropertyValue(propertyName, (int)value);
                    }

                    bool
                    JsonSerializer::setPropertyValue(std::string propertyName, int32_t value)
                    {
                        return setIntPropertyValue(propertyName, value);
                    }

                    bool
                    JsonSerializer::setPropertyValue(std::string propertyName, int64_t value)
                    {
                        return setIntPropertyValue(propertyName, (int)value);
                    }

                    bool
                    JsonSerializer::setPropertyValue(std::string propertyName, uint8_t value)
                    {
                        return setUIntPropertyValue(propertyName, (unsigned int)value);
                    }

                    bool
                    JsonSerializer::setPropertyValue(std::string propertyName, uint16_t value)
                    {
                        return setUIntPropertyValue(propertyName, (unsigned int)value);
                    }

                    bool
                    JsonSerializer::setPropertyValue(std::string propertyName, uint32_t value)
                    {
                        return setUIntPropertyValue(propertyName, value);
                    }

                    bool
                    JsonSerializer::setPropertyValue(std::string propertyName, uint64_t value)
                    {
                        return setUIntPropertyValue(propertyName, (unsigned int) value);
                    }

                    bool
                    JsonSerializer::setPropertyValue(std::string propertyName, float value)
                    {
                        return setDoublePropertyValue(propertyName, (double)value);
                    }


                    /**
                     * @fn          : getOutput()
                     * @brief       : Gets the serialization output.
                     * @Assumptions :
                     * @input       : std::string& output, int& length
                     * @output      : NA
                     * @logic       : NA
                     * @return      : bool
                     * @since       : 5/15/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/

                    bool
                    JsonSerializer::getOutput(string &output, int &length)
                    {
                        if ((m_values.size() == 1) && (m_currentNodes.size() == 0))
                        {
                            ::Json::Value *root = m_values.back();

                            if (root->empty() == false)
                                //if (root != NULL)
                            {
                                ::Json::FastWriter writer;

                                output = writer.write(*root);

                                length = output.size() ;
                            }
                            else
                            {
                                output = "";
                                length = 0;
                            }

                            return true;
                        }
                        else
                        {
                            return false ;
                        }
                    }

                    bool
                    JsonSerializer::getPrettyPrintOutput(std::string &output)
                    {
                        if ((m_values.size() == 1) && (m_currentNodes.size() == 0))
                        {
                            ::Json::Value *root = m_values.back();

                            if (root->empty() == false)
                                //if (root != NULL)
                            {
                                ::Json::StyledWriter writer;

                                output = writer.write(*root);
                            }
                            else
                            {
                                output = "";
                            }

                            return true;
                        }
                        else
                        {
                            return false ;
                        }
                    }


                    /**
                     * @fn          : endGroupProperty()
                     * @brief       : Ends the current node.
                     * @Assumptions :
                     * @input       : string propertyName
                     * @output      : NA
                     * @logic       : NA
                     * @return      : bool
                     * @since       : 5/15/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/

                    bool JsonSerializer::endGroupProperty(string nodeName)
                    {
//  if (m_filteredNodes.size() > 0)
//  {
//      string currentFilteredNode = m_filteredNodes.back();
//      if (currentFilteredNode.compare(nodeName) == 0)
//      {
//          ::Json::Value *value = m_filteredNodeValues.back();
//          m_filteredNodeValues.pop_back();
//          m_filteredNodes.pop_back();
//
//          delete value;
//      }
//  }

                        string currentNode = m_currentNodes.back();
                        if (currentNode.compare(nodeName) == 0)
                        {
                            ::Json::Value *value = m_values.back();
                            m_values.pop_back();
                            m_currentNodes.pop_back();

//      if ((inFilteringMode) && (isFilteringDone == false))
//      {
//          return true ;
//      }

                            if (m_values.size() > 0)
                            {
                                if (((value->empty() == false) || (currentLevel <= 1) ))
                                {
                                    if ((value->empty() == false) || ((value->empty() == true) && (totalLevel <= 1)))
                                    {
                                        if (((*(m_values.back()))[nodeName].isNull() == false) &&  ((*(m_values.back()))[nodeName].isArray()))
                                        {
                                            (*(m_values.back()))[nodeName].append(*value) ;
                                        }
                                        else
                                        {
                                            if ((*(m_values.back()))[nodeName].empty())
                                            {
                                                (*(m_values.back()))[nodeName] = *value ;
                                            }
                                            else
                                            {
//                          if( (*(m_values.back()))[nodeName].isArray())
//                          {
//                              (*(m_values.back()))[nodeName].append(*value) ;
//                          }
//                          else
//                          {
                                                ::Json::Value *arrayValue = new ::Json::Value(::Json::arrayValue);
                                                arrayValue->append((*(m_values.back()))[nodeName]);
                                                arrayValue->append(*value);

                                                (*(m_values.back()))[nodeName] = *arrayValue ;

                                                delete arrayValue;
//                          }

                                            }
                                        }
                                    }
                                }

                                delete value ;
                            }

                            if (inFilteringMode == false)
                            {
                                currentLevel-- ;
                            }

                            totalLevel--;

                            if ((inFilteringMode == false) && (0 == filter.compare(nodeName)))
                            {
                                inFilteringMode = true ;
                                isFilteringDone = true;

                                return true ;
                            }
                        }
                        else
                        {
                            return false;
                        }


                        return true;
                    }
                    /**
                     * @fn          : startDocument()
                     * @brief       : Starts the serialization node structure.
                     * @Assumptions :
                     * @input       : NA
                     * @output      : NA
                     * @logic       : NA
                     * @return      : bool
                     * @since       : 5/15/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/

                    bool
                    JsonSerializer::startDocument()
                    {
                        return true;
                    }
                    /**
                     * @fn          : endGroupProperty()
                     * @brief       : Ends the serialization node structure.
                     * @Assumptions :
                     * @input       : NA
                     * @output      : NA
                     * @logic       : NA
                     * @return      : bool
                     * @since       : 5/15/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/

                    bool
                    JsonSerializer::endDocument()
                    {
                        return true;
                    }
                    /**
                     * @fn          : getContentType()
                     * @brief       : Ends the current node.
                     * @Assumptions :
                     * @input       : NA
                     * @output      : NA
                     * @logic       : NA
                     * @return      : NA
                     * @since       : 5/15/2013
                     *
                     * @author      : kirtigupta
                     *
                     * @version     : 1
                     **/

                    std::string
                    JsonSerializer::getContentType()
                    {
                        return "application/json; charset=UTF-8";
                    }

                    bool
                    JsonSerializer::setFilter(std::string filter)
                    {
                        this->filter = filter ;
                        this->inFilteringMode = true ;

                        return true;
                    }

                    void
                    JsonSerializer::setRecursive(bool recursive)
                    {
                        this->recursive = recursive ;
                    }

                    std::string
                    JsonSerializer::getFilter()
                    {
                        return filter ;
                    }

                    bool
                    JsonSerializer::isRecursive()
                    {
                        return recursive ;
                    }

                    bool
                    JsonSerializer::validateMandatory()
                    {
                        return bValidateMandatory ;
                    }

                    void
                    JsonSerializer::setValidateMandatory(bool enable)
                    {
                        this->bValidateMandatory = enable ;
                    }

                    bool
                    JsonSerializer::isInFilteringMode(std::string filter )
                    {
                        if (bValidateMandatory == false)
                        {
                            if ((filter.compare("id") == 0) && (bIdExcemtion))
                            {
                                return false;
                            }
                            else
                            {
                                return true ;
                            }
                        }

                        if ((filter.compare("id") == 0) && (bIdExcemtion))
                        {
                            return true;
                        }

                        if ( inFilteringMode )
                        {
                            if (0 == this->filter.compare(filter))
                            {
                                return false ;
                            }
                        }

                        return inFilteringMode;
                    }

                    void
                    JsonSerializer::setErrorMessage(std::string errorMessage)
                    {
                        m_error_message.append (errorMessage) ;
                        m_error_message.append ("\n");
                    }

                    std::string
                    JsonSerializer::getErrorMessage()
                    {
                        return m_error_message ;
                    }


                    bool
                    JsonSerializer::startListProperty(string nodeName)
                    {
                        if (m_values.size() <= 0)
                        {
                            return false ;
                        }

                        /*  if ((inFilteringMode) && (0 == filter.compare(nodeName)))
                            {
                                inFilteringMode = false ;
                                isFilteringDone = false ;
                            }

                            if (inFilteringMode == false)
                            {
                                currentLevel++ ;
                            }

                            totalLevel++ ;*/

                        if (((*(m_values.back()))[nodeName].isNull() == false) && ((*(m_values.back()))[nodeName].isArray()))
                        {
                            // Nothing Todo as it is already an array
                        }
                        else
                        {
                            ::Json::Value arrayValue(::Json::arrayValue);

                            if ((*(m_values.back()))[nodeName].empty())
                            {
                            }
                            else
                            {
                                arrayValue.append((*(m_values.back()))[nodeName]);
                            }

                            (*(m_values.back()))[nodeName] = arrayValue ;
                        }

                        return true;
                    }

                    bool
                    JsonSerializer::endListProperty(string nodeName)
                    {
                        if (m_values.size() <= 0)
                        {
                            return false ;
                        }

                        if (((*(m_values.back()))[nodeName].isNull() == false) && ((*(m_values.back()))[nodeName].isArray()) && ((*(m_values.back()))[nodeName].empty()))
                        {
                            if (inFilteringMode == true)
                            {
                                if (totalLevel >= 1)
                                {
                                    (*(m_values.back())).removeMember(nodeName) ;
                                }
                            }
                        }

                        return true;
                    }

                    void
                    JsonSerializer::setIDExcemption(bool excemption)
                    {
                        this->bIdExcemtion = excemption ;
                    }

                    bool
                    JsonSerializer::getIDExcemption()
                    {
                        return bIdExcemtion ;
                    }


                }
            }
        }
    }
}
