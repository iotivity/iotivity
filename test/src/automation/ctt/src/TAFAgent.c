/******************************************************************
 *
 * Copyright 2016 Granite River Labs All Rights Reserved.
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

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <ctype.h>
#include "ocstack.h"
#include "ocpayload.h"
#include "pinoxmcommon.h"
#include "cacommon.h"
#include "cainterface.h"
#include "platform_features.h"
#include "logger.h"
#include <time.h>
#include <DUTController.h>

//#defines
#define TAG                  "OCF_TAF_AGENT"
#define DEFAULT_CONTEXT_VALUE 0x99
#define SLEEP_TIME            2
#define JUSTWORKS_CLIENT      11
#define RANDOMPIN_CLIENT      12
#define MFG_CERT_CLIENT       13
#define JUSTWORKS_SERVER      21
#define RANDOMPIN_SERVER      22
#define MFG_CERT_SERVER       23

//macros
#define FREE(x)           if(x){free(x);x=NULL;}

//Globals
char gIUTSimulator[24] = "IUTSimulator";
int gIPDiscovered = 0;
pid_t gPid;
int gQuitFlag = 0;
int gRestartFlag = 0;
char *gConfigPath = NULL;
char *gConfigFileContent = NULL;
OCDoHandle gObserveHandle;
bool gSecured = false;
char gIp[25];
char gLocalIp[25];
char gPort[25];
int gFd;
char gPinValue[9];
static char s_DISCOVERY_QUERY[] = "%s/oic/res";
char gCommandsArray[10][255];

//Function declaration
void SendDiscovery();

/*Replace String*/
char *replaceString(const char *str, const char *from, const char *to)
{

    /* Adjust each of the below values to suit your needs. */

    /* Increment positions cache size initially by this number. */
    size_t cacheSizeInc = 16;
    /* Thereafter, each time capacity needs to be increased,
     * multiply the increment by this factor. */
    const size_t cacheSizeIncFactor = 3;
    /* But never increment capacity by more than this number. */
    const size_t cacheSizeIncMax = 1048576;

    char *pret, *ret = NULL;
    const char *pstr2, *pstr = str;
    size_t i, count = 0;
    ptrdiff_t *posCache = NULL;
    size_t cacheSize = 0;
    size_t cpyLen, orgLen, retLen, toLen, fromLen = strlen(from);

    /* Find all matches and cache their positions. */
    while ((pstr2 = strstr(pstr, from)) != NULL)
    {
        count++;

        /* Increase the cache size when necessary. */
        if (cacheSize < count)
        {
            cacheSize += cacheSizeInc;
            posCache = realloc(posCache, sizeof(*posCache) * cacheSize);
            if (posCache == NULL)
            {
                goto end_repl_str;
            }
            cacheSizeInc *= cacheSizeIncFactor;
            if (cacheSizeInc > cacheSizeIncMax)
            {
                cacheSizeInc = cacheSizeIncMax;
            }
        }

        posCache[count - 1] = pstr2 - str;
        pstr = pstr2 + fromLen;
    }

    orgLen = pstr - str + strlen(pstr);

    /* Allocate memory for the post-replacement string. */
    if (count > 0)
    {
        toLen = strlen(to);
        retLen = orgLen + (toLen - fromLen) * count;
    }
    else    retLen = orgLen;
    ret = malloc(retLen + 1);
    if (ret == NULL)
    {
        goto end_repl_str;
    }

    if (count == 0)
    {
        /* If no matches, then just duplicate the string. */
        strcpy(ret, str);
    }
    else
    {
        /* Otherwise, duplicate the string whilst performing
         * the replacements using the position cache. */
        pret = ret;
        memcpy(pret, str, posCache[0]);
        pret += posCache[0];
        for (i = 0; i < count; i++)
        {
            memcpy(pret, to, toLen);
            pret += toLen;
            pstr = str + posCache[i] + fromLen;
            cpyLen = (i == count - 1 ? orgLen : posCache[i + 1]) - posCache[i] - fromLen;
            memcpy(pret, pstr, cpyLen);
            pret += cpyLen;
        }
        ret[retLen] = '\0';
    }

end_repl_str:
    /* Free the cache and return the post-replacement string,
     * which will be NULL in the event of an error. */
    FREE(posCache);
    return ret;
}


/*Function to get the ip address of the machine where TAF is running*/
void GetIP()
{
    //Works only on ubuntu
    FILE *fp = popen("hostname -I", "r");
    fscanf(fp, "%s", gLocalIp);
    pclose(fp);
    OIC_LOG_V(INFO, TAG, "Local IP address: %s", gLocalIp);

}

/*Send command to the IUT simulator*/
void SendCommand(char *cmd)
{

    if (cmd != NULL)
    {
        char searchCmd[256] = "xdotool search --name \"";
        strcat(searchCmd, gIUTSimulator);
        strcat(searchCmd, "\" windowactivate");
        char command[128] = "xdotool type --delay 1 --clearmodifiers ";
        strcat(command, cmd);
        system(searchCmd);
        system(command);
        system("xdotool key --clearmodifiers Return");
        OIC_LOG_V(INFO, TAG, "Command %s", searchCmd);
    }
}

/*Find the value of the string in the log file*/
void FindValue(char *searchString, char *value)
{
    FILE *fp1 = fopen("IUTSimulator.log", "r");
    char *line = NULL;
    int i, j;
    size_t len = 0;
    ssize_t read;
    int lineNo = 0;
    if (fp1)
    {
        OIC_LOG(DEBUG,TAG, "Inside File");
        while ((read = getline(&line, &len, fp1)) != -1)
        {
            ++lineNo;
            if ( (strstr(line, searchString) != NULL) &&
                 (strstr(line , ":") != NULL)
               )
            {
                OIC_LOG_V(INFO, TAG, "%s", line);
                char *pos = strstr(line, ":");
                char data;
                for (i = 1, j = 0; * (pos + i) != '\0'; i++)
                {
                    data = *(pos + i);
                    if (!isspace(data))
                    {
                        OIC_LOG_V(INFO, TAG, "data=%c",data);
                        *(value + j) = data;
                        j++;
                    }
                }
                *(value + j) = '\0';

                OIC_LOG_V(INFO, TAG, "value=%s", value);
            }

        }
        fclose(fp1);

    }
}

/*Find the key in the string*/
void FindKey(char *searchString, char *key)
{
    FILE *fp1 = fopen("IUTSimulator.log", "r");
    char *line = NULL;
    int i, j;
    size_t len = 0;
    ssize_t read;
    int lineNo = 0;
    if (fp1)
    {
        OIC_LOG(DEBUG,TAG, "Inside File");
        while ((read = getline(&line, &len, fp1)) != -1)
        {
            ++lineNo;
            if ( (strstr(line, ". ") != NULL) &&
                 (strstr(line , "-") != NULL) &&
                 (strstr(line , searchString) != NULL))
            {
                OIC_LOG_V(INFO, TAG, "%s", line);
                char *pos = strstr(line, ".");
                char data;
                for (i = 0, j = 0; (i < 8) && (line[i] != *pos); i++)
                {
                    data = *(line + i);
                    if (!isspace(data))
                    {
                        OIC_LOG_V(INFO, TAG, "data=%c",data);
                        *(key + j) = data;
                        j++;
                    }
                }
                *(key + j) = '\0';

                OIC_LOG_V(INFO, TAG, "value=%s", key);
            }
            OIC_LOG_V(DEBUG, TAG, "line=%d",lineNo);

        }
        fclose(fp1);

    }
}


/*Send command to the IUT Simulator in bulk*/
void SendCommands(int commandLength)
{
    OIC_LOG_V(DEBUG, TAG, "Inside SendCommand");
    sleep(5 * SLEEP_TIME);
    for (int i = 0; i < commandLength; i++)
    {
        SendCommand(gCommandsArray[i]);
        sleep(SLEEP_TIME);
    }
}

/*XML Doc Ptr Clean*/
void CleanXmlDocPtr(xmlDocPtr doc)
{
    if(doc)
        xmlFreeDoc(doc);
}

/*XML action handler*/
xmlDocPtr ActionHandler(xmlDocPtr doc)
{

    char *result = (char *)"";
    char val[8], value[128];
    char stringToChange[6] = "value";

    const char *messageXml = docToString(doc);
    if (strstr(messageXml, "<message>Waiting for CoAP response... ") != NULL)
    {
        //SKIP
    }
    else if ( strstr(messageXml , "<message>Please enter PIN:") != NULL)
    {
        // Get PIN generated by the IUTSimulator
        OIC_LOG(INFO, TAG, "Please enter PIN:");
        FindValue("PIN CODE : ", gPinValue);
        OIC_LOG_V(INFO, TAG, "PIN=%s", gPinValue);

        asprintf(&result,
                 "<actionResult><userDialogResponse><answer>Set_Value</answer><data>%s</data></userDialogResponse></actionResult>",
                 gPinValue);
        OIC_LOG_V(DEBUG, TAG, "TAF==Send response %s", result);
    }
    else if ( strstr(messageXml,
                     "<message>Please initiate device to revert to \"ready for OTM\" state") != NULL ||
              strstr(messageXml, "<message>Please reset DUT's ACL in order to have empty list.") != NULL)
    {
        // Reset the IUTSimulator
        gRestartFlag = 1;
        asprintf(&result,
                 "<actionResult><userDialogResponse><answer>OK</answer></userDialogResponse></actionResult>");
    }
    else if ( strstr(messageXml, "<message>Please cycle power on the IUT") != NULL)
    {
        // Reset the IUTSimulator
        gRestartFlag = 1;
        asprintf(&result,
                 "<actionResult><userDialogResponse><answer>OK</answer></userDialogResponse></actionResult>");
    }
    else if (strstr(messageXml, "<message>Please change some value in /BinarySwitchResURI resource") !=
             NULL)
    {
        //asprintf(&result, "<actionResult><userDialogResponse><answer>Skip</answer></userDialogResponse></actionResult>");
        if (strstr(messageXml, "and press OK") != NULL)
        {
            sleep(10);
            asprintf(&result,
                     "<actionResult><userDialogResponse><answer>OK</answer></userDialogResponse></actionResult>");
        }
        else
        {

            strcpy(gCommandsArray[0], "34");
            strcpy(gCommandsArray[1], "1");
            strcpy(gCommandsArray[2], "value");
            strcpy(gCommandsArray[3], "4");
            strcpy(gCommandsArray[4], "0");
            SendCommands(5);
        }
    }
    else if (strstr(messageXml, "<message>Did IUT receive response:") != NULL)
    {

        asprintf(&result,
                 "<actionResult><userDialogResponse><answer>Yes</answer></userDialogResponse></actionResult>");
    }
    else if (strstr(messageXml, "<message>Did IUT received NOTIFY:") != NULL)
    {
        asprintf(&result,
                 "<actionResult><userDialogResponse><answer>Yes</answer></userDialogResponse></actionResult>");

    }
    else if (strstr(messageXml, "<message>Please change some value in /AirFlowResURI resource") !=
             NULL)
    {
        //asprintf(&result, "<actionResult><userDialogResponse><answer>Skip</answer></userDialogResponse></actionResult>");
        if (strstr(messageXml, "and press OK") != NULL)
        {
            sleep(10);
            asprintf(&result,
                     "<actionResult><userDialogResponse><answer>OK</answer></userDialogResponse></actionResult>");
        }
        else
        {
            strcpy(gCommandsArray[0], "34");
            strcpy(gCommandsArray[1], "5");
            strcpy(gCommandsArray[2], "speed");
            strcpy(gCommandsArray[3], "1");
            strcpy(gCommandsArray[4], "25");
            SendCommands(5);

        }
    }

    else if (strstr(messageXml, "<message>Please change some value in /TemperatureResURI resource") !=
             NULL)
    {
        //asprintf(&result, "<actionResult><userDialogResponse><answer>Skip</answer></userDialogResponse></actionResult>");
        if (strstr(messageXml, "and press OK") != NULL)
        {
            OIC_LOG(INFO, TAG, "Temperature URI Ok");
            sleep(10);
            asprintf(&result,
                     "<actionResult><userDialogResponse><answer>OK</answer></userDialogResponse></actionResult>");
        }
        else
        {
            OIC_LOG(INFO, TAG, "Temperature URI");
            strcpy(gCommandsArray[0], "34");
            strcpy(gCommandsArray[1], "3");
            strcpy(gCommandsArray[2], "temperature");
            strcpy(gCommandsArray[3], "3");
            strcpy(gCommandsArray[4], "25.5");
            SendCommands(5);
        }
    }
    else if (strstr(messageXml,
                    "<message>Please send a multicast discovery request message (i.e. CoAP GET) to") != NULL)
    {
        SendCommand("12");
        sleep(SLEEP_TIME);
    }
    else if (strstr(messageXml, " send a unicast RETRIEVE request (i.e. CoAP GET) to ") != NULL)
    {
        if (strstr(messageXml, "/BinarySwitchResURI") != NULL)
        {
            OIC_LOG(INFO, TAG, "Inside /BinarySwitchResURI");
            SendCommand("12");
            sleep(2 * SLEEP_TIME);
            SendCommand("17");
            FindKey("/BinarySwitchResURI", val);
            SendCommand(val);
            sleep(SLEEP_TIME);
        }
        else if (strstr(messageXml, "/TemperatureResURI") != NULL)
        {
            OIC_LOG(INFO, TAG, "Inside /TemperatureResURI");
            SendCommand("12");
            sleep(2 * SLEEP_TIME);
            SendCommand("17");
            FindKey("/TemperatureResURI", val);
            SendCommand(val);
            sleep(SLEEP_TIME);
        }
        else
        {
            asprintf(&result,
                     "<actionResult><userDialogResponse><answer>CANCEL</answer></userDialogResponse></actionResult>");
        }
    }
    else if (strstr(messageXml, "send a unicast partial UPDATE request") != NULL)
    {
        sleep(2 * SLEEP_TIME);
        if (strstr(messageXml, "/BinarySwitchResURI") != NULL)
        {
            OIC_LOG(INFO, TAG, "Inside Switch");
            SendCommand("12");
            sleep(2 * SLEEP_TIME);
            SendCommand("17");
            FindKey("/BinarySwitchResURI", val);
            SendCommand(val);
            sleep(SLEEP_TIME);
            FindValue(stringToChange, value);
            SendCommand("22");
            FindKey("/BinarySwitchResURI", val);
            SendCommand(val);
            sleep(SLEEP_TIME);
            SendCommand(stringToChange);
            sleep(SLEEP_TIME);
            SendCommand("4");
            sleep(SLEEP_TIME);
            if (strcmp(value, "false") == 0)
            {
                SendCommand("1");
            }
            else if (strcmp(value, "true") == 0)
            {
                SendCommand("0");

            }
            sleep(SLEEP_TIME);
        }
        else if (strstr(messageXml, "/TemperatureResURI") != NULL)
        {
            OIC_LOG(INFO, TAG, "Inside Switch");
            SendCommand("12");
            sleep(2 * SLEEP_TIME);
            SendCommand("22");
            FindKey("/TemperatureResURI", val);
            SendCommand(val);
            sleep(SLEEP_TIME);
            SendCommand("temperature");
            sleep(SLEEP_TIME);
            SendCommand("3");
            sleep(SLEEP_TIME);
            SendCommand("22.5");
            sleep(SLEEP_TIME);

        }
        else
        {
            asprintf(&result,
                     "<actionResult><userDialogResponse><answer>CANCEL</answer></userDialogResponse></actionResult>");
        }
    }
    else if (strstr(messageXml, " send a unicast request message (") != NULL)
    {
        OIC_LOG(INFO, TAG, "Inside unicast request");
        if (strstr(messageXml, "/BinarySwitchResURI") != NULL)
        {
            sleep(SLEEP_TIME);
            SendCommand("12");
            sleep(2 * SLEEP_TIME);
            if (strstr(messageXml, "with observe option = 0") != NULL)
            {
                SendCommand("25");
            }
            else if (strstr(messageXml, "with observe option = 1") != NULL)
            {
                SendCommand("25");
                FindKey("/BinarySwitchResURI", val);
                SendCommand(val);
                sleep(SLEEP_TIME);
                SendCommand("26");
            }
            FindKey("/BinarySwitchResURI", val);
            SendCommand(val);
            sleep(SLEEP_TIME);
        }
        else if (strstr(messageXml, "/TemperatureResURI") != NULL)
        {
            SendCommand("12");
            sleep(2 * SLEEP_TIME);
            if (strstr(messageXml, "with observe option = 0") != NULL)
            {
                SendCommand("25");
            }
            else if (strstr(messageXml, "with observe option = 1") != NULL)
            {
                SendCommand("26");
            }
            FindKey("/TemperatureResURI", val);
            SendCommand(val);
            sleep(SLEEP_TIME);
        }
    }
    else if (strstr(messageXml,
                    "<message>Does IUT have the possibility to display received properties values?") != NULL)
    {
#if 0
        asprintf(&result, "<actionResult><userDialogResponse><answer>No</answer></userDialogResponse></actionResult>");
#endif
        asprintf(&result,
                 "<actionResult><userDialogResponse><answer>Yes</answer></userDialogResponse></actionResult>");
    }
    else if (strstr(messageXml,
                    "<message>Does IUT has discovered a device (i.e. the CTT) with the expected Resource Types?") !=
             NULL)
    {
        asprintf(&result,
                 "<actionResult><userDialogResponse><answer>Yes</answer></userDialogResponse></actionResult>");
    }
    else if (strstr(messageXml, "<message>Does IUT presents following properties values?") != NULL)
    {
        asprintf(&result,
                 "<actionResult><userDialogResponse><answer>Yes</answer></userDialogResponse></actionResult>");
    }
    else
    {
        OIC_LOG_V(DEBUG, TAG, "TAF==Received not handled POST /actions: %s", messageXml);
    }
    return stringToDoc(result);

}

/*Abort Signal Handler*/
xmlDocPtr AbortSignalHandler(xmlDocPtr doc)
{
    OIC_LOG_V(DEBUG, TAG, "TAF==Received GET /abortSignal:");
    return stringToDoc("<abortSignal><abortTestcase>false</abortTestcase><abortTestRun>false</abortTestRun></abortSignal>");

}

/*Configuration handler*/
xmlDocPtr ConfigurationHandler(xmlDocPtr doc)
{
    OIC_LOG(INFO , TAG, "Inside Configuration Handler");
    if (gConfigFileContent == NULL)
    {
        long length;
        FILE *f = fopen (gConfigPath, "rb");
        if (f)
        {
            OIC_LOG(INFO, TAG, "FILE OPENED");
            fseek (f, 0, SEEK_END);
            length = ftell (f);
            fseek (f, 0, SEEK_SET);
            gConfigFileContent = malloc (length);

            if (gConfigFileContent )
            {
                fread (gConfigFileContent , 1, length, f);
            }
            fclose (f);
        }
        else
        {
            OIC_LOG_V(ERROR, TAG, "Could not open file %s", gConfigFileContent);
            return stringToDoc("");
        }
    }
    uint32_t tempSize = 0;
    char *result = gConfigFileContent;
    char *tmp = replaceString(gConfigFileContent, "%IP%", gIp);
    result = replaceString(tmp, "%PORT%", gPort);
    return stringToDoc(result);
}

/*Init TAF Agent*/
void  InitTAF()
{

    OIC_LOG(INFO, TAG, "TAF Init Start");

    initDutControllers();
    addRouteBasic("POST", "/actions", ActionHandler, CleanXmlDocPtr);
    addRouteBasic("GET", "/abortSignal", AbortSignalHandler, CleanXmlDocPtr);
    if (gConfigPath != NULL)
    {
        addRouteSetup("GET", "/processes/ctt/configuration", ConfigurationHandler, CleanXmlDocPtr);
    }
    startDutControllerBasic("0.0.0.0", 32000);
    startDutControllerExtended("0.0.0.0", 32001);
    startDutControllerSetup("0.0.0.0", 32002);

    OIC_LOG(INFO, TAG, "TAF Init Done");
}


/*Init the IUT Simulator*/
void initIUT(int option)
{

    char app[255] = "IUTSimulator 1 4";

    switch (option)
    {

        case JUSTWORKS_CLIENT:
            strcat(app, " 11");
            break;

        case RANDOMPIN_CLIENT:
            strcat(app, " 12");
            break;

        case MFG_CERT_CLIENT:
            strcat(app, " 13");
            break;

        case JUSTWORKS_SERVER:
            strcat(app, " 21");
            break;

        case RANDOMPIN_SERVER:
            strcat(app, " 22");
            break;

        case MFG_CERT_SERVER:
            strcat(app, " 23");
            break;

        default:
            strcat(app, " -1");
            break;

    }
    char parameters[255] = "result=1\nwhile [ $result -ne 0 ]; do \nxterm -title \"";
    strcat(parameters, gIUTSimulator);
    strcat(parameters, " ./");
    strcat(parameters, app);
    strcat(parameters, " | tee IUTSimulator.log\"\n result=$?\ndone");
    OIC_LOG_V(DEBUG, TAG, "Command Executed:%s", parameters);
    if ((gPid = fork()) < 0)
    {
        OIC_LOG(ERROR, TAG, "Fork failed");
        exit(1);
    }
    if (gPid == 0)
    {
        execlp("/bin/sh", "/bin/sh", "-c", parameters, NULL);
        sleep(5);
        exit(0);
    }

    char value[255] = "104";
    if ((option == JUSTWORKS_SERVER) || (option == RANDOMPIN_SERVER))
    {
        // send a value to the child
        OIC_LOG_V(DEBUG, TAG, "Sending %s to IUTSimulator\n", value);
        sleep(1);
        SendCommand(value);
        sleep(3);
    }

}

/*Callback function on discovery*/
OCStackApplicationResult HandleDiscover(void *ctx,
                                        OCDoHandle handle,
                                        OCClientResponse *clientResponse)
{
    if (!clientResponse)
    {
        OIC_LOG(ERROR, TAG, "Payload is NULL, No resources found");
        return OC_STACK_DELETE_TRANSACTION;
    }
    if (!gIPDiscovered)
    {

        if (!strcmp(gLocalIp, clientResponse->devAddr.addr))
        {
            OIC_LOG_V(INFO, TAG,
                      "Device =============> Discovered @ %s:%d",
                      clientResponse->devAddr.addr,
                      clientResponse->devAddr.port);

            strcpy(gIp, clientResponse->devAddr.addr);
            sprintf(gPort, "%d", clientResponse->devAddr.port);
            OIC_LOG_V(INFO, TAG, "Port: %s", gPort);
            gIPDiscovered = 1;
            if (OCStop() != OC_STACK_OK)
            {
                OIC_LOG(ERROR, TAG, "OCStack process error");
                return OC_STACK_DELETE_TRANSACTION;
            }
            else
            {
                OIC_LOG(INFO, TAG, "OC Stack Stopped");
                InitTAF();
            }
        }
    }
    return OC_STACK_KEEP_TRANSACTION;

}

/*Send Discovery to discover the OCF Devices*/
void SendDiscovery()
{
    OCCallbackData cbData = {NULL, NULL, NULL};
    OCStackResult ret;
    char queryUri[200];
    char ipaddr[100] = {'\0'};

    snprintf(queryUri, sizeof(queryUri), s_DISCOVERY_QUERY, ipaddr);

    cbData.cb = HandleDiscover;
    cbData.context = (void *)DEFAULT_CONTEXT_VALUE;

    //Start a discovery query
    OIC_LOG_V(INFO, TAG, "Resource Discovery:%s\n", queryUri);

    ret = OCDoResource(NULL, OC_REST_DISCOVER, queryUri, NULL, 0, CT_DEFAULT, OC_LOW_QOS, &cbData, NULL,
                       0);
    if (ret != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "Device Discovery error");
    }
    else
    {
        OIC_LOG(INFO, TAG, "Device Discovery Success");
    }
}


/*Stop the IUTSimulator*/
void stopIUT()
{
    char value[255] = "0";
    printf("Sending %s to IUTSimulator\n", value);
    SendCommand(value);
}

/*Discovery the IUT Simulator port*/
void discoverIUT()
{
    if (OCInit1(OC_CLIENT, OC_IP_USE_V4, OC_IP_USE_V4) != OC_STACK_OK)
    {
        OIC_LOG(ERROR, TAG, "OCStack init error");
        exit(0);

    }
    SendDiscovery();
    do
    {
        if (OCProcess() != OC_STACK_OK)
        {
            fprintf(stdout, "OCStack process error\n");
            fflush(stdout);
            exit(0);
        }
    }
    while (!gIPDiscovered);
}

/*SIGINT handler: set gQuitFlag to 1 for graceful termination*/
void HandleSigInt(int signum)
{
    if ((signum == SIGINT) || (signum == SIGTERM) || (signum == SIGTSTP))
    {
        gQuitFlag = 1;
        gRestartFlag = 1;
        stopIUT();
    }

}

/*Main Entry point of the code*/
int main(int argc, char **argv)
{
    int option = 25;
    GetIP();

    // get the configuration file path
    if (argc >= 3)
    {
        if (strcmp("--conf-path", argv[1]) == 0)
        {
            gConfigPath = argv[2];
            FILE *fp;

            fp = fopen(gConfigPath, "r");
            if (fp == NULL)
            {
                OIC_LOG_V(ERROR, TAG, "Could not find file %s", gConfigPath);
                return -1;
            }
            fclose(fp);
        }
    }

    // get the option to start IUT Simulator
    if (argc >= 4)
    {
        option = atoi(argv[3]);
    }
    printf("option: %d",option);

    signal(SIGINT, HandleSigInt);

    while (!gQuitFlag)
    {
        initIUT(option);
        discoverIUT();

        OIC_LOG(INFO, TAG, "Entering TAF Agent main loop...");
        while (!gRestartFlag)
        {

        }
        OIC_LOG(INFO, TAG, "Exiting TAF Agent main loop...");

        stopIUT();
        stopDutControllers();
        disposeDutControllers();
        FREE(gConfigFileContent);
        gIPDiscovered = 0;
        gRestartFlag = 0;
    }

    return 0;
}
