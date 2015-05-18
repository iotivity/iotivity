/******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
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
******************************************************************/

#include "carainterface.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "caadapterutils.h"

/**
 * @def IP_CLIENT_TAG
 * @brief Logging tag for module name
 */
#define RA_CLIENT_TAG "RA_CLIENT"

uint32_t CARASendData(const char *remoteAddress, const void *data, uint32_t dataLength)
{
    OIC_LOG(DEBUG, RA_CLIENT_TAG, "CARASendData IN");

    VERIFY_NON_NULL_RET(remoteAddress, RA_CLIENT_TAG, "remote address is NULL", 0);
    VERIFY_NON_NULL_RET(data, RA_CLIENT_TAG, "data is NULL", 0);

    if (0 == dataLength)
    {
        OIC_LOG(ERROR, RA_CLIENT_TAG, "Data length is 0!");
        return 0;
    }

    uint32_t sendDataLength = 0;

    // TODO: call XMPP_LIB to send data

    OIC_LOG_V(INFO, RA_CLIENT_TAG, "Successfully sent bytes[%d] to addr[%s]",
            sendDataLength, remoteAddress);

    OIC_LOG(DEBUG, RA_CLIENT_TAG, "CARASendData OUT");

    return sendDataLength;
}

