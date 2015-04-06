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
 * @file    QueryEngine.java
 *
 * @brief    This file provides a class that represents main interface of Query Engine
 *
 */

package org.iotivity.service.ssm;

/**
* @class    QueryEngine
* @brief     This class represents main interface of Query Engine to interact with
*           SSMCore with in turn interacts with the application.
*           QueryEninge class forms the database from the base to respond to
*           queries from SSMCore.
*
*/
public class QueryEngine
{
        private int pQueryEngineInstance;

        public QueryEngine(int queryEngineInstance)
        {
            pQueryEngineInstance = queryEngineInstance;
        }

        /**
         * Obtain the instance value of the called query engine object,
         *          this value is set on creation of the QueryEngine object
         *
         * @return int - Instance of the query engine object, set on object's creation
         * 
         */
        public int getQueryEngineInstance()
        {
            return pQueryEngineInstance;
        }

        /**
         * Execute the query with the QueryEngine and return ContextQuery ID
         *            for the query
         *
         * @param contextQuery 
         *               - query string to be executed
         *
         * @return int - ID of context query executed
         * 
         */
        public int executeContextQuery(String contextQuery) throws Exception
        {
            return CoreController.getInstance().executeContextQuery(
                pQueryEngineInstance, contextQuery);
        }

        /**
         * Register QueryEngineEvent to QueryEngine
         *
         * @param queryEngineEvent 
         *               - the event class to receive QueryEngine events
         *
         * @return void
         * 
         */
        public void registerQueryEvent(IQueryEngineEvent queryEngineEvent)
        {
            CoreController.getInstance().registerQueryEvent(pQueryEngineInstance,
                    queryEngineEvent);
        }

        /**
         * Kill the registered ContextQuery corresponding to the  to cqid
         *
         * @param cqid 
         *              - ContextQuery ID of the query to be terminated
         *
         * @return void
         * 
         */
        public void killContextQuery(int cqid) throws Exception
        {
            CoreController.getInstance().killContextQuery(pQueryEngineInstance,
            cqid);
        }
}