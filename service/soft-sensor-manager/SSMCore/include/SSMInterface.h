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

/**
* @file
*
* This file defines basic interface of Soft Sensor Manager.
*/

#ifndef _SSMInterface_H_
#define _SSMInterface_H_

#include <string>
#include <vector>

namespace OIC
{
    enum SSMRESULT
    {
        SSM_S_OK
        , SSM_S_FALSE
        , SSM_E_POINTER
        , SSM_E_OUTOFMEMORY
        , SSM_E_FAIL
        , SSM_E_NOTINIT
        , SSM_E_INITIALIZED
        , SSM_E_INVALIDXML
        , SSM_E_NOINTERFACE
        , SSM_E_NOTIMPL
    };

    /**
    * @class    IModelData
    * @brief    This class represents context model data package
    *
    */
    class IModelData
    {
        public:
            /**
            * Get affected DataId. ContextModel has plenty of data so \n
            *         returned data is matched from given condition
            *
            * @return int - the affected DataId
            */
            virtual int getDataId() = 0;

            /**
            * ContextModel has at least one property that contains data \n
            *         property is described from its specification.
            *
            * @return int - number of properties
            */
            virtual int getPropertyCount() = 0;

            /**
            * Retrieve propertyName
            *
            * @param propertyIndex - index of property to read
            *
            * @return std::string - property name
            */
            virtual std::string getPropertyName(int propertyIndex) = 0;

            /**
            * Retrieve propertyValue
            *
            * @param propertyIndex index of property to read
            *
            * @return std::string - property value
            */
            virtual std::string getPropertyValue(int propertyIndex) = 0;

            /**
            * Retrieve propertyValue using given name
            *
            * @param propertyName - property name looking for
            *
            * @return std::string - property name
            */
            virtual std::string getPropertyValueByName(std::string propertyName) = 0;
        protected:
            virtual ~IModelData() {};
    };

    /**
    * @class    IDataReader
    * @brief    This class represents context model data package's reader
    *
    * @see
    */
    class IDataReader
    {
        public:
            /**
            * Get affected ContextModels. The query can specify multiple ContextModels for retrieving data.
            *
            * @param pAffectedModels - affected ContextModel list
            *
            * @return SSMRESULT - return value of the API. Returns SSM_S_OK if success
            */
            virtual SSMRESULT getAffectedModels(std::vector<std::string> *pAffectedModels) = 0;

            /**
            * Get affected data count. There are multiple data can exist from given condition.
            *
            * @param modelName - affected ContextModel name
            *
            * @param pDataCount - affected dataId count
            *
            * @return SSMRESULT - return value of the API. Returns SSM_S_OK if success
            */
            virtual SSMRESULT getModelDataCount(std::string modelName, int *pDataCount) = 0;

            /**
            * Get actual Context Model data
            *
            * @param modelName - affected ContextModel name
            *
            * @param dataIndex - affected dataId index
            *
            * @param ppModelData - affected ContextModel data reader
            *
            * @return SSMRESULT - return value of the API. Returns SSM_S_OK if success
            */
            virtual SSMRESULT getModelData(std::string modelName, int dataIndex, IModelData **ppModelData) = 0;
        protected:
            virtual ~IDataReader() {};
    };

    /**
    * @class    IQueryEngineEvent
    * @brief    This class represents Query Engine's event that contains results
    *
    * @see
    */
    class IQueryEngineEvent
    {
        public:
            /**
            * Transmit result of SSMCore to Application layer
            *
            * @param cqid - entered ContextQuery ID
            *
            * @param pResult - result of SSMCore
            *
            * @return SSMRESULT - return value of the API. Returns SSM_S_OK if success
            */
            virtual SSMRESULT onQueryEngineEvent(int cqid, IDataReader *pResult) = 0;
        protected:
            virtual ~IQueryEngineEvent() {};
    };

    /**
    * Initialize Soft sensor manager using given configuration information.
    *
    * @param xmlDescription - specification described in XML
    *
    * @return SSMRESULT - return value of the API. Returns SSM_S_OK if success
    */
    SSMRESULT InitializeSSM(std::string xmlDescription);

    /**
    * Terminates Soft sensor manager
    *
    * @return SSMRESULT - return value of the API. Returns SSM_S_OK if success
    */
    SSMRESULT TerminateSSM();

    /**
    * Execute query and return ContextQuery ID
    *
    * @param queryString - query for requesting data
    *
    * @param listener - listener for receiving data related to query
    *
    * @param cqid - ID of ContextQuery
    *
    * @return SSMRESULT - return value of the API. Returns SSM_S_OK if success
    */
    SSMRESULT RegisterQuery(std::string queryString, IQueryEngineEvent *listener, int &cqid);

    /**
    * Unregister registered query according to cqid
    *
    * @param cqid - Query corresponding to the cqid will be terminated
    *
    * @return SSMRESULT - return value of the API. Returns SSM_S_OK if success
    */
    SSMRESULT UnregisterQuery(int cqid);
}
#endif