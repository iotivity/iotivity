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


#include "occlientcb.h"
#include "occoap.h"
#include "utlist.h"
#include "logger.h"
#include <string.h>

/// Module Name
#define MOD_NAME PCF("occlientcb")

static struct ClientCB *cbList = NULL;

OCStackResult AddClientCB(ClientCB** clientCB, OCCallbackData* cbData, OCCoAPToken * token, OCDoHandle handle, OCMethod method) {
    ClientCB *cbNode;
    cbNode = (ClientCB*) OCMalloc(sizeof(ClientCB));
    if (cbNode) {
        cbNode->callBack = cbData->cb;
        cbNode->context = cbData->context;
        cbNode->token = token;
        cbNode->handle = handle;
        cbNode->method = method;
        cbNode->sequenceNumber = 0;
        LL_APPEND(cbList, cbNode);
        *clientCB = cbNode;
        return OC_STACK_OK;
    }
    *clientCB = NULL;
    return OC_STACK_NO_MEMORY;
}

void DeleteClientCB(ClientCB * cbNode) {
    if(cbNode) {
        LL_DELETE(cbList, cbNode);
        OCFree(cbNode->token);
        OCFree(cbNode->handle);
        OCFree(cbNode);
        cbNode = NULL;
    }
}

ClientCB* GetClientCB(OCCoAPToken * token, OCDoHandle * handle) {
    ClientCB* out = NULL;
    if(token) {
        LL_FOREACH(cbList, out) {
            if((out->token->tokenLength == token->tokenLength) && (memcmp(out->token->token, token->token, token->tokenLength) == 0) ) {
                return out;
            }
        }
    }
    else if(handle) {
        LL_FOREACH(cbList, out) {
            if(memcmp(out->handle, handle, sizeof(OCDoHandle)) == 0) {
                return out;
            }
        }
    }
    OC_LOG(INFO, MOD_NAME, PCF("Callback Not found !!"));
    return NULL;
}


void DeleteClientCBList() {
    ClientCB* out;
    ClientCB* tmp;
    LL_FOREACH_SAFE(cbList, out, tmp) {
        DeleteClientCB(out);
    }
    cbList = NULL;
}
