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

#include "CDDeserializer.h"
#include "IDeserializer.h"
#include "IContext.h"
#include "CDContext.h"
#include "log.h"

using namespace std;

namespace OC
{

    CDDeserializer::CDDeserializer(OC::Cm::IContext *context): IDeserializer(context)
    {

        m_pCurrent_property = NULL ;
        inFilteringMode = false ;
        isFilteringDone = false ;
        bValidateMandatory = true ;
        bIdExcemtion        = false ;
        recursive = true ;
        notification = false;
    }
    /**
     * @fn          : parse()
     * @brief       : Stores the Json String in root.
     * @Assumptions :
     * @input       : NA
     * @output      : NA
     * @logic       : NA
     * @return      : NA
     * @since       : 25/09/2014
     *
     * @author      : kumar.vinay@samsung.com
     *
     * @version     : 1
     **/

    bool
    CDDeserializer::parse(OCRepresentation &aOV, int length)
    {
        root  = aOV ;
        m_pCurrent_property = &root;
        m_current_property_name = "rootNode" ;


        CE(std::cout << "\nDeserializer Input : " << m_pCurrent_property->getJSONRepresentation() << "\n" );

        return true;
    }

    CDDeserializer::~CDDeserializer()
    {

    }

    /**
     * @fn          : getGroupProperty()
     * @brief       : Gives the value object of a particular nodeName.
     * @Assumptions :
     * @input       : string nodeName
     * @output      : NA
     * @logic       : NA
     * @return      : bool
     * @since       : 25/09/2014
     *
     * @author      : kumar.vinay@samsung.com
     *
     * @version     : 1
     **/

    bool CDDeserializer::getGroupProperty(string nodeName)
    {
        if ((nodeName.empty()) || (NULL == m_pCurrent_property))
        {
            return false;
        }

        if ((inFilteringMode) && (0 == m_filter.compare(nodeName)))
        {
            inFilteringMode = false ;
            isFilteringDone = false ;
        }

        OCRepresentation *pCurrentParent = m_pCurrent_property ;

        m_currentNodes.push_back(pCurrentParent);
        m_currentNodeNames.push_back(m_current_property_name);

        if (m_pCurrent_property->hasAttribute(nodeName))
        {
            if (m_pCurrent_property->getValue(nodeName, m_CurrentRep) == true)
            {
                m_pCurrent_property = &m_CurrentRep;
                m_current_property_name = nodeName ;
                CE(std::cout << "\n" << nodeName << " : " << m_pCurrent_property->getJSONRepresentation() << "\n" );
                return true ;
            }
        }
        return false;
    }

    bool CDDeserializer::hasGroupProperty(string nodeName)
    {
        if ((nodeName.empty()) || (NULL == m_pCurrent_property))
        {
            return false;
        }

        if (m_pCurrent_property == NULL)
        {
            return false;
        }
        if (m_pCurrent_property->hasAttribute(nodeName))
        {
            OCRepresentation rep ;
            if (m_pCurrent_property->getValue(nodeName, rep))
            {
                return true ;
            }
        }
        return false;
    }

    bool CDDeserializer::hasProperty(string propertyName)
    {
        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
        {
            return false;
        }
        if (m_pCurrent_property->numberOfAttributes() == 0)
        {
            return false;
        }
        return m_pCurrent_property->hasAttribute(propertyName) ;
    }

    /**
     * @fn          : getIsArray()
     * @brief       : To check whether the passed propertyName is a array or not.
     * @Assumptions :
     * @input       : string propertyName
     * @output      : NA
     * @logic       : NA
     * @return      : bool
     * @since       : 25/09/2014
     *
     * @author      : kumar.vinay@samung.com
     *
     * @version     : 1
     **/
    bool CDDeserializer::getIsArray(string propertyName)
    {
        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
        {
            return false;
        }

        if (m_pCurrent_property == NULL || m_pCurrent_property->numberOfAttributes() == 0)
        {
            return false;
        }

        AttributeValue aV ;
        if (m_pCurrent_property->getValue(propertyName , aV))
        {
            if ( aV.type() == typeid(std::vector<int>)    &&   aV.type() == typeid(std::vector<bool>) &&
                 aV.type() == typeid(std::vector<double>) &&   aV.type() == typeid(std::vector<std::string>))

                return true ;
        }
        return false;
    }

    /**
     * @fn          : getSize()
     * @brief       : Gives the size of the array.
     * @Assumptions :
     * @input       : string propertyName
     * @output      : NA
     * @logic       : NA
     * @return      : int
     * @since       : 25/09/2014
     *
     * @author      : kumar.vinay@samung.com
     *
     * @version     : 1
     **/
    int CDDeserializer::getSize(string propertyName)
    {
        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
        {
            return 0;
        }
        if (m_pCurrent_property == NULL)
        {
            return 0;
        }

        AttributeValue aV ;
        if (m_pCurrent_property->getValue(propertyName , aV))
        {
            if (getIsArray(propertyName))  // If this an array. return size of array
            {
                return m_pCurrent_property->numberOfAttributes(); ;
            }
        }
        return 0;
    }

    bool CDDeserializer::endGroupProperty( string propertyName )
    {
        if ( m_current_property_name.compare(propertyName) != 0)
        {
            return false;
        }

        if ((m_currentNodes.size() <= 0) || (m_currentNodeNames.size() <= 0))
        {
            return false ;
        }

        if ((inFilteringMode == false) && (0 == m_filter.compare(propertyName)))
        {
            inFilteringMode = true ;
            isFilteringDone = true ;
        }

        bool bRet = true ;

//  if (m_pCurrent_property->numberOfAttributes() > 0)
//  {
//      std::string errorMessage = "Unable to deserialize" ;
//      //for (std::map<std::string, std::string>::iterator it = attMap.begin(); it != attMap.end(); ++it)
//      //{
//      //  errorMessage += it->first ;
//      //  errorMessage.append(", ");
//      //}
//      errorMessage += " : " ;
//      errorMessage += propertyName ;
//      setErrorMessage(errorMessage);
//      bRet = false;
//  }

        m_current_property_name = m_currentNodeNames.back();
        m_pCurrent_property = m_currentNodes.back() ;

        m_currentNodeNames.pop_back();
        m_currentNodes.pop_back();

        if (m_pCurrent_property == NULL)
        {
            return false;
        }

        bool bDeleteNode = true;

        /*
        AttributeValue currentPropertyValue ;
        if(m_pCurrent_property->getValue(propertyName,currentPropertyValue))
        {
                if ( currentPropertyValue.type() == typeid(std::vector<int>)    &&   currentPropertyValue.type() == typeid(std::vector<bool>) &&
                      currentPropertyValue.type() == typeid(std::vector<double>) &&   currentPropertyValue.type() == typeid(std::vector<std::string>))
                {
                    int index = -1 ;
                    if (getCurrentArrayIndex(propertyName, index))
                    {
                        if (index != getSize(propertyName) - 1)
                        {
                            bDeleteNode = false ;
                        }
                    }
                }

        }
        */

        if ( bDeleteNode )
        {
            m_pCurrent_property->erase(propertyName);
        }

        return bRet;
    }

    bool
    CDDeserializer::getSchemaInstanceType( std::string &type )
    {
        return this->getStringPropertyValue("xsi_type", type) ;
    }


    bool
    CDDeserializer::getProperty(string propertyName , AttributeValue &aV)
    {
        if (false == hasProperty(propertyName))
        {
            return false ;
        }

        AttributeValue attVal ;
        if (!m_pCurrent_property->getValue(propertyName, attVal))
        {
            CE(cout << "Getting property failed in " << __PRETTY_FUNCTION__ << endl);
            return false;
        }

        if (attVal.type() == typeid(std::vector<int>))  // This is an array of int
        {
            std::vector<int> AAV = boost::get<std::vector<int>>(attVal);
            int index = 0;
            if ( false == getCurrentArrayIndex( propertyName, index ) )
            {
                index = 0;
            }

            int size = AAV.size() ;
            if ((index < 0) || (index >= size))
            {
                return false ;
            }
            aV  = (AAV[index]) ;
            return true ;
        }
        else if (attVal.type() == typeid(std::vector<double>))  // This is an array of double
        {
            std::vector<double> AAV = boost::get<std::vector<double>>(attVal);
            int index = 0;
            if ( false == getCurrentArrayIndex( propertyName, index ) )
            {
                index = 0;
            }

            int size = AAV.size() ;
            if ((index < 0) || (index >= size))
            {
                return false ;
            }
            aV  = (AAV[index]) ;
            return true ;
        }
        else if (attVal.type() == typeid(std::vector<bool>))  // This is an array of bool
        {
            std::vector<bool> AAV = boost::get<std::vector<bool>>(attVal);
            int index = 0;
            if ( false == getCurrentArrayIndex( propertyName, index ) )
            {
                index = 0;
            }

            int size = AAV.size() ;
            if ((index < 0) || (index >= size))
            {
                return false ;
            }
            aV  = (AAV[index]) ;
            return true ;
        }
        else if (attVal.type() == typeid(std::vector<std::string>))  // This is an array of string
        {
            std::vector<std::string> AAV = boost::get<std::vector<std::string>>(attVal);
            int index = 0;
            if ( false == getCurrentArrayIndex( propertyName, index ) )
            {
                index = 0;
            }

            int size = AAV.size() ;
            if ((index < 0) || (index >= size))
            {
                return false ;
            }
            aV  = (AAV[index]) ;
            return true ;
        }
        else if (attVal.type() == typeid(OCRepresentation)) // others
        {
            aV = attVal ;
        }
        return false ;
    }


    bool
    CDDeserializer::getPropertyRemoveAfter(string propertyName, AttributeValue &aValue)
    {
        if (false == hasProperty(propertyName))
        {
            return false ;
        }

        AttributeValue  aDV ;
        if (m_pCurrent_property->getValue(propertyName , aDV))
        {
            if (aDV.type() == typeid(std::vector<int>))
            {
                std::vector<int> AAV = boost::get<std::vector<int>>(aDV);
                int index = 0;
                if ( false == getCurrentArrayIndex( propertyName, index ) )
                {
                    index = 0;
                }

                int size = AAV.size() ;
                if ((index < 0) || (index >= size))
                {
                    return false ;
                }
                aValue = AAV[index];
                if (index == (size - 1))
                {
                    m_pCurrent_property->erase(propertyName) ;
                }
                return true ;
            }
            else if (aDV.type() == typeid(std::vector<double>))
            {
                std::vector<double> AAV = boost::get<std::vector<double>>(aDV);
                int index = 0;
                if ( false == getCurrentArrayIndex( propertyName, index ) )
                {
                    index = 0;
                }

                int size = AAV.size() ;
                if ((index < 0) || (index >= size))
                {
                    return false ;
                }
                aValue = AAV[index];
                if (index == (size - 1))
                {
                    m_pCurrent_property->erase(propertyName) ;
                }
                return true ;
            }
            else if (aDV.type() == typeid(std::vector<bool>))
            {
                std::vector<bool> AAV = boost::get<std::vector<bool>>(aDV);
                int index = 0;
                if ( false == getCurrentArrayIndex( propertyName, index ) )
                {
                    index = 0;
                }

                int size = AAV.size() ;
                if ((index < 0) || (index >= size))
                {
                    return false ;
                }
                aValue = AAV[index];
                if (index == (size - 1))
                {
                    m_pCurrent_property->erase(propertyName) ;
                }
                return true ;
            }
            else if (aDV.type() == typeid(std::vector<std::string>))
            {
                std::vector<std::string> AAV = boost::get<std::vector<std::string>>(aDV);
                int index = 0;
                if ( false == getCurrentArrayIndex( propertyName, index ) )
                {
                    index = 0;
                }

                int size = AAV.size() ;
                if ((index < 0) || (index >= size))
                {
                    return false ;
                }
                aValue = AAV[index];
                if (index == (size - 1))
                {
                    m_pCurrent_property->erase(propertyName) ;
                }
                return true ;
            }
            else if ( aDV.type() == typeid(OCRepresentation) ) // if it is AttributeMap
            {
                aValue = aDV ;
                return true ;
            }
            else // If it is int, string ,double , bool etc basic data types
            {
                m_pCurrent_property->erase(propertyName) ;
                aValue = aDV ;
                return true ;
            }
        }
        return false ;
    }

    /**
     * @fn          : getBoolPropertyValue()
     * @brief       : gets the boolean value corresponding to the propertyName.
     * @Assumptions :
     * @input       : string propertyName
     * @output      : NA
     * @logic       : NA
     * @return      : bool
     * @since       : 25/09/2014
     *
     * @author      : kumar.vinay@samung.com
     *
     * @version     : 1
     **/

    bool CDDeserializer::getBoolPropertyValue(string propertyName, bool &propValue)
    {

        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
        {
            return false;
        }

        if (m_pCurrent_property->hasAttribute(propertyName))
        {
            if (m_pCurrent_property->getValue(propertyName, propValue)) // if it is bool then return value
            {
                return true;
            }
            else  //SRIB NOTE : this is an error case
            {
                return false;
            }
        }
        return false ;
    }
    /**
     * @fn          : getStringPropertyValue()
     * @brief       : gets the string value corresponding to the propertyName.
     * @Assumptions :
     * @input       : string propertyName,string& propValue
     * @output      : NA
     * @logic       : NA
     * @return      : bool
     * @since       : 25/09/2014
     *
     * @author      : kumar.vinay@samung.com
     *
     * @version     : 1
     **/

    bool CDDeserializer::getStringPropertyValue(string propertyName, string &propValue)
    {
        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
        {
            return false;
        }
        if (m_pCurrent_property->hasAttribute(propertyName))
        {
            if (m_pCurrent_property->getValue(propertyName, propValue)) // if it is bool then return value
            {
                return true;
            }
            else  //SRIB NOTE : this is an error case
            {
                return false;
            }
        }
        return false;

    }
    /**
     * @fn          : getIntPropertyValue()
     * @brief       : gets the integer value corresponding to the propertyName.
     * @Assumptions :
     * @input       : string propertyName,int& propValue
     * @output      : NA
     * @logic       : NA
     * @return      : bool
     * @since       : 25/09/2014
     *
     * @author      : kumar.vinay@samung.com
     *
     * @version     : 1
     **/

    bool CDDeserializer::getIntPropertyValue(string propertyName, int64_t &propValue)
    {
        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
        {
            return false;
        }

        int value ;
        if (m_pCurrent_property->hasAttribute(propertyName))
        {
            if (m_pCurrent_property->getValue(propertyName, value)) // if it is bool then return value
            {
                propValue = value ;
                return true;
            }
            else  //SRIB NOTE : this is an error case
            {
                return false;
            }
        }
        return false;
    }

    bool CDDeserializer::getUIntPropertyValue(string propertyName, uint64_t &propValue)
    {
        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
        {
            return false;
        }

        int value ;
        if (m_pCurrent_property->hasAttribute(propertyName))
        {
            if (m_pCurrent_property->getValue(propertyName, value)) // if it is bool then return value
            {
                propValue = value ;
                return true;
            }
            else  //SRIB NOTE : this is an error case
            {
                return false;
            }
        }
        return false;
    }

    /**
     * @fn          : getDoublePropertyValue()
     * @brief       : gets the double value corresponding to the propertyName.
     * @Assumptions :
     * @input       : string propertyName,double& propValue
     * @output      : NA
     * @logic       : NA
     * @return      : bool
     * @since       : 25/09/2014
     *
     * @author      : kumar.vinay@samung.com
     *
     * @version     : 1
     **/


    bool CDDeserializer::getDoublePropertyValue(string propertyName, double &propValue)
    {
        if ((propertyName.empty()) || (NULL == m_pCurrent_property))
        {
            return false;
        }

        if (m_pCurrent_property->hasAttribute(propertyName))
        {
            if (m_pCurrent_property->getValue(propertyName, propValue)) // if it is bool then return value
            {
                return true;
            }
            else  //SRIB NOTE : this is an error case
            {
                return false;
            }
        }
        return false;
    }

    bool CDDeserializer::getPropertyValue(std::string propertyName, std::string *value)
    {
        std::string val = "" ;

        bool bRet = getStringPropertyValue(propertyName, val) ;

        *value = val ;

        return bRet ;
    }

    bool CDDeserializer::getPropertyValue(std::string propertyName, bool *value)
    {
        bool val ;

        bool bRet =  getBoolPropertyValue(propertyName, val);

        *value = val ;

        return bRet ;
    }

//bool CDDeserializer::getPropertyValue(std::string propertyName, int* value)
//{
//  int val ;
//
//  bool bRet = getIntPropertyValue(propertyName, val) ;
//
//  *value = val ;
//
//  return bRet ;
//}

    bool CDDeserializer::getPropertyValue(std::string propertyName, double *value)
    {
        double val;

        bool bRet = getDoublePropertyValue(propertyName, val) ;

        *value = val ;

        return bRet ;
    }

    bool
    CDDeserializer::getPropertyValue(std::string propertyName, uint8_t *value)
    {
        uint64_t val = 0L;

        bool bRet = getUIntPropertyValue(propertyName, val) ;

        if (bRet)
        {
            if (val > 255)
            {
                std::string errorMessage = "Value is out of range for property" ;
                errorMessage = errorMessage + propertyName ;
                this->setErrorMessage(errorMessage) ;
                return false ;
            }
        }
        *value = (uint8_t)val ;

        return bRet ;

    }

    bool
    CDDeserializer::getPropertyValue(std::string propertyName, uint16_t *value)
    {
        uint64_t val = 0L;

        bool bRet = getUIntPropertyValue(propertyName, val) ;

        if (bRet)
        {
            if (val > 65535)
            {
                std::string errorMessage = "Value is out of range for property" ;
                errorMessage = errorMessage + propertyName ;
                this->setErrorMessage(errorMessage) ;
                return false ;
            }
        }

        *value = (uint16_t)val ;

        return bRet ;
    }

    bool
    CDDeserializer::getPropertyValue(std::string propertyName, uint32_t *value)
    {
        uint64_t val = 0L;

        bool bRet = getUIntPropertyValue(propertyName, val) ;

        if (bRet)
        {
            if (val > 4294967295u)
            {
                std::string errorMessage = "Value is out of range for property" ;
                errorMessage = errorMessage + propertyName ;
                this->setErrorMessage(errorMessage) ;
                return false ;
            }
        }

        *value = (uint32_t)val ;

        return bRet ;
    }

    bool
    CDDeserializer::getPropertyValue(std::string propertyName, uint64_t *value)
    {
        uint64_t val = 0L;

        bool bRet = getUIntPropertyValue(propertyName, val) ;

//  if (bRet)
//  {
//      if (val < 0L)
//      {
//          std::string errorMessage = "Value is out of range for property" ;
//          errorMessage = errorMessage + propertyName ;
//          this->setErrorMessage(errorMessage) ;
//          return false ;
//      }
//  }

        *value = (uint64_t)val ;  // TODO: Compile JSON CPP library with 64 bit support

        return bRet ;
    }

    bool
    CDDeserializer::getPropertyValue(std::string propertyName, int8_t *value)
    {
        int64_t val = 0L;

        bool bRet = getIntPropertyValue(propertyName, val) ;

        if (bRet)
        {
            if ((val < -128) || (val > 127))
            {
                std::string errorMessage = "Value is out of range for property" ;
                errorMessage = errorMessage + propertyName ;
                this->setErrorMessage(errorMessage) ;
                return false ;
            }
        }

        *value = (int8_t)val ;

        return bRet ;
    }

    bool
    CDDeserializer::getPropertyValue(std::string propertyName, int16_t *value)
    {
        int64_t val = 0L;

        bool bRet = getIntPropertyValue(propertyName, val) ;

        if (bRet)
        {
            if ((val < -32768) || (val > 32767))
            {
                std::string errorMessage = "Value is out of range for property" ;
                errorMessage = errorMessage + propertyName ;
                this->setErrorMessage(errorMessage) ;
                return false ;
            }
        }

        *value = (int16_t)val ;

        return bRet ;
    }

    bool
    CDDeserializer::getPropertyValue(std::string propertyName, int32_t *value)
    {
        int64_t val ;

        bool bRet = getIntPropertyValue(propertyName, val) ;

        if (bRet)
        {
            if ((val < (-2147483647 - 1)) || (val > 2147483647))
            {
                std::string errorMessage = "Value is out of range for property" ;
                errorMessage = errorMessage + propertyName ;
                this->setErrorMessage(errorMessage) ;
                return false ;
            }
        }

        *value = (int32_t)val ; // TODO: Compile JSON CPP library with 64 bit support

        return bRet ;
    }

    bool
    CDDeserializer::getPropertyValue(std::string propertyName, int64_t *value)
    {
        int64_t val = 0L;

        bool bRet = getIntPropertyValue(propertyName, val) ;

        *value = (int64_t)val ;

        return bRet ;
    }

    bool
    CDDeserializer::getPropertyValue(std::string propertyName, float *value)
    {
        double val;

        bool bRet = getDoublePropertyValue(propertyName, val) ;

        *value = (float)val ;

        return bRet ;
    }


    bool
    CDDeserializer::setCurrentArrayIndex( string propertyName, int index )
    {
        m_array_index_map[propertyName] = index ;
        return true ;
    }

    bool
    CDDeserializer::getCurrentArrayIndex( string propertyName, int &index )
    {
        if (m_array_index_map.find(propertyName) == m_array_index_map.end())
        {
            return false ;
        }
        else
        {
            index = m_array_index_map[propertyName] ;
            return true ;
        }
    }

    bool
    CDDeserializer::setFilter(std::string filter)
    {
        this->m_filter = filter ;

        inFilteringMode = true ;
        isFilteringDone = false ;

        return true;
    }

    bool
    CDDeserializer::isInFilteringMode(std::string filter )
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
            if (0 == m_filter.compare(filter))
            {
                return false ;
            }
        }

        return inFilteringMode;
    }

    void
    CDDeserializer::setRecursive( bool recursive )
    {
        this->recursive = recursive ;
    }

    bool
    CDDeserializer::isRecursive()
    {
        return recursive ;
    }

    void
    CDDeserializer::setErrorMessage(std::string errorMessage)
    {
        m_error_message.append (errorMessage) ;
        m_error_message.append ("\n");
    }

    std::string
    CDDeserializer::getErrorMessage()
    {
        return m_error_message ;
    }

    bool
    CDDeserializer::validateMandatory()
    {
        if (true == notification)
        {
            return true;
        }
        else
        {
            return bValidateMandatory ;
        }
    }

    void CDDeserializer::setNotificationException(bool notificationException)
    {
        notification = notificationException;
    }

    void
    CDDeserializer::setValidateMandatory(bool enable)
    {
        this->bValidateMandatory = enable ;
    }

    void
    CDDeserializer::setIDExcemption(bool excemption)
    {
        this->bIdExcemtion = excemption ;
    }

    bool
    CDDeserializer::getIDExcemption()
    {
        return bIdExcemtion ;
    }

    void
    CDDeserializer::setHTTPMethod(std::string method)
    {
        this->method = method ;
    }

    bool
    CDDeserializer::removeParameter(std::string paramName)
    {
        if (m_pCurrent_property != NULL)
        {
            m_pCurrent_property->erase(paramName);
        }
        return true ;
    }

    OC::Cm::Serialization::ISerializable *
    CDDeserializer::createElement(std::string elementName)
    {

        OC::CDContext *pContext = (OC::CDContext *)getContext();
        OC::Cm::Serialization::ISerializable *retValue = NULL;

        OC::Cm::Serialization::SerializableDataFactory *factory = pContext->p_server->getSerializableDataFactory() ;

        if ( NULL == factory )
        {
            return NULL ;
        }
        else
        {
            if ( false == factory->createSerializable( elementName, &( retValue )))
            {
                return NULL ;
            }
        }




        return retValue;
    }
    bool
    CDDeserializer::getArrayPropertyValue(std::string arrayName, std::string propertyName, int index, std::string &value)
    {
        OCRepresentation array ;

        if (arrayName.empty())
        {
            array = root;
        }
        else
        {
            AttributeValue aDataValue ;
            if (root.getValue(arrayName, aDataValue) == true)
            {
                if (aDataValue.type() == typeid(OCRepresentation))
                {
                    array = boost::get<OCRepresentation>(aDataValue);
                }
                else  // Default
                {
                    array = root;
                }
            }
        }
        AttributeValue attVal;
        if (array.getValue(propertyName, attVal) == true)
        {
            if ( attVal.type() == typeid(std::vector<std::string>) )
            {
                std::vector<std::string> aAV = boost::get<std::vector<std::string>>(attVal);
                if (aAV.size() <= index )
                {
                    return false ;
                }
                value = aAV[index] ;
                return true ;
            }
        }
        value = "";
        return false;
    }

    bool
    CDDeserializer::getArrayPropertyValue(std::string arrayName, std::string propertyName, int index, bool &value)
    {
        OCRepresentation array ;

        if (arrayName.empty())
        {
            array = root;
        }
        else
        {
            AttributeValue aDataValue ;
            if (root.getValue(arrayName, aDataValue) == true)
            {
                if (aDataValue.type() == typeid(OCRepresentation))
                {
                    array = boost::get<OCRepresentation>(aDataValue);
                }
                else  // Default
                {
                    array = root;
                }
            }
        }
        AttributeValue attVal;
        if (array.getValue(propertyName, attVal) == true)
        {
            if ( attVal.type() == typeid(std::vector<bool>) )
            {
                std::vector<bool> aAV = boost::get<std::vector<bool>>(attVal);
                if (aAV.size() <= index )
                {
                    return false ;
                }
                value = aAV[index] ;
                //SHP_LOG_DEBUG_A(SHP_JSON,"Deser Value for bool :%s",value.c_str());
                return true ;
            }
        }
        value = false;
        //SHP_LOG_DEBUG_A(SHP_JSON,"Deser Value for String :%s",value.c_str());
        return false;
    }

}
