//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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
//

#ifndef OC_CLIENT_CB
#define OC_CLIENT_CB

#include <ocstack.h>
#include <occoap.h>

typedef struct ClientCB {
    // callback method defined in application address space
    OCClientApplicationCBType callBack;
    // callback context data
    void * context;
    //  when a response is recvd with this token, above callback will be invoked
    OCToken * token;
    // next node in this list
    struct ClientCB    *next;
} ClientCB;

//-- AddClientCB -----------------------------------------------------------
/** @ingroup ocstack
 *
 * This method is used to add a client callback method in cbList.
 *
 * @param[in] cb
 *              address to client callback function.
 * @param[out] token
 *              address to token.
 *
 * @retval 0 for Success, otherwise some error value
 */
//------------------------------------------------------------------------
int AddClientCB(OCCallbackData *cbData, OCToken * token);

//-- DeleteClientCB -----------------------------------------------------------
/** @ingroup ocstack
 *
 * This method is used to remove a callback node from cbList.
 *
 * @param[in] cbNode
 *              address to client callback node.
 */
//------------------------------------------------------------------------
void DeleteClientCB(ClientCB *cbNode);


//-- GetClientCB -----------------------------------------------------------
/** @ingroup ocstack
 *
 * This method is used to search a cb node in cbList.
 *
 * @param[in] token
 *              token to search for.
 *
 * @retval address of the node if found, otherwise NULL
 */
//------------------------------------------------------------------------
ClientCB* GetClientCB(OCToken * token);

//-- DeleteClientCBList -----------------------------------------------------------
/** @ingroup ocstack
 *
 * This method is used to clear the cbList.
 *
 */
//------------------------------------------------------------------------
void DeleteClientCBList();

#endif //OC_CLIENT_CB
