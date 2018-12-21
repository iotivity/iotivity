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

#ifndef REQUEST_MODEL_H_
#define REQUEST_MODEL_H_

#include "simulator_resource_model_schema.h"
#include "response_model.h"

typedef std::map<std::string, std::vector<std::string>> SupportedQueryParams;

/**
 * @class RequestModelBuilder
 */
class RequestModelBuilder;
class RequestModel
{
    public:
        friend class RequestModelBuilder;
        /**
         * This method is to get the request type
         * @return request type
         */
        std::string getType() const;
        /**
         * This method is get the query parameters
         * @return query parameters object
         */
        SupportedQueryParams getQueryParams();
        /**
         * This method is to get the query param with the given key value
         * @return query parameters object
         */
        std::vector<std::string> getQueryParams(const std::string &key);
        /**
         * This method is to get the request representation schema
         * @return request representation schema object
         */
        std::shared_ptr<SimulatorResourceModelSchema> getRequestRepSchema();
        /**
         * This method is to get the response model with the code value
         * @param[in] code   code value
         */
        ResponseModelSP getResponseModel(int code);
        /** This method is to validate the response */
        SimulatorResult validateResponse(int code, const SimulatorResourceModel &resModel);

    private:
        RequestModel() = default;
        RequestModel(const std::string &type);
        void setType(const std::string &type);
        void setQueryParams(const std::string &key, const std::vector<std::string> &values);
        void addQueryParam(const std::string &key, const std::string &value);
        void setResponseModel(int code, ResponseModelSP &responseModel);
        void setRequestBodyModel(const std::shared_ptr<SimulatorResourceModelSchema> &repSchema);

        std::string m_type;
        SupportedQueryParams m_queryParams;
        std::map<int, ResponseModelSP> m_responseList;
        std::shared_ptr<SimulatorResourceModelSchema> m_repSchema;
};

typedef std::shared_ptr<RequestModel> RequestModelSP;

#endif
