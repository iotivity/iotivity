/* ****************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include "OCAccountManager.h"
#include "OCUtilities.h"

#include "ocstack.h"

namespace OC {

using OC::result_guard;
using OC::checked_guard;

OCAccountManager::OCAccountManager(std::weak_ptr<IClientWrapper> clientWrapper,
                                   const std::string& host,
                                   OCConnectivityType connectivityType)
 : m_clientWrapper(clientWrapper), m_host(host), m_connType(connectivityType),
   m_invitationObserveHandle(nullptr)
{
    if (m_host.empty() || m_clientWrapper.expired())
    {
        throw OCException(OC::Exception::INVALID_PARAM);
    }

    const char* di = OCGetServerInstanceIDString();
    if (!di)
    {
        oclog() << "The mode should be Server or Both to generate UUID" << std::flush;
        throw OCException(OC::Exception::INVALID_PARAM);
    }

    m_deviceID.append(di);
    m_groupObserveHandles = {};
    checked_guard(m_clientWrapper.lock(), &IClientWrapper::GetDefaultQos, m_defaultQos);
}

OCAccountManager::~OCAccountManager()
{
}

std::string OCAccountManager::host() const
{
    return m_host;
}

OCConnectivityType OCAccountManager::connectivityType() const
{
    return m_connType;
}

OCStackResult OCAccountManager::signUp(const std::string& authProvider,
                                       const std::string& authCode,
                                       PostCallback cloudConnectHandler)
{
    return result_guard(signUp(authProvider, authCode, QueryParamsMap(), cloudConnectHandler));
}

OCStackResult OCAccountManager::signUp(const std::string& authProvider,
                                       const std::string& authCode,
                                       const QueryParamsMap& options,
                                       PostCallback cloudConnectHandler)
{
    if (authProvider.empty() || authCode.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    std::string uri = m_host + OC_RSRVD_ACCOUNT_URI;

    OCRepresentation rep;
    rep.setValue(OC_RSRVD_DEVICE_ID, m_deviceID);
    rep.setValue(OC_RSRVD_AUTHPROVIDER, authProvider);
    rep.setValue(OC_RSRVD_AUTHCODE, authCode);

    if (!options.empty())
    {
        for (auto iter : options)
        {
            rep.setValue(iter.first, iter.second);
        }
    }

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::PostResourceRepresentation,
                         OCDevAddr(), uri, rep, QueryParamsMap(), HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::signIn(const std::string& userUuid,
                                       const std::string& accessToken,
                                       PostCallback cloudConnectHandler)
{
    if (userUuid.empty() || accessToken.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    return result_guard(signInOut(userUuid, accessToken, true, cloudConnectHandler));
}

OCStackResult OCAccountManager::signOut(PostCallback cloudConnectHandler)
{
    return result_guard(signInOut("", "", false, cloudConnectHandler));
}

OCStackResult OCAccountManager::signInOut(const std::string& userUuid,
                                          const std::string& accessToken,
                                          bool isSignIn,
                                          PostCallback cloudConnectHandler)
{
    std::string uri = m_host + OC_RSRVD_ACCOUNT_SESSION_URI;

    OCRepresentation rep;
    if (isSignIn)
    {
        rep.setValue(OC_RSRVD_USER_UUID, userUuid);
        rep.setValue(OC_RSRVD_DEVICE_ID, m_deviceID);
        rep.setValue(OC_RSRVD_ACCESS_TOKEN, accessToken);
    }
    rep.setValue(OC_RSRVD_LOGIN, isSignIn);

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::PostResourceRepresentation,
                         OCDevAddr(), uri, rep, QueryParamsMap(), HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::refreshAccessToken(const std::string& userUuid,
                                                   const std::string& refreshToken,
                                                   PostCallback cloudConnectHandler)
{
    if (userUuid.empty() || refreshToken.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    std::string uri = m_host + OC_RSRVD_ACCOUNT_TOKEN_REFRESH_URI;

    OCRepresentation rep;
    rep.setValue(OC_RSRVD_USER_UUID, userUuid);
    rep.setValue(OC_RSRVD_DEVICE_ID, m_deviceID);
    rep.setValue(OC_RSRVD_GRANT_TYPE, std::string(OC_RSRVD_GRANT_TYPE_REFRESH_TOKEN));
    rep.setValue(OC_RSRVD_REFRESH_TOKEN, refreshToken);

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::PostResourceRepresentation,
                         OCDevAddr(), uri, rep, QueryParamsMap(), HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::searchUser(const std::string& userUuid,
                                           GetCallback cloudConnectHandler)
{
    if (userUuid.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    return result_guard(searchUser(userUuid, QueryParamsMap(), cloudConnectHandler));
}

OCStackResult OCAccountManager::searchUser(const QueryParamsMap& queryParams,
                                           GetCallback cloudConnectHandler)
{
    if (queryParams.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    return result_guard(searchUser("", queryParams, cloudConnectHandler));
}

OCStackResult OCAccountManager::searchUser(const std::string& userUuid,
                                           const QueryParamsMap& queryParams,
                                           GetCallback cloudConnectHandler)
{
    std::string uri = m_host + OC_RSRVD_ACCOUNT_URI;

    QueryParamsMap fullQuery = {};

    if (!userUuid.empty())
    {
        fullQuery.insert(std::make_pair(OC_RSRVD_USER_UUID, userUuid));
    }

    if (!queryParams.empty())
    {
        std::string searchQuery;
        for (auto iter : queryParams)
        {
            searchQuery.append(iter.first + ":" + iter.second + ",");
        }
        searchQuery.resize(searchQuery.size() - 1);
        fullQuery.insert(std::make_pair(OC_RSRVD_SEARCH, searchQuery));
    }

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::GetResourceRepresentation,
                         OCDevAddr(), uri, fullQuery, HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::deleteDevice(const std::string& deviceId,
                                             DeleteCallback cloudConnectHandler)
{
    if (deviceId.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    std::string uri = m_host + OC_RSRVD_ACCOUNT_URI
                      + "?" + OC_RSRVD_DEVICE_ID + "=" + deviceId;

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::DeleteResource,
                         OCDevAddr(), uri, HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::createGroup(AclGroupType groupType,
                                            PostCallback cloudConnectHandler)
{
    std::string uri = m_host + OC_RSRVD_ACL_GROUP_URI;

    std::string gtype;
    switch (groupType)
    {
        case AclGroupType::PUBLIC:
            gtype = OC_RSRVD_PUBLIC;
            break;
        case AclGroupType::PRIVATE:
            gtype = OC_RSRVD_PRIVATE;
            break;
        default:
            return result_guard(OC_STACK_INVALID_PARAM);
    }
    OCRepresentation rep;
    rep.setValue(OC_RSRVD_GROUP_TYPE, gtype);

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::PostResourceRepresentation,
                         OCDevAddr(), uri, rep, QueryParamsMap(), HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::getGroupList(GetCallback cloudConnectHandler)
{
    std::string uri = m_host + OC_RSRVD_ACL_GROUP_URI;

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::GetResourceRepresentation,
                         OCDevAddr(), uri, QueryParamsMap(), HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::deleteGroup(const std::string& groupId,
                                            DeleteCallback cloudConnectHandler)
{
    if (groupId.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    std::string uri = m_host + OC_RSRVD_ACL_GROUP_URI
                      + "?" + OC_RSRVD_GROUP_ID + "=" + groupId;

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::DeleteResource,
                         OCDevAddr(), uri, HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::joinGroup(const std::string& groupId,
                                          PostCallback cloudConnectHandler)
{
    if (groupId.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    std::string uri = m_host + OC_RSRVD_ACL_GROUP_URI + "/" + groupId;

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::PostResourceRepresentation,
                         OCDevAddr(), uri, OCRepresentation(), QueryParamsMap(), HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::addDeviceToGroup(const std::string& groupId,
                                                 const std::vector<std::string>& deviceId,
                                                 PostCallback cloudConnectHandler)
{
    if (groupId.empty() || deviceId.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    std::string uri = m_host + OC_RSRVD_ACL_GROUP_URI + "/" + groupId;

    OCRepresentation rep;
    rep.setValue<std::vector<std::string>>(std::string(OC_RSRVD_DEVICE_ID_LIST), deviceId);

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::PostResourceRepresentation,
                         OCDevAddr(), uri, rep, QueryParamsMap(), HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::getGroupInfo(const std::string& groupId,
                                             GetCallback cloudConnectHandler)
{
    if (groupId.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    std::string uri = m_host + OC_RSRVD_ACL_GROUP_URI + "/" + groupId;

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::GetResourceRepresentation,
                         OCDevAddr(), uri, QueryParamsMap(), HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::leaveGroup(const std::string& groupId,
                                           DeleteCallback cloudConnectHandler)
{
    if (groupId.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    std::string uri = m_host + OC_RSRVD_ACL_GROUP_URI + "/" + groupId;

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::DeleteResource,
                         OCDevAddr(), uri, HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::deleteDeviceFromGroup(const std::string& groupId,
                                                      const std::vector<std::string>& deviceId,
                                                      DeleteCallback cloudConnectHandler)
{
    if (groupId.empty() || deviceId.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    std::string uri = m_host + OC_RSRVD_ACL_GROUP_URI + "/" + groupId;


    uri.append("?");
    for (auto iter : deviceId)
    {
        uri.append((std::string)OC_RSRVD_DEVICE_ID_LIST + "=" + iter + ";");
    }
    uri.resize(uri.size() - 1);

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::DeleteResource,
                         OCDevAddr(), uri, HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::observeGroup(const std::string& groupId,
                                             ObserveCallback cloudConnectHandler)
{
    if (groupId.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    std::string uri = m_host + OC_RSRVD_ACL_GROUP_URI + "/" + groupId;

    OCDoHandle handle = nullptr;

    OCStackResult result = checked_guard(m_clientWrapper.lock(), &IClientWrapper::ObserveResource,
                                         ObserveType::Observe, &handle, OCDevAddr(), uri,
                                         QueryParamsMap(), HeaderOptions(), cloudConnectHandler,
                                         m_defaultQos);

    if (OC_STACK_OK == result)
    {
        m_groupObserveHandles.insert(std::pair<std::string, OCDoHandle>(groupId, handle));
    }

    return result;

}

OCStackResult OCAccountManager::cancelObserveGroup(const std::string& groupId)
{
    if (groupId.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    auto found = m_groupObserveHandles.find(groupId);
    if (m_groupObserveHandles.end() == found)
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    OCDoHandle handle = found->second;

    std::string uri = m_host + OC_RSRVD_ACL_GROUP_URI + "/" + groupId;

    OCStackResult result = checked_guard(m_clientWrapper.lock(),
                                         &IClientWrapper::CancelObserveResource, handle,
                                         (const char*)"", uri, HeaderOptions(), m_defaultQos);

    if (OC_STACK_OK == result)
    {
        m_groupObserveHandles.erase(groupId);
        handle = nullptr;
    }

    return result;
}

OCStackResult OCAccountManager::observeInvitation(ObserveCallback cloudConnectHandler)
{
    std::string uri = m_host + OC_RSRVD_ACL_INVITE_URI;

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::ObserveResource,
                         ObserveType::Observe, &m_invitationObserveHandle, OCDevAddr(), uri,
                         QueryParamsMap(), HeaderOptions(), cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::cancelObserveInvitation()
{
    if (nullptr == m_invitationObserveHandle)
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    std::string uri = m_host + OC_RSRVD_ACL_INVITE_URI;

    OCStackResult result = checked_guard(m_clientWrapper.lock(),
                                         &IClientWrapper::CancelObserveResource,
                                         m_invitationObserveHandle,
                                         (const char*)"", uri, HeaderOptions(), m_defaultQos);

    if (OC_STACK_OK == result)
    {
        m_invitationObserveHandle = nullptr;
    }

    return result;
}

OCStackResult OCAccountManager::sendInvitation(const std::string& groupId,
                                               const std::string& userUuid,
                                               PostCallback cloudConnectHandler)
{
    if (groupId.empty() || userUuid.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    std::string uri = m_host + OC_RSRVD_ACL_INVITE_URI;

    OCRepresentation invitation;
    invitation.setValue(OC_RSRVD_GROUP_ID, groupId);
    invitation.setValue(OC_RSRVD_MEMBER_ID, userUuid);

    std::vector<OCRepresentation> invite{invitation};

    OCRepresentation rep;
    rep.setValue(OC_RSRVD_INVITE, invite);

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::PostResourceRepresentation,
                         OCDevAddr(), uri, rep, QueryParamsMap(), HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::cancelInvitation(const std::string& groupId,
                                                 const std::string& userUuid,
                                                 DeleteCallback cloudConnectHandler)
{
    if (groupId.empty() || userUuid.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    std::string uri = m_host + OC_RSRVD_ACL_INVITE_URI + "?" + OC_RSRVD_GROUP_ID + "=" + groupId
                      + ";" + OC_RSRVD_MEMBER_ID + "=" + userUuid;

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::DeleteResource,
                         OCDevAddr(), uri, HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

OCStackResult OCAccountManager::deleteInvitation(const std::string& groupId,
                                                 DeleteCallback cloudConnectHandler)
{
    if (groupId.empty())
    {
        return result_guard(OC_STACK_INVALID_PARAM);
    }

    std::string uri = m_host + OC_RSRVD_ACL_INVITE_URI + "?" + OC_RSRVD_GROUP_ID + "=" + groupId;

    return checked_guard(m_clientWrapper.lock(), &IClientWrapper::DeleteResource,
                         OCDevAddr(), uri, HeaderOptions(),
                         m_connType, cloudConnectHandler, m_defaultQos);
}

} // namespace OC
