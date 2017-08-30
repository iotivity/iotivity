//******************************************************************
//
// Copyright 2017 Intel Mobile Communications GmbH All Rights Reserved.
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
//


#include <stdint.h>
#include <pthread.h>

#ifndef __HUE_FILE_H__
#define __HUE_FILE_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * defines go here
 ******************************************************************************/

#define     MAX_CLIENTS                 5
#define     MAX_STRING                  512
#define     HUE_AUTHORIZATION_FILE     "hue_auth_json.txt"

typedef struct _hue_file
{
    char macAddrString[MAX_STRING];
    char clientID[MAX_STRING];
} hueFile;

/**
 * This API has a create/initialize behavior, it reads all of the
 * bridge clients contained in a given file into the cached file bridge list.  The
 * assumption is that this only happens only once when the plugin is loaded.   In
 * other words (to stess this point); this function should not be called periodically
 * for the first implementation.
 *  It has the form as follows:
 *
 * {"id":"001788fffe155cbb","username":"A234532444"}
 *
 *  @return true if file was read else false
 */
bool readAuthorizedBridgeFile();

/**
 * One can add a bridge file context however one must make sure that the bridge
 * context is not already in the list.  There should never be two identical
 * contexts in the list.  One can use the fileFindBridge API to the file
 * bridge objects in the list prior to adding a bridge client.
 *
 * @NOTE: a mac address + client ID constitutes uniqueness of an entry.
 *
 * @param[in] macId    MAC Id of the bridge
 * @param[in] clientId ClientId of the bridge
 *
 * @return TRUE if added else FALSE
 */

bool addAuthorizedBridge(const char *macId, const char *clientId);

/*
 * Find the file bridge records with the matching mac address and client ID. If
 * either input is set to NULL, then this routine will return the very first
 * record where there is a match.  It is not OK to specify both mac address and
 * client ID as being NULL.
 *
 * @param[in] macAddrString   Mac id of the bridge
 * @param[in] clientID        Clientid of the bridge
 * @param[in] bridgeCtx
 *
 * @return true bridge file record was found else false
 */
bool findAuthorizedBridge(const char *macAddrString, const char *clientID, hueFile &bridgeCtx);

void clearBridgeDetails();

/**
 * Collect Clients strings in an array of strings from records in the file list.
 * The client array is really a flattened out two dimensional MAX_CLIENTS, MAX_STRING
 *
 * @param[in] macAddrString    Mac id of the bridge
 * @param[in] clientArray      Array of Clientid's of the bridge
 * @param[in] numClients       no of clients
 *
 * @return true if client found else false
 */
bool collectAuthorizedClients(const char *macAddrString, char *clientArray, uint32_t *numClients);

#ifdef __cplusplus
}
#endif

#endif /* __HUE_FILE_H__ */
