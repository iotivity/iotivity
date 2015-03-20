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
 * @file    CoreController.java
 *
 * @brief    This file provides a class containing a set of APIs for soft sensor manager
 *          application.
 *
 */

package org.iotivity.service.ssm;

import java.util.List;

/**
 * @class CoreController
 * @brief This class provides a set of APIs relating to soft sensor manager
 *         It contains utility API's  for DataReader, QueryEngine and SSMInterface classes.
 *
 *
 */
public class CoreController
{
        static
        {
            try {
                String workingPath = System.getProperty("user.dir");

                // for android: not complete method
                if (System.getProperty("os.name").toLowerCase().equals("linux"))
                {
                    // System.out.println("System load Android library");
                    System.loadLibrary("oc_logger");
                    System.loadLibrary("coap");
                    System.loadLibrary("octbstack");
                    System.loadLibrary("oc");
                    System.loadLibrary("SSMCore");
                }
                else {
                    // System.out.println("System load 32bit library");
                    System.load(workingPath + "SSMCore_Windows.dll");
                }
            }
            catch (UnsatisfiedLinkError e)
            {
                System.out.println(e.getMessage());
            }
        }

        static private CoreController coreController;

        public static CoreController getInstance()
        {
            if (coreController == null)
                coreController = new CoreController();

            return coreController;
        }

        /**
        * @fn    InitializeSSMCore
        * @brief Initialize a framework using the given configuration
        *
        * @param [in] xmlDescription - Framework specifications described in XML format
        *
        *
        * @return void
        */
        public native void InitializeSSMCore(String xmlDescription)
        throws Exception;

        /**
          * @fn    StartSSMCore
          * @brief Starts the framework that allows other devices to discover and communicate
          *     with the SSMCore and underlying query engine.
          *
          * @return void
          */
        public native void StartSSMCore() throws Exception;

        /**
          * @fn    StopSSMCore
          * @brief Stops the framework and terminate all communications.
          *
          * @return void
          */
        public native void StopSSMCore() throws Exception;

        /**
        * @fn    TerminateSSMCore
        * @brief Terminate the framework, return all allocated resources
        *
        * @return void
        */
        public native void TerminateSSMCore() throws Exception;

        /**
        * @fn    CreateQueryEngine
        * @brief Creates an instance of the Query Engine
        *
        * @return QueryEngine - QueryEngine object created
        */
        public native QueryEngine CreateQueryEngine() throws Exception;

        /**
        * @fn    ReleaseQueryEngine
        * @brief Release the QueryEngine object.
        *
        * @param [in] queryEngine - QueryEngine instance to be released
        *
        * @return int - success or failure result
        */
        public native int ReleaseQueryEngine(QueryEngine queryEngine);

        // QueryEngine
        /**
           * @fn     ExecuteContextQuery
           * @brief Execute the query with the QueryEngine and return ContextQuery ID
           *            for the query
           *
           * @param [in] pQueryEngineInstance - instance(created on creation of the QueryEngine)
           *             of the query engine with which the query is to be executed
           *
           * @param [in] contextQuery - query string to be executed
           *
           * @return int - ID of context query executed
           */
        public native int ExecuteContextQuery(int pQueryEngineInstance,
                                              String contextQuery) throws Exception;

        /**
         * @fn     RegisterQueryEvent
         * @brief Register QueryEngineEvent to QueryEngine.
         *
         * @param [in] pQueryEngineInstance - instance(created on creation of the QueryEngine)
         *               of the query engine with which the query is to be registered
         *
         * @param [in] queryEngineEvent - the event to register with QueryEngine
         *
         * @return void
         */
        public native void RegisterQueryEvent(int pQueryEngineInstance,
                                              IQueryEngineEvent queryEngineEvent);


        /**
         * @fn    KillContextQuery
         * @brief Kill the registered ContextQuery corresponding to the  to cqid
         *
         * @param [in] pQueryEngineInstance - instance(created on creation of the QueryEngine)
         *               of the query engine with which the query is to be terminated
         *
         * @param [in] cqid - ContextQuery ID of the query to be terminated
         *
         * @return void
         */
        public native void KillContextQuery(int pQueryEngineInstance, int cqid)
        throws Exception;

        // public native void UnregiterQueryEvent(int pQueryEngineInstance,
        // IQueryEngineEvent queryEngineEvent);

        // IModelData
        /**
            * @fn     GetDataId
           * @brief Get the affected DataId - ContextModel has plenty of data so
            *           returned data is matched from given condition which in this case is the model
            *        data object instance
            *
            *@param [in] pIModelDataInstance - instance(created on creation of the ModelData)
            *                of the model data for which Data ID is to be returned
             *
            * @return int - the affected DataId
            */
        public native int GetDataId(int pIModelDataInstance);

        /**
            * @fn     GetPropertyCount
            * @brief ContextModel has at least one property that contains data \n
            *           property is described from its specification.
            *
            *@param [in] pIModelDataInstance - instance(created on creation of the ModelData)
            *                of the model data for which propertycount is to be returned
            *
            * @return int
            */
        public native int GetPropertyCount(int pIModelDataInstance);

        /**
            * @fn     GetPropertyName
            * @brief Retrieve the name of the property using the given index
            *
            * @param [in] pIModelDataInstance - instance(created on creation of the ModelData)
            *                of the model data for which property name is to be returned
            *
            * @param [in] propertyIndex - index of property to read
            *
            * @return String - property name
            */
        public native String GetPropertyName(int pIModelDataInstance,
                                             int propertyIndex);

        /**
            * @fn     GetPropertyValue
            * @brief Retrieve the value of the property using the given index
            *
            * @param [in] pIModelDataInstance - instance(created on creation of the ModelData)
            *                of the model data for which property value is to be returned
            *
            * @param [in] propertyIndex - index of property to read
            *
            * @return String - property value
            */
        public native String GetPropertyValue(int pIModelDataInstance,
                                              int propertyIndex);

        // IDataReader
        /**
            * @fn     GetAffectedModels
            * @brief Get affected context models. The CQL(context query language) can specify
            *       multiple ContextModels for retrieving data so a list of string of affected
            *       dacontext models is returned
            *
            * @param [in] pDataReaderInstance -  instance(created on creation of the DataReader)
            *                of the data reader class object
            *
            * @return List<String> - affected ContextModel list
            */
        public native List<String> GetAffectedModels(int pDataReaderInstance);

        /**
            * @fn     GetModelDataCount
            * @brief Get affected model data count. There can be multiple data models existing
            *       from the given condition, return the count.
            *
            * @param [in] pDataReaderInstance - instance(created on creation of the DataReader)
            *                of the data reader class object
            *
            * @param [in] modelName - affected ContextModel name
            *
            * @return int  - affected dataId count
            */
        public native int GetModelDataCount(int pDataReaderInstance,
                                            String modelName) throws Exception;

        /**
            * @fn     GetModelData
            * @brief Get actual Context Model data
            *
            * @param [in] pDataReaderInstance - instance(created on creation of the DataReader)
            *                of the data reader class object
            *
            * @param [in] modelName - affected ContextModel name
            *
            * @param [in] dataIndex - affected dataId index
            *
            * @return ModelData  - affected ContextModel data reader
            */
        public native ModelData GetModelData(int pDataReaderInstance,
                                             String modelName, int dataIndex) throws Exception;

        /**
            * @fn     RegisterReportReceiver
            * @brief To register the report receiver to receive messages
            *
            * @param [in] reportReceiver - report receiver instance
            *
            * @return void
            */
        public native void RegisterReportReceiver(IReportReceiver reportReceiver);
}