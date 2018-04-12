/******************************************************************
*
* Copyright 2017 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "Configuration.h"
#include "CommonUtil.h"
#include "IotivityTest_Logger.h"

const int BUFFER_LENGTH = 1024;
Configuration g_configPropFile(COMMON_CONFIG_FILE);
string g_tcpServerIp = "127.0.0.1";
int g_port = 0;

void killConnection(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

int main(int argc, char *argv[])
{
    int socketDescriptor, nunmberOfBytes; /* Socket descriptor */
    struct sockaddr_in tcpServer; /* Echo server address */
    char buffer[BUFFER_LENGTH];
    int g_port = stoi(g_configPropFile.getProperties(TCP_SERVER_PORT_KEY));

#ifdef __LINUX__
    g_tcpServerIp = g_configPropFile.getProperties(TCP_SERVER_ETHERNET_IP_KEY);
#elif __TIZEN__
    g_tcpServerIp = g_configPropFile.getProperties(TCP_SERVER_WIFI_IP_KEY);
#endif

    /* Create a reliable, stream socket using TCP */
    if ((socketDescriptor = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        killConnection("socket() failed");
    }

    /* Construct the server address structure */
    memset(&tcpServer, 0, sizeof(tcpServer));
    tcpServer.sin_family = AF_INET;
    tcpServer.sin_addr.s_addr = inet_addr(g_tcpServerIp.c_str());
    tcpServer.sin_port = htons(g_port);

    /* Establish the connection to the tcp server */
    if (connect(socketDescriptor, (struct sockaddr *) &tcpServer, sizeof(tcpServer)) < 0)
    {
        killConnection("connect() failed");
    }
    else
    {
        IOTIVITYTEST_LOG(INFO, "[TCP Client] Connection Successful ");
    }

    while(true){
        IOTIVITYTEST_LOG(INFO, "[TCP Client] Enter Message : ");
        bzero(buffer, BUFFER_LENGTH);
        fgets(buffer, BUFFER_LENGTH-1, stdin);

        /* Send message to the server */
        nunmberOfBytes = write(socketDescriptor, buffer, strlen(buffer));

        if (nunmberOfBytes < 0)
        {
            IOTIVITYTEST_LOG(ERROR, "[TCP Client] Error writing to socket");
            exit(1);
        }
    }

    IOTIVITYTEST_LOG(INFO, "[TCP Client] Exiting TCP Client : ");
    return 0;
}
