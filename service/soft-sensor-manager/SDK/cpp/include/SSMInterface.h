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
    * @see
    */
    class IModelData
    {
        public:
            /**
            * @fn     getDataId
            * @brief Get affected DataId. ContextModel has plenty of data so \n
            *         returned data is matched from given condition
            *
            *
            * @return int
            * @warning
            * @exception
            * @see
            */
            virtual int getDataId() = 0;

            /**
            * @fn     getPropertyCount
            * @brief ContextModel has at least one property that contains data \n
            *         property as described from its specification.
            *
            *
            * @return int
            * @warning
            * @exception
            * @see
            */
            virtual int getPropertyCount() = 0;

            /**
            * @fn     getPropertyName
            * @brief Retrieve propertyName
            *
            * @param [in] propertyIndex - index of property to read
            *
            * @return std::string
            * @warning
            * @exception
            * @see
            */
            virtual std::string getPropertyName(int propertyIndex) = 0;

            /**
            * @fn     getPropertyValue
            * @brief Retrieve propertyValue
            *
            * @param [in] propertyIndex - index of property to read
            *
            * @return std::string
            * @warning
            * @exception
            * @see
            */
            virtual std::string getPropertyValue(int propertyIndex) = 0;

            /**
            * @fn     getPropertyValueByName
            * @brief Retrieve propertyValue using given name
            *
            * @param [in] propertyName - property name to look for
            *
            * @return std::string
            * @warning
            * @exception
            * @see
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
            * @fn     getAffectedModels
            * @brief Get affected ContextModels. The CQL can specify multiple ContextModels for retrieving data.
            *
            * @param [in, out] pAffectedModels - affected ContextModel list
            *
            * @return SSMRESULT
            * @warning
            * @exception
            * @see
            */
            virtual SSMRESULT getAffectedModels(std::vector<std::string> *pAffectedModels) = 0;

            /**
            * @fn     getModelDataCount
            * @brief Get affected data count. There are multiple data that can exist from given condition.
            *
            * @param [in] modelName - affected ContextModel name
            *
            * @param [in, out] pDataCount - affected dataId count
            *
            * @return SSMRESULT
            * @warning
            * @exception
            * @see
            */
            virtual SSMRESULT getModelDataCount(std::string modelName, int *pDataCount) = 0;

            /**
            * @fn     getModelData
            * @brief Get actual Context Model data
            *
            * @param [in] modelName - affected ContextModel name
            *
            *
            * @param [in] dataIndex - affected dataId index
            *
            *
            * @param [out] ppModelData - affected ContextModel data reader
            *
            * @return SSMRESULT
            * @warning
            * @exception
            * @see
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
            * @fn     onQueryEngineEvent
            * @brief Transmit result of SSMCore to Application layer
            *
            * @param [in] cqid - entered ContextQuery ID
            *
            * @param [in] pResult - result of SSMCore
            *
            * @return SSMRESULT
            * @warning
            * @exception
            * @see
            */
            virtual SSMRESULT onQueryEngineEvent(int cqid, IDataReader *pResult) = 0;
        protected:
            virtual ~IQueryEngineEvent() {};
    };

    /**
    * @fn    InitializeSSM
    * @brief Initialize Soft sensor manager using given configuration information.
    *
    * @param [in] xmlDescription - specification described in XML
    *
    * @return SSMRESULT
    * @warning
    * @exception
    * @see
    */
    SSMRESULT InitializeSSM(std::string xmlDescription);

    /**
    * @fn    TerminateSSM
    * @brief Terminates Soft sensor manager
    *
    *
    * @return SSMRESULT
    * @warning
    * @exception
    * @see
    */
    SSMRESULT TerminateSSM();

    /**
    * @fn     RegisterQuery
    * @brief Execute ContextQuery and return ContextQuery ID
    *
    * @param [in] queryString - query for requesting data
    *
    * @param [in] listener - listener for receiving data related to query
    *
    * @param [in, out] cqid - ID of ContextQuery
    *
    * @return SSMRESULT
    * @warning
    * @exception
    * @see
    */
    SSMRESULT RegisterQuery(std::string queryString, IQueryEngineEvent *listener, int &cqid);

    /**
    * @fn    UnregisterQuery
    * @brief unregister registered ContextQuery according to cqid
    *
    * @param [in] cqid - Context query corresponding to the cqid to be unregistered
    *
    * @return SSMRESULT
    * @warning
    * @exception
    * @see
    */
    SSMRESULT UnregisterQuery(int cqid);
}
#endif