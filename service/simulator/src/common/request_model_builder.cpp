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

#include "request_model_builder.h"
#include "logger.h"

#define TAG "REQ_MODEL_BUILDER"

RequestModelBuilder::RequestModelBuilder(std::shared_ptr<RAML::Raml> &raml)
    : m_raml (raml) {}

std::map<RequestType, RequestModelSP> RequestModelBuilder::build(const std::string &uri)
{
    std::map<RequestType, RequestModelSP> modelList;
    if (!m_raml)
    {
        return modelList;
    }

    for (auto   &resource : m_raml->getResources())
    {
        // Pick the resource based on the resource uri.
        if (std::string::npos == uri.find((resource.second)->getResourceUri()))
            continue;

        // Construct Request and Response Model from RAML::Action
        for (auto   &action :  (resource.second)->getActions())
        {
            RequestModelSP requestModel = createRequestModel(action.second);
            if (requestModel)
                modelList[requestModel->type()] = requestModel;
        }
    }

    return modelList;
}

RequestModelSP RequestModelBuilder::createRequestModel(const RAML::ActionPtr &action)
{
    OC_LOG(DEBUG, TAG, "Creating request model");

    // Validate the action type. Only GET, PUT, POST and DELETE are supported.
    RAML::ActionType actionType = action->getType();
    if (actionType != RAML::ActionType::GET
        && actionType != RAML::ActionType::PUT
        && actionType != RAML::ActionType::POST
        && actionType != RAML::ActionType::DELETE)
    {
        OC_LOG(ERROR, TAG, "Failed to create request model as it is of unknown type!");
        return nullptr;
    }

    // Construct RequestModel
    RequestModelSP requestModel(new RequestModel(getRequestType(actionType)));

    // Get the allowed query parameters of the request
    for (auto &qpEntry : action->getQueryParameters())
    {
        for (auto &value :  (qpEntry.second)->getEnumeration())
        {
            requestModel->addQueryParam(qpEntry.first, value);
        }
    }

    RAML::RequestResponseBodyPtr requestBody = action->getRequestBody("application/json");
    SimulatorResourceModelSP repSchema = createRepSchema(requestBody);
    requestModel->setRepSchema(repSchema);

    // Corresponsing responses
    for (auto   &responseEntry :  action->getResponses())
    {
        std::string codeStr = responseEntry.first;
        int code = boost::lexical_cast<int>(codeStr);
        ResponseModelSP responseModel = createResponseModel(code, responseEntry.second);
        if (nullptr != responseModel)
        {
            requestModel->addResponseModel(code, responseModel);
        }
    }

    return requestModel;
}

ResponseModelSP RequestModelBuilder::createResponseModel(int code,
        const RAML::ResponsePtr &response)
{
    ResponseModelSP responseModel(new ResponseModel(code));
    RAML::RequestResponseBodyPtr responseBody = response->getResponseBody("application/json");
    SimulatorResourceModelSP repSchema = createRepSchema(responseBody);
    responseModel->setRepSchema(repSchema);
    return responseModel;
}
template <typename T>
void RequestModelBuilder::buildValueProperty(SimulatorResourceModel::Attribute &attribute,
        const std::vector<RAML::ValuePropertyPtr> &valueProperties, T)
{
    for (auto &vp : valueProperties)
    {
        switch (vp->type())
        {
            case RAML::ValueProperty::Type::RANGE :
                {
                    double min = vp->min();
                    double max = vp->max();
                    int multipleof = vp->multipleOf();
                    if (min != INT_MIN && max != INT_MAX)
                    {
                        SimulatorResourceModel::AttributeProperty attrProp(min, max);
                        attribute.setProperty(attrProp);
                    }
                    break;
                }
            case RAML::ValueProperty::Type::VALUE_SET :
                {
                    std::vector<T> allowedValues;
                    for (auto allow : vp->valueSet())
                        allowedValues.push_back(boost::get<T>(allow));
                    SimulatorResourceModel::AttributeProperty attrProp(allowedValues);
                    attribute.setProperty(attrProp);
                    break;
                }
            default:
                break;
        }
    }

}
SimulatorResourceModel::Attribute RequestModelBuilder::buildAttribute(
    std::shared_ptr<RAML::Properties> propertyElement)
{
    std::string propName = propertyElement->getName();

    // Build representation attribute
    SimulatorResourceModel::Attribute attribute(propName);
    switch (propertyElement->getType().type())
    {
        case RAML::VariantType::INTEGER:
            {
                int attributeValue = 0;
                if (propertyElement->isDefaultValue())
                    attributeValue = boost::get<int>(propertyElement->getValue());
                attribute.setValue(attributeValue);
                int type = 0;
                buildValueProperty(attribute, (propertyElement->getValueProperties()), type);
            }
            break;

        case RAML::VariantType::DOUBLE:
            {
                double attributeValue = 0;
                if (propertyElement->isDefaultValue())
                    attributeValue = boost::get<double>(propertyElement->getValue());
                attribute.setValue(attributeValue);
                double type = 0;
                buildValueProperty(attribute, (propertyElement->getValueProperties()), type);
            }
            break;

        case RAML::VariantType::BOOLEAN:
            {
                bool attributeValue = false;
                if (propertyElement->isDefaultValue())
                    attributeValue = boost::get<bool>(propertyElement->getValue());
                attribute.setValue(attributeValue);
                bool type = false;
                buildValueProperty(attribute, (propertyElement->getValueProperties()), type);
            }
            break;

        case RAML::VariantType::STRING:
            {
                std::string attributeValue = "";
                if (propertyElement->isDefaultValue())
                    attributeValue = boost::get<std::string>(propertyElement->getValue());
                attribute.setValue(attributeValue);
                std::string type = "";
                buildValueProperty(attribute, (propertyElement->getValueProperties()), type);
            }
            break;
        case RAML::VariantType::PROPERTY:
            {
                RAML::Properties arrayProperty = boost::get<RAML::Properties>(propertyElement->getValue());
                SimulatorResourceModel::Attribute arrayAttribute = buildAttribute(
                            std::make_shared<RAML::Properties>(arrayProperty));

                switch (arrayAttribute.getType().type())
                {
                    case SimulatorResourceModel::ValueType::INTEGER :
                        {
                            std::vector<int> arrValue;
                            arrValue.push_back(boost::get<int>(arrayAttribute.getValue()));
                            attribute.setValue(arrValue);
                            int type;
                            buildValueProperty(attribute, (arrayProperty.getValueProperties()), type);
                            break;
                        }
                    case SimulatorResourceModel::ValueType::DOUBLE :
                        {
                            std::vector<double> arrValue;
                            arrValue.push_back(boost::get<double>(arrayAttribute.getValue()));
                            attribute.setValue(arrValue);
                            double type;
                            buildValueProperty(attribute, (arrayProperty.getValueProperties()), type);
                            break;
                        }
                    case SimulatorResourceModel::ValueType::BOOLEAN :
                        {
                            std::vector<bool> arrValue;
                            arrValue.push_back(boost::get<bool>(arrayAttribute.getValue()));
                            attribute.setValue(arrValue);
                            bool type;
                            buildValueProperty(attribute, (arrayProperty.getValueProperties()), type);
                            break;
                        }
                    case SimulatorResourceModel::ValueType::STRING :
                        {
                            std::vector<std::string> arrValue;
                            arrValue.push_back(boost::get<std::string>(arrayAttribute.getValue()));
                            attribute.setValue(arrValue);
                            std::string type;
                            buildValueProperty(attribute, (arrayProperty.getValueProperties()), type);
                            break;
                        }
                }
            }
            break;
        case RAML::VariantType::ARRAY:
            {

                std::vector<SimulatorResourceModel> arrayResModel;
                SimulatorResourceModel arrayItem;
                std::vector<RAML::Properties> arrayProperty = boost::get<std::vector<RAML::Properties> >
                        (propertyElement->getValue());
                for (auto val : arrayProperty)
                {
                    arrayItem.add(buildAttribute(std::make_shared<RAML::Properties>(val)));
                }
                arrayResModel.push_back(arrayItem);
                attribute.setValue(arrayResModel);
            }
            break;
    }
    return attribute;
}

SimulatorResourceModelSP RequestModelBuilder::createRepSchema(const RAML::RequestResponseBodyPtr
        &rep)
{
    if (!rep)
    {
        return nullptr;
    }

    RAML::SchemaPtr schema = rep->getSchema();
    if (!schema)
    {
        return nullptr;
    }

    RAML::JsonSchemaPtr properties = schema->getProperties();
    if (!properties || 0 == properties->getProperties().size())
        return nullptr;

    SimulatorResourceModelSP repSchema = std::make_shared<SimulatorResourceModel>();
    for (auto &propertyEntry : properties->getProperties())
    {
        std::string propName = propertyEntry.second->getName();
        if ("rt" == propName || "resourceType" == propName || "if" == propName
            || "p" == propName || "n" == propName || "id" == propName)
            continue;

        repSchema->add(buildAttribute(propertyEntry.second));
    }

    return repSchema;
}

RequestType RequestModelBuilder::getRequestType(RAML::ActionType actionType)
{
    switch (actionType)
    {
        case RAML::ActionType::PUT:
            return RequestType::RQ_TYPE_PUT;
        case RAML::ActionType::POST:
            return RequestType::RQ_TYPE_POST;
        case RAML::ActionType::DELETE:
            return RequestType::RQ_TYPE_DELETE;
    }

    return RequestType::RQ_TYPE_GET;
}

