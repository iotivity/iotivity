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

#include "simulator_resource_factory.h"
#include "simulator_single_resource_impl.h"
#include "simulator_collection_resource_impl.h"
#include "simulator_logger.h"
#include "logger.h"

#define TAG "SIM_RESOURCE_FACTORY"

SimulatorResourceFactory *SimulatorResourceFactory::getInstance()
{
    static SimulatorResourceFactory s_instance;
    return &s_instance;
}

std::shared_ptr<SimulatorResource> SimulatorResourceFactory::createResource(
    const std::string &configPath)
{
    // Parse the RAML file
    std::shared_ptr<RAML::RamlParser> ramlParser = std::make_shared<RAML::RamlParser>(configPath);
    RAML::RamlPtr raml = ramlParser->getRamlPtr();

    // Get the first resource model from RAML
    RAML::RamlResourcePtr ramlResource;
    if (0 == raml->getResources().size()
        || nullptr == (ramlResource = raml->getResources().begin()->second))
    {
        OC_LOG(ERROR, TAG, "Zero resources detected from RAML!");
        return nullptr;
    }

    return buildResource(ramlResource);
}

std::vector<std::shared_ptr<SimulatorResource> > SimulatorResourceFactory::createResource(
    const std::string &configPath, unsigned int count)
{
    std::vector<std::shared_ptr<SimulatorResource>> resources;

    // Parse the RAML file
    std::shared_ptr<RAML::RamlParser> ramlParser = std::make_shared<RAML::RamlParser>(configPath);
    RAML::RamlPtr raml = ramlParser->getRamlPtr();

    // Get the first resource model from RAML
    RAML::RamlResourcePtr ramlResource;
    if (0 == raml->getResources().size()
        || nullptr == (ramlResource = raml->getResources().begin()->second))
    {
        OC_LOG(ERROR, TAG, "Zero resources detected from RAML!");
        return resources;
    }

    while (count--)
    {
        std::shared_ptr<SimulatorResource> resource = buildResource(ramlResource);
        if (!resource)
        {
            OC_LOG(ERROR, TAG, "Failed to create resource!");
            return resources;
        }

        resources.push_back(resource);
    }

    return resources;
}

std::shared_ptr<SimulatorSingleResource> SimulatorResourceFactory::createSingleResource(
    const std::string &name, const std::string &uri, const std::string &resourceType)
{
    SimulatorSingleResourceImpl *simpleResource = new SimulatorSingleResourceImpl();
    simpleResource->setName(name);
    simpleResource->setURI(uri);
    simpleResource->setResourceType(resourceType);
    return std::shared_ptr<SimulatorSingleResource>(simpleResource);
}

std::shared_ptr<SimulatorCollectionResource> SimulatorResourceFactory::createCollectionResource(
    const std::string &name, const std::string &uri, const std::string &resourceType)
{
    SimulatorCollectionResourceImpl *collectionResource = new SimulatorCollectionResourceImpl();
    collectionResource->setName(name);
    collectionResource->setURI(uri);
    collectionResource->setResourceType(resourceType);
    return std::shared_ptr<SimulatorCollectionResource>(collectionResource);
}
template <typename T>
void SimulatorResourceFactory::buildValueProperty(SimulatorResourceModel::Attribute &attribute,
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
SimulatorResourceModel::Attribute SimulatorResourceFactory::buildAttribute(
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

RAML::RequestResponseBodyPtr SimulatorResourceFactory::getRAMLResponseBody(
    std::shared_ptr<RAML::RamlResource> ramlResource, RAML::ActionType type, std::string responseCode)
{
    // Get the resource representation schema from response body
    RAML::ActionPtr action = ramlResource->getAction(type);
    if (!action)
    {
        OC_LOG(ERROR, TAG, "Resource does not possess the request!");
        return nullptr;
    }

    RAML::ResponsePtr response = action->getResponse(responseCode);
    if (!response)
    {
        OC_LOG(ERROR, TAG, "Resource does not provide valid GET response!");
        return nullptr;
    }

    RAML::RequestResponseBodyPtr responseBody = response->getResponseBody("application/json");
    if (!responseBody)
    {
        OC_LOG(ERROR, TAG, "GET response is not of type \"application/json\" ");
        return nullptr;
    }

    return responseBody;
}

SimulatorResourceModel SimulatorResourceFactory::buildModelFromResponseBody(
    RAML::RequestResponseBodyPtr responseBody, std::string &resourceType,
    std::vector<std::string> &interfaceType)
{
    SimulatorResourceModel resModel;

    if (!responseBody)
        return resModel;

    // Iterate throgh all resource property and extract information needed for simulating resource.
    RAML::JsonSchemaPtr resourceProperties = responseBody->getSchema()->getProperties();


    for ( auto &propertyElement : resourceProperties->getProperties())
    {
        if (!propertyElement.second)
            continue;

        std::string propName = propertyElement.second->getName();

        // Resource type
        if ("rt" == propName || "resourceType" == propName)
        {
            resourceType = boost::get<std::string>(propertyElement.second->getValue());
            continue;
        }

        // TODO: Is "if" required to be part of resource representation?
        // Interface type
        if ("if" == propName)
        {
            if (RAML::VariantType::STRING == propertyElement.second->getType().type())
            {
                interfaceType.push_back(boost::get<std::string>(propertyElement.second->getValue()));
            }
            else if (RAML::VariantType::ARRAY == propertyElement.second->getType().type())
            {
                RAML::Properties ifProperty = boost::get<RAML::Properties>(propertyElement.second->getValue());
                for (auto vp : ifProperty.getValueProperties())
                {
                    if (vp->type() == RAML::ValueProperty::Type::VALUE_SET)
                    {
                        for (auto allow : vp->valueSet())
                            interfaceType.push_back(boost::get<std::string>(allow));
                    }
                }
            }
            continue;
        }

        // Other Standard properties which should not be part of resource model
        if ("p" == propName || "n" == propName || "id" == propName)
        {
            continue;
        }

        resModel.add(buildAttribute(propertyElement.second));
    }

    return resModel;
}

std::shared_ptr<SimulatorResource> SimulatorResourceFactory::buildResource(
    std::shared_ptr<RAML::RamlResource> ramlResource)
{
    std::string name;
    std::string uri;
    std::string resourceType, rt;
    std::vector<std::string> interfaceType, ifType;

    name = ramlResource->getDisplayName();
    uri = ramlResource->getResourceUri();
    std::map<RAML::ActionType, RAML::ActionPtr> actionType = ramlResource->getActions();

    RAML::RequestResponseBodyPtr successResponseBody = getRAMLResponseBody(
                ramlResource, RAML::ActionType::GET, "200");
    RAML::RequestResponseBodyPtr putErrorResponseBody = getRAMLResponseBody(
                ramlResource, RAML::ActionType::PUT, "403");
    RAML::RequestResponseBodyPtr postErrorResponseBody = getRAMLResponseBody(
                ramlResource, RAML::ActionType::POST, "403");

    SimulatorResourceModel successResponseModel = buildModelFromResponseBody(
                successResponseBody, resourceType, interfaceType);
    SimulatorResourceModel putErrorResponseModel = buildModelFromResponseBody(
                putErrorResponseBody, rt, ifType);
    SimulatorResourceModel postErrorResponseModel = buildModelFromResponseBody(
                postErrorResponseBody, rt, ifType);

    // Create simple/collection resource
    std::shared_ptr<SimulatorResource> simResource;
    if (successResponseModel.containsAttribute("links"))
    {
        try
        {
            std::shared_ptr<SimulatorCollectionResourceImpl> collectionRes(
                new SimulatorCollectionResourceImpl());

            collectionRes->setName(name);
            collectionRes->setResourceType(resourceType);
            collectionRes->setInterface(interfaceType);
            collectionRes->setURI(ResourceURIFactory::getInstance()->constructURI(uri));
            collectionRes->setActionType(actionType);

            collectionRes->setResourceModel(successResponseModel);
            simResource = std::dynamic_pointer_cast<SimulatorResource>(collectionRes);
        }
        catch (InvalidArgsException &e) {}
    }
    else
    {
        try
        {
            std::shared_ptr<SimulatorSingleResourceImpl> singleRes(
                new SimulatorSingleResourceImpl());

            singleRes->setName(name);
            singleRes->setResourceType(resourceType);
            singleRes->setInterface(interfaceType);
            singleRes->setURI(ResourceURIFactory::getInstance()->constructURI(uri));
            singleRes->setActionType(actionType);

            singleRes->setResourceModel(successResponseModel);
            singleRes->setPutErrorResponseModel(putErrorResponseModel);
            singleRes->setPostErrorResponseModel(postErrorResponseModel);

            simResource = std::dynamic_pointer_cast<SimulatorResource>(singleRes);
        }
        catch (InvalidArgsException &e) {}
    }

    return simResource;
}

ResourceURIFactory *ResourceURIFactory::getInstance()
{
    static ResourceURIFactory s_instance;
    return &s_instance;
}

ResourceURIFactory::ResourceURIFactory()
    : m_id(0) {}

std::string ResourceURIFactory::constructURI(const std::string &uri)
{
    std::lock_guard<std::mutex> lock(m_lock);
    if (isUnique(uri))
    {
        updateUri(uri);
        return uri;
    }
    std::ostringstream os;
    os << uri;
    if (!uri.empty() && '/' != uri[uri.length() - 1])
        os << '/';
    os << m_id++;
    updateUri(os.str());
    return os.str();
}

void ResourceURIFactory::updateUri(const std::string &uri)
{
    m_uriList.insert(std::pair<std::string, bool>(uri, true));
}

bool ResourceURIFactory::isUnique(const std::string &uri)
{
    if (m_uriList.end() == m_uriList.find(uri))
        return true;
    else
        return false;
}

