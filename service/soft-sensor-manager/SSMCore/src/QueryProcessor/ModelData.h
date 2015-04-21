/******************************************************************
*
* Copyright 2014 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/
#ifndef _ModelData_H_
#define _ModelData_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"
/**
* @class    CModelData
* @brief    CModelData Interface
*            This class represents context model data package
*
* @see
*/
class CModelData : public IModelData
{
    private:
        int m_dataId;
        std::map<std::string, std::string> m_properties;

    public:
        CModelData();

        ~CModelData();

        /**
        * @fn     setDataId
        * @brief Set affected DataId. This method is only for constructing data.
        *
        * @param [in] int dataId - Represent data Id
        *
        * @return NONE
        * @warning
        * @exception
        * @see
        */
        void setDataId(int dataId);

        /**
        * @fn     addModelData
        * @brief Add affected model data. This method is only for constructing data.
        *
        * @param [in] std::string propertyName - Represent property name
        * @param [in] std::string propertyValue - Represent property value
        *
        * @return NONE
        * @warning
        * @exception
        * @see
        */
        void addModelData(std::string propertyName, std::string propertyValue);

        /**
        * @fn     getDataId
        * @brief Get affected DataId. ContextModel has plenty of data so \n
        *         returned data is matched from given condition
        *
        * @param None
        *
        * @return int
        * @warning
        * @exception
        * @see
        */
        int getDataId();

        /**
        * @fn     getPropertyCount
        * @brief ContextModel has at least one property that contains data \n
        *         property is described from its specification.
        *
        * @param None
        *
        * @return int
        * @warning
        * @exception
        * @see
        */
        int getPropertyCount();

        /**
        * @fn     getPropertyName
        * @brief Retrieve propertyName
        *
        * @param [in] int propertyIndex - index of property to read
        *
        * @return std::string
        * @warning
        * @exception
        * @see
        */
        std::string getPropertyName(int propertyIndex);

        /**
        * @fn     getPropertyValue
        * @brief Retrieve propertyValue
        *
        * @param [in] int propertyIndex - index of property to read
        *
        * @return std::string
        * @warning
        * @exception
        * @see
        */
        std::string getPropertyValue(int propertyIndex);

        /**
        * @fn     getPropertyValueByName
        * @brief Retrieve propertyValue using given name
        *
        * @param [in] std::string propertyName - property name looking for
        *
        * @return std::string
        * @warning
        * @exception
        * @see
        */
        std::string getPropertyValueByName(std::string propertyName);
};

#endif
