/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
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
 *
 ******************************************************************/

#include "NSCommonHelper.h"

NSCommonHelper* NSCommonHelper::s_NSCommonHelperInstance = NULL;
std::mutex NSCommonHelper::s_mutex;
string NSCommonHelper::log = "";

const char * FIFO_INPUT_FILE = "/tmp/inputFifo";
const char * PROVIDER_FIFO = "/tmp/providerFifo";
const char * CONSUMER_FIFO = "/tmp/consumerFifo";

int g_FileRead, g_FileWrite;

NSCommonHelper::NSCommonHelper()
{
    m_isProvider = false;
}

NSCommonHelper::~NSCommonHelper()
{
}

NSCommonHelper* NSCommonHelper::getInstance(void)
{
    if (s_NSCommonHelperInstance == NULL)
    {
        s_mutex.lock();

        s_NSCommonHelperInstance = new NSCommonHelper();

        s_mutex.unlock();
    }

    return s_NSCommonHelperInstance;
}

void NSCommonHelper::initPipe(bool isProvider)
{
    m_isProvider = isProvider;

    mkfifo(FIFO_INPUT_FILE, 0666);

    g_FileWrite = open(FIFO_INPUT_FILE, O_WRONLY);

    if (isProvider) {
        g_FileRead = open(PROVIDER_FIFO, O_RDONLY);
    } else {
        g_FileRead = open(CONSUMER_FIFO, O_RDONLY);
    }


}

void NSCommonHelper::closePipe()
{
    close(g_FileRead);
    close(g_FileWrite);

    unlink(FIFO_INPUT_FILE);

}

void NSCommonHelper::loggerReader()
{
    char inputBuf[MAX_BUF];

    while (true)
    {
        ssize_t rd = read(g_FileRead, inputBuf, MAX_BUF);

        cout << "********** Log: " << inputBuf << endl;

        if (rd == 0)
        {
            break;
        }

        if (rd != -1)
        {
            log = log + string(inputBuf);
            break;
        }
    }
}

void NSCommonHelper::inputMenu(const char input[])
{
    char in[3];

    for (int i=0; i<3; i++)
    {
        in[i] = input[i];
    }
    IOTIVITYTEST_LOG(INFO, "Sending input to app: %s", in);
    write(g_FileWrite, &in, sizeof(in));

    sleep(WAIT_TIME_MIN);

    loggerReader();
}

string NSCommonHelper::getAppLog()
{
    return log;
}
