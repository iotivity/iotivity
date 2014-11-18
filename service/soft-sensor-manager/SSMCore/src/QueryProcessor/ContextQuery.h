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
#ifndef _ContextQuery_H_
#define _ContextQuery_H_

#include "SSMInterface/SSMCore.h"
#include "Common/PlatformLayer.h"
#include "Common/InternalInterface.h"
#include "CQLParser.h"

#define SIZE 100
#define MIN_NUM 10000


class result_model
{
    public:
        result_model();
        int modelID;
        std::vector<int> dataId;
        std::string modelName;
};

class sub_modelId
{
    public:
        std::vector<int> modelId;
};


/**
* @class    ContextQuery
* @brief    ContextQuery Interface
*            This class provides functions for parsing.
*
* @see
*           CContextQuery contextquery;
*           contextquery->check_result_model();
*/
class CContextQuery
{

    public:
        /**
        * @fn CContextQuery
        * @brief initializer
        *
        * @param none
        * @return none
        * @warning
        * @exception
        * @see
        */
        CContextQuery();

        /**
        * @fn initialize
        * @brief input Token
        *
        * @param [in] Token input_root - root Token
        * @return SSMRESULT
        * @warning
        * @exception
        * @see
        */
        SSMRESULT initialize(IN Token &input_root);

        /**
        * @fn integrate_result
        * @brief This function checks for duplicate results.
        *
        * @param [out] std::vector<result_model>* result - result
        * @param [in] int modelId - entered modelID
        * @param [in] std::vector<int> *dataid - dataID according to modelID
        * @param [in] std::string modelName - modelName according to modelID
        * @return none
        * @warning
        * @exception
        * @see
        */
        void integrate_result(OUT std::vector<result_model> *result, IN int modelId,
                              IN std::vector<int> *dataid, IN std::string modelName);

        /**
        * @fn return_contextName
        * @brief This function outputs ModelName of result clause
        *
        * @param [out] std::vector<std::string>* contextName - ModelName of result clause
        * @return none
        * @warning
        * @exception
        * @see
        */
        void return_contextName(OUT std::vector<std::string> *contextName);

        /**
        * @fn check_result_model
        * @brief This function checks the existence of ContextModel from result clause
        *
        * @param none
        * @return none
        * @warning
        * @exception
        * @see
        */
        void check_result_model();

        /**
        * @fn return_modelID
        * @brief This function returns ContextModelID of result clause
        *
        * @param [out] std::vector<int>* vector_int - ContextModelID
        * @return none
        * @warning
        * @exception
        * @see
        */
        void return_modelID(OUT std::vector<int> *vector_int);

        /**
        * @fn make_QueryCondition
        * @brief This function makes QueryCondition using tokens.
        *
        * @param [out] QueryCondition* result - QueryCondition
        * @return none
        * @warning
        * @exception
        * @see
        */
        void make_QueryCondition(OUT QueryCondition *result);

    private:
        /**
        * @fn search_last_modelName
        * @brief return last child name
        *
        * @param [in] Token* temp - entered token
        * @return std::string
        * @warning
        * @exception
        * @see
        */
        std::string search_last_modelName(IN Token *temp);

        Token m_root;
        CObjectPtr<IPropagationEngine>      m_pPropagationEngine;
};

#endif /*_ContextQuery_H_*/
