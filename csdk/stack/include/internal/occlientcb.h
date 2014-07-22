//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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
