//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <PrimitiveResource.h>
#include <ResourceAttributes.h>
#include <ResponseStatement.h>

#include <internal/ResourceAtrributesConverter.h>

#include <OCPlatform.h>

using OC::QueryParamsMap;
using OC::OCRepresentation;
using OC::ObserveType;

using std::bind;
using std::shared_ptr;
using std::vector;
using std::string;

using namespace std::placeholders;

namespace
{

    ResponseStatement createResponseStatement(const OCRepresentation& ocRepresentation)
    {
        return ResponseStatement::create(
                ResourceAttributesConverter::fromOCRepresentation(ocRepresentation));
    }

} // unnamed namespace

PrimitiveResource::PrimitiveResource(const BaseResourcePtr& ocResource) :
        m_ocResource{ ocResource }
{
}

PrimitiveResource::Ptr PrimitiveResource::create(const BaseResourcePtr& ptr)
{
    return std::shared_ptr< PrimitiveResource >(new PrimitiveResource{ ptr });
}

void PrimitiveResource::requestGet(GetCallback callback)
{
    m_ocResource->get(QueryParamsMap(), bind(callback, _1, bind(createResponseStatement, _2), _3));
}

void PrimitiveResource::requestSet(const ResourceAttributes& attrs, SetCallback callback)
{
    m_ocResource->put(ResourceAttributesConverter::toOCRepresentation(attrs), QueryParamsMap{},
            bind(callback, _1, bind(createResponseStatement, _2), _3));
}

void PrimitiveResource::requestObserve(ObserveCallback callback)
{
    m_ocResource->observe(ObserveType::ObserveAll, QueryParamsMap{},
            bind(callback, _1, bind(createResponseStatement, _2), _3, _4));
}

void PrimitiveResource::cancelObserve()
{
    m_ocResource->cancelObserve();
}

bool PrimitiveResource::isObservable() const
{
    return m_ocResource->isObservable();
}

string PrimitiveResource::getUri() const
{
    return m_ocResource->uri();
}

string PrimitiveResource::getHost() const
{
    return m_ocResource->host();
}

vector< string > PrimitiveResource::getTypes() const
{
    return m_ocResource->getResourceTypes();
}

vector< string > PrimitiveResource::getInterfaces() const
{
    return m_ocResource->getResourceInterfaces();
}



void discoverResource(const std::string& host, const std::string& resourceURI,
        OCConnectivityType connectivityType, FindCallback resourceHandler)
{
    OC::OCPlatform::findResource(host, resourceURI, connectivityType,
            std::bind(&PrimitiveResource::create, std::placeholders::_1));
}
