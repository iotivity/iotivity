/******************************************************************
*
* Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "logger.h"
#include "oic_malloc.h"
#include "utlist.h"
#include "securevirtualresourcetypes.h"
#include "provisioningmanager.h"

#define MAX_URI_LENGTH (64)
#define MAX_PERMISSION_LENGTH (5)
#define MAX_INPUT_ID_LENGTH (16)
#define PREDEFINED_TIMEOUT (10)
#define CREATE (1)
#define READ (2)
#define UPDATE (4)
#define DELETE (8)
#define NOTIFY (16)
#define DASH '-'
#define TAG  "provisioningclient"

static OicSecAcl_t        *gAcl = NULL;
static char PROV_TOOL_DB_FILE[] = "oic_svr_db_prov_tool.json";

/**
 * Perform cleanup for ACL list
 *
 * @param[in]    ACL list
 */
static void DeleteACLList(OicSecAcl_t *acl)
{
    if (acl)
    {
        OicSecAcl_t *aclTmp1 = NULL;
        OicSecAcl_t *aclTmp2 = NULL;
        LL_FOREACH_SAFE(acl, aclTmp1, aclTmp2)
        {
            LL_DELETE(acl, aclTmp1);

            /* Clean Resources */
            for (int i = 0; i < aclTmp1->resourcesLen; i++)
            {
                OICFree(aclTmp1->resources[i]);
            }
            OICFree(aclTmp1->resources);

            /* Clean Owners */
            OICFree(aclTmp1->owners);

            /* Clean ACL node itself */
            OICFree(aclTmp1);
        }
        acl = NULL;
    }
}

/**
 * Calculate ACL permission from string to bit
 *
 * @param[in] temp_psm    Input data of ACL permission string
 * @param[in,out] pms    The pointer of ACL permission value
 * @return  0 on success otherwise a positive error value
 * @retval  SP_RESULT_SUCCESS    Successful
 * @retval  SP_RESULT_INVALID_PARAM    Invaild input arguments
 */
static SPResult CalculateAclPermission(const char *temp_pms, uint16_t *pms)
{
    int i = 0;

    if (NULL == temp_pms || NULL == pms)
    {
        return SP_RESULT_INVALID_PARAM;
    }
    *pms = 0;
    while (temp_pms[i] != '\0')
    {
        switch (temp_pms[i])
        {
            case 'C':
                {
                    *pms += CREATE;
                    i++;
                    break;
                }
            case 'R':
                {
                    *pms += READ;
                    i++;
                    break;
                }
            case 'U':
                {
                    *pms += UPDATE;
                    i++;
                    break;
                }
            case 'D':
                {
                    *pms += DELETE;
                    i++;
                    break;
                }
            case 'N':
                {
                    *pms += NOTIFY;
                    i++;
                    break;
                }
            case '_':
                {
                    i++;
                    break;
                }
            default:
                {
                    return SP_RESULT_INVALID_PARAM;
                }
        }
    }
    return SP_RESULT_SUCCESS;
}

/**
 * Get the ACL property from user
 *
 * @param[in]    ACL Datastructure to save user inputs
 * @return  0 on success otherwise a positive error value
 * @retval  SP_RESULT_SUCCESS    Successful
 * @retval  SP_RESULT_MEM_ALLOCATION_FAIL    Memmory allocation failure
 */
static SPResult InputACL(OicSecAcl_t *acl)
{
    int unused __attribute__((unused));
    char temp_id [MAX_INPUT_ID_LENGTH + 4] = {0,};
    char temp_rsc[MAX_URI_LENGTH + 1] = {0,};
    char temp_pms[MAX_PERMISSION_LENGTH + 1] = {0,};
    printf("******************************************************************************\n");
    printf("-Set ACL policy for target device\n");
    printf("******************************************************************************\n");
    //Set Subject.
    printf("-URN identifying the subject\n");
    printf("ex) 1111-1111-1111-1111 (16 Numbers except to '-')\n");
    printf("Subject : ");
    unused = scanf("%19s", temp_id);
    int j = 0;
    for (int i = 0; temp_id[i] != '\0'; i++)
    {
        if (DASH != temp_id[i])
            acl->subject.id[j++] = temp_id[i];
    }

    //Set Resource.
    printf("Num. of Resource : ");
    unused = scanf("%zu", &acl->resourcesLen);
    printf("-URI of resource\n");
    printf("ex)/oic/sh/temp/0 (Max_URI_Length: 64 Byte )\n");
    acl->resources = (char **)OICMalloc(acl->resourcesLen * sizeof(char *));
    if (NULL == acl->resources)
    {
        OC_LOG(ERROR, TAG, "Error while memory allocation");
        return SP_RESULT_MEM_ALLOCATION_FAIL;
    }
    for (int i = 0; i < acl->resourcesLen; i++)
    {
        printf("[%d]Resource : ", i + 1);
        unused = scanf("%64s", temp_rsc);
        acl->resources[i] = (char *)OICMalloc((strlen(temp_rsc) + 1) * sizeof(char));
        if (NULL == acl->resources[i])
        {
            OC_LOG(ERROR, TAG, "Error while memory allocation");
            return SP_RESULT_MEM_ALLOCATION_FAIL;
        }
        strncpy(acl->resources[i], temp_rsc, strlen(temp_rsc));
        acl->resources[i][strlen(temp_rsc)] = '\0';
    }
    // Set Permission
    do
    {
        printf("-Set the permission(C,R,U,D,N)\n");
        printf("ex) CRUDN, CRU_N,..(5 Charaters)\n");
        printf("Permission : ");
        unused = scanf("%5s", temp_pms);
    }
    while (SP_RESULT_SUCCESS != CalculateAclPermission(temp_pms, &(acl->permission)) );
    // Set Rowner
    printf("Num. of Rowner : ");
    unused = scanf("%zu", &acl->ownersLen);
    printf("-URN identifying the rowner\n");
    printf("ex) 1111-1111-1111-1111 (16 Numbers except to '-')\n");
    acl->owners = (OicUuid_t *)OICCalloc(acl->ownersLen, sizeof(OicUuid_t));
    if (NULL == acl->owners)
    {
        OC_LOG(ERROR, TAG, "Error while memory allocation");
        return SP_RESULT_MEM_ALLOCATION_FAIL;
    }
    for (int i = 0; i < acl->ownersLen; i++)
    {
        printf("[%d]Rowner : ", i + 1);
        unused = scanf("%19s", temp_id);
        j = 0;
        for (int k = 0; temp_id[k] != '\0'; k++)
        {
            if (DASH != temp_id[k])
            {
                acl->owners[i].id[j++] = temp_id[k];
            }
        }
    }
    return SP_RESULT_SUCCESS;
}

FILE* client_fopen(const char *path, const char *mode)
{
    (void)path;
    return fopen(PROV_TOOL_DB_FILE, mode);
}

/**
 * Provisioning client sample using ProvisioningAPI on provisioningmanager.c
 * To change network type use command line option -w for Wifi and -e for Ethernet
 */
int main(int argc, char **argv)
{
    SPResult res = SP_RESULT_SUCCESS;
    SPTargetDeviceInfo_t *pDeviceList = NULL;
    gAcl = (OicSecAcl_t *)OICMalloc(sizeof(OicSecAcl_t));

    // Initialize Persistent Storage for SVR database
    OCPersistentStorage ps = {};
    ps.open = client_fopen;
    ps.read = fread;
    ps.write = fwrite;
    ps.close = fclose;
    ps.unlink = unlink;
    OCRegisterPersistentStorageHandler(&ps);

    if (OCInit(NULL, 0, OC_CLIENT_SERVER) != OC_STACK_OK)
    {
        OC_LOG(ERROR, TAG, "OCStack init error");
        return 0;
    }

    if (NULL == gAcl)
    {
        OC_LOG(ERROR, TAG, "Error while memory allocation");
        return SP_RESULT_MEM_ALLOCATION_FAIL;
    }

    res = SPProvisioningDiscovery(PREDEFINED_TIMEOUT, &pDeviceList);
    if (SP_RESULT_SUCCESS == res)
    {
        while (pDeviceList != NULL)
        {
            printf("-Provisioning device ID : ");
            for (int i = 0; i < MAX_INPUT_ID_LENGTH; i++)
            {
                if (pDeviceList->doxm->deviceID.id[i] == '\0')
                {
                     break;
                }
                printf("%c", pDeviceList->doxm->deviceID.id[i]);
            }
            printf("\n");
            res = SPInitProvisionContext(PREDEFINED_TIMEOUT, pDeviceList);
            if (SP_RESULT_SUCCESS != res)
            {
                OC_LOG(ERROR, TAG, "Error while init provisioning Context");
                goto error;
            }
            res = InputACL(gAcl);
            if (SP_RESULT_SUCCESS != res)
            {
                OC_LOG(ERROR, TAG, "Error while user ACL input ");
                goto error;
            }
            res = SPProvisionACL(PREDEFINED_TIMEOUT, pDeviceList, gAcl);
            if (SP_RESULT_SUCCESS != res)
            {
                OC_LOG(ERROR, TAG, "Error while provisioning ACL");
                goto error;
            }
            res = SPFinalizeProvisioning(PREDEFINED_TIMEOUT, pDeviceList);
            if (SP_RESULT_SUCCESS == res)
            {
                printf("Provisioning Success~!!\n");
            }
            else if (SP_RESULT_SUCCESS != res)
            {
                OC_LOG(ERROR, TAG, "Error while Finalize Provisioning");
                goto error;
            }
            pDeviceList = pDeviceList->next;
        }
    }
    else
    {
        OC_LOG(ERROR, TAG, "Error while Provisioning Discovery");
        goto error;
    }

error:
    DeleteACLList(gAcl);
    SPTerminateProvisioning();
    return 0;
}
