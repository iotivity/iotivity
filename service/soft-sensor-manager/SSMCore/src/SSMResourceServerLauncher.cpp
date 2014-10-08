/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
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

/*
 * SSMResourceServerLauncher.cpp
 */
#include "SSMResourceServer.h"

string xmlDescription = "<SSMCore>"
        "<Device>"
        "<UDN>abcde123-31f8-11b4-a222-08002b34c003</UDN>"
        "<Name>MyPC</Name>"
        "<Type>PC</Type>"
        "</Device>"
        "</SSMCore>";

using namespace std;

int main(int argc, char* argv[])
{
    int inputKey = 0;

    try
    {
        SSMResourceServer ssmResourceServer;

        cout << "Initializing Resource Server" << endl;

        if (ssmResourceServer.initializeManager(xmlDescription) != 0)
        {
            cout << "SSM Resource Server init failed" << endl;
            return -1;
        }

        cout << "SSM Resource Server is working, press anykey to terminate" << endl;

        cin >> inputKey;

        cout << "SSM Resource Server is terminating" << endl;

        if (ssmResourceServer.terminateManager() != 0)
        {
            cout << "SSM Resource Server terminate failed" << endl;
        }
    }
    catch (std::exception e)
    {
        cout << e.what() << endl;
    }

    cout << "bye bye" << endl;
}
