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

#include <sstream>
#include "CDSerializer.h"
#include "log.h"

using namespace std;

namespace OC
{

    CDSerializer::CDSerializer()
    {
        bValidateMandatory = true;
        inFilteringMode = false ;
        isFilteringDone = false;
        recursive = true ;
        currentLevel = 0 ;
        totalLevel = 0;
        bIdExcemtion = false ;

        m_values.push_back(new OCRepresentation );

    }

    CDSerializer::~CDSerializer()
    {

        for ( std::list<OCRepresentation *>::iterator itr = m_values.begin(); itr != m_values.end(); itr++ )
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

    bool CDSerializer::startGroupProperty(string nodeName)  // Modified by SRIB
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

        m_currentNodes.push_back(nodeName);
        m_values.push_back(new OCRepresentation);

        return true;
    }

    bool
    CDSerializer::setSchemaInstanceType(std::string type)
    {
        return this->setStringPropertyValue( "xsi_type", type ) ;
    }

    /**
     * @fn          : setAttributeValue()
     * @brief       : set key and vlaues.
     * @Assumptions :
     * @input       : string propertyName & AttributeValue &value
     * @output      : NA
     * @logic       : NA
     * @return      : bool
     * @since       : 11/09/2014
     * @author      : kumar.vinay@samsung.com
     * @version     : 1
     **/

    /** Modified by SRIB  **/
    bool CDSerializer::setAttributeValue(string propertyName, AttributeValue &value)
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


        OCRepresentation *attValue = m_values.back();

        AttributeValue attVal ;
        if (!attValue->getValue(propertyName , attVal))  // There is no map for given key . map it with new key and its value
        {
            attValue->setValue(propertyName, value) ;
        }
        else // This is may be case of array
        {
            if (attVal.type() == typeid(std::vector<int>))
            {
                std::vector<int> AAV = boost::get<std::vector<int>>(attVal);
                AAV.push_back(boost::get<int>(value));
                attValue->setValue(propertyName, AAV) ;
            }
            else if (attVal.type() == typeid(std::vector<double>))
            {
                std::vector<double> AAV = boost::get<std::vector<double>>(attVal);
                AAV.push_back(boost::get<double>(value));
                attValue->setValue(propertyName, AAV) ;
            }
            else if (attVal.type() == typeid(std::vector<bool>))
            {
                std::vector<bool> AAV = boost::get<std::vector<bool>>(attVal);
                AAV.push_back(boost::get<bool>(value));
                attValue->setValue(propertyName, AAV) ;
            }
            else if (attVal.type() == typeid(std::vector<std::string>))
            {
                std::vector<std::string> AAV = boost::get<std::vector<std::string>>(attVal);
                AAV.push_back(boost::get<std::string>(value));
                attValue->setValue(propertyName, AAV) ;
            }
            else if (attVal.type() == typeid(int))
            {
                int oldVal = boost::get<int>(attVal);
                std::vector<int> AAV ;
                AAV.push_back(oldVal);
                AAV.push_back(boost::get<int>(value));
                attValue->setValue(propertyName, AAV) ;
            }
            else if (attVal.type() == typeid(double))
            {
                double oldVal = boost::get<double>(attVal);
                std::vector<double> AAV ;
                AAV.push_back(oldVal);
                AAV.push_back(boost::get<double>(value));
                attValue->setValue(propertyName, AAV) ;
            }
            else if (attVal.type() == typeid(bool))
            {
                bool oldVal = boost::get<bool>(attVal);
                std::vector<bool> AAV ;
                AAV.push_back(oldVal);
                AAV.push_back(boost::get<bool>(value));
                attValue->setValue(propertyName, AAV) ;
            }
            else if (attVal.type() == typeid(std::string))
            {
                std::string oldVal = boost::get<std::string>(attVal);
                std::vector<std::string> AAV ;
                AAV.push_back(oldVal);
                AAV.push_back(boost::get<std::string>(value));
                attValue->setValue(propertyName, AAV) ;
            }
            else if (attVal.type() == typeid(OCRepresentation))
            {
                // SRIB NOTE : This should not come here ,
                return false ;
            }
            else    // This can be NULL or anything else
            {

                // SRIB NOTE : . We need to handle this case.
            }
        }
        return true ;

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

    bool CDSerializer::setStringPropertyValue(string propertyName, string value)  // Modified by SRIB
    {
        AttributeValue aV  = value ;
        return setAttributeValue(propertyName, aV);
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

    bool CDSerializer::setBoolPropertyValue(string propertyName, bool value)  // Modified by SRIB
    {
        AttributeValue aV  = value ;
        return setAttributeValue(propertyName, aV);

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


    bool CDSerializer::setIntPropertyValue(string propertyName, int value) // Modified by SRIB
    {
        AttributeValue aV  = value ;
        return setAttributeValue(propertyName, aV);
    }

    bool CDSerializer::setUIntPropertyValue(string propertyName, unsigned int value)  // Modified by SRIB
    {
        return setIntPropertyValue(propertyName, value);
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

    bool CDSerializer::setDoublePropertyValue(string propertyName, double value)  // Modified by SRIB
    {
        AttributeValue aV  = value ;
        return setAttributeValue(propertyName, aV);
    }

    bool
    CDSerializer::setPropertyValue(std::string propertyName, float value)
    {
        return setDoublePropertyValue(propertyName, (double)value);
    }

    bool CDSerializer::setPropertyValue(std::string propertyName, std::string value)
    {
        return setStringPropertyValue(propertyName, value);
    }

    bool CDSerializer::setPropertyValue(std::string propertyName, bool value)
    {
        return setBoolPropertyValue(propertyName, value);
    }

    bool CDSerializer::setPropertyValue(std::string propertyName, double value)
    {
        return setDoublePropertyValue(propertyName, value);
    }

    bool
    CDSerializer::setPropertyValue(std::string propertyName, int8_t value)
    {
        return setIntPropertyValue(propertyName, (int)value);
    }

    bool
    CDSerializer::setPropertyValue(std::string propertyName, int16_t value)
    {
        return setIntPropertyValue(propertyName, (int)value);
    }

    bool
    CDSerializer::setPropertyValue(std::string propertyName, int32_t value)
    {
        return setIntPropertyValue(propertyName, (int)value);
    }

    bool
    CDSerializer::setPropertyValue(std::string propertyName, int64_t value)
    {
        return setIntPropertyValue(propertyName, (int)value);
    }

    bool
    CDSerializer::setPropertyValue(std::string propertyName, uint8_t value)
    {
        return setUIntPropertyValue(propertyName, (unsigned int)value);
    }

    bool
    CDSerializer::setPropertyValue(std::string propertyName, uint16_t value)
    {
        return setUIntPropertyValue(propertyName, (unsigned int)value);
    }

    bool
    CDSerializer::setPropertyValue(std::string propertyName, uint32_t value)
    {
        return setUIntPropertyValue(propertyName, (unsigned int)value);
    }

    bool
    CDSerializer::setPropertyValue(std::string propertyName, uint64_t value)
    {
        return setUIntPropertyValue(propertyName, (unsigned int) value);
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
    CDSerializer::getOutput(OCRepresentation &output, int &length)  // Modified by SRIB , Need to verify - Vinay
    {
        if ((m_values.size() == 1) && (m_currentNodes.size() == 0))
        {
            m_output = *(m_values.back());
            int size = m_output.numberOfAttributes() ;
            if (size > 0)
            {
                output = m_output;
                length = size ;

                CE(std::cout << "\nSerialization m_output:" << m_output.getJSONRepresentation() << "\n" );
                CE(std::cout << "\nSerialization Output:" << output.getJSONRepresentation() << "\n") ;

            }
            else
            {
                OCRepresentation obj ;  // Empty Attribute Map
                output = obj;
                length = 0;
            }

            return true;
        }
        else
        {
            return false ;
        }
        return true;
    }

    bool
    CDSerializer::getPrettyPrintOutput(std::string &output)
    {

        if ((m_values.size() == 1) && (m_currentNodes.size() == 0))
        {
            OCRepresentation *root = m_values.back();
            if (root->numberOfAttributes() > 0)
            {
                output = root->getJSONRepresentation() ;  // JSON representation

                CE(std::cout << "\nSerialization Output: " << output << "\n") ;

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
        return true;
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

    bool CDSerializer::endGroupProperty(string nodeName)
    {
        string currentNode = m_currentNodes.back();
        if (currentNode.compare(nodeName) == 0)
        {
            OCRepresentation *value = m_values.back() ;
            m_values.pop_back();
            m_currentNodes.pop_back();

//      if ((inFilteringMode) && (isFilteringDone == false))
//      {
//          return true ;
//      }

            if (m_values.size() > 0)
            {
                if (((value->numberOfAttributes() > 0) || (currentLevel <= 1) ))
                {
                    if ((value->numberOfAttributes() > 0) || ((value->numberOfAttributes() > 0) && (totalLevel <= 1)))
                    {
                        OCRepresentation *current = m_values.back() ;
                        OCRepresentation av =  *value ;
                        AttributeValue attVal ;
                        if (false == current->getValue(nodeName, attVal))   // There is no map for given key . map it with new key and its value
                        {
                            current->setValue(nodeName, av) ;
                        }
                        else
                        {
                            // SRIB NOTE : :  This case wont come now.There wont be vector of OCRepresentation - SRIB - Vinay

                            /*
                            AttributeDataValue attVal = (*current)[nodeName].data ;
                            if (attVal.type() == typeid(AttributeArrayValues))  // This is an array
                            {
                                AttributeArrayValues AAV = boost::get<AttributeArrayValues>(attVal);
                                AAV.push_back(av);
                                AttributeValue aV ;
                                aV.data = AAV ;
                                (*current)[nodeName] = aV ;
                            }
                            else if(attVal.type() == typeid(AttributeNull)) // Currently value is NULL for given key .
                            {
                                (*current)[nodeName] = av ;  // overide NULL with current value
                            }
                            else if(attVal.type() == typeid(boost::recursive_wrapper<AttributeValue>))  // TODO , Not clarity about this case
                            {
                                // TODO . We need to handle this case.
                            }
                            else  // This is any basic data type (like int, bool, string, double etc. ) and currently this is not array , but we need make it array value
                            {
                                AttributeArrayValues AAV ;
                                AttributeValue aV1 ;
                                aV1.data = attVal ;
                                AAV.push_back(aV1);

                                AAV.push_back(av);
                                AttributeValue aV2 ;
                                aV2.data = AAV ;
                                (*current)[nodeName] = aV2 ;

                            } */
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
    CDSerializer::startDocument()
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
    CDSerializer::endDocument()
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
    CDSerializer::getContentType()
    {
        return "application/json; charset=UTF-8";
    }

    bool
    CDSerializer::setFilter(std::string filter)
    {
        this->filter = filter ;
        this->inFilteringMode = true ;

        return true;
    }

    void
    CDSerializer::setRecursive(bool recursive)
    {
        this->recursive = recursive ;
    }

    std::string
    CDSerializer::getFilter()
    {
        return filter ;
    }

    bool
    CDSerializer::isRecursive()
    {
        return recursive ;
    }

    bool
    CDSerializer::validateMandatory()
    {
        return bValidateMandatory ;
    }

    void
    CDSerializer::setValidateMandatory(bool enable)
    {
        this->bValidateMandatory = enable ;
    }

    bool
    CDSerializer::isInFilteringMode(std::string filter )
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
    CDSerializer::setErrorMessage(std::string errorMessage)
    {
        m_error_message.append (errorMessage) ;
        m_error_message.append ("\n");
    }

    std::string
    CDSerializer::getErrorMessage()
    {
        return m_error_message ;
    }


    /** Modified by SRIB - Needs to verify **/
    bool CDSerializer::startListProperty(string nodeName)
    {
        if (m_values.size() <= 0 )
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

        OCRepresentation *attValue = m_values.back() ;
        AttributeValue attVal ;
        if (attValue->getValue(nodeName, attVal)) // if this is not NULL
        {
            if (attVal.type() == typeid(int))
            {
                int oldVal = boost::get<int>(attVal);
                std::vector<int> AAV ;
                AAV.push_back(oldVal);
                attValue->setValue(nodeName, AAV) ;
            }
            else if (attVal.type() == typeid(double))
            {
                double oldVal = boost::get<double>(attVal);
                std::vector<double> AAV ;
                AAV.push_back(oldVal);
                attValue->setValue(nodeName, AAV) ;
            }
            else if (attVal.type() == typeid(bool))
            {
                bool oldVal = boost::get<bool>(attVal);
                std::vector<bool> AAV ;
                AAV.push_back(oldVal);
                attValue->setValue(nodeName, AAV) ;
            }
            else if (attVal.type() == typeid(std::string))
            {
                std::string oldVal = boost::get<std::string>(attVal);
                std::vector<std::string> AAV ;
                AAV.push_back(oldVal);
                attValue->setValue(nodeName, AAV) ;
            }
            else
            {
                // SRIB NOTE : This should not reach here
                return false ;

            }
        }
        else
        {
            // SRIB NOTE : Need to map key with NULL , Later this ll convert into array based on type of value
        }
        return true;
    }

    bool
    CDSerializer::endListProperty(string nodeName)  // Modify by SRIB  ,  Why it is removing the key node ?
    {
        if (m_values.size() <= 0 )
        {
            return false ;
        }

        OCRepresentation *attValue = m_values.back() ;
        AttributeValue aV ;
        if (attValue->getValue(nodeName, aV))
        {
            if (    (aV.type() == typeid(std::vector<int>) &&  ((boost::get<std::vector<int>>(aV)).size() == 0) )
                    || (aV.type() == typeid(std::vector<bool>) &&  ((boost::get<std::vector<bool>>(aV)).size() == 0) )
                    || (aV.type() == typeid(std::vector<double>) &&  ((boost::get<std::vector<double>>(aV)).size() == 0) )
                    || (aV.type() == typeid(std::vector<std::string>) &&  ((boost::get<std::vector<std::string>>(aV)).size() == 0) )
               )
            {
                if (inFilteringMode == true)
                {
                    if (totalLevel >= 1)
                    {
                        attValue->erase(nodeName) ;
                    }
                }
            }
        }
        return true;
    }

    void
    CDSerializer::setIDExcemption(bool excemption)
    {
        this->bIdExcemtion = excemption ;
    }

    bool
    CDSerializer::getIDExcemption()
    {
        return bIdExcemtion ;
    }


}
