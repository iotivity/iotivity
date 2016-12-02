//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License a
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

#include <stdlib.h>
#include <string.h>

#ifdef RD_SERVER
#include "sqlite3.h"
#endif

#include "octypes.h"
#include "ocstack.h"
#include "ocrandom.h"
#include "logger.h"
#include "ocpayload.h"
#include "oic_malloc.h"
#include "oic_string.h"

#define TAG "OIC_RI_RESOURCEDIRECTORY"

#define VERIFY_NON_NULL(arg, logLevel, retVal) { if (!(arg)) { OIC_LOG((logLevel), \
             TAG, #arg " is NULL"); return (retVal); } }

#ifdef RD_SERVER

#define RD_PATH "RD.db"

static sqlite3 *gRDDB = NULL;

/* Column indices of RD_DEVICE_LINK_LIST table */
static const uint8_t ins_index = 0;
static const uint8_t uri_index = 1;
static const uint8_t p_index = 4;
static const uint8_t d_index = 7;

/* Column indices of RD_LINK_RT table */
static const uint8_t rt_value_index = 0;

/* Column indices of RD_LINK_IF table */
static const uint8_t if_value_index = 0;

#define VERIFY_SQLITE(arg) \
if (SQLITE_OK != (arg)) \
{ \
    OIC_LOG_V(ERROR, TAG, "Error in " #arg ", Error Message: %s",  sqlite3_errmsg(gRDDB)); \
    sqlite3_exec(gRDDB, "ROLLBACK", NULL, NULL, NULL); \
    return OC_STACK_ERROR; \
}

static void errorCallback(void *arg, int errCode, const char *errMsg)
{
    OC_UNUSED(arg);
    OC_UNUSED(errCode);
    OC_UNUSED(errMsg);
    OIC_LOG_V(ERROR, TAG, "SQLLite Error: %s : %d", errMsg, errCode);
}

static OCStackResult initializeDatabase(const char *path)
{
    if (SQLITE_OK == sqlite3_config(SQLITE_CONFIG_LOG, errorCallback))
    {
        OIC_LOG_V(INFO, TAG, "SQLite debugging log initialized.");
    }

    sqlite3_open_v2(!path ? RD_PATH : path, &gRDDB, SQLITE_OPEN_READONLY, NULL);
    if (!gRDDB)
    {
        return OC_STACK_ERROR;
    }
    return OC_STACK_OK;
}

static OCStackResult appendStringLL(OCStringLL **type, const unsigned char *value)
{
    OCStringLL *temp= (OCStringLL*)OICCalloc(1, sizeof(OCStringLL));
    if (!temp)
    {
        return OC_STACK_NO_MEMORY;
    }
    temp->value = OICStrdup((char *)value);
    if (!temp->value)
    {
        return OC_STACK_NO_MEMORY;
    }
    temp->next = NULL;

    if (!*type)
    {
        *type = temp;
    }
    else
    {
        OCStringLL *tmp = *type;
        for (; tmp->next; tmp = tmp->next);
        tmp->next = temp;
    }
    return OC_STACK_OK;
}

/* stmt is of form "SELECT * FROM RD_DEVICE_LINK_LIST ..." */
static OCStackResult ResourcePayloadCreate(sqlite3_stmt *stmt, OCDiscoveryPayload *discPayload)
{
    int res = sqlite3_step(stmt);
    if (SQLITE_ROW != res)
    {
        return OC_STACK_NO_RESOURCE;
    }
    OCStackResult result = OC_STACK_OK;
    OCResourcePayload *resourcePayload = NULL;
    while (SQLITE_ROW == res)
    {
        resourcePayload = (OCResourcePayload *)OICCalloc(1, sizeof(OCResourcePayload));
        if (!resourcePayload)
        {
            result = OC_STACK_NO_MEMORY;
            goto exit;
        }

        int id = sqlite3_column_int(stmt, ins_index);
        const unsigned char *uri = sqlite3_column_text(stmt, uri_index);
        int bitmap = sqlite3_column_int(stmt, p_index);
        int deviceId = sqlite3_column_int(stmt, d_index);
        OIC_LOG_V(DEBUG, TAG, " %s %d", uri, deviceId);
        resourcePayload->uri = OICStrdup((char *)uri);
        if (!resourcePayload->uri)
        {
            result = OC_STACK_NO_MEMORY;
            goto exit;
        }

        sqlite3_stmt *stmtRT = 0;
        const char *rt = "SELECT rt FROM RD_LINK_RT WHERE LINK_ID=?";
        VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, rt, strlen(rt) + 1, &stmtRT, NULL));
        VERIFY_SQLITE(sqlite3_bind_int(stmtRT, 1, id));
        while (SQLITE_ROW == sqlite3_step(stmtRT))
        {
            const unsigned char *rt1 = sqlite3_column_text(stmtRT, rt_value_index);
            result = appendStringLL(&resourcePayload->types, rt1);
            if (OC_STACK_OK != result)
            {
                goto exit;
            }
        }
        VERIFY_SQLITE(sqlite3_finalize(stmtRT));

        sqlite3_stmt *stmtIF = 0;
        const char *itf = "SELECT if FROM RD_LINK_IF WHERE LINK_ID=?";
        VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, itf, strlen(itf) + 1, &stmtIF, NULL));
        VERIFY_SQLITE(sqlite3_bind_int(stmtIF, 1, id));
        while (SQLITE_ROW == sqlite3_step(stmtIF))
        {
            const unsigned char *itf = sqlite3_column_text(stmtIF, if_value_index);
            result = appendStringLL(&resourcePayload->interfaces, itf);
            if (OC_STACK_OK != result)
            {
                goto exit;
            }
        }
        VERIFY_SQLITE(sqlite3_finalize(stmtIF));

        resourcePayload->bitmap = bitmap & (OC_OBSERVABLE | OC_DISCOVERABLE);
        resourcePayload->secure = (bitmap & OC_SECURE) != 0;

        const char *address = "SELECT di, address FROM RD_DEVICE_LIST "
            "INNER JOIN RD_DEVICE_LINK_LIST ON RD_DEVICE_LINK_LIST.DEVICE_ID = RD_DEVICE_LIST.ID "
            "WHERE RD_DEVICE_LINK_LIST.DEVICE_ID=?";
        const uint8_t di_index = 0;
        const uint8_t address_index = 1;

        sqlite3_stmt *stmt1 = 0;
        VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, address, strlen(address) + 1, &stmt1, NULL));
        VERIFY_SQLITE(sqlite3_bind_int(stmt1, 1, deviceId));

        res = sqlite3_step(stmt1);
        if (SQLITE_ROW == res || SQLITE_DONE == res)
        {
            const unsigned char *di = sqlite3_column_text(stmt1, di_index);
            const unsigned char *address = sqlite3_column_text(stmt1, address_index);
            OIC_LOG_V(DEBUG, TAG, " %s %s", di, address);
            discPayload->baseURI = OICStrdup((char *)address);
            if (!discPayload->baseURI)
            {
                result = OC_STACK_NO_MEMORY;
                goto exit;
            }
            discPayload->sid = OICStrdup((char *)di);
            if (!discPayload->sid)
            {
                result = OC_STACK_NO_MEMORY;
                goto exit;
            }
        }
        VERIFY_SQLITE(sqlite3_finalize(stmt1));
        OCDiscoveryPayloadAddNewResource(discPayload, resourcePayload);
        res = sqlite3_step(stmt);
    }
exit:
    if (OC_STACK_OK != result)
    {
        OCDiscoveryResourceDestroy(resourcePayload);
    }
    return result;
}

static OCStackResult CheckResources(const char *interfaceType, const char *resourceType,
        OCDiscoveryPayload *discPayload)
{
    if (initializeDatabase(NULL) != OC_STACK_OK)
    {
        return OC_STACK_INTERNAL_SERVER_ERROR;
    }
    if (!interfaceType && !resourceType)
    {
        return OC_STACK_INVALID_QUERY;
    }
    if (!discPayload || !discPayload->sid)
    {
        return OC_STACK_INTERNAL_SERVER_ERROR;
    }

    OCStackResult result = OC_STACK_OK;
    sqlite3_stmt *stmt = 0;
    if (resourceType)
    {
        if (!interfaceType || 0 == strcmp(interfaceType, OC_RSRVD_INTERFACE_LL))
        {
            const char *input = "SELECT * FROM RD_DEVICE_LINK_LIST "
                                "INNER JOIN RD_DEVICE_LIST ON RD_DEVICE_LINK_LIST.DEVICE_ID=RD_DEVICE_LIST.ID "
                                "INNER JOIN RD_LINK_RT ON RD_DEVICE_LINK_LIST.INS=RD_LINK_RT.LINK_ID "
                                "WHERE RD_DEVICE_LIST.di LIKE ?1 AND RD_LINK_RT.rt LIKE ?2";
            VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, input, strlen(input) + 1, &stmt, NULL));
            VERIFY_SQLITE(sqlite3_bind_text(stmt, 1, discPayload->sid, strlen(discPayload->sid),
                                            SQLITE_STATIC));
            VERIFY_SQLITE(sqlite3_bind_text(stmt, 2, resourceType, strlen(resourceType), SQLITE_STATIC));
        }
        else
        {
            const char *input = "SELECT * FROM RD_DEVICE_LINK_LIST "
                                "INNER JOIN RD_DEVICE_LIST ON RD_DEVICE_LINK_LIST.DEVICE_ID=RD_DEVICE_LIST.ID "
                                "INNER JOIN RD_LINK_RT ON RD_DEVICE_LINK_LIST.INS=RD_LINK_RT.LINK_ID "
                                "INNER JOIN RD_LINK_IF ON RD_DEVICE_LINK_LIST.INS=RD_LINK_IF.LINK_ID "
                                "WHERE RD_DEVICE_LIST.di LIKE ?1 AND RD_LINK_RT.rt LIKE ?2 AND RD_LINK_IF.if LIKE ?3";
            VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, input, strlen(input) + 1, &stmt, NULL));
            VERIFY_SQLITE(sqlite3_bind_text(stmt, 1, discPayload->sid, strlen(discPayload->sid),
                                            SQLITE_STATIC));
            VERIFY_SQLITE(sqlite3_bind_text(stmt, 2, resourceType, strlen(resourceType), SQLITE_STATIC));
            VERIFY_SQLITE(sqlite3_bind_text(stmt, 3, interfaceType, strlen(interfaceType), SQLITE_STATIC));
        }
        result = ResourcePayloadCreate(stmt, discPayload);
    }
    else if (interfaceType)
    {
        if (0 == strcmp(interfaceType, OC_RSRVD_INTERFACE_LL))
        {
            const char *input = "SELECT * FROM RD_DEVICE_LINK_LIST "
                                "INNER JOIN RD_DEVICE_LIST ON RD_DEVICE_LINK_LIST.DEVICE_ID=RD_DEVICE_LIST.ID "
                                "WHERE RD_DEVICE_LIST.di LIKE ?1";
            VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, input, strlen(input) + 1, &stmt, NULL));
            VERIFY_SQLITE(sqlite3_bind_text(stmt, 1, discPayload->sid, strlen(discPayload->sid),
                                            SQLITE_STATIC));
        }
        else
        {
            const char *input = "SELECT * FROM RD_DEVICE_LINK_LIST "
                                "INNER JOIN RD_DEVICE_LIST ON RD_DEVICE_LINK_LIST.DEVICE_ID=RD_DEVICE_LIST.ID "
                                "INNER JOIN RD_LINK_IF ON RD_DEVICE_LINK_LIST.INS=RD_LINK_IF.LINK_ID "
                                "WHERE RD_DEVICE_LIST.di LIKE ?1 AND RD_LINK_IF.if LIKE ?2";
            VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, input, strlen(input) + 1, &stmt, NULL));
            VERIFY_SQLITE(sqlite3_bind_text(stmt, 1, discPayload->sid, strlen(discPayload->sid),
                                            SQLITE_STATIC));
            VERIFY_SQLITE(sqlite3_bind_text(stmt, 2, interfaceType, strlen(interfaceType), SQLITE_STATIC));
        }
        result = ResourcePayloadCreate(stmt, discPayload);
    }
    if (stmt)
    {
        VERIFY_SQLITE(sqlite3_finalize(stmt));
    }
    return result;
}

OCStackResult OCRDDatabaseDiscoveryPayloadCreate(const char *interfaceType,
        const char *resourceType,
        OCDiscoveryPayload **payload)
{
    OCStackResult result = OC_STACK_NO_RESOURCE;
    OCDiscoveryPayload *head = NULL;
    OCDiscoveryPayload **tail = &head;

    if (*payload)
    {
        /*
         * This is an error of the caller, return here instead of touching the
         * caller provided payload.
         */
        OIC_LOG_V(ERROR, TAG, "Payload is already allocated");
        return OC_STACK_INTERNAL_SERVER_ERROR;
    }
    if (initializeDatabase(NULL) != OC_STACK_OK)
    {
        goto exit;
    }

    const char *serverID = OCGetServerInstanceIDString();
    sqlite3_stmt *stmt = 0;
    const char *input = "SELECT di FROM RD_DEVICE_LIST";
    const uint8_t di_index = 0;
    VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, input, strlen(input) + 1, &stmt, NULL));
    while (SQLITE_ROW == sqlite3_step(stmt))
    {
        const unsigned char *di = sqlite3_column_text(stmt, di_index);
        if (0 == strcmp((const char *)di, serverID))
        {
            continue;
        }
        *tail = OCDiscoveryPayloadCreate();
        VERIFY_PARAM_NON_NULL(TAG, *tail, "Failed creating discovery payload.");
        (*tail)->sid = (char *)OICCalloc(1, UUID_STRING_SIZE);
        VERIFY_PARAM_NON_NULL(TAG, (*tail)->sid, "Failed adding device id to discovery payload.");
        memcpy((*tail)->sid, di, UUID_STRING_SIZE);
        result = CheckResources(interfaceType, resourceType, *tail);
        if (OC_STACK_OK == result)
        {
            tail = &(*tail)->next;
        }
        else
        {
            OCPayloadDestroy((OCPayload *) *tail);
            *tail = NULL;
        }
    }
    VERIFY_SQLITE(sqlite3_finalize(stmt));
    *payload = head;
    return result;
exit:
    OCPayloadDestroy((OCPayload *) *tail);
    *payload = NULL;
    return OC_STACK_INTERNAL_SERVER_ERROR;
}
#endif
