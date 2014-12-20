//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

#include "CDContext.h"
#include "Subscription.h"
#include "Subscriptions.h"
#include "Notification.h"

#include "SubscriptionManager.h"
#include "SubscriptionRecord.h"

#include "sstream"
#include "ISubscriptionDB.h"

#include "XsList.h"
#include "SubscriptionResourceHandlerFactory.h"
#include "SubscriptionSerializationFactory.h"

#include "ctime"
#include "Utils.h"
#include "log.h"

#include <iostream>

namespace OC
{
    namespace Cm
    {
        namespace Notification
        {

            int SubscriptionManager::s_subscriptionIdCounter;

            std::string getCurrentTimeInISO8601()
            {
                time_t currentTime;
                time(&currentTime);

                std::string retValue;
                char outBuffer[50] = { 0x00, };
                strftime(outBuffer, sizeof(outBuffer), "%Y-%m-%dT%H:%M:%S",
                         gmtime(&currentTime));

                retValue.append(outBuffer);

                return outBuffer;
            }

            std::string SubscriptionManager::getNotificationLink()
            {
                std::ostringstream uriStream;

                uriStream << m_protocol << "://" << m_deviceAddress << "/notifications";

                return uriStream.str();
            }

            std::string SubscriptionManager::getSubscriptionsLink()
            {
                return "/sub";
            }

            SubscriptionManager::SubscriptionManager(ISubscriptionDB *pDb)
            {
                this->m_pNotificationListener = NULL;
                this->m_pContext = NULL;
                this->m_deviceAddress = "";
                this->m_pDatabase = pDb;
                this->m_pSubscriptionProvider = NULL;
            }

            SubscriptionManager::~SubscriptionManager()
            {
                // Close Subscription DB
            }

            bool SubscriptionManager::init(OC::Cm::IContext *pContext,
                                           std::string &dbFilePath)
            {
                this->m_pContext = pContext;
                return m_pDatabase->init(dbFilePath);
            }

            bool SubscriptionManager::deinit()
            {
                return m_pDatabase->deinit();
            }

            bool SubscriptionManager::setDeviceAddress(std::string protocol,
                    std::string address, std::string uuid)
            {
                this->m_deviceAddress = address;
                this->m_protocol = protocol;
                this->mSelfUUID = uuid;
                return true;
            }

            OC::Cm::Serialization::SerializableDataFactory *
            SubscriptionManager::getSerializationFactory()
            {
                return new SubscriptionSerializationFactory();
            }

            OC::Cm::Server::ResourceHandler::ResourceHandlerFactory *
            SubscriptionManager::getResourceHandlerFactory()
            {
                return new SubscriptionResourceHandlerFactory();
            }

            std::list<std::string> SubscriptionManager::getMySubscriptions(
                std::string uuid)
            {
                std::list<std::string> retList;

                std::list<OC::Cm::Notification::SubscriptionRecord> subscriptionList;
                if (false
                    == m_pDatabase->getMySubscriptionByDeviceId(uuid,
                            subscriptionList))
                {
                    return retList;
                }

                for (std::list<OC::Cm::Notification::SubscriptionRecord>::const_iterator itr =
                         subscriptionList.begin(); itr != subscriptionList.end(); itr++)
                {
                    OC::Cm::Notification::SubscriptionRecord pSubscription = *(itr);

                    XsList strArray;
                    strArray.setValue(pSubscription.m_subscribedResource);
                    std::list<std::string> tokens = strArray.getValueTokens();

                    for (std::list<std::string>::const_iterator tokenitr = tokens.begin();
                         tokenitr != tokens.end(); tokenitr++)
                    {
                        retList.push_back(*tokenitr);
                    }
                }

                return retList;
            }

            void SubscriptionManager::onNotifyResponseReceived(int clientSessionId,
                    int status)
            {
            }

            bool SubscriptionManager::sendRequest(std::string &uri, std::string method,
                                                  OC::Cm::Serialization::ISerializable *data, int &requestId)
            {
                std::string host;
                std::string port;
                std::string path;
                std::string hostheader;
                std::string protocol;

                if (false == parseUrl(uri, protocol, host, port, path, hostheader))
                {
                    return false;
                }

                return this->sendRequest(protocol, hostheader, path, method, data,
                                         requestId);
            }

            bool SubscriptionManager::sendRequest(std::string &protocol,
                                                  std::string &deviceAddress, std::string &relativeUri,
                                                  std::string method, OC::Cm::Serialization::ISerializable *data,
                                                  int &requestId)
            {

                CE(std::cout << "SubscriptionManager::sendRequest => " << "called!" << std::endl);

                OC::CDContext *cdContext = (OC::CDContext *) m_pContext;
                cdContext->p_server->sendClientRequest(method, deviceAddress, relativeUri,
                                                       data);
                return true;
            }

            void SubscriptionManager::addSubscriptionProvider(
                OC::Cm::Notification::ISubscriptionProvider &subscriptionProvider)
            {
                m_pSubscriptionProvider = &subscriptionProvider;
            }

            void SubscriptionManager::removeSubscriptionProvider(
                OC::Cm::Notification::ISubscriptionProvider &subscriptionProvider)
            {
                m_pSubscriptionProvider = NULL;
            }

            bool SubscriptionManager::addNotificationListener(
                OC::Cm::Notification::INotificationListener &notificationListener)
            {
                m_pNotificationListener = &notificationListener;
                return true;
            }

            void SubscriptionManager::removeNotificationListener(
                OC::Cm::Notification::INotificationListener &notigicationListener)
            {
                m_pNotificationListener = NULL;
            }

            bool SubscriptionManager::addSubscription(int &requestId, std::string protocol,
                    std::string duuid, std::string deviceAddress, std::string suuid,
                    std::list<std::string> resourcePaths, const std::string notificationUri,
                    OC::Cm::Client::Resource::IResourceResponseListener *listener,
                    std::string descript)
            {

                CE(printf("######### SubscriptionManager::addSubscription started 1  ##############\n"));

                if (protocol.empty() || duuid.empty() || deviceAddress.empty()
                    || suuid.empty() || (resourcePaths.size() == 0))
                {
                    return false;
                }

                std::string subscriptionLink = protocol + "://" + deviceAddress
                                               + getSubscriptionsLink();

                std::string notificationLink = notificationUri;
                if (notificationLink.empty())
                {
                    notificationLink = getNotificationLink();
                }

                if (notificationLink.empty())
                {
                    return false;
                }

                CE(printf("######### SubscriptionManager::addSubscription  2  ##############\n"));

                String50 description;
                description.value = descript;

                Subscription *subscriptionData = new Subscription();
                if (subscriptionData == NULL)
                {
                    return false;
                }

                subscriptionData->mpDescription = &description;
                subscriptionData->mpNotificationURI = &notificationLink;
                subscriptionData->mpUuid = new OC::Cm::Serialization::Xsd::UuidType();
                if (subscriptionData->mpUuid == NULL)
                {
                    CE(cout << "Error creating a UUID in " << __PRETTY_FUNCTION__ << endl);
                }
                subscriptionData->mpUuid->value = suuid;
                subscriptionData->mpResourceURIs = new ArrayOfAnyURI();
                if (subscriptionData->mpResourceURIs == NULL)
                {
                    CE(cout << "Error creating a Resource URIs in " << __PRETTY_FUNCTION__ << endl);
                }
                for (std::list<std::string>::iterator itr = resourcePaths.begin();
                     itr != resourcePaths.end(); itr++)
                {
                    subscriptionData->mpResourceURIs->members.push_back(*itr);
                }

                if ((!isValidUri(subscriptionLink))
                    || (!sendRequest(subscriptionLink, "POST", subscriptionData,
                                     requestId)))
                {
                    CE(printf("######### SubscriptionManager::addSubscription sendRequest is false ##############\n"));
                    if (subscriptionData)
                    {
                        subscriptionData->mpDescription = NULL;
                        subscriptionData->mpNotificationURI = NULL;
                        if (subscriptionData->mpUuid)
                        {
                            delete subscriptionData->mpUuid;
                            subscriptionData->mpUuid = NULL;
                        }
                        if (subscriptionData->mpResourceURIs)
                        {
                            delete subscriptionData->mpResourceURIs;
                            subscriptionData->mpResourceURIs = NULL;
                        }
                        delete subscriptionData;
                        subscriptionData = NULL;
                    }
                    return false;
                }

                CE(printf("######### SubscriptionManager::addSubscription  3  ##############\n"));

                if ( NULL != listener)
                {
                    // Add record response listeners
                    SubscriptionRequest reqData;
                    reqData.m_reqType = SRT_ADD_SUBSCRIPTION;
                    reqData.m_device_address = deviceAddress;
                    reqData.m_uuid = duuid;
                }

// save pending Subscription Record
                OC::Cm::Notification::SubscriptionRecord subscription;
                subscription.m_deviceId = duuid; //Change it to UUID of device
                subscription.m_noificationUri = notificationLink;
                subscription.m_subscriptionUri = "";

                XsList strArray;
                for (std::list<std::string>::iterator itr = resourcePaths.begin();
                     itr != resourcePaths.end(); itr++)
                {
                    strArray.addValueToken(*itr);
                }
                strArray.getValue(subscription.m_subscribedResource);

                m_pDatabase->savePendingMySubscription(requestId, subscription);

                subscriptionData->mpDescription = NULL;
                subscriptionData->mpNotificationURI = NULL; // To avoid freeing in destructor of Subscription
                delete subscriptionData->mpUuid;
                subscriptionData->mpUuid = NULL;
                delete subscriptionData->mpResourceURIs;
                subscriptionData->mpResourceURIs = NULL;
                delete subscriptionData;
                subscriptionData = NULL;

                CE(printf("######### SubscriptionManager::addSubscription 4 done...  ##############\n"));

                return true;
            }

            bool SubscriptionManager::removeSubscription(std::string &uuid,
                    std::string &deviceAddress, std::string subscriptionUri,
                    std::string &protocol,
                    OC::Cm::Client::Resource::IResourceResponseListener *listener,
                    int &requestId)
            {
                if (isValidUri(subscriptionUri))
                {
                    // Send DELETE Request
                    if (false == sendRequest(subscriptionUri, "DELETE", NULL, requestId))
                    {
                        return false;
                    }
                }
                else
                {
                    if (false
                        == sendRequest(protocol, deviceAddress, subscriptionUri,
                                       "DELETE",
                                       NULL, requestId))
                    {
                        return false;
                    }
                }

                if ( NULL != listener)
                {
                    // Add record response listeners
                    SubscriptionRequest reqData;
                    reqData.m_reqType = SRT_REMOVE_SUBSCRIPTION;
                    reqData.m_device_address = deviceAddress;
                    reqData.m_subscription_uri = subscriptionUri;
                    reqData.m_uuid = uuid;
                }

                return true;
            }

            bool SubscriptionManager::removeSubscription(std::string &uuid,
                    std::string &deviceAddress, std::string &protocol,
                    std::list<std::string> resourcePaths,
                    OC::Cm::Client::Resource::IResourceResponseListener *listener,
                    std::map<int, std::list<std::string> >& requestIDMap)
            {
                std::list<OC::Cm::Notification::SubscriptionRecord> subscriptionList;
                std::string subscribedResourcePath;

                XsList strArray;
                strArray.values = resourcePaths;
                strArray.getValue(subscribedResourcePath);

                if (false
                    == m_pDatabase->getMySubscriptionContainingResourceList(uuid,
                            resourcePaths, subscriptionList))
                {
                    return false;
                }

                for (std::list<OC::Cm::Notification::SubscriptionRecord>::const_iterator itr =
                         subscriptionList.begin(); itr != subscriptionList.end(); itr++)
                {
                    OC::Cm::Notification::SubscriptionRecord pSubscription = *(itr);

                    int requestId;
                    std::list<string> resourceUris;

                    if (false
                        == removeSubscription(uuid, deviceAddress,
                                              pSubscription.m_subscriptionUri, protocol, listener,
                                              requestId))
                    {
                        return false;
                    }

                    resourceUris.push_back(pSubscription.m_subscriptionUri);
                    requestIDMap[requestId] = resourceUris;
                }

                return true;
            }

            bool SubscriptionManager::notify(NotificationResponse *notificationData)
            {
                std::list<OC::Cm::Notification::SubscriptionRecord> subscriptionList;
                std::string resPath = (notificationData->getResourcePath());

                std::string resPath1 = resPath;
                std::list<std::string> resourcePathList;
                std::string delimiter = "/";

                int pos = 0;
                std::string token;
                while ((pos = resPath1.find(delimiter)) != std::string::npos)
                {
                    token = resPath1.substr(0, pos);
                    if (token != "")
                    {
                        resourcePathList.push_back(
                            resPath.substr(0, resPath.find(token) + token.length()));
                    }
                    resPath1.erase(0, pos + delimiter.length());
                }
                resourcePathList.push_back(resPath);

                if (false
                    == m_pDatabase->getSubscriptionContainingResourceList(
                        resourcePathList, subscriptionList))
                {
                    return false;
                }

                if (subscriptionList.empty())
                {

                    CE(printf("######### SubscriptionManager::notify called with empty matching subscription set...  ##############\n"));

                    return false;
                }

                NotificationEventType notificationTypeValidate;
                notificationTypeValidate.value = notificationData->getNotificationType();
                if (false == notificationTypeValidate.validateContent())
                {
                    return false;
                }

                for (std::list<OC::Cm::Notification::SubscriptionRecord>::const_iterator itr =
                         subscriptionList.begin(); itr != subscriptionList.end(); itr++)
                {
                    OC::Cm::Notification::SubscriptionRecord pSubscription = *(itr);

                    ::Notification notification;
                    int requestId;

                    //notification.Id.value                 = "Notification" ;
                    //notification.Description          = NULL ;

                    std::string subscriptionsLink = getSubscriptionsLink();
                    std::ostringstream uriStream;
                    uriStream << subscriptionsLink << "/" << pSubscription.m_rowId;
                    notification.mpSubscriptionURI = new std::string(uriStream.str());

                    Events *pEvent = new Events();
                    pEvent->mpEvent = new NotificationEventType();
                    pEvent->mpEvent->value = notificationData->getNotificationType();
                    pEvent->mpEventTime = new OC::Cm::Serialization::Xsd::DateTimeType();
                    pEvent->mpEventTime->value = getCurrentTimeInISO8601();
                    pEvent->mpResourceURI = new std::string(
                        notificationData->getResourcePath());
                    pEvent->mpUuid = new OC::Cm::Serialization::Xsd::UuidType();
                    pEvent->mpUuid->value = notificationData->getUuid();

                    if (notificationData->getNotificationData() != NULL)
                    {
                        pEvent->mpChangedResource = notificationData->getNotificationData();
                    }

                    notification.mpEvents.push_back(pEvent);
                    sendRequest(pSubscription.m_noificationUri, "POST", &notification,
                                requestId);

                    if (pEvent->mpChangedResource)
                    {
                        pEvent->mpChangedResource = NULL; // Ranjith : To Avoid freeing..... on destructor of ISerializable
                    }
                }

                return true;
            }

            bool SubscriptionManager::notify(
                std::list<NotificationResponse *> notificationDataList)
            {
                CE(std::cout << "SubscriptionManager::notify => called" << std::endl);

                std::list<std::string> resourcePathList;

                NotificationEventType notificationTypeValidate;
                for (std::list<OC::Cm::Notification::NotificationResponse *>::iterator itr =
                         notificationDataList.begin(); itr != notificationDataList.end();
                     itr++)
                {
                    notificationTypeValidate.value = (*itr)->getNotificationType();
                    if (false == notificationTypeValidate.validateContent())
                    {
                        return false;
                    }

                    resourcePathList.push_back((*itr)->getResourcePath());

                    std::string resPath = (*itr)->getResourcePath();
                    std::string resPath1 = resPath;
                    std::string delimiter = "/";

                    int pos = 0;
                    std::string token;
                    while ((pos = resPath1.find(delimiter)) != std::string::npos)
                    {
                        token = resPath1.substr(0, pos);
                        if (token != "")
                        {
                            resourcePathList.push_back(
                                resPath.substr(0,
                                               resPath.find(token) + token.length()));
                        }
                        resPath1.erase(0, pos + delimiter.length());
                    }
                }

                std::list<OC::Cm::Notification::SubscriptionRecord> subscriptionList;
                if (false
                    == m_pDatabase->getSubscriptionContainingResourceList(
                        resourcePathList, subscriptionList))
                {
                    return false;
                }

                CE(std::cout << "SubscriptionManager::notify => Subscription List size: " << subscriptionList.size() << std::endl);

                if (subscriptionList.empty())
                {
                    return false;
                }

                for (std::list<OC::Cm::Notification::SubscriptionRecord>::const_iterator itr =
                         subscriptionList.begin(); itr != subscriptionList.end(); itr++)
                {
                    OC::Cm::Notification::SubscriptionRecord pSubscription = *(itr);

                    ::Notification notification;
                    int requestId;

                    std::string subscriptionsLink = getSubscriptionsLink();
                    std::ostringstream uriStream;
                    uriStream << subscriptionsLink << "/" << pSubscription.m_rowId;
                    notification.mpSubscriptionURI = new std::string(uriStream.str());

                    for (std::list<OC::Cm::Notification::NotificationResponse *>::iterator nitr =
                             notificationDataList.begin();
                         nitr != notificationDataList.end(); nitr++)
                    {
                        bool isSubscribed = false;
                        std::string resPath = (*nitr)->getResourcePath();
                        std::string resPath1 = resPath;
                        std::string delimiter = "/";
                        std::list<std::string> resPathList;

                        int pos = 0;
                        std::string token;
                        while ((pos = resPath1.find(delimiter)) != std::string::npos)
                        {
                            token = resPath1.substr(0, pos);
                            if (token != "")
                            {
                                resPathList.push_back(
                                    resPath.substr(0,
                                                   resPath.find(token) + token.length()));
                            }
                            resPath1.erase(0, pos + delimiter.length());
                        }
                        resPathList.push_back(resPath);

                        for (std::list<std::string>::iterator strItr = resPathList.begin();
                             strItr != resPathList.end(); strItr++)
                        {
                            if ((*itr).m_subscribedResource.find((*strItr))
                                != std::string::npos)
                            {
                                isSubscribed = true;
                                break;
                            }
                        }

                        //(*itr).m_subscribedResource.find((*nitr)->getResourcePath())!=std::string::npos

                        if (isSubscribed)
                        {
                            Events *pEvent = new Events();
                            pEvent->mpEvent = new NotificationEventType();
                            pEvent->mpEvent->value = (*nitr)->getNotificationType();
                            pEvent->mpEventTime =
                                new OC::Cm::Serialization::Xsd::DateTimeType();
                            pEvent->mpEventTime->value = getCurrentTimeInISO8601();
                            pEvent->mpResourceURI = new std::string(
                                (*nitr)->getResourcePath());
                            pEvent->mpUuid = new OC::Cm::Serialization::Xsd::UuidType();
                            pEvent->mpUuid->value = (*nitr)->getUuid();

                            if ((*nitr)->getNotificationData() != NULL)
                            {
                                pEvent->mpChangedResource = (*nitr)->getNotificationData();
                            }

                            notification.mpEvents.push_back(pEvent);
                        }
                    }

                    sendRequest(pSubscription.m_noificationUri, "POST", &notification,
                                requestId);

                    for (std::list<Events *>::const_iterator eitr =
                             notification.mpEvents.begin();
                         eitr != notification.mpEvents.end(); eitr++)
                    {
                        if ((*eitr)->mpChangedResource)
                        {
                            (*eitr)->mpChangedResource = NULL; // Ranjith : To Avoid freeing..... on destructor of ISerializable
                        }
                    }
                }

                return true;
            }

            bool SubscriptionManager::notifyAll(std::string &resourcePath,
                                                std::string notificationType,
                                                OC::Cm::Serialization::ISerializable &notificationData)
            {
// TODO: Multicast notifications not implemented
                return true;
            }

            OC::Cm::Notification::ISubscriptionHandler *
            SubscriptionManager::getSubscriptionHandler()
            {
                return this;
            }

            bool SubscriptionManager::handleGetSubscriptionUri(std::string &deviceAddress,
                    std::string &subscriptionId,
                    OC::Cm::Serialization::ISerializable *response, int &statusCode)
            {
                OC::Cm::Notification::SubscriptionRecord subscription;
                Subscription *subscriptionResponse = (Subscription *) response;

                if (false
                    == m_pDatabase->getSubscriptionBySubscriptionID(subscriptionId,
                            subscription))
                {
                    statusCode = 404;
                    return false;
                }

                subscriptionResponse->mpId = new String50();
                subscriptionResponse->mpId->value = subscriptionId;
                subscriptionResponse->mpDescription = NULL;
                subscriptionResponse->mpNotificationURI = new std::string(
                    subscription.m_noificationUri);

                subscriptionResponse->mpUuid = new OC::Cm::Serialization::Xsd::UuidType();
                subscriptionResponse->mpUuid->value = subscription.m_deviceId;

                XsList strArray;
                strArray.setValue(subscription.m_subscribedResource);

//remainingResources = strArray.getValueTokens();
                subscriptionResponse->mpResourceURIs = new ArrayOfAnyURI();
                subscriptionResponse->mpResourceURIs->members = strArray.getValueTokens();

                statusCode = 200;

                return true;
            }

            bool SubscriptionManager::handleDeleteSubscriptionRequest(
                std::string &deviceAddress, std::string &subscriptionId,
                int &statusCode)
            {

                CE(printf("######### SubscriptionManager::handleDeleteSubscriptionRequest 1  ##############\n"));

                OC::Cm::Notification::SubscriptionRecord prevSubscription;
                if (false
                    == m_pDatabase->getSubscriptionBySubscriptionID(subscriptionId,
                            prevSubscription))
                {
                    statusCode = 404;
                    return false;
                }

                if (false
                    == m_pDatabase->deleteSubscriptionBySubscriptionID(
                        subscriptionId))
                {
                    statusCode = 404;
                    return false;
                }

                statusCode = 204;

                return true;
            }

            bool SubscriptionManager::handleUpdateSubscriptionRequest(
                std::string &deviceAddress, std::string &subscriptionId,
                OC::Cm::Serialization::ISerializable *request, int &statusCode)
            {
                Subscription *subscriptionReq = (Subscription *) request;
                if (subscriptionReq == NULL)
                {
                    statusCode = 400;
                    return false;
                }

                OC::Cm::Notification::SubscriptionRecord prevSubscription;
                if (false
                    == m_pDatabase->getSubscriptionBySubscriptionID(subscriptionId,
                            prevSubscription))
                {
                    statusCode = 404;
                    return false;
                }

                if ( NULL != subscriptionReq->mpNotificationURI)
                {
                    prevSubscription.m_noificationUri =
                        *(subscriptionReq->mpNotificationURI);
                }

                if ( NULL != subscriptionReq->mpResourceURIs)
                {
                    XsList strArray;
                    strArray.values = subscriptionReq->mpResourceURIs->members;
                    strArray.getValue(prevSubscription.m_subscribedResource);
                }

                if (true == m_pDatabase->updateSubscription(prevSubscription))
                {
                    statusCode = 204;
                    return true;
                }
                else
                {
                    statusCode = 500;
                    return false;
                }
            }

            bool SubscriptionManager::handleGetSubscriptionsRequest(
                OC::Cm::Serialization::ISerializable *response, int &statusCode)
            {
                Subscriptions *subscriptionsResp = (Subscriptions *) response;
                if (subscriptionsResp == NULL)
                {
                    statusCode = 400;
                    return true;
                }

                if (m_pDatabase == NULL)
                {
                    statusCode = 500;
                    return true;
                }

                std::list<OC::Cm::Notification::SubscriptionRecord> subscriptionList;
                if (false == m_pDatabase->getSubscriptions(subscriptionList))
                {
                    statusCode = 500;
                    return true;
                }

                for (std::list<OC::Cm::Notification::SubscriptionRecord>::const_iterator itr =
                         subscriptionList.begin(); itr != subscriptionList.end(); itr++)
                {
                    OC::Cm::Notification::SubscriptionRecord pSubscription = *(itr);

                    Subscription *subscriptionResponse = new Subscription();

                    if (NULL != subscriptionResponse)
                    {
                        std::ostringstream idStream;
                        idStream << pSubscription.m_rowId;

                        subscriptionResponse->mpId = new String50();
                        subscriptionResponse->mpId->value = idStream.str();

                        subscriptionResponse->mpDescription = NULL;
                        subscriptionResponse->mpNotificationURI = new std::string(
                            pSubscription.m_noificationUri);

                        XsList strArray;
                        strArray.setValue(pSubscription.m_subscribedResource);

                        subscriptionResponse->mpResourceURIs = new ArrayOfAnyURI();
                        subscriptionResponse->mpResourceURIs->members =
                            strArray.getValueTokens();

                        subscriptionResponse->mpUuid =
                            new OC::Cm::Serialization::Xsd::UuidType();
                        subscriptionResponse->mpUuid->value = pSubscription.m_deviceId;

                        subscriptionsResp->mpSubscription.push_back(subscriptionResponse);
                    }
                }

                statusCode = 200;
                return true;
            }

            bool SubscriptionManager::handleSubscriptionRequest(std::string &deviceAddress,
                    OC::Cm::Serialization::ISerializable *request, std::string &location,
                    int &statusCode)
            {

                CE(printf("######### SubscriptionManager::handleSubscriptionRequest 1  ##############\n"));

                Subscription *subscriptionReq = (Subscription *) request;
                if ((subscriptionReq == NULL)
                    || (NULL == subscriptionReq->mpNotificationURI)
                    || (NULL == subscriptionReq->mpUuid))
                {
                    statusCode = 400;
                    return false;
                }

                {
                    // Validating notification URL

                    std::string host;
                    std::string port;
                    std::string path;
                    std::string hostheader;
                    std::string protocol;

                    if (false
                        == parseUrl(*(subscriptionReq->mpNotificationURI), protocol,
                                    host, port, path, hostheader))
                    {
                        statusCode = 400;
                        return false;
                    }
                }

// Create new subscription
                OC::Cm::Notification::SubscriptionRecord subscription;
                subscription.m_deviceId = subscriptionReq->mpUuid->value;
                if (NULL != subscriptionReq->mpNotificationURI)
                {
                    subscription.m_noificationUri = *(subscriptionReq->mpNotificationURI);
                }
                subscription.m_subscriptionUri = ""; // TODO: need to remove subscriptionUri from subscription db

                XsList strArray;
                if (subscriptionReq->mpResourceURIs != NULL)
                {
                    strArray.values = subscriptionReq->mpResourceURIs->members;
                }
                strArray.getValue(subscription.m_subscribedResource);

//save subscription
                if (true == m_pDatabase->saveSubscription(subscription))
                {
                    std::string subscriptionsLink = getSubscriptionsLink();

                    std::ostringstream uriStream;
                    uriStream << subscriptionsLink << "/" << subscription.m_rowId;

                    location = uriStream.str();
                    statusCode = 201;

                    CE(printf("######### SubscriptionManager::handleSubscriptionRequest 2  ##############\n"));

                    return true;
                }
                else
                {
                    statusCode = 500;

                    CE(printf("######### SubscriptionManager::handleSubscriptionRequest 3  ##############\n"));

                    return false;
                }
            }

        }
    }
}
