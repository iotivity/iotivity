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
         * @fn     GetQueryEngineInstance
         * @brief Obtain the instance value of the called query engine object,
         *          this value is set on creation of the QueryEngine object
         *
         * @return int - Instance of the query engine object, set on object's creation
         */
        public int GetQueryEngineInstance()
        {
            return pQueryEngineInstance;
        }

        /**
         * @fn     ExecuteContextQuery
         * @brief Execute the query with the QueryEngine and return the generated ContextQuery ID
         *          for the query string.
         *
         * @param [in] contextQuery - query string to execute
         *
         * @return int - generated ID for the ContextQuery string
         */
        public int ExecuteContextQuery(String contextQuery) throws Exception
        {
            return CoreController.getInstance().ExecuteContextQuery(
                pQueryEngineInstance, contextQuery);
        }

        /**
         * @fn     RegisterQueryEvent
         * @brief Register QueryEngineEvent to the QueryEngine.
         *
         * @param [in] queryEngineEvent - the event to register with QueryEngine
         *
         * @return void
         */
        public void RegisterQueryEvent(IQueryEngineEvent queryEngineEvent)
        {
            CoreController.getInstance().RegisterQueryEvent(pQueryEngineInstance,
                    queryEngineEvent);
        }

        /**
         * @fn    KillContextQuery
         * @brief Kill the registered ContextQuery corresponding to the  to cqid
         *
         * @param [in] cqid - ContextQuery ID of the query to be terminated.
         *                   This ID was generated on execution of the query
         *
         * @return void
         */
        public void KillContextQuery(int cqid) throws Exception
        {
            CoreController.getInstance().KillContextQuery(pQueryEngineInstance,
            cqid);
        }
}