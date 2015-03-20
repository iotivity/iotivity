/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
/**
 * @file    ModelData.java
 *
 * @brief    This file provides a class for Model data operations
 * message
 *
 */

package org.iotivity.service.ssm;

/**
  * @class    ModelData
  * @brief    This class provides a set of API for DataReader class to handle the data
  *         received by IQueryEngineEvent listeners from SSMCore
  *
  */
public class ModelData
{

        private int pIModelDataInstance;

        public ModelData(int iModelDataInstance)
        {
            pIModelDataInstance = iModelDataInstance;
        }

        /**
            * @fn     GetDataId
            * @brief Get the affected DataId - ContextModel has plenty of data so the
            *            returned data is matched fom given condition and returned,
            *        which in this case is the model data object instance
            *
            * @return int - the affected DataId
            */
        public int GetDataId()
        {
            return CoreController.getInstance().GetDataId(pIModelDataInstance);
        }

        /**
            * @fn     GetPropertyCount
            * @brief ContextModel has at least one property that contains data \n
            *         the property is described from its specification.
            *
            * @return int -the property count
            */
        public int GetPropertyCount()
        {
            return CoreController.getInstance().GetPropertyCount(
                       pIModelDataInstance);
        }

        /**
            * @fn     GetPropertyName
            * @brief Retrieve the name of the property using the given index value
            *
            * @param [in] propertyIndex - index of property to read
            *
            * @return String - the property name
            */
        public String GetPropertyName(int propertyIndex)
        {
            return CoreController.getInstance().GetPropertyName(
                       pIModelDataInstance, propertyIndex);
        }

        /**
            * @fn     GetPropertyValue
            * @brief Retrieve the value of the property using the given index value
            *
            * @param [in] propertyIndex - index of property to read
            *
            * @return String - the property value
            */
        public String GetPropertyValue(int propertyIndex)
        {
            return CoreController.getInstance().GetPropertyValue(
                       pIModelDataInstance, propertyIndex);
        }
}