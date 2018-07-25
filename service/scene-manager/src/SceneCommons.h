/******************************************************************
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

/**
 * @file
 *
 * This file contains the declaration of SceneUtils class and constant variables.
 */

#ifndef SCENE_COMMONS_H
#define SCENE_COMMONS_H

#include <string>
#include <vector>

#include "OCApi.h"
#include "RCSRemoteResourceObject.h"

/** OIC namespace */
namespace OIC
{
    /** service namespace */
    namespace Service
    {
        const std::string SCENE_LIST_DEFAULT_NAME = "list of scene Collections";   /*!< scene list default name */

        const std::string SCENE_KEY_LAST_SCENE = "lastScene";                      /*!< last selected scene */
        const std::string SCENE_KEY_SCENEVALUES = "sceneValues";                   /*!< all available scene values */
        const std::string SCENE_KEY_NAME = "n";                                    /*!< name of the resource */
        const std::string SCENE_KEY_ID = "id";                                     /*!< unique value */
        const std::string SCENE_KEY_RTS = "rts";
        const std::string SCENE_KEY_RT = "rt";
        const std::string SCENE_KEY_IF = "if";
        const std::string SCENE_KEY_PAYLOAD_LINK = "link";                         /*!< payload link */
        const std::string SCENE_KEY_SCENEMAPPINGS = "sceneMappings";               /*!< scene mappings */
        const std::string SCENE_KEY_HREF = "href";
        const std::string SCENE_KEY_SCENE = "scene";
        const std::string SCENE_KEY_MEMBERPROPERTY = "memberProperty";             /*!< property name that will be papped */
        const std::string SCENE_KEY_MEMBERVALUE = "memberValue";                   /*!< scene member property value */
        const std::string SCENE_KEY_CREATEDLINK = "createdlink";                   /*!< created link */

        const std::string SCENE_KEY_URI = "uri";
        const std::string SCENE_KEY_CHILD = "child";

        const std::string SCENE_LIST_RT = "oic.wk.sceneList";
        const std::string SCENE_MEMBER_RT = "oic.wk.sceneMember";
        const std::string SCENE_COLLECTION_RT = "oic.wk.sceneCollection";

        const std::string COAP_TAG = "coap://";                                    /*!< coap tag information */
        const std::string SCENE_LIST_URI = "/SceneListResURI";                     /*!< scene list uri */
        const std::string PREFIX_SCENE_COLLECTION_URI = "/a/sceneCollection";
        const std::string PREFIX_SCENE_MEMBER_URI = "/a/sceneMember";

        const std::string LINK_BATCH = "oic.if.lb";                                /*!< batch */
        const std::string BASELINE_IF = "oic.if.baseline";                         /*!< baseline interface */

        const OCConnectivityType SCENE_CONNECTIVITY = CT_ADAPTER_IP;
        const std::string SCENE_CLIENT_REQ_IF = BASELINE_IF;
        const std::string SCENE_CLIENT_CREATE_REQ_IF = OC::BATCH_INTERFACE;

        const int SCENE_RESPONSE_SUCCESS = 200;                                    /*!< success response */
        const int SCENE_CLIENT_BADREQUEST = 400;                                   /*!< bad request */
        const int SCENE_SERVER_INTERNALSERVERERROR = 500;                          /*!< internal server error */

        /**
         * @class   SceneUtils
         */
        class SceneUtils
        {
        public:
            /**
             * Returns UUID for Scene collection resource and members ID.
             *
             * @throw RCSException
             */
            static std::string OICGenerateUUIDStr();

            /**
             * Returns host resource's address and URI from coap address.
             *
             * @param address URI of host resource (e.g. coap://192.168.0.2:12345/a/light)
             * @param[out] host host resource's address (e.g. 192.168.0.2:12345)
             * @param[out] uri host resource's URI (e.g. /a/light)
             *
             * @throw RCSInvalidParameterException
             */
            static void getHostUriString(
                    const std::string address, std::string *host, std::string *uri);

            /**
             * Returns information of my own network address.
             *
             * This functionality use the CA interface for getting network information.
             * But It has design issue. So, It will should change to other interface.
             *
             * @throw RCSException
             */
            static std::string getNetAddress();

            /**
            * Returns RCSRemoteResourceObject pointer created with provided resource information.
            *
            * @param address URI of resource (e.g. coap://192.168.0.2:12345/a/light)
            * @param ct OCConnectivityType type of connectivity indicating the interface
            * @param vecRT a vector of resource types implemented by the resource
            * @param vecIF a vector of interfaces that the resource supports/implements
            *
            * @throw RCSException
            */
            static RCSRemoteResourceObject::Ptr createRCSResourceObject(
                const std::string &address, const OCConnectivityType ct,
                const std::vector< std::string > &vecRT, const std::vector< std::string > &vecIF);
        };
    }
}

#endif // SCENE_COMMONS_H
