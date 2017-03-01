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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite3.h"
#include "logger.h"
#include "ocpayload.h"
#include "octypes.h"
#include "oic_malloc.h"
#include "oic_string.h"
#include "ocstackinternal.h"

#ifdef RD_SERVER

#define TAG "OIC_RD_DATABASE"

static sqlite3 *gRDDB = NULL;

#define VERIFY_SQLITE(arg) \
    if (SQLITE_OK != (arg)) \
    { \
        OIC_LOG_V(ERROR, TAG, "Error in " #arg ", Error Message: %s",  sqlite3_errmsg(gRDDB)); \
        sqlite3_exec(gRDDB, "ROLLBACK", NULL, NULL, NULL); \
        return OC_STACK_ERROR; \
    }

#define CHECK_DATABASE_INIT \
    if (!gRDDB) \
    { \
        OIC_LOG(ERROR, TAG, "Database is not initialized."); \
        return OC_STACK_ERROR; \
    }

#define STR(a) #a
#define XSTR(a) STR(a)

#define RD_TABLE \
    "create table RD_DEVICE_LIST(ID INTEGER PRIMARY KEY AUTOINCREMENT, " \
    XSTR(OC_RSRVD_DEVICE_ID) " UNIQUE NOT NULL, " \
    XSTR(OC_RSRVD_TTL) " NOT NULL);"

#define RD_LL_TABLE  \
    "create table RD_DEVICE_LINK_LIST("XSTR(OC_RSRVD_INS)" INTEGER PRIMARY KEY AUTOINCREMENT, " \
    XSTR(OC_RSRVD_HREF) "," \
    XSTR(OC_RSRVD_REL) ","  \
    XSTR(OC_RSRVD_URI) "," \
    XSTR(OC_RSRVD_BITMAP) "," \
    "DEVICE_ID INT NOT NULL, " \
    "FOREIGN KEY(DEVICE_ID) REFERENCES RD_DEVICE_LIST(ID) ON DELETE CASCADE);"

#define RD_RT_TABLE \
    "create table RD_LINK_RT(" XSTR(OC_RSRVD_RESOURCE_TYPE) " NOT NULL, " \
    "LINK_ID INT NOT NULL, " \
    "FOREIGN KEY("XSTR(LINK_ID)") REFERENCES RD_DEVICE_LINK_LIST("XSTR(OC_RSRVD_INS)") " \
    "ON DELETE CASCADE);"

#define RD_IF_TABLE \
    "create table RD_LINK_IF(" XSTR(OC_RSRVD_INTERFACE) " NOT NULL, " \
    "LINK_ID INT NOT NULL, "\
    "FOREIGN KEY("XSTR(LINK_ID)") REFERENCES RD_DEVICE_LINK_LIST("XSTR(OC_RSRVD_INS)") " \
    "ON DELETE CASCADE);"

#define RD_EP_TABLE \
    "create table RD_LINK_EP(" XSTR(OC_RSRVD_ENDPOINT) " NOT NULL, " \
    XSTR(OC_RSRVD_PRIORITY) " INT NOT NULL, " \
    "LINK_ID INT NOT NULL, "\
    "FOREIGN KEY("XSTR(LINK_ID)") REFERENCES RD_DEVICE_LINK_LIST("XSTR(OC_RSRVD_INS)") " \
    "ON DELETE CASCADE);"

static void errorCallback(void *arg, int errCode, const char *errMsg)
{
    OC_UNUSED(arg);
    OC_UNUSED(errCode);
    OC_UNUSED(errMsg);
    OIC_LOG_V(ERROR, TAG, "SQLLite Error: %s : %d", errMsg, errCode);
}

OCStackResult OCRDDatabaseInit()
{
    if (SQLITE_OK == sqlite3_config(SQLITE_CONFIG_LOG, errorCallback))
    {
        OIC_LOG_V(INFO, TAG, "SQLite debugging log initialized.");
    }

    int sqlRet;
    sqlRet = sqlite3_open_v2(OCRDDatabaseGetStorageFilename(), &gRDDB,
                             SQLITE_OPEN_READWRITE, NULL);
    if (SQLITE_OK != sqlRet)
    {
        OIC_LOG(DEBUG, TAG, "RD database file did not open, as no table exists.");
        OIC_LOG(DEBUG, TAG, "RD creating new table.");
        sqlRet = sqlite3_open_v2(OCRDDatabaseGetStorageFilename(), &gRDDB,
                                 SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
        if (SQLITE_OK == sqlRet)
        {
            VERIFY_SQLITE(sqlite3_exec(gRDDB, RD_TABLE, NULL, NULL, NULL));
            OIC_LOG(DEBUG, TAG, "RD created RD_DEVICE_LIST table.");

            VERIFY_SQLITE(sqlite3_exec(gRDDB, RD_LL_TABLE, NULL, NULL, NULL));
            OIC_LOG(DEBUG, TAG, "RD created RD_DEVICE_LINK_LIST table.");

            VERIFY_SQLITE(sqlite3_exec(gRDDB, RD_RT_TABLE, NULL, NULL, NULL));
            OIC_LOG(DEBUG, TAG, "RD created RD_LINK_RT table.");

            VERIFY_SQLITE(sqlite3_exec(gRDDB, RD_IF_TABLE, NULL, NULL, NULL));
            OIC_LOG(DEBUG, TAG, "RD created RD_LINK_IF table.");

            VERIFY_SQLITE(sqlite3_exec(gRDDB, RD_EP_TABLE, NULL, NULL, NULL));
            OIC_LOG(DEBUG, TAG, "RD created RD_LINK_EP table.");
            sqlRet = SQLITE_OK;
        }
    }

    if (sqlRet == SQLITE_OK)
    {
        sqlite3_stmt *stmt = 0;
        VERIFY_SQLITE(sqlite3_prepare_v2 (gRDDB, "PRAGMA foreign_keys = ON;", -1, &stmt, NULL));

        if (SQLITE_DONE != sqlite3_step(stmt))
        {
            sqlite3_finalize(stmt);
            return OC_STACK_ERROR;
        }

        VERIFY_SQLITE(sqlite3_finalize(stmt));
    }

    return OC_STACK_OK;
}

OCStackResult OCRDDatabaseClose()
{
    CHECK_DATABASE_INIT;
    VERIFY_SQLITE(sqlite3_close_v2(gRDDB));
    return OC_STACK_OK;
}

static int storeResourceType(char **resourceTypes, size_t size, sqlite3_int64 rowid)
{
    int res = 1;
    VERIFY_SQLITE(sqlite3_exec(gRDDB, "BEGIN TRANSACTION", NULL, NULL, NULL));

    const char *deleteRT = "DELETE FROM RD_LINK_RT WHERE LINK_ID=@id";
    const char *insertRT = "INSERT INTO RD_LINK_RT VALUES(@resourceType, @id)";
    sqlite3_stmt *stmt = 0;

    VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, deleteRT, strlen(deleteRT) + 1, &stmt, NULL));
    VERIFY_SQLITE(sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@id"), rowid));
    res = sqlite3_step(stmt);
    VERIFY_SQLITE(sqlite3_finalize(stmt));
    if (res != SQLITE_DONE)
    {
        sqlite3_exec(gRDDB, "ROLLBACK", NULL, NULL, NULL);
        return res;
    }

    for (size_t i = 0; i < size; i++)
    {
        VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, insertRT, strlen(insertRT) + 1, &stmt, NULL));
        if (resourceTypes[i])
        {
            VERIFY_SQLITE(sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@resourceType"),
                                            resourceTypes[i], strlen(resourceTypes[i]), SQLITE_STATIC));
            VERIFY_SQLITE(sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@id"), rowid));
        }
        res = sqlite3_step(stmt);
        VERIFY_SQLITE(sqlite3_finalize(stmt));
        if (res != SQLITE_DONE)
        {
            sqlite3_exec(gRDDB, "ROLLBACK", NULL, NULL, NULL);
            return res;
        }
    }

    VERIFY_SQLITE(sqlite3_exec(gRDDB, "COMMIT", NULL, NULL, NULL));
    res = SQLITE_OK;

    return res;
}

static int storeInterfaceType(char **interfaceTypes, size_t size, sqlite3_int64 rowid)
{
    int res = 1;
    VERIFY_SQLITE(sqlite3_exec(gRDDB, "BEGIN TRANSACTION", NULL, NULL, NULL));

    const char *deleteIF = "DELETE FROM RD_LINK_IF WHERE LINK_ID=@id";
    const char *insertIF = "INSERT INTO RD_LINK_IF VALUES(@interfaceType, @id)";
    sqlite3_stmt *stmt = 0;

    VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, deleteIF, strlen(deleteIF) + 1, &stmt, NULL));
    VERIFY_SQLITE(sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@id"), rowid));
    res = sqlite3_step(stmt);
    VERIFY_SQLITE(sqlite3_finalize(stmt));
    if (res != SQLITE_DONE)
    {
        sqlite3_exec(gRDDB, "ROLLBACK", NULL, NULL, NULL);
        return res;
    }

    for (size_t i = 0; i < size; i++)
    {
        VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, insertIF, strlen(insertIF) + 1, &stmt, NULL));
        if (interfaceTypes[i])
        {
            VERIFY_SQLITE(sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@interfaceType"),
                                            interfaceTypes[i], strlen(interfaceTypes[i]), SQLITE_STATIC));
            VERIFY_SQLITE(sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@id"), rowid));
        }
        res = sqlite3_step(stmt);
        VERIFY_SQLITE(sqlite3_finalize(stmt));
        if (res != SQLITE_DONE)
        {
            sqlite3_exec(gRDDB, "ROLLBACK", NULL, NULL, NULL);
            return res;
        }
    }

    VERIFY_SQLITE(sqlite3_exec(gRDDB, "COMMIT", NULL, NULL, NULL));
    res = SQLITE_OK;
    return res;
}

static int storeEndpoints(OCRepPayload **eps, size_t size, sqlite3_int64 rowid)
{
    int res = 1;
    VERIFY_SQLITE(sqlite3_exec(gRDDB, "BEGIN TRANSACTION", NULL, NULL, NULL));

    const char *deleteEp = "DELETE FROM RD_LINK_EP WHERE LINK_ID=@id";
    const char *insertEp = "INSERT INTO RD_LINK_EP VALUES(@ep, @pri, @id)";
    sqlite3_stmt *stmt = 0;

    VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, deleteEp, strlen(deleteEp) + 1, &stmt, NULL));
    VERIFY_SQLITE(sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@id"), rowid));
    res = sqlite3_step(stmt);
    VERIFY_SQLITE(sqlite3_finalize(stmt));
    if (res != SQLITE_DONE)
    {
        sqlite3_exec(gRDDB, "ROLLBACK", NULL, NULL, NULL);
        return res;
    }

    for (size_t i = 0; i < size; i++)
    {
        VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, insertEp, strlen(insertEp) + 1, &stmt, NULL));
        char *ep;
        if (OCRepPayloadGetPropString(eps[i], OC_RSRVD_ENDPOINT, &ep))
        {
            VERIFY_SQLITE(sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@ep"),
                                            ep, strlen(ep), SQLITE_STATIC));
        }
        sqlite3_int64 pri;
        if (OCRepPayloadGetPropInt(eps[i], OC_RSRVD_PRIORITY, (int64_t *) &pri))
        {
            VERIFY_SQLITE(sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@pri"), pri));
        }
        VERIFY_SQLITE(sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@id"), rowid));
        res = sqlite3_step(stmt);
        VERIFY_SQLITE(sqlite3_finalize(stmt));
        if (res != SQLITE_DONE)
        {
            sqlite3_exec(gRDDB, "ROLLBACK", NULL, NULL, NULL);
            return res;
        }
        OICFree(ep);
    }

    VERIFY_SQLITE(sqlite3_exec(gRDDB, "COMMIT", NULL, NULL, NULL));
    res = SQLITE_OK;
    return res;
}

static int storeLinkPayload(OCRepPayload *rdPayload, sqlite3_int64 rowid)
{
    int res = 1 ;
    size_t j = 0;

    /*
     * Iterate over the properties manually rather than OCRepPayloadGetPropObjectArray to avoid
     * the clone since we want to insert the 'ins' values into the payload.
     */
    OCRepPayloadValue *links;
    for (links = rdPayload->values; links; links = links->next)
    {
        if (0 == strcmp(links->name, OC_RSRVD_LINKS))
        {
            if (links->type != OCREP_PROP_ARRAY || links->arr.type != OCREP_PROP_OBJECT)
            {
                links = NULL;
            }
            break;
        }
    }
    if (links != NULL)
    {
        const char *insertDeviceLLList = "INSERT OR IGNORE INTO RD_DEVICE_LINK_LIST (ins, href, DEVICE_ID) "
            "VALUES((SELECT ins FROM RD_DEVICE_LINK_LIST WHERE DEVICE_ID=@id AND href=@uri),@uri,@id)";
        const char *updateDeviceLLList = "UPDATE RD_DEVICE_LINK_LIST SET anchor=@anchor,bm=@bm "
            "WHERE DEVICE_ID=@id AND href=@uri";

        sqlite3_stmt *stmt = 0;

        for (size_t i = 0; i < links->arr.dimensions[0]; i++)
        {
            VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, insertDeviceLLList, strlen(insertDeviceLLList) + 1, &stmt, NULL));
            VERIFY_SQLITE(sqlite3_exec(gRDDB, "BEGIN TRANSACTION", NULL, NULL, NULL));

            OCRepPayload *link = links->arr.objArray[i];
            VERIFY_SQLITE(sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@id"), rowid));
            char *uri = NULL;
            if (OCRepPayloadGetPropString(link, OC_RSRVD_HREF, &uri))
            {
                VERIFY_SQLITE(sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@uri"),
                                                uri, strlen(uri), SQLITE_STATIC));
            }
            if (sqlite3_step(stmt) != SQLITE_DONE)
            {
                sqlite3_exec(gRDDB, "ROLLBACK", NULL, NULL, NULL);
                sqlite3_finalize(stmt);
                return res;
            }
            VERIFY_SQLITE(sqlite3_finalize(stmt));

            VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, updateDeviceLLList, strlen(updateDeviceLLList) + 1,
                                             &stmt, NULL));
            VERIFY_SQLITE(sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@id"), rowid));
            if (uri)
            {
                VERIFY_SQLITE(sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@uri"),
                                                uri, strlen(uri), SQLITE_STATIC));
            }
            char *anchor;
            if (OCRepPayloadGetPropString(link, OC_RSRVD_URI, &anchor))
            {
                VERIFY_SQLITE(sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@anchor"),
                                                anchor, strlen(anchor), SQLITE_STATIC));
            }
            OCRepPayload *p = NULL;
            if (OCRepPayloadGetPropObject(link, OC_RSRVD_POLICY, &p))
            {
                sqlite3_int64 bm = 0;
                if (OCRepPayloadGetPropInt(p, OC_RSRVD_BITMAP, (int64_t *) &bm))
                {
                    VERIFY_SQLITE(sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@bm"), bm));
                }
            }
            if (sqlite3_step(stmt) != SQLITE_DONE)
            {
                sqlite3_exec(gRDDB, "ROLLBACK", NULL, NULL, NULL);
                sqlite3_finalize(stmt);
                return res;
            }
            VERIFY_SQLITE(sqlite3_finalize(stmt));
            VERIFY_SQLITE(sqlite3_exec(gRDDB, "COMMIT", NULL, NULL, NULL));

            char **rt = NULL;
            size_t rtDim[MAX_REP_ARRAY_DEPTH] = {0};
            char **itf = NULL;
            size_t itfDim[MAX_REP_ARRAY_DEPTH] = {0};
            OCRepPayload** eps = NULL;
            size_t epsDim[MAX_REP_ARRAY_DEPTH] = {0};
            const char *input = "SELECT ins FROM RD_DEVICE_LINK_LIST WHERE DEVICE_ID=@id AND href=@uri";
            VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, input, strlen(input) + 1, &stmt, NULL));
            VERIFY_SQLITE(sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@id"), rowid));
            if (uri)
            {
                VERIFY_SQLITE(sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@uri"),
                                                uri, strlen(uri), SQLITE_STATIC));
            }
            res = sqlite3_step(stmt);
            if (res == SQLITE_ROW || res == SQLITE_DONE)
            {
                sqlite3_int64 ins = sqlite3_column_int64(stmt, 0);
                VERIFY_SQLITE(sqlite3_finalize(stmt));
                if (!OCRepPayloadSetPropInt(link, OC_RSRVD_INS, ins))
                {
                    OIC_LOG_V(ERROR, TAG, "Error setting 'ins' value");
                    return OC_STACK_ERROR;
                }
                OCRepPayloadGetStringArray(link, OC_RSRVD_RESOURCE_TYPE, &rt, rtDim);
                OCRepPayloadGetStringArray(link, OC_RSRVD_INTERFACE, &itf, itfDim);
                OCRepPayloadGetPropObjectArray(link, OC_RSRVD_ENDPOINTS, &eps, epsDim);
                VERIFY_SQLITE(storeResourceType(rt, rtDim[0], ins));
                VERIFY_SQLITE(storeInterfaceType(itf, itfDim[0], ins));
                VERIFY_SQLITE(storeEndpoints(eps, epsDim[0], ins));
            }
            else
            {
                VERIFY_SQLITE(sqlite3_finalize(stmt));
            }

            OICFree(uri);
            OCPayloadDestroy((OCPayload *)p);
            for (j = 0; j < rtDim[0]; j++)
            {
                OICFree(rt[j]);
            }
            OICFree(rt);
            for (j = 0; j < itfDim[0]; j++)
            {
                OICFree(itf[j]);
            }
            OICFree(itf);
        }

        res = SQLITE_OK;
    }
    return res;
}

OCStackResult OCRDDatabaseStoreResources(OCRepPayload *payload)
{
    CHECK_DATABASE_INIT;

    char *deviceId = NULL;
    OCRepPayloadGetPropString(payload, OC_RSRVD_DEVICE_ID, &deviceId);
    sqlite3_int64 ttl = 0;
    OCRepPayloadGetPropInt(payload, OC_RSRVD_DEVICE_TTL, (int64_t *) &ttl);

    /* INSERT OR IGNORE then UPDATE to update or insert the row without triggering the cascading deletes */
    VERIFY_SQLITE(sqlite3_exec(gRDDB, "BEGIN TRANSACTION", NULL, NULL, NULL));

    sqlite3_stmt *stmt = 0;
    const char *insertDeviceList = "INSERT OR IGNORE INTO RD_DEVICE_LIST (ID, di, ttl) "
        "VALUES ((SELECT ID FROM RD_DEVICE_LIST WHERE di=@deviceId), @deviceId, @ttl)";
    VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, insertDeviceList, strlen(insertDeviceList) + 1, &stmt, NULL));

    if (deviceId)
    {
        VERIFY_SQLITE(sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@deviceId"),
                                        deviceId, strlen(deviceId), SQLITE_STATIC));
    }
    if (ttl)
    {
        VERIFY_SQLITE(sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@ttl"), ttl));
    }
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_exec(gRDDB, "ROLLBACK", NULL, NULL, NULL);
        sqlite3_finalize(stmt);
        return OC_STACK_ERROR;
    }
    VERIFY_SQLITE(sqlite3_finalize(stmt));

    const char *updateDeviceList = "UPDATE RD_DEVICE_LIST SET ttl=@ttl WHERE di=@deviceId";
    VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, updateDeviceList, strlen(updateDeviceList) + 1,
                                     &stmt, NULL));
    if (deviceId)
    {
        VERIFY_SQLITE(sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@deviceId"),
                                        deviceId, strlen(deviceId), SQLITE_STATIC));
    }
    if (ttl)
    {
        VERIFY_SQLITE(sqlite3_bind_int64(stmt, sqlite3_bind_parameter_index(stmt, "@ttl"), ttl));
    }
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_exec(gRDDB, "ROLLBACK", NULL, NULL, NULL);
        sqlite3_finalize(stmt);
        return OC_STACK_ERROR;
    }
    VERIFY_SQLITE(sqlite3_finalize(stmt));

    VERIFY_SQLITE(sqlite3_exec(gRDDB, "COMMIT", NULL, NULL, NULL));

    /* Store the rest of the payload */
    const char *input = "SELECT ID FROM RD_DEVICE_LIST WHERE di=@deviceId";
    VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, input, strlen(input) + 1, &stmt, NULL));
    if (deviceId)
    {
        VERIFY_SQLITE(sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@deviceId"),
                                        deviceId, strlen(deviceId), SQLITE_STATIC));
    }
    int res = sqlite3_step(stmt);
    if (res == SQLITE_ROW || res == SQLITE_DONE)
    {
        sqlite3_int64 rowid = sqlite3_column_int64(stmt, 0);
        VERIFY_SQLITE(sqlite3_finalize(stmt));
        VERIFY_SQLITE(storeLinkPayload(payload, rowid));
    }
    else
    {
        VERIFY_SQLITE(sqlite3_finalize(stmt));
    }

    OICFree(deviceId);
    return OC_STACK_OK;
}

OCStackResult OCRDDatabaseDeleteResources(const char *deviceId, const uint8_t *instanceIds, uint8_t nInstanceIds)
{
    CHECK_DATABASE_INIT;

    VERIFY_SQLITE(sqlite3_exec(gRDDB, "BEGIN TRANSACTION", NULL, NULL, NULL));
    sqlite3_stmt *stmt = 0;
    if (!instanceIds || !nInstanceIds)
    {
        char *delDevice = "DELETE FROM RD_DEVICE_LIST WHERE di=@deviceId";
        VERIFY_SQLITE(sqlite3_prepare_v2(gRDDB, delDevice, strlen(delDevice) + 1, &stmt, NULL));

        VERIFY_SQLITE(sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@deviceId"),
                                        deviceId, strlen(deviceId), SQLITE_STATIC));
    }
    else
    {
        const char pre[] = "DELETE FROM RD_DEVICE_LINK_LIST "
            "WHERE ins IN ("
            "SELECT RD_DEVICE_LINK_LIST.ins FROM RD_DEVICE_LINK_LIST "
            "INNER JOIN RD_DEVICE_LIST ON RD_DEVICE_LINK_LIST.DEVICE_ID=RD_DEVICE_LIST.ID "
            "WHERE RD_DEVICE_LIST.di=@deviceId AND RD_DEVICE_LINK_LIST.ins IN (";
        size_t inLen = nInstanceIds + (nInstanceIds - 1);
        const char post[] = "))";
        size_t len = sizeof(pre) + inLen + sizeof(post);
        char *delResource = OICCalloc(len, 1);
        if (!delResource)
        {
            OIC_LOG(ERROR, TAG, "SQL query is NULL");
            return OC_STACK_ERROR;
        }
        OICStrcat(delResource, len, pre);
        OICStrcat(delResource, len, "?");
        for (uint8_t i = 1; i < nInstanceIds; ++i)
        {
            OICStrcat(delResource, len, ",?");
        }
        OICStrcat(delResource, len, post);
        int prepareResult = sqlite3_prepare_v2(gRDDB, delResource, strlen(delResource) + 1, &stmt, NULL);
        OICFree(delResource);
        VERIFY_SQLITE(prepareResult);

        VERIFY_SQLITE(sqlite3_bind_text(stmt, sqlite3_bind_parameter_index(stmt, "@deviceId"),
                                        deviceId, strlen(deviceId), SQLITE_STATIC));
        assert(sqlite3_bind_parameter_index(stmt, "@deviceId") == 1);
        for (uint8_t i = 0; i < nInstanceIds; ++i)
        {
            VERIFY_SQLITE(sqlite3_bind_int(stmt, 2 + i, instanceIds[i]));
        }
    }
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_exec(gRDDB, "ROLLBACK", NULL, NULL, NULL);
        sqlite3_finalize(stmt);
        return OC_STACK_ERROR;
    }
    VERIFY_SQLITE(sqlite3_finalize(stmt));
    VERIFY_SQLITE(sqlite3_exec(gRDDB, "COMMIT", NULL, NULL, NULL));

    return OC_STACK_OK;
}

#endif
