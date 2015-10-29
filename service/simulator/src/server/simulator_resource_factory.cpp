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
#include "RamlParser.h"
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

std::shared_ptr<SimulatorResource> SimulatorResourceFactory::buildResource(
    std::shared_ptr<RAML::RamlResource> ramlResource)
{
    std::string name;
    std::string uri;
    std::string resourceType;
    std::string interfaceType;

    name = ramlResource->getDisplayName();
    uri = ramlResource->getResourceUri();

    // Get the resource representation schema from GET response body
    RAML::ActionPtr action = ramlResource->getAction(RAML::ActionType::GET);
    if (!action)
    {
        OC_LOG(ERROR, TAG, "Resource does not possess the GET request!");
        return nullptr;
    }

    RAML::ResponsePtr getResponse = action->getResponse("200");
    if (!getResponse)
    {
        OC_LOG(ERROR, TAG, "Resource does not provide valid GET response!");
        return nullptr;
    }

    RAML::RequestResponseBodyPtr responseBody = getResponse->getResponseBody("application/json");
    if (!responseBody)
    {
        OC_LOG(ERROR, TAG, "GET response is not of type \"application/json\" ");
        return nullptr;
    }

    // Iterate throgh all resource property and extract information needed for simulating resource
    RAML::JsonSchemaPtr resourceProperties = responseBody->getSchema()->getProperties();
    SimulatorResourceModel resModel;
    for ( auto &propertyElement : resourceProperties->getProperties())
    {
        if (!propertyElement.second)
            continue;

        std::string propName = propertyElement.second->getName();

        // Resource type
        if ("rt" == propName || "resourceType" == propName)
        {
            resourceType = propertyElement.second->getValueString();
            continue;
        }

        // Interface type
        if ("if" == propName)
        {
            interfaceType = propertyElement.second->getValueString();
            continue;
        }

        // Other Standard properties which should not be part of resource model
        if ("p" == propName || "n" == propName || "id" == propName)
        {
            continue;
        }

        // Build representation attribute
        SimulatorResourceModel::Attribute attribute(propName);
        switch (propertyElement.second->getValueType())
        {
            case 0: // Integer
                {
                    attribute.setValue(propertyElement.second->getValue<int>());

                    // Convert suppoted values
                    std::vector<int> allowedValues = propertyElement.second->getAllowedValuesInt();
                    if (allowedValues.size() > 0)
                    {
                        SimulatorResourceModel::AttributeProperty attrProp(allowedValues);
                        attribute.setProperty(attrProp);
                    }
                }
                break;

            case 1: // Double
                {
                    attribute.setValue(propertyElement.second->getValue<double>());

                    // Convert suppoted values
                    std::vector<double> allowedValues = propertyElement.second->getAllowedValuesDouble();
                    if (allowedValues.size() > 0)
                    {
                        SimulatorResourceModel::AttributeProperty attrProp(allowedValues);
                        attribute.setProperty(attrProp);
                    }
                }
                break;

            case 2: // Boolean
                {
                    attribute.setValue(propertyElement.second->getValue<bool>());

                    std::vector<bool> allowedValues = {true, false};
                    SimulatorResourceModel::AttributeProperty attrProp(allowedValues);
                    attribute.setProperty(attrProp);
                }
                break;

            case 3: // String
                {
                    attribute.setValue(propertyElement.second->getValue<std::string>());

                    // Convert suppoted values
                    std::vector<std::string> allowedValues = propertyElement.second->getAllowedValuesString();
                    if (allowedValues.size() > 0)
                    {
                        SimulatorResourceModel::AttributeProperty attrProp(allowedValues);
                        attribute.setProperty(attrProp);
                    }
                }
                break;
        }

        // Set the range property if its present
        double min, max;
        int multipleof;
        propertyElement.second->getRange(min, max, multipleof);
        if (min != INT_MIN && max != INT_MAX)
        {
            SimulatorResourceModel::AttributeProperty attrProp(min, max);
            attribute.setProperty(attrProp);
        }

        // Add the attribute to resource model
        resModel.add(attribute);
    }

    // Create simple/collection resource
    std::shared_ptr<SimulatorResource> simResource;
    if (interfaceType == "oic.if.ll"
        || resModel.containsAttribute("links") || resModel.containsAttribute("rts"))
    {
        try
        {
            std::shared_ptr<SimulatorCollectionResourceImpl> collectionRes(
                new SimulatorCollectionResourceImpl());

            collectionRes->setName(name);
            collectionRes->setResourceType(resourceType);
            if (ResourceURIFactory::getInstance()->isUnique(uri))
                collectionRes->setURI(uri);
            else
                collectionRes->setURI(ResourceURIFactory::getInstance()->constructURI(uri));

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
            if (ResourceURIFactory::getInstance()->isUnique(uri))
                singleRes->setURI(uri);
            else
                singleRes->setURI(ResourceURIFactory::getInstance()->constructURI(uri));

            singleRes->setResourceModel(resModel);
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
    std::ostringstream os;
    os << uri;
    if (!uri.empty() && '/' != uri[uri.length() - 1])
        os << '/';
    os << m_id++;
    return os.str();
}

bool ResourceURIFactory::isUnique(const std::string &uri)
{
    if (m_uriList.end() == m_uriList.find(uri))
        return true;
    else
        return false;
}

