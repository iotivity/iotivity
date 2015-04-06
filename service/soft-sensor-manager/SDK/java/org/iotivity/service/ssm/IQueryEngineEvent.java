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
 * @file    IQueryEngineEvent.java
 *
 * @brief    This file provides a class that represents Query Engine's event that contains results
 *
 */

package org.iotivity.service.ssm;

    /**
    * @class    IQueryEngineEvent
    * @brief    This is an interface class for sample applications.It handles the response from
    *            SSMCore received asynchronously on making a query with the SSMCore.
    *            Application should implement the pure virtual function OnQueryEngineEvent.
    *            The Application should call registerQuery() and add a listener.
    *
    *<pre>
    Sample code :

        int cqid = SoftSensorManager.registerQuery(edtQuery.getText().toString(), mQueryEngineEventListener);
        mRunningQueries.add(cqid);

        private IQueryEngineEvent mQueryEngineEventListener = new IQueryEngineEvent()
        {
        @Override
        public void onQueryEngineEvent(int cqid, DataReader result)
        {
                List<String> models = result.GetAffectedModels();

                for(String modelName : models)
                {
                    int dataCount = result.GetModelDataCount(modelName);
                    for(int i=0; i < dataCount; i++)
                    {
                        ModelData modelData = result.GetModelData(modelName, i);
                        for(int j=0; j < modelData.GetPropertyCount(); j++)
                        {
                            PrintLog("Name: " + modelData.GetPropertyName(j) +
                                " Value: " + modelData.GetPropertyValue(j));
                        }
                    }
                }
        }
        };</pre>
    */
public abstract class IQueryEngineEvent
{

        /**
         * Transmits result of SSMCore to Application layer
         *     This abstract method needs to be implemeted by application
         *     as a precondition for query engine implementation.
         *
         * @param cqid 
         *              - ContextQuery ID of the registered query
         *
         * @param result 
         *              - data received from SSMCore
         *
         * @return void
         *
         */
        public abstract void onQueryEngineEvent(int cqid, DataReader result);
}