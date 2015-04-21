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
 * @file    SSMInterface.java
 *
 * @brief    This file gives description of SSMInterface class and its utility functions.
 *         This is the interface between an application and the query engine.
 *         SSMinterface makes desired queries in form of CQL(Context Query Language)
 *         to the query engine and passes the returned result obtained in form of callback
 *         back to the application.
 */

package org.iotivity.service.ssm;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * @class SSMInterface
 * @brief This class provides a set of APIs to manage the SSM framework
 *      This class sits in between the application and query engine and acts
 *      as an interface between them. SSMinterface makes desired queries in form of CQL(Context Query Language)
 *      to the query engine and passes the returned result obtained in form of callback
 *      back to the application
 */
public class SSMInterface
{

        /**
         * @class QueryEngineEventReceiver
         * @brief This class provides a set of APIs to handle query engine events
         *          related to soft sensor Interface framework.
         *
         *
         */
        private class QueryEngineEventReceiver extends IQueryEngineEvent
        {
                private Lock mMtxListener = new ReentrantLock();
                private Map<Integer, IQueryEngineEvent> mMapListener = new HashMap<Integer, IQueryEngineEvent>();

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
                public void onQueryEngineEvent(int cqid, DataReader result)
                {
                    mMtxListener.lock();

                    mMapListener.get(cqid).onQueryEngineEvent(cqid, result);

                    mMtxListener.unlock();
                }

                /**
                 * To lock QueryEngineEventReceiver object to execute a query atomically,
                 *         this is done to provide synchronization in case of multiple queries.
                 *
                 * @return void
                 * 
                 */
                void lockListener()
                {
                    mMtxListener.lock();
                }

                /**
                 * release the QueryEngineEventReceiver object
                 *
                 * @return void
                 * 
                 */
                void unlockListener()
                {
                    mMtxListener.unlock();
                }

                /**
                 * add listener to receive response for the registered query with SSMCore.
                 *     Listen for callbacks from SSMCore.
                 *     This is also a precondition for implementing query engine object.
                 *
                 * @param cqid 
                 *              - ContextQuery ID of the registered query
                 *
                 * @param engineEvent 
                 *              - query engine's event that contains the results
                 *
                 * @return void
                 */
                void addListener(int cqid, IQueryEngineEvent engineEvent)
                {
                    mMapListener.put(cqid, engineEvent);
                }

                /**
                 * Remove listener for a query on unregistering it
                 *
                 * @param cqid 
                 *              - ContextQuery ID of the registered query
                 *
                 * @return void
                 * 
                 */
                void removeListener(int cqid)
                {
                    mMapListener.remove(cqid);
                }
        };

        private CoreController mSSMCore = null;
        private QueryEngine mQueryEngine = null;
        private QueryEngineEventReceiver mQueryEngineEventListenerReceiver = new QueryEngineEventReceiver();
        private List<Integer> mRunningCQLs = new ArrayList<Integer>();

        public SSMInterface()
        {
        }

        /**
         * Starts the framework that allows other devices to discover and communicate
         *     with the SSMCore and underlying query engine.
         *
         * @param initConfig 
         *              - initial framework specifications
         *
         * @return void
         * 
         */
        public void startSSMCore(String initConfig) throws Exception
        {
            mSSMCore = CoreController.getInstance();
            mSSMCore.initializeSSMCore(initConfig);
            mSSMCore.startSSMCore();

            mQueryEngine = mSSMCore.createQueryEngine();

            mQueryEngine.registerQueryEvent(mQueryEngineEventListenerReceiver);
        }

        /**
         * Stops the framework and terminate all communications.
         *
         * @return void
         * 
         */
        public void stopSSMCore() throws Exception
        {
            mQueryEngine.registerQueryEvent(null);
            mSSMCore.releaseQueryEngine(mQueryEngine);
            mQueryEngineEventListenerReceiver = null;
            mQueryEngine = null;
            mSSMCore.stopSSMCore();
            mSSMCore.terminateSSMCore();
        }

        /**
         * Register the query and execute statement with the query engine
         *     and add listener for the registered query so as to get response data.
         *     After success response message for registration, SSMCore sends an
         *     event to the client, if the specified condtions in the query is satisfied.
         *
         * @param contextQuery 
         *             - query for requesting data
         *
         * @param listener 
         *             - listener for receiving response data of the query
         *
         * @return int - ContextQuery ID
         * 
         */
        public int registerQuery(String contextQuery, IQueryEngineEvent listener)
        throws Exception
        {
            int cqid = 0;

            try {
                mQueryEngineEventListenerReceiver.lockListener();
                cqid = mQueryEngine.executeContextQuery(contextQuery);
                mQueryEngineEventListenerReceiver.addListener(cqid, listener);
                mRunningCQLs.add(cqid);
            }
            catch (Exception e)
            {
                throw e;
            } finally {
                mQueryEngineEventListenerReceiver.unlockListener();
            }

            return cqid;
        }

        /**
         * unregister a registered query using its query ID. The query corresponding
         *      to the cqid will be terminated and removes listener for the given query
         *      The SSMCore will not send any callbacks after successful unregistration.
         *
         *
         * @param cqid - ContextQuery ID of the query to be unregistered
         *
         * @return void
         * 
         */
        public void unregisterQuery(int cqid) throws Exception
        {
            try {
                mQueryEngineEventListenerReceiver.lockListener();
                mQueryEngine.killContextQuery(cqid);
                mQueryEngineEventListenerReceiver.removeListener(cqid);
                mRunningCQLs.remove((Object) cqid);
            }
            catch (Exception e)
            {
                throw e;
            } finally {
                mQueryEngineEventListenerReceiver.unlockListener();
            }
        }
}