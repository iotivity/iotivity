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
 * @brief This class provides a set of APIs relating to soft sensor manager It
 *        contains utility API's for DataReader, QueryEngine and SSMInterface
 *        classes.
 */
public class CoreController {
    static {
        try {
            String workingPath = System.getProperty("user.dir");

            // for android: not complete method
            if (System.getProperty("os.name").toLowerCase().equals("linux")) {
                System.loadLibrary("gnustl_shared");
                System.loadLibrary("oc_logger");
                System.loadLibrary("connectivity_abstraction");
                System.loadLibrary("octbstack");
                System.loadLibrary("oc");
                System.loadLibrary("SSMCore");
            } else {
                System.load(workingPath + "SSMCore_Windows.dll");
            }
        } catch (UnsatisfiedLinkError e) {
            System.out.println(e.getMessage());
        }
    }

    static private CoreController coreController;

    /**
     * Return instance of CoreController.
     * 
     * @return CoreController - instance of this class
     */
    public static CoreController getInstance() {
        if (coreController == null)
            coreController = new CoreController();

        return coreController;
    }

    /**
     * Initialize a framework using the given configuration
     * 
     * @param xmlDescription
     *            - Framework specifications described in XML format
     * 
     * @return void
     */
    public native void initializeSSMCore(String xmlDescription)
            throws Exception;

    /**
     * Starts the framework that allows other devices to discover and
     * communicate with the SSMCore and underlying query engine.
     * 
     * @return void
     */
    public native void startSSMCore() throws Exception;

    /**
     * Stops the framework and terminate all communications.
     * 
     * @return void
     */
    public native void stopSSMCore() throws Exception;

    /**
     * Terminate the framework, return all allocated resources
     * 
     * @return void
     */
    public native void terminateSSMCore() throws Exception;

    /**
     * Creates an instance of the Query Engine
     * 
     * @return QueryEngine - QueryEngine object created
     */
    public native QueryEngine createQueryEngine() throws Exception;

    /**
     * Release the QueryEngine object.
     * 
     * @param queryEngine
     *            - QueryEngine instance to be released
     * 
     * @return int - success or failure result
     */
    public native int releaseQueryEngine(QueryEngine queryEngine);

    /**
     * Execute the query with the QueryEngine and return ContextQuery ID for the
     * query
     * 
     * @param pQueryEngineInstance
     *            - instance(created on creation of the QueryEngine) of the
     *            query engine with which the query is to be executed
     * 
     * @param contextQuery
     *            - query string to be executed
     * 
     * @return int - ID of context query executed
     */
    public native int executeContextQuery(int pQueryEngineInstance,
            String contextQuery) throws Exception;

    /**
     * Register QueryEngineEvent to QueryEngine.
     * 
     * @param pQueryEngineInstance
     *            - instance(created on creation of the QueryEngine) of the
     *            query engine with which the query is to be registered
     * 
     * @param queryEngineEvent
     *            - the event class to receive QueryEngine events
     * 
     * @return void
     */
    public native void registerQueryEvent(int pQueryEngineInstance,
            IQueryEngineEvent queryEngineEvent);

    /**
     * Kill the registered ContextQuery corresponding to the cqid
     * 
     * @param pQueryEngineInstance
     *            - instance(created on creation of the QueryEngine) of the
     *            query engine with which the query is to be terminated
     * 
     * @param cqid
     *            - ContextQuery ID of the query to be terminated
     * 
     * @return void
     */
    public native void killContextQuery(int pQueryEngineInstance, int cqid)
            throws Exception;

    /**
     * Get the affected DataId. ContextModel has plenty of data so returned data
     * is matched from given condition which in this case is the model data
     * object instance
     * 
     * @param pIModelDataInstance
     *            - instance(created on creation of the ModelData) of the model
     *            data for which Data ID is to be returned
     * 
     * @return int - the affected DataId
     */
    public native int getDataId(int pIModelDataInstance);

    /**
     * ContextModel has at least one property that contains data \n property is
     * described from its specification.
     * 
     * @param pIModelDataInstance
     *            - instance(created on creation of the ModelData) of the model
     *            data for which property count is to be returned
     * 
     * @return @return int - number of properties
     */
    public native int getPropertyCount(int pIModelDataInstance);

    /**
     * Retrieve the name of the property using the given index
     * 
     * @param pIModelDataInstance
     *            - instance(created on creation of the ModelData) of the model
     *            data for which property name is to be returned
     * 
     * @param propertyIndex
     *            - index of property to read
     * 
     * @return String - property name
     */
    public native String getPropertyName(int pIModelDataInstance,
            int propertyIndex);

    /**
     * Retrieve the value of the property using the given index
     * 
     * @param pIModelDataInstance
     *            - instance(created on creation of the ModelData) of the model
     *            data for which property value is to be returned
     * 
     * @param propertyIndex
     *            - index of property to read
     * 
     * @return String - property value
     */
    public native String getPropertyValue(int pIModelDataInstance,
            int propertyIndex);

    /**
     * Get affected context models. The CQL(context query language) can specify
     * multiple ContextModels for retrieving data so a list of string of
     * affected context models is returned
     * 
     * @param pDataReaderInstance
     *            - instance(created on creation of the DataReader) of the data
     *            reader class object
     * 
     * @return List<String> - affected ContextModel list
     */
    public native List<String> getAffectedModels(int pDataReaderInstance);

    /**
     * Get affected model data count. There can be multiple data models existing
     * from the given condition, return the count.
     * 
     * @param pDataReaderInstance
     *            - instance(created on creation of the DataReader) of the data
     *            reader class object
     * 
     * @param modelName
     *            - affected ContextModel name
     * 
     * @return int - affected dataId count
     */
    public native int getModelDataCount(int pDataReaderInstance,
            String modelName) throws Exception;

    /**
     * Get actual Context Model data
     * 
     * @param pDataReaderInstance
     *            - instance(created on creation of the DataReader) of the data
     *            reader class object
     * 
     * @param modelName
     *            - affected ContextModel name
     * 
     * @param dataIndex
     *            - affected dataId index
     * 
     * @return ModelData - affected ContextModel data reader
     */
    public native ModelData getModelData(int pDataReaderInstance,
            String modelName, int dataIndex) throws Exception;

    /**
     * To register the report receiver to receive messages
     * 
     * @param reportReceiver
     *            - report receiver instance
     * 
     * @return void
     */
    public native void registerReportReceiver(IReportReceiver reportReceiver);
}