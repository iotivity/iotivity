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

#ifndef _VIRTUAL_RESOURCE_H_
#define _VIRTUAL_RESOURCE_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ocstack.h"
#include "logger.h"

#include "cJSON.h"
#define TAG  PCF("MirrorResource")
//-----------------------------------------------------------------------------
// Definition of Constant
//-----------------------------------------------------------------------------
#define VR_TAG "__NM__"
#define RH_TAG "__RM__"

//-----------------------------------------------------------------------------
// Typedefs
//-----------------------------------------------------------------------------

/**
* Resource Coordinating Parameter Type
*/
typedef enum
{
    /*
     * for mirrorResourceHandle
     */
    OIC_SOURCE_ADDRESS  = 0,
    OIC_MIRROR_ADDRESS  = 1,
    OIC_REQUEST_HANDLE  = 0,
    OIC_MIRROR_HANDLE       = 1,

    /*
     * for requestHandle
     */
    OIC_REQUEST_BY_CLIENT       = 0,
    OIC_REQUEST_BY_COORDINATOR  = 1,

    OIC_NONE                        = 255
} OICResourceCoordinatorParamType;

/**
* Property of Mirror Resource
*/
typedef struct MirrorResourceProperty
{
    int countResourceType;
    int countInterface;
    char **resourceType;
    char **resourceInterfaceName;
} MirrorResourceProperty;

/**
* Mirror Resource Object
*/
typedef struct MirrorResource
{
    OCDoHandle resourceHandle[2];   // OIC_REQUEST_HANDLE = 0, OIC_VIRTUAL_HANDLE = 1
    char *address[2];               // OIC_SOURCE_ADDRESS = 0, OIC_VIRTUAL_ADDRESS = 1
    cJSON *rep;
    char *uri;
    MirrorResourceProperty prop;

    struct MirrorResource *next;

    /*
     * for multiple resource
     */
//  virtualRepresentation rep2;

} MirrorResource;

/**
* Mirror Resource List
*/
typedef struct MirrorResourceList
{
    struct MirrorResource *headerNode;
    struct MirrorResource *tailNode;
} MirrorResourceList;

/**
* Request Object
*/
typedef struct RequestHandle
{
    void *requestHandle[2];         // OIC_REQUEST_BY_CLIENT = 0, OIC_REQUEST_BY_COORDINATOR = 1
    OCResourceHandle resourceHandle;
    OCRequestHandle entityRequestHandle;

    OCMethod method;

    unsigned char isAliveCheck;

    struct RequestHandle *next;
} RequestHandle;

/**
* Request Object List
*/
typedef struct RequestHandleList
{
    struct RequestHandle *headerNode;
    struct RequestHandle *tailNode;
} RequestHandleList;

//-----------------------------------------------------------------------------
// Function prototypes for mirrorResourceHandle
//-----------------------------------------------------------------------------

/**
* Create an empty mirror resource list
*
* @return
*     pointer to empty Mirror resource list
*/
MirrorResourceList *createMirrorResourceList();

/**
* Create an empty mirror resource.
*
* @return
*     pointer to empty mirror resource
*/
MirrorResource *createMirrorResource();

/**
* Insert the mirror resource in the mirror resource list
*
* @param mirrorResourceList - pointer to the target mirror resource list that mirror resource will be inserted
* @param mirrorResource - pointer to mirror resource list to be inserted
* @return
*     OIC_HOSTING_INVALID_PARAM - if list or vResource is null
*     OIC_HOSTING_OK - successfully inserted
*/
OCStackResult insertMirrorResource(MirrorResourceList *mirrorResourceList,
                                   MirrorResource *mirrorResource);

/**
* Delete the mirror resource from the mirror resource list
*
* @param mirrorResourceList - pointer to the target mirror resource list that mirror resource will be deleted
* @param mirrorResource - pointer to mirror resource list to be deleted
* @return
*     OIC_HOSTING_INVALID_PARAM - if mirrorResourceList or mirrorResource is null
*     OIC_HOSTING_ERROR - mirror resource delete process error
*     OIC_HOSTING_OK - successfully deleted
*/
OCStackResult deleteMirrorResourceFromList(MirrorResourceList *mirrorResourceList,
        MirrorResource *mirrorResource);

/**
* Destroy mirror resource
*
* @param mirrorResource - pointer to mirror resource to be destroyed
* @return
*     OIC_HOSTING_INVALID_PARAM - if mirrorResource is null
*     OIC_HOSTING_OK - successfully destroyed
*/
OCStackResult destroyMirrorResource(MirrorResource *mirrorResource);

/**
* Destroy mirror resource list
*
* @param mirrorResourceList - pointer to mirror resource list to be destroyed
* @return
*     OIC_HOSTING_INVALID_PARAM - if mirrorResourceList is null
*     OIC_HOSTING_OK - successfully destroyed
*/
OCStackResult destroyMirrorResourceList(MirrorResourceList *mirrorResourceList);

/**
* Find mirror resource using handle
*
* @param mirrorResourceList - pointer to the target mirror resource list that mirror resource will be found
* @param handle - handle value to be found
* @param paramType - handle type to be found
*
* NOTE: every parameter(handle and type) will be compared
*
* @return
*     pointer to the found mirror resource
*     NULL - mirror resource not found
*/
MirrorResource *findMirrorResourceUsingHandle(MirrorResourceList *mirrorResourceList,
        OCResourceHandle handle, OICResourceCoordinatorParamType paramType);

/*
 * find virtual resource using address function 사용하는지 확인 필요.
 */
//virtualResource *findvResourceUsingAddress(virtualResourceList *list,
//        const char *Address, OICResourceHostingParamType type, const char *subData);

/**
* Find mirror resource using Address and URI
*
* @param mirrorResourceList - pointer to the target mirror resource list that mirror resource will be found
* @param address - pointer to address to be found
* @param paramType - address type to be found
* @param uri - pointer to uri to be found
*
* NOTE: every parameter(address, type and uri) will be compared
*
* @return
*     pointer to the found mirror resource
*     NULL - invalid input parameter or mirror resource not found
*/
MirrorResource *findMirrorResourceUsingAddressAndURI(MirrorResourceList *mirrorResourceList,
        const char *address, OICResourceCoordinatorParamType paramType, const char *uri);

/**
* Find mirror resource list using Address and Hosting Parameter Type
*
* @param mirrorResourceList - pointer to the target mirror resource list that mirror resource will be found
* @param address - pointer to address to be found
* @param paramType - address type to be found
*
* NOTE: every parameter(address, type and uri) will be compared
*
* @return
*     pointer to the found mirror resource
*     NULL - invalid input parameter or mirror resource list not found
*/
MirrorResourceList *findMirrorResourceListUsingAddress(MirrorResourceList *mirrorResourceList,
        const char *address, OICResourceCoordinatorParamType paramType);

/**
* Copy mirror resource
*
* @param sourceMirrorResource - pointer to source mirror resource
*
* @return
*     pointer to the copied mirror resource
*/
MirrorResource *cloneMirrorResource(MirrorResource *sourceMirrorResource);

/**
* Eject mirror resource from mirror resource list
*
* @param mirrorResourceList - pointer to the target mirror resource list that mirror resource will be found
* @param mirrorResource - pointer to mirror resource to be ejected
*
* @return
*     OIC_HOSTING_INVALID_PARAM - mirrorResourceList or mirrorResource is null
*     OIC_HOSTING_OK - ejected successfully
*     OIC_HOSTING_ERROR - cannot found mirror resource from mirrorResourceList same with mirrorResource
*/
OCStackResult ejectMirrorResource(MirrorResourceList *mirrorResourceList,
                                  MirrorResource *mirrorResource);

/**
* Print mirror resources from mirror resource list
*
* @param mirrorResourceList - pointer to the mirror resource list that mirror resource will be printed
*
* @return
*     OIC_HOSTING_INVALID_PARAM - mirrorResourceList is null or mirrorResourceList is empty
*     OIC_HOSTING_OK - print successfully
*/
OCStackResult printMirrorResourceList(MirrorResourceList *mirrorResourceList);

//-----------------------------------------------------------------------------
// Function prototypes for RequestHandle
//-----------------------------------------------------------------------------
RequestHandleList *createRequestHandleList();
RequestHandle *createRequestHandle();
OCStackResult insertRequestHandle(RequestHandleList *requestHandleList,
                                  RequestHandle *requestHandle);

OCStackResult deleteRequestHandleFromList(RequestHandleList *requestHandleList,
        RequestHandle *requestHandle);
OCStackResult destroyRequestHandle(RequestHandle *requestHandle);

OCStackResult destroyRequestHandleList(RequestHandleList *requestHandleList);

RequestHandle *findRequestHandle(RequestHandleList *requestHandleList,
                                 OCDoHandle handle, OICResourceCoordinatorParamType paramType);

#endif //_MIRROR_RESOURCE_H_
