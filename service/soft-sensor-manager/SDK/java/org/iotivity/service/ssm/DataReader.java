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
 * @file    DataReader.java
 *
 * @brief    This file provides a class that represents context model data package's reader
 *
 */

package org.iotivity.service.ssm;

import java.util.List;

/**
* @class    DataReader
* @brief     This class represents context model data package's reader and contains
*           API's to be used by IQueryEngineEvent listeners to handle the model data.
*
*/
public class DataReader
{

        private int pDataReaderInstance;

        public DataReader(int dataReaderInstance)
        {
            pDataReaderInstance = dataReaderInstance;
        }

        /**
         * Get affected context models. The CQL(context query language) can specify
         *       multiple ContextModels for retrieving data so a list of string of affected
         *       context models is returned
         *
         * @return List<String> - affected ContextModel list
         * 
         */
        public List<String> getAffectedModels()
        {
            return CoreController.getInstance().getAffectedModels(
                       pDataReaderInstance);
        }

        /**
         * Get affected model data count. There can be multiple data models existing
         *       from the given condition, return the count.
         *
         * @param modelName 
         *              - affected ContextModel name
         *
         * @return int  - affected dataId count
         * 
         */
        public int getModelDataCount(String modelName) throws Exception
        {
            return CoreController.getInstance().getModelDataCount(
                pDataReaderInstance, modelName);
        }

        /**
         * Get actual Context Model data
         *
         * @param modelName 
         *              - affected ContextModel name
         *
         * @param dataIndex 
         *              - affected dataId index
         *
         * @return ModelData  - affected ContextModel data reader
         * 
         */
        public ModelData getModelData(String modelName, int dataIndex)
        throws Exception
        {
            return CoreController.getInstance().getModelData(pDataReaderInstance,
            modelName, dataIndex);
        }
}
