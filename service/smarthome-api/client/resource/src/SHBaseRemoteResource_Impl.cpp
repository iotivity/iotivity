/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
#include <SHBaseRemoteResource_Impl.h>
#include <CommonException.h>
#include <ClientCallbackContext.h>
#include <SHPlatform_Impl.h>
#include <SHPlatformConfig.h>
#include <PropertyBundle.h>
#include <ResourceQuery.h>
#include <PayloadConverter.h>
#include <ocstack.h>
#include <oic_string.h>
#include <cacommon.h>
#include <cautilinterface.h>
#include "logger.h"
#include <sstream>
#include <list>
#include <algorithm>
#include <cstdlib>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_WS2TCPIP_H
#include <ws2tcpip.h>
#endif
#ifdef HAVE_IN6ADDR_H
#include <in6addr.h>
#endif

#ifndef UINT16_MAX
#define UINT16_MAX (0xffff)
#endif

#define TAG "OIC_SH_CLIENT_SHBASEREMOTERESOURCE"

static const std::string COAP = "coap://";
static const std::string COAPS = "coaps://";
static const std::string COAP_TCP = "coap+tcp://";
static const std::string COAPS_TCP = "coaps+tcp://";
static const std::string COAP_GATT = "coap+gatt://";
static const std::string COAP_RFCOMM = "coap+rfcomm://";

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            SHBaseRemoteResource_Impl::SHBaseRemoteResource_Impl() : m_delegate(NULL)
            {
            }

            SHBaseRemoteResource_Impl::~SHBaseRemoteResource_Impl()
            {
            }

            bool SHBaseRemoteResource_Impl::setSHBaseRemoteResource_Impl(const std::string& uri,
                    const OCDevAddr& devAddr,
                    uint8_t property,
                    std::list<std::string>& resourceTypes,
                    std::list<std::string>& interfaces)
            {
                std::list<std::string> endpoints;

                return setSHBaseRemoteResource_Impl(uri,
                        devAddr, property,
                        resourceTypes, interfaces,
                        endpoints);
            }

            bool SHBaseRemoteResource_Impl::setSHBaseRemoteResource_Impl(const std::string& uri,
                    const OCDevAddr& devAddr,
                    uint8_t property,
                    std::list<std::string>& resourceTypes,
                    std::list<std::string>& interfaces,
                    std::list<std::string>& endpoints)
            {
                if (uri.empty())
                {
                    throw CommonException("resource uri is not proper value!");
                    return false;
                }

                m_resourceUri = uri;
                m_devAddr = devAddr;
                m_property = property;
                m_resourceTypes = resourceTypes;
                m_interfaces = interfaces;
                m_endpoints = endpoints;

                return true;
            }

            std::string SHBaseRemoteResource_Impl::getUri() const
            {
                return m_resourceUri;
            }

            std::string SHBaseRemoteResource_Impl::getHost() const
            {
                std::ostringstream ss;

                if (m_devAddr.adapter & OC_ADAPTER_TCP)
                {
                    if (m_devAddr.flags & OC_SECURE)
                    {
                        ss << COAPS_TCP;
                    }
                    else
                    {
                        ss << COAP_TCP;
                    }
                }
                else if (m_devAddr.adapter & OC_ADAPTER_GATT_BTLE)
                {
                    ss << COAP_GATT;
                }
                else if (m_devAddr.adapter & OC_ADAPTER_RFCOMM_BTEDR)
                {
                    ss << COAP_RFCOMM;
                }
                else
                {
                    if (m_devAddr.flags & OC_SECURE)
                    {
                        ss << COAPS;
                    }
                    else
                    {
                        ss << COAP;
                    }
                }

                if (m_devAddr.flags & OC_IP_USE_V6)
                {
                    char addressEncoded[128] = {0};

                    OCStackResult result = OCEncodeAddressForRFC6874(addressEncoded,
                            sizeof(addressEncoded),
                            m_devAddr.addr);
                    if (OC_STACK_OK != result)
                    {
                        throw CommonException("OCEncodeAddressForRFC6874 failed!");
                    }
                    ss << '[' << addressEncoded << ']';
                }
                else
                {
                    ss << m_devAddr.addr;
                }
                if (m_devAddr.port)
                {
                    ss << ':' << m_devAddr.port;
                }
                return ss.str();
            }

            std::list<std::string> SHBaseRemoteResource_Impl::getHosts() const
            {
                return m_endpoints;
            }

            std::string SHBaseRemoteResource_Impl::setHost(const std::string &host)
            {
                size_t prefix_len;

                OCDevAddr new_devAddr;
                memset(&new_devAddr, 0, sizeof(new_devAddr));
                new_devAddr.adapter = OC_DEFAULT_ADAPTER;
                new_devAddr.flags = OC_DEFAULT_FLAGS;

                // init m_devAddr
                m_devAddr = new_devAddr;
                bool usingIpAddr = false;

                if (host.compare(0, sizeof(COAPS) - 1, COAPS) == 0)
                {
                    if (!OC_SECURE)
                    {
                        throw CommonException("invalid parameter");
                    }
                    prefix_len = sizeof(COAPS) - 1;
                    m_devAddr.flags = static_cast<OCTransportFlags>(m_devAddr.flags | OC_SECURE);
                    m_devAddr.adapter = OC_ADAPTER_IP;
                    usingIpAddr = true;
                }
                else if (host.compare(0, sizeof(COAP) - 1, COAP) == 0)
                {
                    prefix_len = sizeof(COAP) - 1;
                    m_devAddr.adapter = OC_ADAPTER_IP;
                    usingIpAddr = true;
                }
                else if (host.compare(0, sizeof(COAP_TCP) - 1, COAP_TCP) == 0)
                {
                    prefix_len = sizeof(COAP_TCP) - 1;
                    m_devAddr.adapter = OC_ADAPTER_TCP;
                    usingIpAddr = true;
                }
                else if (host.compare(0, sizeof(COAPS_TCP) - 1, COAPS_TCP) == 0)
                {
                    if (!OC_SECURE)
                    {
                        throw CommonException("invalid parameter");
                    }
                    prefix_len = sizeof(COAPS_TCP) - 1;
                    m_devAddr.flags = static_cast<OCTransportFlags>(m_devAddr.flags | OC_SECURE);
                    m_devAddr.adapter = OC_ADAPTER_TCP;
                    usingIpAddr = true;
                }
                else if (host.compare(0, sizeof(COAP_GATT) - 1, COAP_GATT) == 0)
                {
                    prefix_len = sizeof(COAP_GATT) - 1;
                    m_devAddr.adapter = OC_ADAPTER_GATT_BTLE;
                }
                else if (host.compare(0, sizeof(COAP_RFCOMM) - 1, COAP_RFCOMM) == 0)
                {
                    prefix_len = sizeof(COAP_RFCOMM) - 1;
                    m_devAddr.adapter = OC_ADAPTER_RFCOMM_BTEDR;
                }
                else
                {
                    throw CommonException("invalid parameter");
                }

                // set flag
                if (usingIpAddr)
                {
                    if (host.find('[') != std::string::npos)
                    {
                        // ipv6
                        m_devAddr.flags =
                            static_cast<OCTransportFlags>(m_devAddr.flags | OC_IP_USE_V6);
                    }
                    else
                    {
                        // ipv4
                        m_devAddr.flags =
                            static_cast<OCTransportFlags>(m_devAddr.flags | OC_IP_USE_V4);
                    }
                }

                // remove
                //'coap://' or 'coaps://' or 'coap+tcp://' or 'coap+gatt://' or 'coap+rfcomm://'
                std::string host_token = host.substr(prefix_len);

                if (host_token[0] == '[') // IPv6
                {
                    size_t bracket = host_token.find(']');

                    if (std::string::npos == bracket || 0 == bracket)
                    {
                        throw CommonException("invalid parameter");
                    }
                    // extract the ipv6 address
                    std::string ip6Addr = host_token.substr(1, bracket - 1);

                    // address validity check
                    std::string ip6AddrToValidityCheck(ip6Addr);
                    size_t percent = ip6AddrToValidityCheck.find('%');
                    if (std::string::npos != percent)
                    {
                        ip6AddrToValidityCheck.resize(percent);
                    }
                    struct in6_addr buf;
                    const char *cAddr = ip6AddrToValidityCheck.c_str();
                    if (0 == inet_pton(AF_INET6, cAddr, &buf))
                    {
                        throw CommonException("invalid parameter");
                    }

                    //skip ']' and ':' characters in host string
                    host_token = host_token.substr(bracket + 2);
                    int port = atoi(host_token.c_str());

                    if (0 > port || UINT16_MAX < port)
                    {
                        throw CommonException("invalid parameter");
                    }

                    if (std::string::npos != ip6Addr.find("%25"))
                    {
                        OCStackResult result = OCDecodeAddressForRFC6874(m_devAddr.addr,
                                sizeof(m_devAddr.addr), ip6Addr.c_str(), NULL);

                        if (OC_STACK_OK != result)
                        {
                            throw CommonException("invalid parameter");
                        }
                    }
                    else
                    {
                        // It means zone-id is missing, check ipv6Addr is link local
                        CATransportFlags_t scopeLevel;
                        CAResult_t caResult = CAGetIpv6AddrScope(ip6Addr.c_str(), &scopeLevel);

                        if (CA_STATUS_OK != caResult)
                        {
                            throw CommonException("invalid parameter");
                        }
                        else
                        {
                            if (CA_SCOPE_LINK == scopeLevel)
                            {
                                // Given ip address is link-local scope without zone-id.
                                throw CommonException(
                                        "Given ip address is link-local scope without zone-id.");
                            }
                            else
                            {
                                if (!OICStrcpy(m_devAddr.addr, sizeof(m_devAddr.addr),
                                            ip6Addr.c_str()))
                                {
                                    throw CommonException("OICStrcpy error");
                                }
                            }
                        }
                    }

                    m_devAddr.port = static_cast<uint16_t>(port);
                    m_devAddr.flags =
                        static_cast<OCTransportFlags>(m_devAddr.flags | OC_IP_USE_V6);
                }
                else if (host_token[0] == ':')
                {
                    throw CommonException("invalid parameter");
                }
                else
                {
                    size_t dot = host_token.find('.');
                    if (std::string::npos == dot) // MAC
                    {
                        std::string macAddr = host_token;

                        // address validity check
                        if (MAC_ADDR_STR_SIZE != macAddr.length())
                        {
                            throw CommonException("invalid parameter");
                        }

                        for (size_t blockCnt = 0; blockCnt < MAC_ADDR_BLOCKS; blockCnt++)
                        {
                            std::string block = macAddr.substr(blockCnt * 3, 2);

                            if (std::string::npos !=
                                    block.find_first_not_of("0123456789ABCDEFabcdef"))
                            {
                                throw CommonException("invalid parameter");
                            }

                            if (MAC_ADDR_BLOCKS - 1 > blockCnt)
                            {
                                char delimiter = macAddr[blockCnt * 3 + 2];

                                if (':' != delimiter)
                                {
                                    throw CommonException("invalid parameter");
                                }
                            }
                        }

                        macAddr.copy(m_devAddr.addr, sizeof(m_devAddr.addr));
                        m_devAddr.addr[MAC_ADDR_STR_SIZE] = '\0';
                    }
                    else // IPv4
                    {
                        size_t colon = host_token.find(':');

                        if (colon == std::string::npos || colon == 0)
                        {
                            throw CommonException("invalid parameter");
                        }

                        // extract the ipv4 address
                        std::string ip4Addr = host_token.substr(0, colon);

                        // address validity check
                        struct in_addr buf;
                        const char *cAddr = ip4Addr.c_str();
                        if (0 == inet_pton(AF_INET, cAddr, &buf))
                        {
                            throw CommonException("invalid parameter");
                        }

                        //skip ':' characters in host string
                        host_token = host_token.substr(colon + 1);
                        int port = atoi(host_token.c_str());

                        if (0 > port || UINT16_MAX < port)
                        {
                            throw CommonException("invalid parameter");
                        }

                        ip4Addr.copy(m_devAddr.addr, sizeof(m_devAddr.addr));
                        m_devAddr.addr[ip4Addr.length()] = '\0';
                        m_devAddr.port = static_cast<uint16_t>(port);
                    }
                }
                return this->getHost();

            }

            std::list<std::string> SHBaseRemoteResource_Impl::getResourceTypes() const
            {
                return m_resourceTypes;
            }

            std::list<std::string> SHBaseRemoteResource_Impl::getInterfaces() const
            {
                return m_interfaces;
            }

            bool SHBaseRemoteResource_Impl::hasResourceType(const std::string resourceType) const
            {
                std::list<std::string>::const_iterator iter =
                    std::find(m_resourceTypes.begin(), m_resourceTypes.end(), resourceType);

                return (iter != m_resourceTypes.end()) ? true : false;
            }

            void SHBaseRemoteResource_Impl::setDelegate(SHBaseRemoteResourceDelegate *delegate)
            {
                this->m_delegate = delegate;
            }

            SHRequestHandle SHBaseRemoteResource_Impl::getPropertyBundle()
            {
                OIC_LOG(DEBUG, TAG, "getProperty bundle.");
                ResourceQuery query;
                return getPropertyBundle(query);
            }

            SHRequestHandle SHBaseRemoteResource_Impl::getPropertyBundle(const ResourceQuery &query)
            {
                OIC_LOG(DEBUG, TAG, "getProperty bundle with query.");

                if (!m_delegate)
                {
                    throw CommonException("delegate is not set");
                }

                ClientCallbackContext *ctx = new ClientCallbackContext(this->m_delegate);

                OCCallbackData cbdata;
                cbdata.context = static_cast<void*>(ctx);
                cbdata.cb = ClientCallbackWrapper::RemoteResourceCallback::onGet;
                cbdata.cd = ClientCallbackWrapper::CallbackHelper::CallbackContextDeleter;

                //make query included uri
                std::string uri = this->m_resourceUri;
                if (0 < query.size())
                {
                    uri += "?" + query.getQueryAsString();
                }

                SHPlatformConfig cfg = SH_Impl::getConfig();

                //TODO
                //csdk lock?

                OCDoHandle handle;

                OCStackResult result = OCDoRequest(&handle, OC_REST_GET,
                        uri.c_str(),
                        &m_devAddr, NULL,
                        CT_DEFAULT,
                        static_cast<OCQualityOfService>(cfg.qos),
                        &cbdata,
                        NULL, 0);    //TODO

                if (OC_STACK_OK != result)
                {
                    return NULL;
                }

                return static_cast<SHRequestHandle>(handle);
            }

            SHRequestHandle SHBaseRemoteResource_Impl::setPropertyBundle(
                    const PropertyBundle &bundle)
            {
                OIC_LOG(DEBUG, TAG, "setProperty bundle");
                ResourceQuery query;
                return setPropertyBundle(bundle, query);
            }

            SHRequestHandle SHBaseRemoteResource_Impl::setPropertyBundle(
                    const PropertyBundle &bundle,
                    const ResourceQuery &query)
            {
                OIC_LOG(DEBUG, TAG, "setProperty bundle with query");

                if (!m_delegate)
                {
                    throw CommonException("delegate is not set");
                }

                ClientCallbackContext *ctx = new ClientCallbackContext(this->m_delegate);

                OCCallbackData cbdata;
                cbdata.context = static_cast<void*>(ctx);
                cbdata.cb = ClientCallbackWrapper::RemoteResourceCallback::onSet;
                cbdata.cd = ClientCallbackWrapper::CallbackHelper::CallbackContextDeleter;

                //make query included uri
                std::string uri = this->m_resourceUri;
                if (0 < query.size())
                {
                    uri += "?" + query.getQueryAsString();
                }

                SHPlatformConfig cfg = SH_Impl::getConfig();

                OCPayload *payload = Converter::convertBundleToPayload(bundle);
                if (!payload)
                {
                    throw CommonException("convertBundleToPayload Fail!");
                    return NULL;
                }

                //TODO
                //csdk lock?

                OCDoHandle handle;

                OCStackResult result = OCDoRequest(&handle, OC_REST_POST,
                        uri.c_str(),
                        &m_devAddr,
                        payload,
                        CT_DEFAULT,
                        static_cast<OCQualityOfService>(cfg.qos),
                        &cbdata,
                        NULL, 0);    //TODO

                if (OC_STACK_OK != result)
                {
                    return NULL;
                }

                return static_cast<SHRequestHandle>(handle);
            }

            bool SHBaseRemoteResource_Impl::startObserve()
            {
                ResourceQuery query;
                return startObserve(query);
            }

            bool SHBaseRemoteResource_Impl::startObserve(const ResourceQuery &query)
            {
                if (!m_delegate)
                {
                    throw CommonException("delegate is not set");
                }

                ClientCallbackContext *ctx = new ClientCallbackContext(this->m_delegate);

                OCCallbackData cbdata;
                cbdata.context = static_cast<void*>(ctx);
                cbdata.cb = ClientCallbackWrapper::RemoteResourceCallback::onObserve;
                cbdata.cd = ClientCallbackWrapper::CallbackHelper::CallbackContextDeleter;

                //make query included uri
                std::string uri = this->m_resourceUri;
                if (0 < query.size())
                {
                    uri += "?" + query.getQueryAsString();
                }

                SHPlatformConfig cfg = SH_Impl::getConfig();

                OCDoHandle handle;

                OCStackResult result = OCDoRequest(&handle, OC_REST_OBSERVE,
                        uri.c_str(),
                        &m_devAddr,
                        NULL,
                        CT_DEFAULT,
                        static_cast<OCQualityOfService>(cfg.qos),
                        &cbdata,
                        NULL, 0);    //TODO

                if (OC_STACK_OK != result || !handle)
                {
                    return false;
                }

                m_observeHandle = static_cast<SHRequestHandle>(handle);

                return true;
            }

            bool SHBaseRemoteResource_Impl::stopObserve()
            {
                if (!m_delegate)
                {
                    throw CommonException("delegate is not set");
                }

                if (!m_observeHandle)
                {
                    throw CommonException("observe is not started!");
                }

                SHPlatformConfig cfg = SH_Impl::getConfig();

                OCStackResult result = OCCancel(static_cast<OCDoHandle>(m_observeHandle),
                        static_cast<OCQualityOfService>(cfg.qos),
                        NULL, 0);   //TODO

                return OC_STACK_OK == result ? true : false;
            }
        }
    }
}
