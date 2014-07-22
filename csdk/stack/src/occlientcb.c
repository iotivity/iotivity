//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


#include "occlientcb.h"
#include "utlist.h"
#include "logger.h"
#include <string.h>

/// Module Name
#define MOD_NAME PCF("occlientcb")

static struct ClientCB *cbList = NULL;

int AddClientCB(OCCallbackData* cbData, OCToken * token) {
    ClientCB *cbNode;
    cbNode = (ClientCB*)malloc(sizeof(ClientCB));
    if (cbNode) {
        cbNode->callBack = cbData->cb;
        cbNode->context = cbData->context;
        cbNode->token = token;
        LL_APPEND(cbList, cbNode);
        return OC_STACK_OK;
    }
    return OC_STACK_NO_MEMORY;
}

void DeleteClientCB(ClientCB * cbNode) {
    LL_DELETE(cbList, cbNode);
    free(cbNode->token);
    free(cbNode);
}

ClientCB* GetClientCB(OCToken *token) {
    ClientCB* out;
    LL_FOREACH(cbList, out) {
        if((out->token->tokenLength == token->tokenLength) && (memcmp(out->token->token, token->token, token->tokenLength) == 0) ) {
            return out;
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
