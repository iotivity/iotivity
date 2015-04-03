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

#include "virtualResource.h"

RequestHandleList *createRequestHandleList()
{
    RequestHandleList *requestHandleList = (RequestHandleList*)malloc(sizeof(RequestHandleList));
    if(!requestHandleList)
    {
        OC_LOG(DEBUG, RH_TAG,"Request Handle List Creation Fail.");
    }
    else
    {
        requestHandleList->headerNode = NULL;
        requestHandleList->tailNode = NULL;
    }

    return requestHandleList;
}

RequestHandle *createRequestHandle()
{
    RequestHandle *requestHandle = (RequestHandle*)malloc(sizeof(RequestHandle));
    if(!requestHandle)
    {
        OC_LOG(DEBUG, RH_TAG,"Request Handle Creation Fail.");
    }
    else
    {
        requestHandle->requestHandle[OIC_REQUEST_BY_CLIENT] = NULL;
        requestHandle->requestHandle[OIC_REQUEST_BY_COORDINATOR] = NULL;
        requestHandle->resourceHandle = NULL;

        requestHandle->isAliveCheck = 0;

        requestHandle->next = NULL;
    }

    return requestHandle;

}
OCStackResult insertRequestHandle(RequestHandleList *requestHandleList,
        RequestHandle *requestHandle)
{
    if(requestHandleList == NULL || requestHandle == NULL)
    {
        return OC_STACK_INVALID_PARAM;
    }

    if(requestHandleList->headerNode == NULL)
    {
        requestHandleList->headerNode = requestHandle;
        requestHandleList->tailNode = requestHandle;
    }
    else
    {
        requestHandleList->tailNode->next = requestHandle;
        requestHandleList->tailNode = requestHandle;
    }

    return OC_STACK_OK;
}

OCStackResult deleteRequestHandleFromList(RequestHandleList *requestHandleList,
        RequestHandle *requestHandle)
{
    if(requestHandleList == NULL || requestHandle == NULL)
    {
        OC_LOG(DEBUG, RH_TAG,"Delete Request Handle : invalid parameter.");
        return OC_STACK_INVALID_PARAM;
    }
    if(requestHandleList->headerNode == NULL)
    {
        OC_LOG(DEBUG, RH_TAG,"Delete Request Handle : Empty Request Handle List.");
        return OC_STACK_ERROR;
    }

    if(requestHandle == requestHandleList->headerNode)
    {
        requestHandleList->headerNode = requestHandleList->headerNode->next;
        requestHandle->next = NULL;
        return destroyRequestHandle(requestHandle);
    }

    RequestHandle *preNode = requestHandleList->headerNode;
    RequestHandle *curNode = preNode->next;
    while(curNode != NULL)
    {
        if(curNode == requestHandle)
        {
            if(curNode == requestHandleList->tailNode)
            {
                requestHandleList->tailNode = preNode;
                preNode->next = NULL;
            }
            else
            {
                preNode->next = curNode->next;
            }
            requestHandle->next = NULL;
            return destroyRequestHandle(requestHandle);
        }
        preNode = curNode;
        curNode = curNode->next;
    }

    return OC_STACK_ERROR;
}

OCStackResult destroyRequestHandle(RequestHandle *requestHandle)
{
    if(requestHandle)
    {
        if(requestHandle->next)
        {
//            destroyRequestHandle(requestHandle->next);
            requestHandle->next = NULL;
        }

        requestHandle->requestHandle[OIC_REQUEST_BY_CLIENT] = NULL;
        requestHandle->requestHandle[OIC_REQUEST_BY_COORDINATOR] = NULL;

        requestHandle->isAliveCheck = 0;

        free(requestHandle);
    }
    else
    {
        return OC_STACK_INVALID_PARAM;
    }

    return OC_STACK_OK;
}

OCStackResult destroyRequestHandleList(RequestHandleList *requestHandleList){
    if(requestHandleList)
    {
        while(requestHandleList->headerNode)
        {
            deleteRequestHandleFromList(requestHandleList,requestHandleList->headerNode);
        }
        free(requestHandleList);
    }
    else{
        return OC_STACK_INVALID_PARAM;
    }

    return OC_STACK_OK;
}

RequestHandle *findRequestHandle(RequestHandleList *requestHandleList,
        OCDoHandle handle, OICResourceCoordinatorParamType paramType)
{
    if(requestHandleList == NULL || handle == NULL)
    {
        OC_LOG(DEBUG, RH_TAG,"Find Virtual Resource : invalid parameter.");
        return NULL;
    }
    if(requestHandleList->headerNode == NULL)
    {
        OC_LOG(DEBUG, RH_TAG,"Find Virtual Resource : Empty Virtual Resource List.");
        return NULL;
    }

    if(paramType == OIC_REQUEST_BY_CLIENT)
    {
        RequestHandle *tempRequestHandle = requestHandleList->headerNode;
        while(tempRequestHandle != NULL)
        {
            if((OCEntityHandlerRequest*)tempRequestHandle->requestHandle[paramType] == handle)
            {
                return tempRequestHandle;
            }
            tempRequestHandle = tempRequestHandle->next;
        }
    }
    else
    {
        RequestHandle *tempRequestHandle = requestHandleList->headerNode;
        while(tempRequestHandle != NULL)
        {
            if((OCDoHandle)tempRequestHandle->requestHandle[paramType] == handle)
            {
                return tempRequestHandle;
            }
            tempRequestHandle = tempRequestHandle->next;
        }
    }
    return NULL;
}
