//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

#include "ocsecurityconfig.h"
#include "logger.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define TAG "gen_sec_bin"

//scratch buffer
const int WORK_BUF_LEN = 512;

const char SERVER_CRED_FILE[] = "server_cred.bin";
const char CLIENT_CRED_FILE[] = "client_cred.bin";

static void printStruct(const char * device, OCSecConfigData* s)
{
    if (device && s)
    {
        OC_LOG(INFO, TAG, device);
        OC_LOG_V(INFO, TAG, "Version - %d", s->version);
        OC_LOG_V(INFO, TAG, "Number of blobs - %d", s->numBlob);

        OCSecBlob* osb = (OCSecBlob*)(s->blob);
        OC_LOG_V(INFO, TAG, "Blob Type - %d", osb->type);
        OC_LOG_V(INFO, TAG, "Blob Data Length - %d", osb->len);

        OCDtlsPskCredsBlob* odpcb = (OCDtlsPskCredsBlob*)(osb->val);
        OC_LOG(INFO, TAG, "My Identity :");
        OC_LOG_BUFFER(INFO, TAG, odpcb->identity, DTLS_PSK_ID_LEN);

        OC_LOG_V(INFO, TAG, "Number of trusted Peers  - %d", odpcb->num);
        OC_LOG(INFO, TAG, "Peer Identity :");
        OC_LOG_BUFFER(INFO, TAG, odpcb->creds[0].id, DTLS_PSK_ID_LEN);
        OC_LOG(INFO, TAG, "Peer Psk :");
        OC_LOG_BUFFER(INFO, TAG, odpcb->creds[0].psk, DTLS_PSK_PSK_LEN);
    }
}


static int SizeOfOCConfigData (OCSecConfigData *oscd)
{
    int len = 0;
    if(oscd)
    {
        int i = 0;
        OCSecBlob * osb;
        len = len + sizeof(OCSecConfigData) - sizeof(uint8_t);

        //go to first blob
        osb = (OCSecBlob*)(oscd->blob);
        for( i =0; i < oscd->numBlob; i++)
        {
            len += (sizeof(OCSecBlob) - sizeof(uint8_t) + osb->len);
            osb = config_data_next_blob(osb);
        }
    }
    return len;
}

int main()
{
    unsigned char buf_s[WORK_BUF_LEN];
    unsigned char buf_c[WORK_BUF_LEN];

    srand(time(NULL));

    OCSecConfigData * oscd_s = (OCSecConfigData*)buf_s;
    OCSecConfigData * oscd_c = (OCSecConfigData*)buf_c;
    oscd_s->version = oscd_c->version = OCSecConfigVer_CurrentVersion;

    //Only storing 1 blob of type 'OC_BLOB_TYPE_PSK'
    oscd_s->numBlob = oscd_c->numBlob = 1;

    OCSecBlob * osb_s = (OCSecBlob*)oscd_s->blob;
    OCSecBlob * osb_c = (OCSecBlob*)oscd_c->blob;
    osb_s->type = osb_c->type = OC_BLOB_TYPE_PSK;
    //length of this blob will be the length to contain PSK credentials
    // for '1' peer device
    osb_s->len = osb_c->len = sizeof(OCDtlsPskCredsBlob);

    OCDtlsPskCredsBlob * odpcb_s = (OCDtlsPskCredsBlob*)(osb_s->val);
    OCDtlsPskCredsBlob * odpcb_c = (OCDtlsPskCredsBlob*)(osb_c->val);

    odpcb_s->num = odpcb_c->num = 1;

    for(int i = 0; i < DTLS_PSK_ID_LEN; i++)
    {
        odpcb_c->creds[0].id[i] = odpcb_s->identity[i] = rand() % (2^8);

        odpcb_s->creds[0].id[i] = odpcb_c->identity[i] = rand() % (2^8);

        odpcb_c->creds[0].psk[i] = odpcb_s->creds[0].psk[i] = rand() % (2^8);
    }

    // Print Credentials
    printStruct("Server", oscd_s);
    printStruct("Client", oscd_c);

    // Write to files
    FILE* fps, *fpc;
    if ((fps = (FILE*) fopen("server_cred.bin", "wb")) != NULL)
    {
        fwrite(oscd_s, SizeOfOCConfigData(oscd_s), 1, fps);
        fclose(fps);
    }


    if ((fpc = (FILE*) fopen("client_cred.bin", "wb")) != NULL)
    {
        fwrite(oscd_c, SizeOfOCConfigData(oscd_c), 1, fpc);
        fclose(fpc);
    }

    struct stat st;
    memset(buf_s, 0, sizeof(buf_s));
    memset(buf_c, 0, sizeof(buf_c));
    // Read from files; print and verify manually
    if ((fps = (FILE*) fopen(SERVER_CRED_FILE, "rb")) != NULL)
    {
        stat(SERVER_CRED_FILE, &st);
        if ((sizeof(buf_s) < (unsigned int)st.st_size) ||
            (fread(buf_s, 1, st.st_size, fps) != (unsigned int)st.st_size))
        {
            OC_LOG(INFO, TAG, PCF("Reading from the file failed."));
        }
        fclose(fps);
    }


    if ((fpc = (FILE*) fopen(CLIENT_CRED_FILE, "rb")) != NULL)
    {
        stat(CLIENT_CRED_FILE, &st);
        if ((sizeof(buf_c) < (unsigned int)st.st_size) ||
            (fread(buf_c, 1, st.st_size, fpc) != (unsigned int)st.st_size))
        {
            OC_LOG(INFO, TAG, PCF("Reading from the file failed."));
        }
        fclose(fpc);
    }

    printf("\n\n");
    OC_LOG(INFO, TAG, PCF("Reading from file and printing again to verify manually"));
    printStruct("Server", (OCSecConfigData*)buf_s);
    printStruct("Client", (OCSecConfigData*)buf_c);

    return 1;
}


