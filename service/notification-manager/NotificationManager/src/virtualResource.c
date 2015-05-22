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


MirrorResourceList *createMirrorResourceList()
{
    MirrorResourceList *mirrorResourceList = (MirrorResourceList *)malloc(sizeof(MirrorResourceList));
    if (!mirrorResourceList)
    {
        OC_LOG_V(DEBUG, VR_TAG,"Virtual Resource List Creation Fail.");
    }
    else
    {
        mirrorResourceList->headerNode = NULL;
        mirrorResourceList->tailNode = NULL;
    }

    return mirrorResourceList;
}

MirrorResource *createMirrorResource()
{
    MirrorResource *mirrorResource = (MirrorResource*)malloc(sizeof(MirrorResource));
    if (!mirrorResource)
    {
        OC_LOG_V(DEBUG, VR_TAG,"Virtual Resource List Creation Fail.");
    }
    else
    {
        mirrorResource->resourceHandle[OIC_REQUEST_HANDLE] = NULL;
        mirrorResource->resourceHandle[OIC_MIRROR_HANDLE] = NULL;
        mirrorResource->address[OIC_SOURCE_ADDRESS]    = NULL;
        mirrorResource->address[OIC_MIRROR_ADDRESS]   = NULL;

        mirrorResource->rep    = NULL;
        mirrorResource->uri    = NULL;
        mirrorResource->next   = NULL;

        mirrorResource->prop.countResourceType = 0;
        mirrorResource->prop.resourceType = NULL;
        mirrorResource->prop.countInterface = 0;
        mirrorResource->prop.resourceInterfaceName = NULL;
    }

    return mirrorResource;
}

OCStackResult destroyMirrorResourceList(MirrorResourceList *mirrorResourceList)
{
    OC_LOG_V(DEBUG, VR_TAG,"enter destroyVirtualResourceList");
    if(mirrorResourceList)
    {
        while (mirrorResourceList->headerNode)
        {
            deleteMirrorResourceFromList(mirrorResourceList, mirrorResourceList->headerNode);
        }

        free(mirrorResourceList);
    }
    else
    {
        return OC_STACK_INVALID_PARAM;
    }

    return OC_STACK_OK;
}

OCStackResult destroyMirrorResource(MirrorResource *mirrorResource)
{
    OC_LOG_V(DEBUG, VR_TAG,"enter destroy virtual resource.");
    if(mirrorResource)
    {
        if(mirrorResource->next)
        {
            mirrorResource->next = NULL;
        }
        if (mirrorResource->rep)
        {
            cJSON_Delete(mirrorResource->rep);
        }
        if (mirrorResource->uri)
        {
            free(mirrorResource->uri);
        }
        if (mirrorResource->address[OIC_SOURCE_ADDRESS])
        {
            free(mirrorResource->address[OIC_SOURCE_ADDRESS]);
        }
        if (mirrorResource->address[OIC_MIRROR_ADDRESS])
        {
            free(mirrorResource->address[OIC_MIRROR_ADDRESS]);
        }
        if (mirrorResource->prop.resourceType)
        {
            int i = 0;
            for (i = 0; i < mirrorResource->prop.countResourceType; ++i)
            {
                free(mirrorResource->prop.resourceType[i]);
                mirrorResource->prop.resourceType[i] = NULL;
            }
            free(mirrorResource->prop.resourceType);
            mirrorResource->prop.countResourceType = 0;
        }
        if (mirrorResource->prop.resourceInterfaceName)
        {
            int i = 0;
            for (i = 0; i < mirrorResource->prop.countInterface; ++i)
            {
                free(mirrorResource->prop.resourceInterfaceName[i]);
                mirrorResource->prop.resourceInterfaceName[i] = NULL;
            }
            free(mirrorResource->prop.resourceInterfaceName);
            mirrorResource->prop.countInterface = 0;
        }
        free(mirrorResource);
    }
    else
    {
        return OC_STACK_INVALID_PARAM;
    }

    return OC_STACK_OK;
}

OCStackResult insertMirrorResource(MirrorResourceList *mirrorResourceList,
                                   MirrorResource *mirrorResource)
{
    if (mirrorResourceList == NULL || mirrorResource == NULL)
    {
        return OC_STACK_INVALID_PARAM;
    }

    if (mirrorResourceList->headerNode == NULL)
    {
        mirrorResourceList->headerNode = mirrorResource;
        mirrorResourceList->tailNode = mirrorResource;
    }
    else
    {
        mirrorResourceList->tailNode->next = mirrorResource;
        mirrorResourceList->tailNode = mirrorResource;
    }

    return OC_STACK_OK;
}

MirrorResource *findMirrorResourceUsingAddressAndURI(MirrorResourceList *mirrorResourceList,
        const char *address, OICResourceCoordinatorParamType paramType, const char *uri)
{
    if (mirrorResourceList == NULL || address == NULL)
    {
        OC_LOG_V(DEBUG, VR_TAG,"Find Virtual Resource : invalid parameter.");
        return NULL;
    }
    if (mirrorResourceList->headerNode == NULL)
    {
        OC_LOG_V(DEBUG, VR_TAG,"Find Virtual Resource : Empty Virtual Resource List.");
        return NULL;
    }

    MirrorResource *tempMirrorResource = mirrorResourceList->headerNode;
    while (tempMirrorResource != NULL)
    {
        OC_LOG_V(DEBUG, VR_TAG,"node's uri = %s", tempMirrorResource->uri);
        if (strcmp(tempMirrorResource->address[paramType], address) == 0) // if(It is Same)
        {
            if (strcmp(tempMirrorResource->uri, uri) == 0) // if(It is Same)
            {
                return tempMirrorResource;
            }
        }
        tempMirrorResource = tempMirrorResource->next;
    }

    return NULL;
}

MirrorResource *findMirrorResourceUsingHandle(MirrorResourceList *mirrorResourceList,
        OCResourceHandle handle, OICResourceCoordinatorParamType paramType)
{
    if (mirrorResourceList == NULL || handle == NULL)
    {
        OC_LOG_V(DEBUG, VR_TAG,"Find Virtual Resource : invalid parameter.");
        return NULL;
    }
    if (mirrorResourceList->headerNode == NULL)
    {
        OC_LOG_V(DEBUG, VR_TAG,"Find Virtual Resource : Empty Virtual Resource List.");
        return NULL;
    }

    MirrorResource *tempMirrorResource = mirrorResourceList->headerNode;
    while (tempMirrorResource != NULL)
    {
        if (tempMirrorResource->resourceHandle[paramType] == handle)
        {
            return tempMirrorResource;
        }
        tempMirrorResource = tempMirrorResource->next;
    }

    return NULL;
}

OCStackResult deleteMirrorResourceFromList(MirrorResourceList *mirrorResourceList,
        MirrorResource *mirrorResource)
{

    OC_LOG_V(DEBUG, VR_TAG,"enter delete virtual resource.");

    if (mirrorResourceList == NULL || mirrorResource == NULL)
    {
        OC_LOG_V(DEBUG, VR_TAG,"Delete Virtual Resource : invalid parameter.");
        return OC_STACK_INVALID_PARAM;
    }
    if (mirrorResourceList->headerNode == NULL)
    {
        OC_LOG_V(DEBUG, VR_TAG,"Delete Virtual Resource : Empty Virtual Resource List.");
        return OC_STACK_ERROR;
    }

    if (mirrorResource == mirrorResourceList->headerNode)
    {
        mirrorResourceList->headerNode = mirrorResourceList->headerNode->next;
        mirrorResource->next = NULL;
        return destroyMirrorResource(mirrorResource);
    }

    MirrorResource *preNode = mirrorResourceList->headerNode;
    MirrorResource *curNode = preNode->next;
    while (curNode != NULL)
    {
        if (curNode == mirrorResource)
        {
            if (curNode == mirrorResourceList->tailNode)
            {
                mirrorResourceList->tailNode = preNode;
                preNode->next = NULL;
            }
            else
            {
                preNode->next = curNode->next;
            }
            mirrorResource->next = NULL;
            return destroyMirrorResource(mirrorResource);
        }
        preNode = curNode;
        curNode = curNode->next;
    }

    return OC_STACK_ERROR;
}

OCStackResult ejectMirrorResource(MirrorResourceList *mirrorResourceList,
                                  MirrorResource *mirrorResource)
{
    if (mirrorResourceList == NULL || mirrorResource == NULL)
    {
        OC_LOG_V(DEBUG, VR_TAG,"Eject Virtual Resource : invalid parameter.");
        return OC_STACK_INVALID_PARAM;
    }
    if (mirrorResourceList->headerNode == NULL)
    {
        OC_LOG_V(DEBUG, VR_TAG, "Eject Virtual Resource : Empty Virtual Resource List.");
        return OC_STACK_ERROR;
    }

    if (mirrorResource == mirrorResourceList->headerNode)
    {
        mirrorResourceList->headerNode = mirrorResourceList->headerNode->next;
        return OC_STACK_OK;
    }

    MirrorResource *preNode = mirrorResourceList->headerNode;
    MirrorResource *curNode = preNode->next;
    while (curNode != NULL)
    {
        if (curNode == mirrorResource)
        {
            if(curNode == mirrorResourceList->headerNode)
            {
                mirrorResourceList->headerNode = NULL;
                mirrorResourceList->tailNode = NULL;
            }
            else if(curNode == mirrorResourceList->tailNode)
            {
                mirrorResourceList->tailNode = preNode;
            }
            else
            {
            preNode->next = curNode->next;
            }
            return OC_STACK_OK;
        }
        preNode = curNode;
        curNode = curNode->next;
    }

    return OC_STACK_ERROR;

}

MirrorResource *cloneMirrorResource(MirrorResource *sourceMirrorResource)
{
    MirrorResource *clonedMirrorResource = createMirrorResource();

    int sizeofstr = 0;
    int i = 0;

    clonedMirrorResource->rep = cJSON_Parse(cJSON_PrintUnformatted(sourceMirrorResource->rep));

    sizeofstr = strlen(sourceMirrorResource->uri) + 1;
    clonedMirrorResource->uri = (char *)malloc(sizeof(char) * sizeofstr);
    memset(clonedMirrorResource->uri, '\0', sizeofstr);
    strcpy(clonedMirrorResource->uri, sourceMirrorResource->uri);

    for (i = OIC_SOURCE_ADDRESS; i < OIC_MIRROR_ADDRESS; ++i)
    {
        sizeofstr = strlen(sourceMirrorResource->address[i]) + 1;
        clonedMirrorResource->address[i] = (char *)malloc(sizeof(char) * sizeofstr);
        memset(clonedMirrorResource->address[i], '\0', sizeofstr);
        strcpy(clonedMirrorResource->address[i], sourceMirrorResource->address[i]);
    }

    return clonedMirrorResource;
}

MirrorResourceList *findMirrorResourceListUsingAddress(MirrorResourceList *mirrorResourceList,
        const char *address, OICResourceCoordinatorParamType paramType)
{
    if (mirrorResourceList == NULL || address == NULL)
    {
        OC_LOG_V(DEBUG, VR_TAG,"Find Virtual Resource List : invalid parameter.");
        return NULL;
    }
    if (mirrorResourceList->headerNode == NULL)
    {
        OC_LOG_V(DEBUG, VR_TAG,"Find Virtual Resource List : Empty Virtual Resource List.");
        return NULL;
    }

    MirrorResource *tempNode = mirrorResourceList->headerNode;
    while (tempNode != NULL)
    {
        OC_LOG_V(DEBUG, VR_TAG, "uri = %s", tempNode->uri);
        tempNode = tempNode->next;
    }


    MirrorResourceList *resultMirrorResourceList = createMirrorResourceList();
    MirrorResource *mirrorResource = mirrorResourceList->headerNode;
    while (mirrorResource != NULL)
    {
        if (strcmp(mirrorResource->address[paramType], address) == 0) // if(It is Same)
        {
            insertMirrorResource(resultMirrorResourceList, cloneMirrorResource(mirrorResource));
        }
        mirrorResource = mirrorResource->next;
    }

    return resultMirrorResourceList;
}


OCStackResult printMirrorResourceList(MirrorResourceList *mirrorResourceList)
{
    if (mirrorResourceList == NULL)
    {
        OC_LOG_V(DEBUG, VR_TAG, "print Virtual Resource list : invalid parameter.");
        return OC_STACK_INVALID_PARAM;
    }
    if (mirrorResourceList->headerNode == NULL)
    {
        OC_LOG_V(DEBUG, VR_TAG, "print Virtual Resource list : Empty Virtual Resource List.");
        return OC_STACK_INVALID_PARAM;
    }
    OC_LOG_V(DEBUG, VR_TAG, "==============================================================");
    MirrorResource *mirrorResource = mirrorResourceList->headerNode;
    while (mirrorResource != NULL)
    {
        OC_LOG_V(DEBUG, VR_TAG, "uri = %s", mirrorResource->uri);
        mirrorResource = mirrorResource->next;
    }
    OC_LOG_V(DEBUG, VR_TAG, "==============================================================");

    return OC_STACK_OK;
}
