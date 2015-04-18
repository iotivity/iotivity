//******************************************************************
//
// Copyright 2015 Intel Mobile Communications GmbH All Rights Reserved.
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

#include "ocstack.h"
#include "logger.h"
#include "ocmalloc.h"
#include "cJSON.h"
#include "cainterface.h"
#include "securityresourcemanager.h"
#include "resourcemanager.h"
#include "srmresourcestrings.h"
#include <stdlib.h>
#include <string.h>

#define TAG  PCF("SRM-PSI")

/* TODO Consolidate all macros in one file */
#define VERIFY_SUCCESS(op, logLevel) { if (!(op)) \
            {OC_LOG((logLevel), TAG, PCF(#op " failed!!")); goto exit;} }

#define VERIFY_NON_NULL(arg, logLevel) { if (!(arg)) { OC_LOG((logLevel), \
             TAG, PCF(#arg " is NULL")); goto exit; } }


#ifdef WITH_ARDUINO
/* Mega 2560 have 4 KB of EEPROM. */
const size_t MAX_DB_FILE_SIZE = 1023;
#else
const size_t MAX_DB_FILE_SIZE = 4095;
#endif

/**
 * Reads the Secure Virtual Database from PS into dynamically allocated
 * memory buffer.
 *
 * @note Caller of this method MUST use OCFree() method to release memory
 *       referenced by return value.
 *
 * @retval  reference to memory buffer containing SVR database.
 */
char * GetSVRDatabase()
{
    char * jsonStr = NULL;
    FILE * fp = NULL;
    OCPersistentStorage* ps = SRMGetPersistentStorageHandler();

    /* TODO Do we need a GetFileSize API ? */
    if (ps && ps->open)
    {
        /* TODO Build consensus on location of SRM database file on Ubuntu */
        fp = ps->open(SVR_DB_FILE_NAME, "r");
        if (fp)
        {
            jsonStr = (char*)OCMalloc(MAX_DB_FILE_SIZE + 1);
            VERIFY_NON_NULL(jsonStr, FATAL);
            size_t bytesRead = ps->read(jsonStr, 1, MAX_DB_FILE_SIZE, fp);
            jsonStr[bytesRead] = '\0';

            OC_LOG_V(INFO, TAG, PCF("Read %d bytes from SVR database file"), bytesRead);
            ps->close(fp);
            fp = NULL;
        }
        else
        {
            OC_LOG (ERROR, TAG, PCF("Unable to open SVR database file!! "));
        }
    }

exit:
    if (ps && fp)
    {
        ps->close(fp);
    }
    return jsonStr;
}


/**
 * This method can be used by a Secure Virtual Resource entity handler
 * to update SVR database.
 *
 * @param rsrcName - string denoting the SVR name ("acl", "cred", "pstat" etc).
 * @param jsonObj - JSON object containing the SVR contents.
 *
 * @retval  OC_STACK_OK for Success, otherwise some error value
 */
OCStackResult UpdateSVRDatabase(const char* rsrcName, cJSON* jsonObj)
{
    /* Read SVR database from PS */
    /* Use cJSON_Parse to parse the SVR database */
    /* Use cJSON_ReplaceItemInObject to replace the 'rsrcName' passed */
    /* Use cJSON_PrintUnformatted to write back the SVR database in PS */

    return OC_STACK_NOTIMPL;
}
