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


#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <inttypes.h>

#include <unistd.h>

#include <termios.h>
#include <string.h>
#include <errno.h>

#include "oic_string.h"
#include "oic_malloc.h"
#include "logger.h"

#include "telegesis_wrapper.h"
#include "twtypes.h"

#define TAG PCF("telegesiswrapper")     // Module Name
#define ARRAY_LENGTH            (100)

//-----------------------------------------------------------------------------
// Private internal function prototypes
//-----------------------------------------------------------------------------

static TWResultCode IssueATCommand(const char* command, uint8_t timeout,
                                   uint64_t* atErrorCode,
                                   char* responseArray[], char* promptArray[],
                                   uint8_t* responseCount, uint8_t* promptCount);

static TWResultCode HandleATResponse(char* responseArray[], int count);

static TWResultCode Reset();
static TWResultCode GetLocalEUI();
static TWResultCode CreatePAN();
static TWResultCode EnableJoin(bool isKeyEncrypted, uint8_t* outDeviceJoinedCount);
static TWResultCode FindMatchNodes(uint8_t* outDeviceMatchedCount);
static TWResultCode FindClusters(char nodeId[], char endpoint[]);
static TWResultCode TelNetworkInfoHandler(int count, char* tokens[]);
static TWResultCode TelJpanHandler(int count, char* tokens[]);
static TWResultCode TelEndDeviceJoinHandler(int count, char* tokens[]);
static TWResultCode TelMatchDescHandler(int count, char* tokens[]);
static TWResultCode TelAddressResponseHandler(int count, char* tokens[]);
static TWResultCode TelSimpleDescHandler(int count, char* tokens[]);
static TWResultCode TelSimpleDescInClusterHandler(int count, char* tokens[]);
static TWResultCode TelWriteAttrHandler(int count, char* tokens[]);
static TWResultCode TelReadAttrHandler(int count, char* tokens[]);
static TWResultCode TelReadAttrHandlerTemperature(int count, char* tokens[]);
static TWResultCode TelZCLDefaultResponseHandler(int count, char* tokens[]);
static TWResultCode TelSwitchDoorLockStateHandler(int count, char* tokens[]);
static TWResultCode TelZoneEnrollRequestHandler(int count, char* tokens[]);
static TWResultCode TelEnrolledHandler(int count, char* tokens[]);
static TWResultCode TelZoneStatusHandler(int count, char* tokens[]);

static TWResultCode OpenPort(int * fd);
static TWResultCode ClosePort(int fd);

static TWResultCode WriteBuffer(const char * command);
static TWResultCode ReadBuffer(uint64_t * atErrorCode,
                               char * responseArray[],
                               uint8_t * count,
                               const char * command,
                               uint8_t timeout);

static TWResultCode AsciiHexToValue(char* hexString, int length, uint64_t* value);
static int AsciiToHex(char c);
static int Tokenize(const char *input, const char* delimiters, char* output[]);

static int SetTerminalInfo(int fd, int speed, int parity, int shouldBlock);

static void CleanArray(char* responseArray[], char* promptArray[],
                       uint8_t responseCount, uint8_t promptCount);
static void PrintArray(char* responseArray[], char* promptArray[],
                       uint8_t responseCount, uint8_t promptCount);

static void PrintTWDevice(TWDevice* device);
static void PrintTWDeviceList(TWDeviceList* list);
static void DeallocateTWDeviceList();

//-----------------------------------------------------------------------------
// Private variables
//-----------------------------------------------------------------------------

static TWATResultHandlerPair g_TWATResultHandlerPairArray[] =
{
    {"+N=",         TelNetworkInfoHandler},
    {"JPAN:",       TelJpanHandler},
    {"RFD:",        TelEndDeviceJoinHandler},       //e.g SmartThings Open/Closed Sensor
    {"FFD:",        TelEndDeviceJoinHandler},       //e.g SmartThings Plug
    {"SED:",        TelEndDeviceJoinHandler},
    {"ZED:",        TelEndDeviceJoinHandler},
    {"MatchDesc:",  TelMatchDescHandler},
    {"SimpleDesc:", TelSimpleDescHandler},
    {"InCluster:",  TelSimpleDescInClusterHandler},
    {"WRITEATTR:",  TelWriteAttrHandler},
    {"RESPATTR:",   TelReadAttrHandler},
    {"TEMPERATURE:",TelReadAttrHandlerTemperature},
    {"DFTREP",      TelZCLDefaultResponseHandler},
    {"DRLOCRSP:",   TelSwitchDoorLockStateHandler},
    {"DRUNLOCKRSP:",TelSwitchDoorLockStateHandler},
    {"ZENROLLREQ:", TelZoneEnrollRequestHandler},
    {"ENROLLED:",   TelEnrolledHandler},
    {"ZONESTATUS:", TelZoneStatusHandler},
    {"AddrResp:",   TelAddressResponseHandler},
    {"Unknown:",    TelNetworkInfoHandler}
};

static TWEndControlMap g_TWEndMessageMap[] =
{
    {"OK",      TW_ENDCONTROL_OK},
    {"ERROR:",  TW_ENDCONTROL_ERROR},
    {"ACK:",    TW_ENDCONTROL_ACK},
    {"SEQ:",    TW_ENDCONTROL_SEQ}
};

//TODO: Take care of all global variables
static const char* g_port = NULL;
static int g_fd = 0;

static char g_LocalEUI[SIZE_EUI] = "";
static char g_WIPRemoteEUI[SIZE_EUI] = "";
static char g_WIPRemoteNodeId[SIZE_NODEID] = "";

static TWStatus g_ZigBeeStatus = {ZB_STATE_UNKNOWN,0,0,NULL,0};
static TWDeviceList* g_FoundMatchedDeviceList = NULL;
static TWDevice* g_WIPDevice = NULL;

static TWDeviceFoundCallback g_DeviceFoundCallback = NULL;
static TWEnrollmentSucceedCallback g_EnrollmentSucceedCallback = NULL;
static TWDeviceStatusUpdateCallback g_DeviceStatusUpdateCallback = NULL;

/*****************************************************************************/
/*                                                                           */
/* Public functions                                                          */
/*                                                                           */
/*****************************************************************************/
OCStackResult TWInitialize(const char* deviceDevPath)
{
    OC_LOG_V(INFO, TAG, "Enter TWInitializeZigBee().");

    OCStackResult ret = OC_STACK_ERROR;
    TWResultCode twCode = TW_RESULT_ERROR;

    g_port = deviceDevPath;

    OC_LOG_V(INFO, TAG, "Attempt to open %s", deviceDevPath);
    twCode = OpenPort(&g_fd);

    if (twCode != TW_RESULT_OK)
    {
        OC_LOG_V(ERROR, TAG, "Failed to open %s because of error: %d", deviceDevPath, ret);
        ret = OC_STACK_ERROR;
    }
    else
    {
        twCode = TW_RESULT_ERROR;

        bool continueInit = true;
        bool wantReset = false;

        if (wantReset)
        {
            twCode = Reset();
            if (twCode == TW_RESULT_OK)
            {
                OC_LOG_V(INFO, TAG, "ZigBee Initialization - Reset - OK.");
                continueInit = true;
            }
            else
            {
                OC_LOG_V(INFO, TAG, "ZigBee Initialization - Reset - FAILED.");
                continueInit = false;
            }
        }

        if (continueInit)
        {
            twCode = CreatePAN();
            if (twCode == TW_RESULT_OK)
            {
                OC_LOG_V(INFO, TAG, "CreatePan OK.");

                twCode = GetLocalEUI();
                if (twCode == TW_RESULT_OK)
                {
                    OC_LOG_V(INFO, TAG, "GetLocalEUI OK.");
                    OC_LOG_V(INFO, TAG, "ZigBee Initialization - DONE.");
                    g_ZigBeeStatus.state = ZB_STATE_INIT;
                    ret = OC_STACK_OK;
                }
                else
                {
                    OC_LOG_V(ERROR, TAG, "GetLocalEUI FAILED.");
                    OC_LOG_V(ERROR, TAG, "ZigBee Initialization - FAILED - MUST STOP NOW.");
                             g_ZigBeeStatus.state = ZB_STATE_UNKNOWN;
                     ret = OC_STACK_ERROR;
                }
            }
            else
            {
                OC_LOG_V(ERROR, TAG, "CreatePan FAILED. Result: %d", twCode);
                OC_LOG_V(ERROR, TAG, "ZigBee Initialization - FAILED - MUST STOP NOW.");
                g_ZigBeeStatus.state = ZB_STATE_UNKNOWN;
                ret = OC_STACK_ERROR;
            }
        }
        else
        {
            OC_LOG_V(ERROR, TAG, "ZigBee Initialization - FAILED - MUST STOP NOW.");
            ret = OC_STACK_ERROR;
        }
    }

    OC_LOG_V(INFO, TAG, "Leave TWInitializeZigBee().");
    return ret;
}

OCStackResult TWDiscover()
{
    OC_LOG_V(INFO, TAG, "Enter TWDiscover().");

    OCStackResult ret = OC_STACK_ERROR;
    TWResultCode twRet = TW_RESULT_ERROR;
    uint8_t deviceJoinedCount = 0;

    if (g_DeviceFoundCallback == NULL)
    {
        OC_LOG_V(ERROR, TAG, "Required TWDeviceFoundCallback.");
        ret = OC_STACK_ERROR;
    }
    else
    {
        twRet = EnableJoin(false, &deviceJoinedCount);

        if (twRet == TW_RESULT_OK)
        {
            OC_LOG_V(INFO, TAG, "Number of devices joined:%d.", deviceJoinedCount);
            uint8_t nodeMatchedCount = 0;
            twRet = FindMatchNodes(&nodeMatchedCount);
            if (twRet == TW_RESULT_FOUND_NO_MATCHED_DEVICE)
            {
                ret = OC_STACK_OK;
            }
            else if (twRet == TW_RESULT_FOUND_MATCHED_DEVICES)
            {
                ret = OC_STACK_OK;
                int i = 0;
                for (; i < g_FoundMatchedDeviceList->count; i++)
                {
                    g_WIPDevice = &g_FoundMatchedDeviceList->deviceList[i];
                    twRet = FindClusters(g_WIPDevice->nodeId,
                            g_WIPDevice->endpointOfInterest->endpointId);

                    if (twRet == TW_RESULT_HAS_CLUSTERS)
                    {
                        OC_LOG_V(INFO, TAG, "Attempt to invoke a callback.");
                        g_DeviceFoundCallback(g_WIPDevice);
                    }
                    else
                    {
                        OC_LOG_V(ERROR, TAG,
                                 "ERROR finding cluster for NodeID=%s,EUI=%s,EP=%s.",
                                 g_WIPDevice->nodeId,
                                 g_WIPDevice->eui,
                                 g_WIPDevice->endpointOfInterest->endpointId);
                        ret = OC_STACK_ERROR;
                        break;
                    }
                    g_WIPDevice = NULL;
                }
            }
            else
            {
                OC_LOG_V(ERROR, TAG, "Find matched nodes.");
                ret = OC_STACK_ERROR;
            }
        }
        else
        {
            OC_LOG_V(ERROR, TAG, "Enable joining.");
            ret = OC_STACK_ERROR;
        }

        //Done with wip device.
        g_WIPDevice = NULL;

        //deallocate the list of found devices.
        //so that it's cleaned for the next TWDiscover() call.
        DeallocateTWDeviceList();
    }

    OC_LOG_V(INFO, TAG, "Leave TWDiscover().");
    return ret;
}

OCStackResult TWSetAttribute(char* extendedUniqueId, char* nodeId, char* endpointId,
                             char* clusterId, char* attributeId, char* attributeType,
                             char* newValue)
{
    //Ask:  AT+WRITEATR:5DA7,01,0,0003,0000,21,01

    OC_LOG_V(INFO, TAG, "Enter TWSetAttribute().");

    (void)extendedUniqueId;

    OCStackResult ret = OC_STACK_ERROR;
    TWResultCode twRet = TW_RESULT_ERROR;

    int size =  strlen(AT_CMD_WRITE_ATR) + strlen(nodeId) +
                SEPARATOR_LENGTH + strlen(endpointId) +
                SEPARATOR_LENGTH + strlen(SENDMODE) +
                SEPARATOR_LENGTH + strlen(clusterId) +
                SEPARATOR_LENGTH + strlen(attributeId) +
                SEPARATOR_LENGTH + strlen(attributeType) +
                SEPARATOR_LENGTH + strlen(newValue) + 1;

    char* cmdString = (char*)OICMalloc(size * sizeof(char));
    if (cmdString != NULL)
    {
        snprintf(cmdString, size, "%s%s,%s,%s,%s,%s,%s,%s",
                 AT_CMD_WRITE_ATR, nodeId, endpointId, SENDMODE,
                 clusterId, attributeId, attributeType, newValue);

        char* responseArray[ARRAY_LENGTH] = {};
        uint8_t responseCount = 0;

        char* promptArray[ARRAY_LENGTH] = {};
        uint8_t promptCount = 0;

        uint64_t atErrorCode = 0;

        OC_LOG_V(INFO, TAG, "Issued Command: %s.", cmdString);
        twRet = IssueATCommand(cmdString, TIME_OUT_05_SECONDS,
                               &atErrorCode,
                               responseArray, promptArray,
                               &responseCount, &promptCount);

        if (twRet != TW_RESULT_OK)
        {
            OC_LOG_V(ERROR, TAG, "Failed to write: %s", cmdString);
            ret = OC_STACK_ERROR;
        }
        else
        {
            if (atErrorCode != AT_ERROR_EVERYTHING_OK)
            {
                OC_LOG_V(ERROR, TAG,
                         "Write to a remote attribute - AT_ERROR: %" PRId64 "", atErrorCode);
                ret = OC_STACK_ERROR;
            }
            else
            {
                OC_LOG_V(INFO, TAG, "Issue AT Command %s returned OK.", cmdString);
                PrintArray(responseArray, promptArray, responseCount, promptCount);

                //IMPORTANT NOTE:
                //      Although responseArray is not NULL sometimes,
                //      do not use it to check for prompt.
                //      WRITEATTR prompt is stored in promptArray.
                if (promptCount > 0)
                {
                    twRet = HandleATResponse(promptArray, promptCount);
                    if (twRet == TW_RESULT_OK)
                    {
                        OC_LOG_V(INFO, TAG, "Set attribute value received a succeed prompt.");
                        ret = OC_STACK_OK;
                    }
                    else
                    {
                        OC_LOG_V(ERROR, TAG, "Set attribute value received an error prompt.");
                        ret = OC_STACK_ERROR;
                    }
                }
                else
                {
                    OC_LOG_V(ERROR, TAG, "Why didn't it receive WRITEATTR prompt?");
                    ret = OC_STACK_ERROR;
                }
            }
        }
        CleanArray(responseArray, promptArray, responseCount, promptCount);
        OICFree(cmdString);
    }

    OC_LOG_V(INFO, TAG, "Leave TWSetAttribute().");
    return ret;
}

OCStackResult TWGetAttribute(char* extendedUniqueId, char* nodeId, char* endpointId,
                             char* clusterId, char* attributeId,
                             char** outValue, uint8_t* outValueLength)
{
    //Ask:  AT+READATR:FE5A,01,0,0402,0002

    OC_LOG_V(INFO, TAG, "Enter TWGetAttribute().");

    (void)extendedUniqueId;

    OCStackResult ret = OC_STACK_ERROR;
    TWResultCode twRet = TW_RESULT_ERROR;

    int size =  strlen(AT_CMD_READ_ATR) + strlen(nodeId) +
                SEPARATOR_LENGTH + strlen(endpointId) +
                SEPARATOR_LENGTH + strlen(SENDMODE) +
                SEPARATOR_LENGTH + strlen(clusterId) +
                SEPARATOR_LENGTH + strlen(attributeId) + 1;

    char* cmdString = (char*)OICMalloc(size * sizeof(char));
    if (cmdString == NULL)
    {
        OC_LOG_V(ERROR, TAG, "No Memory");
        ret = OC_STACK_NO_MEMORY;
    }
    else
    {
        snprintf(cmdString, size, "%s%s%s%s%s%s%s%s%s%s",
                 AT_CMD_READ_ATR, nodeId,
                 SEPARATOR, endpointId,
                 SEPARATOR, SENDMODE,
                 SEPARATOR, clusterId,
                 SEPARATOR, attributeId);

        char* responseArray[ARRAY_LENGTH] = {};
        uint8_t responseCount = 0;

        char* promptArray[ARRAY_LENGTH] = {};
        uint8_t promptCount = 0;

        uint64_t atErrorCode = 0;

        OC_LOG_V(INFO, TAG, "Issued Command: %s.", cmdString);
        twRet = IssueATCommand(cmdString, TIME_OUT_05_SECONDS,
                               &atErrorCode,
                               responseArray, promptArray,
                               &responseCount, &promptCount);

        if (twRet != TW_RESULT_OK)
        {
            OC_LOG_V(ERROR, TAG, "Write %s", cmdString);
            ret = OC_STACK_ERROR;
        }
        else
        {
            if (atErrorCode != AT_ERROR_EVERYTHING_OK)
            {
                OC_LOG_V(ERROR, TAG,
                         "Read a remote attribute - AT_ERROR: %" PRId64 ".", atErrorCode);
                ret = OC_STACK_ERROR;
            }
            else
            {
                OC_LOG_V(INFO, TAG, "Issue AT Command %s returned OK.", cmdString);
                PrintArray(responseArray, promptArray, responseCount, promptCount);

                if (promptCount > 0)
                {
                    twRet = HandleATResponse(promptArray, promptCount);
                    if (twRet == TW_RESULT_REMOTE_ATTR_HAS_VALUE)
                    {
                        OC_LOG_V(INFO, TAG, "Get attribute value received a succeed prompt.");
                        int size = strlen(g_ZigBeeStatus.remoteAttributeValueRead) + 1;
                        *outValue = (char*)OICMalloc(sizeof(char) * size);
                        if (*outValue != NULL)
                        {
                            OICStrcpy(*outValue, size, g_ZigBeeStatus.remoteAttributeValueRead);
                            *outValueLength = g_ZigBeeStatus.remoteAtrributeValueReadLength;
                            ret = OC_STACK_OK;
                        }
                        else
                        {
                            ret = OC_STACK_NO_MEMORY;
                        }
                    }
                    else
                    {
                        OC_LOG_V(ERROR, TAG, "Get attribute value received an error prompt.");
                        ret = OC_STACK_ERROR;
                    }
                }
                else
                {
                    OC_LOG_V(ERROR, TAG, "Didn't receive READATTR prompt.");
                    ret = OC_STACK_ERROR;
                }
            }
        }
        OICFree(cmdString);
        CleanArray(responseArray, promptArray, responseCount, promptCount);
    }

    OC_LOG_V(INFO, TAG, "Leave TWGetAttribute().");

    return ret;
}

OCStackResult TWSwitchOnOff(char* nodeId, char* endpointId, char* newState)
{
    //AT+RONOFF:<Address>,<EP>,<SendMode>[,<ON/OFF>]
    //AT+RONOFF:9E2B,01,0,1
    //      OK
    //      DFTREP:9E2B,01,0006,01,00

    OC_LOG_V(INFO, TAG, "Enter TWSwitchOnOff().");

    OCStackResult ret = OC_STACK_ERROR;
    TWResultCode twRet = TW_RESULT_UNKNOWN;

    int size = 0;
    if (newState == NULL)
    {
        size =  strlen(AT_CMD_RUN_ON_OFF) + strlen(nodeId) +
                SEPARATOR_LENGTH + strlen(endpointId) +
                SEPARATOR_LENGTH + strlen(SENDMODE) + 1;
    }
    else
    {
        size =  strlen(AT_CMD_RUN_ON_OFF) + strlen(nodeId) +
                SEPARATOR_LENGTH + strlen(endpointId) +
                SEPARATOR_LENGTH + strlen(SENDMODE) +
                SEPARATOR_LENGTH + strlen(newState) + 1;
    }

    char* cmdString = (char*)OICMalloc(size * sizeof(char));
    if (cmdString == NULL)
    {
        ret = OC_STACK_NO_MEMORY;
    }
    else
    {
        snprintf(cmdString, size, "%s%s%s%s%s%s",
                 AT_CMD_RUN_ON_OFF, nodeId, SEPARATOR,
                 endpointId, SEPARATOR, SENDMODE);

        if (newState != NULL)
        {
            OICStrcat(cmdString, size, SEPARATOR);
            OICStrcat(cmdString, size, newState);
        }

        char* responseArray[ARRAY_LENGTH] = {};
        uint8_t responseCount = 0;

        char* promptArray[ARRAY_LENGTH] = {};
        uint8_t promptCount = 0;

        uint64_t atErrorCode = 0;
        twRet = IssueATCommand(cmdString, TIME_OUT_05_SECONDS,
                               &atErrorCode,
                               responseArray, promptArray,
                               &responseCount, &promptCount);

        if (twRet != TW_RESULT_OK)
        {
            OC_LOG_V(ERROR, TAG, "Write AT+RONOFF - FAILED.");
            ret = OC_STACK_ERROR;
        }
        else
        {
            if (atErrorCode == AT_ERROR_EVERYTHING_OK)
            {
                twRet = HandleATResponse(promptArray, promptCount);
                if (twRet == TW_RESULT_OK)
                {
                    ret = OC_STACK_OK;
                }
                else
                {
                    ret = OC_STACK_ERROR;
                }
            }
            else
            {
                 if (atErrorCode == AT_ERROR_MESSAGE_NOT_SENT_TO_TARGET_SUCCESSFULLY)
                 {
                     OC_LOG_V(ERROR, TAG,
                              "Switch to ON/OFF - FAILED - Send to the target not succeed.");
                 }
                 else
                 {
                     OC_LOG_V(ERROR, TAG, "Switch to ON/OFF - FAILED.");
                 }
                 ret = OC_STACK_ERROR;
            }
        }
        CleanArray(responseArray, promptArray, responseCount, promptCount);
        OICFree(cmdString);
    }
    OC_LOG_V(INFO, TAG, "Leave TWSwitchOnOff().");
    return ret;
}

OCStackResult TWMoveToLevel(char* nodeId, char* endpointId,
                            char* onOffState, char* level, char* transTime)
{
    //AT+LCMVTOLEV:<Address>,<EP>,<SendMode>,<ON/OFF>,<LevelValue>,<TransTime>

    OC_LOG_V(INFO, TAG, "Enter TWMoveToLevel().");

    OCStackResult ret = OC_STACK_ERROR;
    TWResultCode twRet = TW_RESULT_UNKNOWN;

    int size = 0;
    size =  strlen(AT_CMD_MOVE_TO_LEVEL) + strlen(nodeId) +
            SEPARATOR_LENGTH + strlen(endpointId) +
            SEPARATOR_LENGTH + strlen(SENDMODE) +
            SEPARATOR_LENGTH + strlen(onOffState) +
            SEPARATOR_LENGTH + strlen(level) +
            SEPARATOR_LENGTH + strlen(transTime) + 1;

    char* cmdString = (char*)OICMalloc(size * sizeof(char));
    if (cmdString == NULL)
    {
        ret = OC_STACK_NO_MEMORY;
    }
    else
    {
        snprintf(cmdString, size, "%s%s%s%s%s%s%s%s%s%s%s%s",
                 AT_CMD_MOVE_TO_LEVEL, nodeId,
                 SEPARATOR, endpointId,
                 SEPARATOR, SENDMODE,
                 SEPARATOR, onOffState,
                 SEPARATOR, level,
                 SEPARATOR, transTime);

        char* responseArray[ARRAY_LENGTH] = {};
        uint8_t responseCount = 0;

        char* promptArray[ARRAY_LENGTH] = {};
        uint8_t promptCount = 0;

        uint64_t atErrorCode = 0;
        twRet = IssueATCommand(cmdString, TIME_OUT_05_SECONDS,
                               &atErrorCode,
                               responseArray, promptArray,
                               &responseCount, &promptCount);

        if (twRet != TW_RESULT_OK)
        {
            OC_LOG_V(ERROR, TAG, "Write AT+RONOFF - FAILED.");
            ret = OC_STACK_ERROR;
        }
        else
        {
            if (atErrorCode == AT_ERROR_EVERYTHING_OK)
            {
                twRet = HandleATResponse(promptArray, promptCount);
                if (twRet == TW_RESULT_OK)
                {
                    ret = OC_STACK_OK;
                }
                else
                {
                    ret = OC_STACK_ERROR;
                }
            }
            else
            {
                 if (atErrorCode == AT_ERROR_MESSAGE_NOT_SENT_TO_TARGET_SUCCESSFULLY)
                 {
                     OC_LOG_V(ERROR, TAG,
                              "Turn To Level - FAILED - Send to the target not succeeded.");
                 }
                 else
                 {
                     OC_LOG_V(ERROR, TAG, "Turn To Level - FAILED.");
                 }
                 ret = OC_STACK_ERROR;
            }
        }

        CleanArray(responseArray, promptArray, responseCount, promptCount);
        OICFree(cmdString);
    }

    OC_LOG_V(INFO, TAG, "Leave TWMoveToLevel().");
    return ret;
}

OCStackResult TWSwitchDoorLockState(char* nodeId, char* endpointId, char* newState)
{
    //AT+DRLOCK:<Address>,<EP>,<SendMode>,<Lock/Unlock>

    OC_LOG_V(INFO, TAG, "Enter TWSwitchDoorLockState().");

    OCStackResult ret = OC_STACK_ERROR;
    TWResultCode twRet = TW_RESULT_UNKNOWN;

    int size = 0;
    size =  strlen(AT_CMD_DOOR_LOCK) + strlen(nodeId) +
            SEPARATOR_LENGTH + strlen(endpointId) +
            SEPARATOR_LENGTH + strlen(SENDMODE) +
            SEPARATOR_LENGTH + strlen(newState) + 1;

    char* cmdString = (char*)OICMalloc(size * sizeof(char));
    if (cmdString == NULL)
    {
        ret = OC_STACK_NO_MEMORY;
    }
    else
    {
        snprintf(cmdString, size, "%s%s%s%s%s%s%s%s",
                 AT_CMD_DOOR_LOCK, nodeId,
                 SEPARATOR, endpointId,
                 SEPARATOR, SENDMODE,
                 SEPARATOR, newState);

        char* responseArray[ARRAY_LENGTH] = {};
        uint8_t responseCount = 0;

        char* promptArray[ARRAY_LENGTH] = {};
        uint8_t promptCount = 0;

        uint64_t atErrorCode = 0;
        twRet = IssueATCommand(cmdString, TIME_OUT_05_SECONDS,
                               &atErrorCode,
                               responseArray, promptArray,
                               &responseCount, &promptCount);

        if (twRet != TW_RESULT_OK)
        {
            OC_LOG_V(ERROR, TAG, "Write AT+DRLOCK - FAILED.");
            ret = OC_STACK_ERROR;
        }
        else
        {
            if (atErrorCode == AT_ERROR_EVERYTHING_OK)
            {
                twRet = HandleATResponse(promptArray, promptCount);
                if (twRet == TW_RESULT_OK)
                {
                    ret = OC_STACK_OK;
                }
                else
                {
                    ret = OC_STACK_ERROR;
                }
            }
            else
            {
                if (atErrorCode == AT_ERROR_MESSAGE_NOT_SENT_TO_TARGET_SUCCESSFULLY)
                {
                    OC_LOG_V(ERROR, TAG,
                             "Switch to ON/OFF - FAILED - Send to the target not succeeded.");
                }
                else
                {
                    OC_LOG_V(ERROR, TAG, "Switch to ON/OFF - FAILED.");
                }
                ret = OC_STACK_ERROR;
            }
        }

        CleanArray(responseArray, promptArray, responseCount, promptCount);
        OICFree(cmdString);
    }

    OC_LOG_V(INFO, TAG, "Leave TWSwitchDoorLockState().");
    return ret;
}

OCStackResult TWSetDiscoveryCallback(const TWDeviceFoundCallback callback)
{
    OC_LOG_V(INFO, TAG, "Enter TWSetDiscoveryCallback().");
    if (callback != NULL)
    {
        g_DeviceFoundCallback= callback;
    }
    else
    {
        g_DeviceFoundCallback = NULL;
    }

    OC_LOG_V(INFO, TAG, "Leave TWSetDiscoveryCallback().");
    return OC_STACK_OK;
}

OCStackResult TWUninitialize()
{
    OC_LOG_V(INFO, TAG, "Enter TWUninitializeZigBee().");

    OCStackResult ret = OC_STACK_ERROR;

    TWResultCode twRet = ClosePort(g_fd);
    if (twRet == TW_RESULT_OK)
    {
        ret = OC_STACK_OK;
    }
    else
    {
        ret = OC_STACK_ERROR;
    }

    OC_LOG_V(INFO, TAG, "Leave TWUninitializeZigBee().");
    return ret;
}

//-----------------------------------------------------------------------------
// Internal functions
//-----------------------------------------------------------------------------

TWResultCode Reset()
{
    OC_LOG_V(INFO, TAG, "Enter Reset().");

    TWResultCode ret = TW_RESULT_ERROR;

    char* responseArray[ARRAY_LENGTH] = {};
    uint8_t responseCount = 0;
    char* promptArray[ARRAY_LENGTH] = {};
    uint8_t promptCount = 0;
    uint64_t atErrorCode = 0;

    ret = IssueATCommand(AT_CMD_RESET, TIME_OUT_00_SECOND,
                         &atErrorCode,
                         responseArray, NULL,
                         &responseCount, NULL);

    if (ret != TW_RESULT_OK)
    {
        OC_LOG_V(ERROR, TAG, "TWriting error on AT&F.");
        ret = TW_RESULT_ERROR;
    }
    else
    {
        if (atErrorCode != AT_ERROR_EVERYTHING_OK)
        {
            OC_LOG_V(ERROR, TAG, "Dongle Reset - FAILED - AT_ERROR: %" PRId64 "", atErrorCode);
            ret = TW_RESULT_ERROR;
        }
        else
        {
            OC_LOG_V(INFO, TAG, "Dongle Reset - OK.");
            ret = TW_RESULT_OK;
        }

    }

    CleanArray(responseArray, promptArray, responseCount, promptCount);

    OC_LOG_V(INFO, TAG, "Leave Reset().");
    return ret;
}

TWResultCode GetLocalEUI()
{
    OC_LOG_V(INFO, TAG, "Enter GetLocalEUI().");
    TWResultCode ret = TW_RESULT_UNKNOWN;

    char* responseArray[ARRAY_LENGTH] = {};
    uint8_t responseCount = 0;

    char* promptArray[ARRAY_LENGTH] = {};
    uint8_t promptCount = 0;

    uint64_t atErrorCode = 0;

    OC_LOG_V(INFO, TAG, "Issued Command: %s.", AT_CMD_GET_LOCAL_EUI);
    ret = IssueATCommand(AT_CMD_GET_LOCAL_EUI, TIME_OUT_05_SECONDS,
                         &atErrorCode,
                         responseArray, promptArray,
                         &responseCount, &promptCount);

    if (ret != TW_RESULT_OK)
    {
        OC_LOG_V(ERROR, TAG, "Write %s", AT_CMD_GET_LOCAL_EUI);
        ret = TW_RESULT_ERROR;
    }
    else
    {
        if (atErrorCode != AT_ERROR_EVERYTHING_OK)
        {
            OC_LOG_V(ERROR, TAG,
                     "Get Local EUI Failed - AT_ERROR: %" PRId64 "", atErrorCode);
            ret = TW_RESULT_ERROR;
        }
        else
        {
            OICStrcpy(g_LocalEUI, SIZE_EUI, responseArray[0]);
            OC_LOG_V(INFO, TAG, "Local EUI: %s ", g_LocalEUI);
            ret = TW_RESULT_OK;
        }
    }

    CleanArray(responseArray, promptArray, responseCount, promptCount);

    OC_LOG_V(INFO, TAG, "Leave GetLocalEUI().");
    return ret;
}

TWResultCode CreatePAN()
{
    /*
    //at+n
    //      +N=NoPAN
    //      OK

    //at+n
    //      +N=COO,26,-6,7306,133F04EA669C6B24
    //      OK

    //at+en
    //      OK
    //      JPAN:26,7306,133F04EA669C6B24

    //at+en
    //      ERROR:28
    */

    OC_LOG_V(INFO, TAG, "Enter CreatePAN()");

    TWResultCode ret = TW_RESULT_ERROR;

    uint8_t responseCount1 = 0;
    uint8_t promptCount1 = 0;
    char* responseArray1[ARRAY_LENGTH] = {};
    char* promptArray1[ARRAY_LENGTH] = {};

    uint8_t responseCount2 = 0;
    uint8_t promptCount2 = 0;
    char* responseArray2[ARRAY_LENGTH] = {};
    char* promptArray2[ARRAY_LENGTH] = {};

    uint64_t atErrorCode = 0;

    ret = IssueATCommand(AT_CMD_GET_NETWORK_INFO, TIME_OUT_00_SECOND,
                         &atErrorCode,
                         responseArray1, promptArray1,
                         &responseCount1, &promptCount1);
    if (ret != TW_RESULT_OK)
    {
        OC_LOG_V(ERROR, TAG, "Write AT+N - FAILED.");
        ret = TW_RESULT_ERROR;
    }
    else
    {
        if (atErrorCode != AT_ERROR_EVERYTHING_OK)
        {
            OC_LOG_V(ERROR, TAG, "Get network info - FAILED.");
            ret = TW_RESULT_ERROR;
        }
        else
        {
            //Handle +N
            ret = HandleATResponse(responseArray1, responseCount1);
            if (ret == TW_RESULT_NO_LOCAL_PAN)
            {
                OC_LOG_V(INFO, TAG, "Attempt to write AT+EN. ");
                ret = IssueATCommand(AT_CMD_ESTABLISH_NETWORK, TIME_OUT_01_SECOND,
                                     &atErrorCode,
                                     responseArray2, promptArray2,
                                     &responseCount2, &promptCount2);

                if (ret != TW_RESULT_OK)
                {
                    OC_LOG_V(ERROR, TAG, "Write AT+EN - FAILED.");
                    ret = TW_RESULT_ERROR;
                }
                else
                {
                    if (atErrorCode == AT_ERROR_EVERYTHING_OK) //OK
                    {
                        //IMPORTANT: Special case for JPAN prompt:
                        //JPAN prompts are stored in responseArray instead of promptArray
                        //Handle JPAN:26,7306,133F04EA669C6B24
                        ret = HandleATResponse(responseArray2, responseCount2);
                        if (ret == TW_RESULT_NEW_LOCAL_PAN_ESTABLISHED)
                        {
                            OC_LOG_V(INFO, TAG, "New Local PAN established - OK.");
                            ret = TW_RESULT_OK;
                        }
                        else
                        {
                            OC_LOG_V(ERROR, TAG, "Handling AT+EN.");
                            ret = TW_RESULT_ERROR;
                        }
                    }
                    else if (atErrorCode == AT_ERROR_NODE_IS_PART_OF_PAN)
                    {
                        OC_LOG_V(INFO, TAG, "Already Established PAN - OK.");
                        ret = TW_RESULT_OK;
                    }
                    else
                    {
                        OC_LOG_V(ERROR, TAG, "Couldn't establish PAN.");
                        ret = TW_RESULT_ERROR;
                    }
                }
            }
            else if (ret == TW_RESULT_HAS_LOCAL_PAN)
            {
                OC_LOG_V(INFO, TAG, "Already established PAN - OK.");
                ret = TW_RESULT_OK;
            }
            else if (atErrorCode != 0)
            {
                OC_LOG_V(ERROR, TAG,
                         "CreatePAN() FAILED with AT_ERROR: %" PRId64 "", atErrorCode);
                ret = TW_RESULT_ERROR;
            }
        }
    }

    CleanArray(responseArray1, promptArray1, responseCount1, promptCount1);
    CleanArray(responseArray2, promptArray2, responseCount2, promptCount2);

    OC_LOG_V(INFO, TAG, "Leave CreatePAN().");
    return ret;
}

TWResultCode EnableJoin(bool isKeyEncrypted, uint8_t* outDeviceJoinedCount)
{
    //Ask:          AT+PJOIN
    //Response:     OK

    //Ask:          AT+PJOIN
    //Response:     ERROR:70

    OC_LOG_V(INFO, TAG, "Enter Enabling Join().");

    (void)isKeyEncrypted;

    TWResultCode ret = TW_RESULT_ERROR;

    if (outDeviceJoinedCount != NULL)
    {
        *outDeviceJoinedCount = 0;

        isKeyEncrypted = false;         //TODO: for now - don't encrypt

        char*   joinTimeHex     = "0F"; //TODO: for now - 15 seconds
        uint8_t joinTimeDecimal = 15;   //TODO: for now - 15 seconds

        char* broadcast = "FFFC";

        int size =  strlen(AT_CMD_PERMIT_JOIN) + strlen(joinTimeHex) +
                    SEPARATOR_LENGTH + strlen(broadcast) + 1;

        char* cmdString = (char*)OICMalloc(size * sizeof(char));
        if (cmdString == NULL)
        {
            ret = TW_RESULT_ERROR_NO_MEMORY;
        }
        else
        {
            snprintf(cmdString, size, "%s%s%s%s",
                     AT_CMD_PERMIT_JOIN, joinTimeHex, SEPARATOR, broadcast);

            char* responseArray[ARRAY_LENGTH] = {};
            uint8_t responseCount = 0;

            char* promptArray[ARRAY_LENGTH] = {};
            uint8_t promptCount = 0;

            uint64_t atErrorCode = 0;

            OC_LOG_V(INFO, TAG, "Attempt to allow joining...");

            OC_LOG_V(INFO, TAG, "Issued Command: %s.", cmdString);
            ret = IssueATCommand(cmdString, joinTimeDecimal,
                                 &atErrorCode,
                                 responseArray, promptArray,
                                 &responseCount, &promptCount);

            if (ret != TW_RESULT_OK)
            {
                OC_LOG_V(ERROR, TAG, "Write AT+PJOIN - AT_ERROR = %" PRId64 "", atErrorCode);
                ret = TW_RESULT_ERROR;
            }
            else
            {
                if (atErrorCode == AT_ERROR_EVERYTHING_OK)
                {
                    OC_LOG_V(INFO, TAG, "Enable joining - OK.");
                    OC_LOG_V(INFO, TAG,
                             "Joining has been allowed for %d seconds. Time's up!",
                             joinTimeDecimal);
                    if (promptCount > 0)    // some devices joined.
                    {
                        OC_LOG_V(INFO, TAG, "Number of devices joined:%d.", promptCount);
                        *outDeviceJoinedCount = promptCount;

                        //Handle SED, FFD, RFD,...
                        ret = HandleATResponse(promptArray, promptCount);
                    }
                    else
                    {
                        ret = TW_RESULT_OK;
                    }
                }
                else if (atErrorCode == AT_ERROR_INVALID_OPERATION)
                {
                    OC_LOG_V(ERROR, TAG, "Enable joining - FAILED - because there was no PAN.");
                    ret = TW_RESULT_ERROR;
                }
                else
                {
                    OC_LOG_V(ERROR, TAG, "Enable joining - FAILED.");
                    ret = TW_RESULT_ERROR;
                }
            }
            CleanArray(responseArray, promptArray, responseCount, promptCount);
        }
    }

    OC_LOG_V(INFO, TAG, "Leave Enabling Join()");
    return ret;
}

TWResultCode FindMatchNodes(uint8_t* outDeviceMatchedCount)
{
    //AT+MATCHREQ:0104,03,0003,0006,0402,00
    //      OK
    //      MatchDesc:0B4A,00,01

    //AT+MATCHREQ:0104,03,0999,0999,0999,00
    //      OK

    OC_LOG_V(INFO, TAG, "Enter FindMatchNodes().");

    TWResultCode ret = TW_RESULT_UNKNOWN;

    char* profileHomeAutomation = "0104";
    char* inClusterCount = "04";
    char* outClusterCount = "00";

    //TODO: add more clusters
    char* clusterIdentify = "0003";
    char* clusterOnOff = "0006";
    char* clusterTemperatureMeasurement = "0402";
    char* clusterIASZone = "0500";

    int size = strlen(AT_CMD_MATCH_REQUEST) + strlen(profileHomeAutomation) +
               SEPARATOR_LENGTH + strlen(inClusterCount) +
               SEPARATOR_LENGTH + strlen(clusterIdentify) +
               SEPARATOR_LENGTH + strlen(clusterOnOff) +
               SEPARATOR_LENGTH + strlen(clusterTemperatureMeasurement) +
               SEPARATOR_LENGTH + strlen(clusterIASZone) +
               SEPARATOR_LENGTH + strlen(outClusterCount) + 1;

    char* cmdString = (char*)OICMalloc(size * sizeof(char));
    if (cmdString == NULL)
    {
        ret = TW_RESULT_ERROR_NO_MEMORY;
    }
    else
    {
        snprintf(cmdString, size, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                 AT_CMD_MATCH_REQUEST, profileHomeAutomation,
                 SEPARATOR, inClusterCount,
                 SEPARATOR, clusterIdentify,
                 SEPARATOR, clusterOnOff,
                 SEPARATOR, clusterTemperatureMeasurement,
                 SEPARATOR, clusterIASZone,
                 SEPARATOR, outClusterCount);

        char* responseArray[ARRAY_LENGTH] = {};
        uint8_t responseCount = 0;

        char* promptArray[ARRAY_LENGTH] = {};
        uint8_t promptCount = 0;

        uint64_t atErrorCode = 0;

        OC_LOG_V(INFO, TAG, "Issued Command: %s.", cmdString);
        ret = IssueATCommand(cmdString, TIME_OUT_10_SECONDS,
                             &atErrorCode,
                             responseArray, promptArray,
                             &responseCount, &promptCount);

        if (ret != TW_RESULT_OK)
        {
            OC_LOG_V(ERROR, TAG, "Write %s - AT_ERROR = %" PRId64 ".", cmdString, atErrorCode);
            ret = TW_RESULT_ERROR;
        }
        else
        {
            if (atErrorCode != AT_ERROR_EVERYTHING_OK)
            {
                OC_LOG_V(ERROR, TAG,
                         "AT_ERROR %" PRId64 " returned from %s", atErrorCode, cmdString);
                ret = TW_RESULT_ERROR;
            }
            else
            {
                OC_LOG_V(INFO, TAG, "Number of found matched nodes:%d.", promptCount);
                if (promptCount > 0)
                {
                    //Handle all received MatchDesc:0B4A,00,01
                    ret = HandleATResponse(promptArray, promptCount);
                    if (ret == TW_RESULT_OK)
                    {
                        OC_LOG_V(INFO, TAG, "Handle matched nodes - SUCCEED.");
                        *outDeviceMatchedCount = promptCount;
                        ret = TW_RESULT_FOUND_MATCHED_DEVICES;
                    }
                    else
                    {
                        OC_LOG_V(ERROR, TAG, "Handle matched nodes.");
                        ret = TW_RESULT_ERROR;
                    }
                }
                else
                {
                    ret = TW_RESULT_FOUND_NO_MATCHED_DEVICE;
                }
            }
        }

        OICFree(cmdString);
        CleanArray(responseArray, promptArray, responseCount, promptCount);

        if (g_FoundMatchedDeviceList != NULL)
        {
            OC_LOG_V(INFO, TAG, "Found these devices via match request.");
            PrintTWDeviceList(g_FoundMatchedDeviceList);
        }
    }

    OC_LOG_V(INFO, TAG, "Leave FindMatchNodes().");
    return ret;
}

TWResultCode FindClusters(char nodeId[], char endpoint[])
{
    //AT+SIMPLEDESC:3746,3746,01
    //      SEQ:97
    //      OK
    //
    //      SimpleDesc:3746,00
    //      EP:01
    //      ProfileID:0104
    //      DeviceID:0402v00
    //      InCluster:0000,0001,0003,0402,0500,0020,0B05
    //      OutCluster:0019
    //
    //      ACK:97

    OC_LOG_V(INFO, TAG, "Enter FindClusters().");

    TWResultCode ret = TW_RESULT_UNKNOWN;

    int size = strlen(AT_CMD_SIMPLE_DESC) + strlen(nodeId) +
               SEPARATOR_LENGTH + strlen(nodeId) +
               SEPARATOR_LENGTH + strlen(endpoint) + 1;

    char* cmdString = (char*)OICMalloc(size * sizeof(char));
    if (cmdString == NULL)
    {
        ret = TW_RESULT_ERROR_NO_MEMORY;
    }
    else
    {
        snprintf(cmdString, size, "%s%s%s%s%s%s",
                 AT_CMD_SIMPLE_DESC, nodeId,
                 SEPARATOR, nodeId,
                 SEPARATOR, endpoint);

        char* responseArray[ARRAY_LENGTH] = {};
        uint8_t responseCount = 0;

        char* promptArray[ARRAY_LENGTH] = {};
        uint8_t promptCount = 0;

        uint64_t atErrorCode = 0;

        OC_LOG_V(INFO, TAG, "Issued Command: %s.", cmdString);
        ret = IssueATCommand(cmdString, TIME_OUT_07_SECONDS,
                             &atErrorCode,
                             responseArray, promptArray,
                             &responseCount, &promptCount);

        if (ret != TW_RESULT_OK)
        {
            OC_LOG_V(ERROR, TAG, "Write %s", cmdString);
            ret = TW_RESULT_ERROR;
        }
        else
        {
            if (atErrorCode != AT_ERROR_EVERYTHING_OK)
            {
                OC_LOG_V(ERROR, TAG,
                         "AT_ERROR %" PRId64 " returned from %s", atErrorCode, cmdString);
                ret = TW_RESULT_ERROR;
            }
            else
            {
                if (promptCount == SIMPLEDESC_RESPONSE_EXPECTED_LINES)
                {
                    //Handle - SimpleDesc...
                    ret = HandleATResponse(promptArray, promptCount);
                    if (ret == TW_RESULT_OK)
                    {
                        OC_LOG_V(INFO, TAG,
                                 "Succeed handling clusters for nodeid %s.", nodeId);
                        ret = TW_RESULT_HAS_CLUSTERS;
                    }
                    else
                    {
                        OC_LOG_V(ERROR, TAG, "Error handling clusters for nodeid %s.", nodeId);
                        ret = TW_RESULT_ERROR;
                    }
                }
                else
                {
                    OC_LOG_V(ERROR, TAG, "Received an invalid Simple Descriptor prompt.");
                    PrintArray(responseArray, promptArray, responseCount, promptCount);
                    ret = TW_RESULT_ERROR;
                }
            }
        }
        CleanArray(responseArray, promptArray, responseCount, promptCount);
        OICFree(cmdString);
    }

    OC_LOG_V(INFO, TAG, "Leave FindClusters().");

    return ret;
}

OCStackResult TWListenForStatusUpdates(char* nodeId, char* endpointId)
{
    OC_LOG_V(INFO, TAG, "Enter TWListenForStatusUpdates().");

    char* zoneClusterID = "0500";
    char* zoneAttributeID = "0010";
    char* attributeDateType = "F0";

    OCStackResult ret = TWSetAttribute(NULL, nodeId, endpointId,
                                       zoneClusterID, zoneAttributeID, attributeDateType,
                                       g_LocalEUI);

    OC_LOG_V(INFO, TAG, "Leave TWListenForStatusUpdates().");

    return ret;
}

TWResultCode HandleATResponse(char* responseArray[], int count)
{
    OC_LOG_V(INFO, TAG, "Enter HandleATResponse().");

    TWResultCode ret = TW_RESULT_ERROR;

    int32_t i = 0;
    for (; i < count; i++)
    {
        uint32_t k = 0;
        for (; k < sizeof(g_TWATResultHandlerPairArray)/sizeof(TWATResultHandlerPair); ++k)
        {
            if (strncmp(responseArray[i],
                        g_TWATResultHandlerPairArray[k].resultTxt,
                        strlen(g_TWATResultHandlerPairArray[k].resultTxt)
                        ) == 0)
            {
                char* tokens[ARRAY_LENGTH] = {};
                const char* delimiters = ",\r\n";
                int paramCount = Tokenize(responseArray[i] +
                                           strlen(g_TWATResultHandlerPairArray[k].resultTxt),
                                           delimiters, tokens);
                if (paramCount > 0)
                {
                    ret = g_TWATResultHandlerPairArray[k].handler(paramCount, tokens);
                }

                int n = 0;
                for (; n < paramCount; n++)
                {
                    OICFree(tokens[n]);
                }

                break;
            }
        }
    }

    OC_LOG_V(INFO, TAG, "Leave HandleATResponse().");
    return ret;
}

//-----------------------------------------------------------------------------
// Internal functions - AT Response/Prompt Handlers
//-----------------------------------------------------------------------------

TWResultCode TelAddressResponseHandler(int count, char* tokens[])
{
    //AddrResp:<errorcode>[,<NodeID>,<EUI64>]
    //AddrResp:00,15ED,000D6F00040574B8

    if(!tokens || count <= 3)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }

    OC_LOG_V(INFO, TAG, "Enter TelAddressResponseHandler()");

    TWResultCode ret = TW_RESULT_ERROR;

    if (strcmp(tokens[0], AT_STR_ERROR_OK) != 0)
    {
        OC_LOG_V(ERROR, TAG, "AddrResp prompt contained error status.");
    }
    else
    {
        OICStrcpy(g_WIPRemoteNodeId, SIZE_NODEID, tokens[1]);
        OICStrcpy(g_WIPRemoteEUI, SIZE_EUI, tokens[2]);
        ret = TW_RESULT_OK;
    }

    OC_LOG_V(INFO, TAG, "Leave TelAddressResponseHandler()");

    return ret;
}

TWResultCode TelNetworkInfoHandler(int count, char* tokens[])
{
    // Ask:         AT+N
    // Response:    +N=COO,24,-6,9726,12BB200F073AB573
    //                          or
    //              +N=NoPAN
    //
    //              +N=<devicetype>,<channel>,<power>,<PANID>,<EPANID>

    OC_LOG_V(INFO, TAG, "Enter TelNetworkInfoHandler()");
    if(!tokens || count <= 4)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    TWResultCode ret = TW_RESULT_UNKNOWN;

    int i = 0;
    for (; i < count; ++i)
    {
        if (tokens[i] != NULL)
        {
            OC_LOG_V(INFO, TAG, "Token[%d] = %s", i, tokens[i]);
        }
    }

    char* temp = tokens[0];
    if (strcmp(temp, "NoPAN") == 0)
    {
        OC_LOG_V(INFO, TAG, "TelNetworkInfoHandler(): It is NoPan.");
        ret = TW_RESULT_NO_LOCAL_PAN;
    }
    else
    {
        OC_LOG_V(INFO, TAG, "TelNetworkInfoHandler(): already have an established network.");
        ret = AsciiHexToValue(tokens[3], 2, &g_ZigBeeStatus.panId);
        if(ret != TW_RESULT_OK)
        {
            return ret;
        }
        ret = AsciiHexToValue(tokens[4], 8, &g_ZigBeeStatus.extPanId);
        if(ret != TW_RESULT_OK)
        {
            return ret;
        }
        OC_LOG_V(INFO, TAG,
                 "TelNetworkInfoHandler(): PanId=%" PRId64 "", g_ZigBeeStatus.panId);
        OC_LOG_V(INFO, TAG,
                 "TelNetworkInfoHandler(): ExtPanId=%" PRId64 "", g_ZigBeeStatus.extPanId);

        OC_LOG_V(INFO, TAG, "TelNetworkInfoHandler(): PanId=%s", tokens[3]);
        OC_LOG_V(INFO, TAG, "TelNetworkInfoHandler(): ExtPanId=%s", tokens[4]);

        OC_LOG_V(INFO, TAG, "TelNetworkInfoHandler set ExtPanId to %08X%08X",
                 (unsigned int)(g_ZigBeeStatus.extPanId >> 32),
                 (unsigned int)(g_ZigBeeStatus.extPanId & 0xFFFFFFFF));

        ret = TW_RESULT_HAS_LOCAL_PAN;
    }

    OC_LOG_V(INFO, TAG, "Leave TelNetworkInfoHandler()");
    return ret;
}

TWResultCode TelJpanHandler(int count, char* tokens[])
{
    //Ask:        AT+EN:[<channel>],[<POWER>],[<PANID>]
    //Response:   JPAN:<channel>,<PANID>,<EPANID>

    OC_LOG_V(INFO, TAG, "Enter TelJpanHandler().");
    if(!tokens || count <= 2)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    (void)count;

    TWResultCode ret = TW_RESULT_UNKNOWN;

    ret = AsciiHexToValue(tokens[1], 2, &g_ZigBeeStatus.panId);
    if(ret != TW_RESULT_OK)
    {
        return ret;
    }
    ret = AsciiHexToValue(tokens[2], 8, &g_ZigBeeStatus.extPanId);
    if(ret != TW_RESULT_OK)
    {
        return ret;
    }
    OC_LOG_V(INFO, TAG, "PanId = %" PRId64 "", g_ZigBeeStatus.panId);
    OC_LOG_V(INFO, TAG, "ExtPanId = %" PRId64 "", g_ZigBeeStatus.extPanId);
    ret = TW_RESULT_NEW_LOCAL_PAN_ESTABLISHED;

    OC_LOG_V(INFO, TAG, "Leave TelJpanHandler().");
    return ret;
}

TWResultCode TelEndDeviceJoinHandler(int count, char* tokens[])
{
    //Ask:      AT+PJOIN
    //
    //Prompt:   RFD:<IEEE Address>,<NodeID>
    //Prompt:   FFD:<IEEE Address>,<NodeID>
    //Prompt:   SED:<IEEE Address>,<NodeID>
    //Prompt:   ZED:<IEEE Address>,<NodeID>

    OC_LOG_V(INFO, TAG, "Enter TelEndDeviceJoinHandler().");
    if(!tokens || count <= 1)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    (void)count;

    TWResultCode ret = TW_RESULT_UNKNOWN;

    //TODO: Might need to add into the list if needed - log it for now.
    OC_LOG_V(INFO, TAG, "Just Joined - EUI:%s; NodeID:%s.", tokens[0], tokens[1]);

    ret = TW_RESULT_OK;

    OC_LOG_V(INFO, TAG, "Leave TelEndDeviceJoinHandler().");
    return ret;
}

TWResultCode TelMatchDescHandler(int count, char* tokens[])
{
    //Prompt:       MatchDesc:0B4A,00,01

    OC_LOG_V(INFO, TAG, "Enter TelMatchDescHandler().");
    if(!tokens || count <= 2)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    (void)count;

    TWResultCode ret = TW_RESULT_ERROR;

    if (strcmp(tokens[1], AT_STR_ERROR_OK) != 0)
    {
        OC_LOG_V(ERROR, TAG, "MatchDesc prompt contained error status.");
    }
    else
    {
        //Create TWDevice
        TWDevice* device = (TWDevice*)OICCalloc(1, sizeof(TWDevice));
        if (device == NULL)
        {
            ret = TW_RESULT_ERROR_NO_MEMORY;
        }
        else
        {
            device->endpointOfInterest = (TWEndpoint*)OICCalloc(1, sizeof(TWEndpoint));
            if (device->endpointOfInterest == NULL)
            {
                OICFree(device);
                ret = TW_RESULT_ERROR_NO_MEMORY;
            }
            else
            {
                //OICStrcpy(device->eui, SIZE_EUI, remoteEUI);
                OICStrcpy(device->nodeId, SIZE_NODEID, tokens[0]);
                OICStrcpy(device->endpointOfInterest->endpointId, SIZE_ENDPOINTID, tokens[2]);

                if (g_FoundMatchedDeviceList == NULL)
                {
                    //Create a list of promptCount entries
                    g_FoundMatchedDeviceList = (TWDeviceList*)OICMalloc(sizeof(TWDeviceList));
                    if (g_FoundMatchedDeviceList == NULL)
                    {
                        OICFree(device->endpointOfInterest);
                        OICFree(device);
                        ret = TW_RESULT_ERROR_NO_MEMORY;
                    }
                    else
                    {
                        g_FoundMatchedDeviceList->count = 1;
                        g_FoundMatchedDeviceList->deviceList =
                                (TWDevice*)OICMalloc(sizeof(TWDevice));
                        if (g_FoundMatchedDeviceList->deviceList == NULL)
                        {
                            OICFree(device->endpointOfInterest);
                            OICFree(device);
                            ret = TW_RESULT_ERROR_NO_MEMORY;
                        }
                        else
                        {
                            memcpy(g_FoundMatchedDeviceList->deviceList,
                                   device,
                                   sizeof(TWDevice));
                            ret = TW_RESULT_OK;
                        }
                    }
                }
                else
                {
                    //Expand the list
                    int newSize = sizeof(TWDevice) * (g_FoundMatchedDeviceList->count + 1);
                    TWDevice* temp = (TWDevice*)realloc(g_FoundMatchedDeviceList->deviceList,
                                                        newSize);
                    if (temp == NULL)
                    {
                        OICFree(device->endpointOfInterest);
                        OICFree(device);
                        ret =TW_RESULT_ERROR_NO_MEMORY;
                    }
                    else
                    {
                        g_FoundMatchedDeviceList->deviceList = temp;

                        //Add to the end of list
                        int count = g_FoundMatchedDeviceList->count;
                        memcpy(&g_FoundMatchedDeviceList->deviceList[count],
                               device,
                               sizeof(TWDevice));

                        //Increase the count
                        g_FoundMatchedDeviceList->count++;

                        ret = TW_RESULT_OK;
                    }
                }
            }
        }
    }

    OC_LOG_V(INFO, TAG, "Leave TelMatchDescHandler().");
    return ret;
}

TWResultCode TelSimpleDescHandler(int count, char* tokens[])
{
    //AT+SIMPLEDESC:3746,3746,01
    //      SEQ:97
    //      OK
    //
    //      SimpleDesc:3746,00              <<<<<<<---------------------
    //      EP:01
    //      ProfileID:0104
    //      DeviceID:0402v00
    //      InCluster:0000,0001,0003,0402,0500,0020,0B05
    //      OutCluster:0019
    //      ACK:97

    OC_LOG_V(INFO, TAG, "Enter TelSimpleDescHandler().");
    if(!tokens || count <= 1)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    (void)count;

    TWResultCode ret = TW_RESULT_UNKNOWN;

    if (strcmp(tokens[1], AT_STR_ERROR_OK) != 0)
    {
        OC_LOG_V(ERROR, TAG, "SimpleDesc: prompt contained error status %s.", tokens[1]);
        ret = TW_RESULT_ERROR;
    }
    else
    {
        if (strcmp(tokens[0], g_WIPDevice->nodeId) == 0)
        {
            OC_LOG_V(INFO, TAG, "Got simple descriptor for nodeid %s", tokens[0]);
            ret = TW_RESULT_OK;
        }
        else
        {
            OC_LOG_V(ERROR, TAG,
                     "Finding simple descriptor for non existing nodeid %s.", tokens[0]);
            ret = TW_RESULT_ERROR;
        }
    }

    OC_LOG_V(INFO, TAG, "Leave TelSimpleDescHandler().");
    return ret;
}

TWResultCode TelSimpleDescInClusterHandler(int count, char* tokens[])
{
    //AT+SIMPLEDESC:3746,3746,01
    //      SEQ:97
    //      OK
    //
    //      SimpleDesc:3746,00
    //      EP:01
    //      ProfileID:0104
    //      DeviceID:0402v00
    //      InCluster:0000,0001,0003,0402,0500,0020,0B05        <<<<<<<<--------------
    //      OutCluster:0019
    //      ACK:97

    OC_LOG_V(INFO, TAG, "Enter TelSimpleDescInClusterHandler().");
    TWResultCode ret = TW_RESULT_ERROR;

    //Add found clusters for the node.
    if (g_WIPDevice->endpointOfInterest->clusterList == NULL)
    {
        g_WIPDevice->endpointOfInterest->clusterList =
                (TWClusterList*)OICCalloc(sizeof(TWClusterList), 1);
        if (g_WIPDevice->endpointOfInterest->clusterList == NULL)
        {
            ret = TW_RESULT_ERROR_NO_MEMORY;
        }
        else
        {
            g_WIPDevice->endpointOfInterest->clusterList->clusterIds =
                    (TWClusterId*)OICCalloc(sizeof(TWClusterId) * count, 1);
            if (g_WIPDevice->endpointOfInterest->clusterList->clusterIds == NULL)
            {
                OICFree(g_WIPDevice->endpointOfInterest->clusterList);
                ret = TW_RESULT_ERROR_NO_MEMORY;
            }
            else
            {
                int i = 0;
                for (; i < count; i++)
                {
                    OICStrcpy(g_WIPDevice->endpointOfInterest->clusterList->
                                clusterIds[i].clusterId,
                                SIZE_CLUSTERID,
                                tokens[i]);

                    OC_LOG_V(INFO, TAG, "ClusterIds[%d]=%s",
                             i,
                             g_WIPDevice->endpointOfInterest->
                             clusterList->clusterIds[i].clusterId);
                }
                g_WIPDevice->endpointOfInterest->clusterList->count = count;
                ret = TW_RESULT_OK;
            }
        }
    }
    else
    {
        OC_LOG_V(ERROR, TAG, "Expected an empty cluster list.");
    }

    OC_LOG_V(INFO, TAG, "Leave TelSimpleDescInClusterHandler().");
    return ret;
}

TWResultCode TelWriteAttrHandler(int count, char* tokens[])
{
    //AT+WRITEATR:3A3D,01,0,0003,0000,21,00
    //      OK
    //      WRITEATTR:3A3D,01,0003,,00

    OC_LOG_V(INFO, TAG, "Enter TelWriteAttrHandler().");
    if(!tokens || count <= 3)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    (void)count;

    TWResultCode ret = TW_RESULT_UNKNOWN;

    if (strcmp(tokens[3], AT_STR_ERROR_OK) == 0)
    {
        ret = TW_RESULT_OK;
    }
    else
    {
        ret = TW_RESULT_ERROR;
    }

    OC_LOG_V(INFO, TAG, "Leave TelWriteAttrHandler().");
    return ret;
}

TWResultCode TelReadAttrHandlerTemperature(int count, char* tokens[])
{
    //AT+READATR:F2D7,01,0,0402,0002
    //      OK
    //      TEMPERATURE:F2D7,01,0002,00,1770
    //
    //AT+READATR:F2D7,01,0,0402,0002
    //      OK
    //      ERROR:66

    OC_LOG_V(INFO, TAG, "Enter TelReadAttrHandlerTemperature().");
    if(!tokens || count <= 4)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    (void)count;

    TWResultCode ret = TW_RESULT_ERROR;

    if (strcmp(tokens[3], AT_STR_ERROR_OK) != 0)
    {
        OC_LOG_V(ERROR, TAG, "TEMPERATURE prompt contained error status.");
    }
    else
    {
        // AttrInfo is 16-bit value representing (100 * Degrees Celsius)
        // so 0x812 = 20.66 C = 69.188 F
        if (g_ZigBeeStatus.remoteAttributeValueRead != NULL)
        {
            OICFree(g_ZigBeeStatus.remoteAttributeValueRead);
        }
        OC_LOG_V(INFO, TAG, "Read Attribute Value: %s.", tokens[4]);
        g_ZigBeeStatus.remoteAttributeValueRead =
                (char*)OICMalloc(sizeof(char) * strlen(tokens[4]));
        if (g_ZigBeeStatus.remoteAttributeValueRead == NULL)
        {
            ret = TW_RESULT_ERROR_NO_MEMORY;
        }
        else
        {
            strcpy(g_ZigBeeStatus.remoteAttributeValueRead, tokens[4]);
            g_ZigBeeStatus.remoteAtrributeValueReadLength = strlen(tokens[4]);
            ret = TW_RESULT_REMOTE_ATTR_HAS_VALUE;
        }
    }

    OC_LOG_V(INFO, TAG, "Leave TelReadAttrHandlerTemperature().");

    return ret;
}

TWResultCode TelReadAttrHandler(int count, char* tokens[])
{
    //AT+READATR:F2D7,01,0,0402,0002
    //      OK
    //      RESPATTR:<NodeID>,<EP>,<ClusterID>,<AttrID>,<Status>,<AttrInfo>
    //
    //AT+READATR:F2D7,01,0,0402,0002
    //      OK
    //      ERROR:66

    OC_LOG_V(INFO, TAG, "Enter TelReadAttrHandler().");
    if(!tokens || count <= 5)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    (void)count;

    TWResultCode ret = TW_RESULT_ERROR;

    if (strcmp(tokens[4], AT_STR_ERROR_OK) != 0)
    {
        OC_LOG_V(ERROR, TAG, "READATTR prompt contained error status.");
    }
    else
    {
        if (g_ZigBeeStatus.remoteAttributeValueRead != NULL)
        {
            OICFree(g_ZigBeeStatus.remoteAttributeValueRead);
        }
        OC_LOG_V(INFO, TAG, "Read Attribute Value: %s.", tokens[5]);
        g_ZigBeeStatus.remoteAttributeValueRead =
                (char*)OICMalloc(sizeof(char) * strlen(tokens[5]));
        if (g_ZigBeeStatus.remoteAttributeValueRead != NULL)
        {
            strcpy(g_ZigBeeStatus.remoteAttributeValueRead, tokens[5]);
            g_ZigBeeStatus.remoteAtrributeValueReadLength = strlen(tokens[5]);
            ret = TW_RESULT_REMOTE_ATTR_HAS_VALUE;
        }
    }

    OC_LOG_V(INFO, TAG, "Leave TelReadAttrHandler().");

    return ret;
}

TWResultCode TelZCLDefaultResponseHandler(int count, char* tokens[])
{
    //AT+RONOFF:<Address>,<EP>,<SendMode>[,<ON/OFF>]
    //      DFTREP:<NodeID>,<EP>,<ClusterID>,<CMD>,<Status>
    //
    //AT+DRLOCK:<Address>,<EP>,<SendMode>,<Lock/Unlock>
    //      DFTREP:<NodeID>,<EP>,<ClusterID>,<CMD>,<Status>
    //
    //AT+LCMVTOLEV:<Address>,<EP>,<SendMode>,<ON/OFF>,<LevelValue>,<TransTime>
    //      DFTREP:<NodeID>,<EP>,<ClusterID>,<CMD>,<Status>

    OC_LOG_V(INFO, TAG, "Enter TelZCLDefaultResponseHandler().");
    if(!tokens || count <= 4)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    (void)count;

    TWResultCode ret = TW_RESULT_UNKNOWN;

    OC_LOG_V(INFO, TAG,
             "DFTREP prompt succeed for NodeId:%s, EP:%s, ClusterId:%s, CMD:%s.",
             tokens[0], tokens[1], tokens[2], tokens[3]);

    if (strcmp(tokens[4], AT_STR_ERROR_OK) != 0)
    {
        ret = TW_RESULT_ERROR;
    }
    else
    {
        ret = TW_RESULT_OK;
    }

    OC_LOG_V(INFO, TAG, "Leave TelZCLDefaultResponseHandler().");

    return ret;
}

TWResultCode TelSwitchDoorLockStateHandler(int count, char* tokens[])
{
    //AT+DRLOCK:<Address>,<EP>,<SendMode>,<Lock/Unlock>
    //      DRLOCRSP:<nodeID>,<ep>,<status>
    //      or
    //      DRUNLOCKRSP:<nodeID>,<ep>,<status>

    OC_LOG_V(INFO, TAG, "Enter TelSwitchDoorLockStateHandler().");
    if(!tokens || count <= 4)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    (void)count;

    TWResultCode ret = TW_RESULT_UNKNOWN;
    if (strcmp(tokens[2], AT_STR_ERROR_OK) != 0)
    {
        OC_LOG_V(ERROR, TAG,
                 "DRLOCRSP/DRUNLOCKRSP prompt contained error status %s.", tokens[4]);
        ret = TW_RESULT_ERROR;
    }
    else
    {
        OC_LOG_V(INFO, TAG, "DRLOCRSP/DRUNLOCKRSP prompt succeed for nodeId:%s, ep:%s.",
                 tokens[0], tokens[1]);
        ret = TW_RESULT_OK;
    }

    OC_LOG_V(INFO, TAG, "Leave TelSwitchDoorLockStateHandler().");

    return ret;
}

TWResultCode TelZoneEnrollRequestHandler(int count, char* tokens[])
{
    //ZENROLLREQ:<NodeID>,<EndPoint>,<ZoneType>,<ManufactureCode>

    OC_LOG_V(INFO, TAG, "Enter TelZoneEnrollRequestHandler().");
    if(!tokens || count <= 3)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    (void)count;

    TWResultCode ret = TW_RESULT_OK;

    OC_LOG_V(INFO, TAG, "Received zone request from:");
    OC_LOG_V(INFO, TAG, "Node:%s", tokens[0]);
    OC_LOG_V(INFO, TAG, "EP:%s", tokens[1]);
    OC_LOG_V(INFO, TAG, "ZoneType:%s", tokens[2]);
    OC_LOG_V(INFO, TAG, "ManufactureCode:%s", tokens[3]);

    OC_LOG_V(INFO, TAG, "Leave TelZoneEnrollRequestHandler().");

    return ret;

}

TWResultCode TelEnrolledHandler(int count, char* tokens[])
{
    //ENROLLED:<ZID>,<ZoneType>,<EUI>

    OC_LOG_V(INFO, TAG, "Enter TelEnrolledHandler().");
    if(!tokens || count <= 2)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    (void)count;

    TWResultCode ret = TW_RESULT_OK;

    OC_LOG_V(INFO, TAG, "Received zone request from:");
    OC_LOG_V(INFO, TAG, "ZID:%s", tokens[0]);
    OC_LOG_V(INFO, TAG, "ZoneType:%s", tokens[1]);
    OC_LOG_V(INFO, TAG, "EUI:%s", tokens[2]);

    TWEnrollee enrollee;
    OICStrcpy(enrollee.zoneId, SIZE_ZONEID, tokens[0]);
    OICStrcpy(enrollee.zoneType, SIZE_ZONETYPE, tokens[1]);
    OICStrcpy(enrollee.eui, SIZE_EUI, tokens[2]);

    if (g_EnrollmentSucceedCallback != NULL)
    {
        g_EnrollmentSucceedCallback(&enrollee);
    }

    OC_LOG_V(INFO, TAG, "Leave TelEnrolledHandler().");

    return ret;
}

TWResultCode TelZoneStatusHandler(int count, char* tokens[])
{
    //ZONESTATUS:<NodeID>,<EP>,<ZoneStatus>,<ExtendStatus>[,<ZoneID>,<Delay>]
    //ZONESTATUS:5FBA,01,0021,00,01,00AF

    OC_LOG_V(INFO, TAG, "Enter TelZoneStatusHandler().");
    if(!tokens || count <= 3)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    TWResultCode ret = TW_RESULT_OK;

    TWUpdate update;
    OICStrcpy(update.nodeId, SIZE_NODEID, tokens[0]);
    OICStrcpy(update.endpoint, SIZE_NODEID, tokens[1]);
    OICStrcpy(update.status, SIZE_NODEID, tokens[2]);
    OICStrcpy(update.extendedStatus, SIZE_NODEID, tokens[3]);

    if (count == 6)
    {
        OICStrcpy(update.zoneId, SIZE_NODEID, tokens[4]);
        OICStrcpy(update.delay, SIZE_NODEID, tokens[5]);
    }

    if (g_DeviceStatusUpdateCallback != NULL)
    {
        g_DeviceStatusUpdateCallback(&update);
    }

    OC_LOG_V(INFO, TAG, "Leave TelZoneStatusHandler().");

    return ret;
}

//-----------------------------------------------------------------------------
// Internal functions - Helpers
//-----------------------------------------------------------------------------

/**
 *
 * Tokenize 'input' parameter by 'delimiters' into 'output' array.
 *
 */
int Tokenize(const char *input, const char* delimiters, char* output[])
{
    OC_LOG_V(INFO, TAG, "Enter Tokenize()");

    if (output == NULL)
    {
        OC_LOG_V(INFO, TAG, "Invalid parameter.");
        return -1;
    }

    int length = strlen(input);
    char * str = (char *) OICCalloc(1, length + 1);
    OICStrcpy(str, length+1, input);

    char* savePtr = NULL;
    char* p   = strtok_r(str, delimiters, &savePtr);
    int index = 0;
    while (p && index <= ARRAY_LENGTH)
    {
        int size = strlen(p) + 1;   //for null char
        output[index] = (char*)OICCalloc(size, sizeof(char));
        OICStrcpy(output[index], size, p);

        p = strtok_r (NULL, delimiters, &savePtr);
        index++;
    }

    OICFree(str);
    OC_LOG_V(INFO, TAG, "Leave Tokenize()");
    return index;
}

int AsciiToHex(char c)
{
    int num = (int) c;
    if(c >= '0' && c <= '9')
    {
        return num - '0';
    }

    if(num >= 'A' && num <= 'F')
    {
        return num - 'A' + 10;
    }
    return -1;
}

TWResultCode AsciiHexToValue(char* hexString, int length, uint64_t* value)
{
    if(!hexString || !value || length < 0)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    int retVal = AsciiToHex(hexString[0]);
    if(retVal == -1)
    {
        OC_LOG(ERROR, TAG, "Bad conversion from ASCII To Hex.");
        return TW_RESULT_ERROR;
    }
    *value = (uint64_t)retVal;
    for (int i = 1; i < length*2; ++i)
    {
        if (sizeof(hexString) > (uint32_t)i)
        {
            *value <<= 4;
            retVal = AsciiToHex(hexString[i]);
            if(retVal == -1)
            {
                OC_LOG(ERROR, TAG, "Bad conversion from ASCII To Hex.");
                return TW_RESULT_ERROR;
            }
            *value |= (uint64_t)retVal;
        }
    }
    return TW_RESULT_OK;
}

/**
 *
 * Apply interface attribute values to terminal settings.
 *
 */
int SetTerminalInfo(int fd, int speed, int parity, int shouldBlock)
{
    OC_LOG_V(INFO, TAG, "Enter SetTerminalInfo()");

    int ret = 0;
    struct termios terminalInfo = {0, 0, 0, 0, 0, 0, 0};
    ret = tcgetattr(fd, &terminalInfo);
    if (ret != 0)
    {
        OC_LOG_V(ERROR, TAG, "tcgetattr() - error=%d", ret);
        ret = -1;
        goto exit;
    }

    ret = cfsetispeed (&terminalInfo, speed);
    if (ret != 0)
    {
        OC_LOG_V(ERROR, TAG, "cfsetispeed() - error=%d", ret);
        ret = -1;
        goto exit;
    }

    ret = cfsetospeed (&terminalInfo, speed);
    if (ret != 0)
    {
        OC_LOG_V(ERROR, TAG, "cfsetospeed() - error=%d", ret);
        ret = -1;
        goto exit;
    }

    terminalInfo.c_cflag = (terminalInfo.c_cflag & ~CSIZE);     //byte size
    terminalInfo.c_cflag |= CS8;        //byte size is 8

    terminalInfo.c_cflag &= ~PARENB;    //no parity
    terminalInfo.c_cflag |= parity;     //no parity

    terminalInfo.c_cflag &= ~CSTOPB;    //1 stop bit

    terminalInfo.c_cflag |= CREAD;      //enable the receiver

    //Input Control Settings
    terminalInfo.c_iflag &= ~IGNBRK;    //break condition

    //Local Mode Settings
    terminalInfo.c_lflag = 0;

    // whether to block on read and read time-out
    terminalInfo.c_cc[VMIN]  = (shouldBlock >= 1) ? 1 : 0;
    terminalInfo.c_cc[VTIME] = 5;

    //Input Control Settings
    terminalInfo.c_oflag = 0;

    if (tcsetattr (fd, TCSANOW, &terminalInfo) != 0)
    {
        OC_LOG_V(ERROR, TAG, "tcsetattr");
        ret = -1;
    }

exit:
    OC_LOG_V(INFO, TAG, "Leave SetTerminalInfo() with ret=%d", ret);
    return ret;
}

/**
 *
 * Issues an AT Command to the Telegesis Dongle. Waits for response unless
 * a timeout is specified. Blocks until operation is completed. Responses are
 * in placed responseArray, while prompts are placed in promptArray.
 *
 * @param[out] atErrorCode The AT Error code received as Hex. 0 on success,
 *                       otherwise refer to Telegesis manual.
 * @param[out] responseArray Array corresponding to response lines received in
 *                           the buffer during a write/read. (ie. A synchronous
 *                           AT result.)
 * @param[out] promptArray Array corresponding to prompt lines received in
 *                         the buffer post write/read. (ie. An asynchronous AT
 *                         result.)
 * @param[out] responseCount The number of responses in the response array.
 * @param[out] promptCount The number of prompts in the prompt array.
 * @param[in] command The AT Command you wish to issue.
 * @param[in] timeout The allotted time (seconds) you expect the AT Command's
 *                    prompt results to reach the buffer.
 *
 */
TWResultCode IssueATCommand(const char * command,
                            uint8_t timeout,
                            uint64_t * atErrorCode,
                            char * responseArray[],
                            char * promptArray[],
                            uint8_t * responseCount,
                            uint8_t * promptCount)
{
    TWResultCode ret = TW_RESULT_ERROR;

    ret = WriteBuffer(command);
    if(ret != TW_RESULT_OK)
    {
        OC_LOG_V(ERROR, TAG, "Failed to write to buffer with result: %d", ret);
        return ret;
    }

    ret = ReadBuffer(atErrorCode,
                     responseArray,
                     responseCount,
                     command,
                     0);
    if(ret != TW_RESULT_OK)
    {
        OC_LOG_V(ERROR, TAG, "Failed to read from buffer with result: %d", ret);
        return ret;
    }
    if(timeout != 0)
    {
        sleep(timeout);
        ret = ReadBuffer(atErrorCode,
                         promptArray,
                         promptCount,
                         NULL,
                         timeout);
        if(ret != TW_RESULT_OK)
        {
            OC_LOG_V(ERROR, TAG, "Failed to read from buffer with result: %d", ret);
            return ret;
        }
    }
    return ret;
}

/**
 *
 * Returns file descriptor by reference based on the global comPort specified
 * at the top of this file.
 *
 * @param[out] fd The file descriptor associated with the comport specified.
 *
 */
TWResultCode OpenPort(int * fd)
{
    if(g_port == NULL)
    {
        OC_LOG_V(ERROR, TAG, "No port specified.");
        return TW_RESULT_ERROR_INVALID_PORT;
    }
    if(!fd)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    int tw_fd = open(g_port, O_RDWR | O_NOCTTY | O_SYNC);
    *fd = tw_fd;
    if(tw_fd <= 0)
    {
        OC_LOG_V(ERROR, TAG, "Could not open port. Errno is: %d", errno);
        return TW_RESULT_ERROR;
    }

    // set speed to 19,200 bps, 8n1 (no parity), no block
    SetTerminalInfo(tw_fd, DEVICE_BAUDRATE, 0, 0);

    return TW_RESULT_OK;
}

/**
 *
 * Closes port based on the specified file descriptor.
 *
 *  Returns TW_RESULT_OK on success.
 *
 * @param[in] The file descriptor to be closed.
 *
 */
TWResultCode ClosePort(int fd)
{
    int ret = close(fd);
    if(ret != 0)
    {
        OC_LOG_V(ERROR, TAG, "Could not close port. Errno is: %d", errno);
        return TW_RESULT_ERROR;
    }
    return TW_RESULT_OK;
}

bool IsThisAValidLine(char * singleLine, bool * endControlHit, uint64_t * atErrorCode)
{
    if(!singleLine || !endControlHit || !atErrorCode)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    size_t singleLineSize = strlen(singleLine);
    int i = 0;
    size_t endStrSize = 0;
    size_t strSize = 0;
    TWResultCode ret = TW_RESULT_ERROR;
    for(; i<TW_ENDCONTROL_MAX_VALUE; i++)
    {
        endStrSize = strlen(g_TWEndMessageMap[i].endStr);
        strSize = singleLineSize >= endStrSize ? endStrSize : singleLineSize;
        // Found a single EndControl string.
        if (strncmp(singleLine, g_TWEndMessageMap[i].endStr, strSize) == 0)
        {
            //endControlHit = true;
            if (g_TWEndMessageMap[i].endControl == TW_ENDCONTROL_OK)
            {
               *atErrorCode = 0x0000;
               *endControlHit = true;
            }
            else if (g_TWEndMessageMap[i].endControl == TW_ENDCONTROL_ERROR)
            {
                ret = AsciiHexToValue(&singleLine[strSize],
                                      strlen(&singleLine[strSize])-1,
                                      atErrorCode);
                if(ret != TW_RESULT_OK)
                {
                    return false;
                }
                *endControlHit = true;
            }
            else if (g_TWEndMessageMap[i].endControl == TW_ENDCONTROL_SEQ)
            {
                //endControlHit = false;
            }
            else if (g_TWEndMessageMap[i].endControl == TW_ENDCONTROL_ACK)
            {
                //endControlHit = false;
            }
            return false;
            //break;
        }
    }
    return true;

}

char * myStrTok(char* readData, const char* tokens, bool * endControlHit,
                 uint64_t * atErrorCode, char ** savePtr)
{
    if(!tokens || !endControlHit || !atErrorCode || !savePtr)
    {
        return NULL;
    }
    char * buf = strtok_r(readData, tokens, savePtr);
    if(!buf)
    {
        return NULL;
    }
    bool valid = false;
    valid = IsThisAValidLine(buf, endControlHit, atErrorCode);
    if(valid == true)
    {
        return buf;
    }
    else
    {
        return myStrTok(NULL, tokens, endControlHit, atErrorCode, savePtr);
    }
}

/**
 *
 * Reads an entire buffer, parses the buffer into individual lines, and places
 * these individual lines into the response array.
 *
 * The original command echo'd back is ignored. Any endControl items are also
 * ignored.
 *
 * If a timeout is specified greater than zero, it means you expect prompts in
 * the buffer.
 *
 * @param[out] atErrorCode The AT Error code received as Hex. 0 on success,
 *                       otherwise refer to Telegesis manual.
 * @param[out] responseArray Each individual line from read() is placed in this
                             array.
 * @param[out] count The number of items in the response array.
 * @param[in] command The AT command that was already issued. Used to ignore
 *                    when it is echo'd back in the read buffer.
 * @param[in] timeout If not zero, it is used as a flag to determine if
 *                    blocking on read() should occur.
 *
 */
TWResultCode ReadBuffer(uint64_t * atErrorCode,
                        char * responseArray[],
                        uint8_t * count,
                        const char * command,
                        uint8_t timeout)
{

    TWResultCode ret = TW_RESULT_OK;
    char readData[1000] = {};
    char itrData[200] = {};

    ssize_t readDataBytes = 0;
    ssize_t itrDataBytes = 0;
    bool endControlHit = false;
    uint8_t responseArrayPos = 0;
    while(itrDataBytes == 0)
    {
        itrDataBytes += read(g_fd, itrData, sizeof(itrData)-1);
        if(itrDataBytes < 0)
        {
            OC_LOG_V(ERROR, TAG, "Could not read from port. Errno is: %d", errno);
            return TW_RESULT_ERROR;
        }

        if(command)
        {
            size_t itrDataIndexLength = (strlen(itrData) - 1);
            if(strncmp(itrData, command, strlen(command)) == 0 &&
               itrData[itrDataIndexLength] == '\r')
            {
                memset(itrData, 0, sizeof(itrData));
                itrDataBytes = 0; // Ignore any single line which only echo'd
                                  // the command. This allows us to block on read().
            }
        }
        if(itrDataBytes > 0 &&
           readDataBytes < (ssize_t)sizeof(readData) &&
           (readDataBytes + itrDataBytes) <= (ssize_t)sizeof(readData))
        {
            readDataBytes += itrDataBytes;
            OICStrcat(readData, sizeof(readData), itrData);
            if(itrDataBytes == sizeof(itrData))
            {
reread:
                memset(itrData, 0, sizeof(itrData));
                itrDataBytes = 0;
            }
        }

        if(timeout != 0)
        {
            break; // Don't block on read as there are no prompts waiting in the buffer.
        }
    }
    if(readDataBytes > 0)
    {
        char * savePtr = NULL;
        char * singleLine = myStrTok(readData, "\r\n", &endControlHit, atErrorCode, &savePtr);
        while(singleLine)
        {
            // Stop on endControl items. Also leave function when an endControl is found.
            //singleLine = myStrTok(NULL, "\r", &endControlHit, atErrorCode, &savePtr);
            if(singleLine)
            {
                // Ignore the echo'd command line if it was passed in.
                if(!command || (command && strcmp(singleLine, command) != 0))
                {
                    responseArray[responseArrayPos] = (char *) OICMalloc((strlen(singleLine)+1) *
                                                      sizeof(char));
                    if(!responseArray[responseArrayPos])
                    {
                        return TW_RESULT_ERROR_NO_MEMORY;
                    }
                    memset(responseArray[responseArrayPos],
                           0,
                           (strlen(singleLine)+1) * sizeof(char));
                    strcpy(responseArray[responseArrayPos], singleLine);
                    responseArrayPos++;
                }
                singleLine = myStrTok(NULL, "\r\n", &endControlHit, atErrorCode, &savePtr);
            }
        }
        *count = responseArrayPos;
    }
    if(endControlHit == false && timeout == 0)
    {
       memset(readData, 0, sizeof(readData));
       readDataBytes = 0;
       goto reread;
    }
    return ret;
}

/**
 *
 * Writes AT command to buffer.
 *
 * @param[in] command The command to be written to the buffer.
 *
 */
TWResultCode WriteBuffer(const char * command)
{
    TWResultCode ret = TW_RESULT_OK;

    if(!command)
    {
        return TW_RESULT_ERROR_INVALID_PARAMS;
    }
    size_t sendCommandSize = (strlen(command) + 2) * sizeof(char);
    char * sendCommand = (char *) OICMalloc(sendCommandSize);
    if(!sendCommand)
    {
        return TW_RESULT_ERROR_NO_MEMORY;
    }
    memset(sendCommand, 0, sendCommandSize);
    sprintf(sendCommand, "%s\r", command);
    size_t expectedWrittenBytes = strlen(sendCommand);
    size_t actualWrittenBytes = write(g_fd, sendCommand, expectedWrittenBytes);
    if(actualWrittenBytes <= 0)
    {
        OC_LOG_V(ERROR, TAG, "Could not write to port. Errno is: %d", errno);
        ret =  TW_RESULT_ERROR;
        goto exit;
    }
    if(actualWrittenBytes != expectedWrittenBytes)
    {
        ret = TW_RESULT_ERROR;
        goto exit;
    }

exit:
    OICFree(sendCommand);
    return ret;
}

/**
 *
 * Deallocate items in the given arrays.
 *
 */
void CleanArray(char* responseArray[], char* promptArray[],
                uint8_t responseCount, uint8_t promptCount)
{
    OC_LOG_V(INFO, TAG, "Enter CleanArray().");

    if(responseCount > ARRAY_LENGTH || promptCount > ARRAY_LENGTH)
    {
        OC_LOG_V(ERROR, TAG, "Trying to access arrays out of bounds.");
        return;
    }

    OC_LOG_V(INFO, TAG, "Attempt to free responseArray and promptArray.");
    int i = 0;
    if (responseArray)
    {
        for (; i < responseCount; i++)
        {
            OC_LOG_V(INFO, TAG, "Free responseArray[%d]: %s", i, responseArray[i]);
            OICFree(responseArray[i]);
        }
    }

    if (promptArray)
    {
        i = 0;
        for (; i < promptCount; i++)
        {
            OC_LOG_V(INFO, TAG, "Free promptArray[%d]: %s", i, promptArray[i]);
            OICFree(promptArray[i]);
        }
    }

    OC_LOG_V(INFO, TAG, "Leave CleanArray().");
}

/**
 *
 * Print the given arrays.
 *
 */
void PrintArray(char* responseArray[], char* promptArray[],
                uint8_t responseCount, uint8_t promptCount)
{
    OC_LOG_V(INFO, TAG, "Enter PrintArray().");

    if(responseCount > ARRAY_LENGTH || promptCount > ARRAY_LENGTH)
    {
        OC_LOG_V(ERROR, TAG, "Trying to access arrays out of bounds.");
        return;
    }

    int n = 0;
    OC_LOG_V(INFO, TAG, "responseCount=%d", responseCount);
    for (; n < responseCount; n++)
    {
        OC_LOG_V(INFO, TAG, "responseArray[%d]=%s", n, responseArray[n]);
    }

    OC_LOG_V(INFO, TAG, "promptCount=%d", promptCount);
    for (n = 0; n < promptCount; n++)
    {
        OC_LOG_V(INFO, TAG, "promptArray[%d]=%s", n, promptArray[n]);
    }

    OC_LOG_V(INFO, TAG, "Leave PrintArray().");
}


/**
 *
 * Print the given device.
 *
 */
void PrintTWDevice(TWDevice* device)
{
    if (device != NULL)
    {
        OC_LOG_V(INFO, TAG, "Device - NodeID=%s", device->nodeId);
        OC_LOG_V(INFO, TAG, "Device - EPID=%s", device->endpointOfInterest->endpointId);
    }
    else
    {
        OC_LOG_V(ERROR, TAG, "Received a NULL device - CAN'T PRINT IT.");
    }
}

/**
 *
 * Print the given device list.
 *
 */
void PrintTWDeviceList(TWDeviceList* list)
{
    int i = 0;
    for (; i < list->count; i++)
    {
        PrintTWDevice(&list->deviceList[i]);
    }
}

/**
 *
 * Deallocate device list.
 *
 */
void DeallocateTWDeviceList()
{
    if (g_FoundMatchedDeviceList == NULL)
    {
        return;
    }

    if (g_FoundMatchedDeviceList->deviceList == NULL)
    {
        OICFree(g_FoundMatchedDeviceList);
        g_FoundMatchedDeviceList = NULL;
        return;
    }

    if (g_FoundMatchedDeviceList->deviceList->endpointOfInterest == NULL)
    {
        OICFree(g_FoundMatchedDeviceList->deviceList);
        g_FoundMatchedDeviceList->deviceList = NULL;

        OICFree(g_FoundMatchedDeviceList);
        g_FoundMatchedDeviceList = NULL;
        return;
    }

    if (g_FoundMatchedDeviceList->deviceList->endpointOfInterest->clusterList == NULL)
    {
        OICFree(g_FoundMatchedDeviceList->deviceList->endpointOfInterest);
        g_FoundMatchedDeviceList->deviceList->endpointOfInterest = NULL;

        OICFree(g_FoundMatchedDeviceList->deviceList);
        g_FoundMatchedDeviceList->deviceList = NULL;

        OICFree(g_FoundMatchedDeviceList);
        g_FoundMatchedDeviceList = NULL;
        return;
    }

    if (g_FoundMatchedDeviceList->deviceList->endpointOfInterest-> clusterList->clusterIds == NULL)
    {
        OICFree(g_FoundMatchedDeviceList->deviceList->endpointOfInterest->clusterList);
        g_FoundMatchedDeviceList->deviceList->endpointOfInterest->clusterList = NULL;

        OICFree(g_FoundMatchedDeviceList->deviceList->endpointOfInterest);
        g_FoundMatchedDeviceList->deviceList->endpointOfInterest = NULL;

        OICFree(g_FoundMatchedDeviceList->deviceList);
        g_FoundMatchedDeviceList->deviceList = NULL;

        OICFree(g_FoundMatchedDeviceList);
        g_FoundMatchedDeviceList = NULL;
        return;
    }

    OICFree(g_FoundMatchedDeviceList->deviceList->endpointOfInterest-> clusterList->clusterIds);
    g_FoundMatchedDeviceList->deviceList->endpointOfInterest->clusterList->clusterIds = NULL;

    OICFree(g_FoundMatchedDeviceList->deviceList->endpointOfInterest->clusterList);
    g_FoundMatchedDeviceList->deviceList->endpointOfInterest->clusterList = NULL;


    OICFree(g_FoundMatchedDeviceList->deviceList->endpointOfInterest);
    g_FoundMatchedDeviceList->deviceList->endpointOfInterest = NULL;

    OICFree(g_FoundMatchedDeviceList->deviceList);
    g_FoundMatchedDeviceList->deviceList = NULL;

    OICFree(g_FoundMatchedDeviceList);
    g_FoundMatchedDeviceList = NULL;
}
