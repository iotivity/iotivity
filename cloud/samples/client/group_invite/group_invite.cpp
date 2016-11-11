/* ****************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
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

#include <memory>
#include <iostream>
#include <stdexcept>
#include <condition_variable>
#include <map>
#include <vector>
#include <string>
#include <unistd.h>
#include <stdio.h>

#include "ocstack.h"
#include "ocpayload.h"

#include <OCApi.h>
#include <OCPlatform.h>

using namespace std;
using namespace OC;

void printRepresentation(OCRepresentation rep)
{
    for (auto itr = rep.begin(); itr != rep.end(); ++itr)
    {
        cout << "\t" << itr->attrname() << ":\t" << itr->getValueToString() << endl;
        if (itr->type() == AttributeType::Vector)
        {
            switch (itr->base_type())
            {
                case AttributeType::OCRepresentation:
                    for (auto itr2 : (*itr).getValue<vector<OCRepresentation> >())
                    {
                        printRepresentation(itr2);
                    }
                    break;

                case AttributeType::Integer:
                    for (auto itr2 : (*itr).getValue<vector<int> >())
                    {
                        cout << "\t\t" << itr2 << endl;
                    }
                    break;

                case AttributeType::String:
                    for (auto itr2 : (*itr).getValue<vector<string> >())
                    {
                        cout << "\t\t" << itr2 << endl;
                    }
                    break;

                default:
                    cout << "Unhandled base type " << itr->base_type() << endl;
                    break;
            }
        }
        else if (itr->type() == AttributeType::OCRepresentation)
        {
            printRepresentation((*itr).getValue<OCRepresentation>());
        }
    }
}

//tmp callback
void ocPost(const HeaderOptions & /*headerOptions*/,
            const OCRepresentation &rep, const int eCode)
{
    if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CHANGED)
    {
        cout << "\tRequest was successful: " << eCode << endl;

        printRepresentation(rep);
    }
    else
    {
        cout << "\tResponse error: " << eCode << endl;
    }
}

void onObserve(const HeaderOptions /*headerOptions*/, const OCRepresentation &rep,
               const int &eCode, const int &sequenceNumber)
{
    try
    {
        if (eCode == OC_STACK_OK && sequenceNumber <= MAX_SEQUENCE_NUMBER)
        {
            if (sequenceNumber == OC_OBSERVE_REGISTER)
            {
                cout << "Observe registration action is successful" << endl;
            }

            cout << "OBSERVE RESULT:" << endl;
            printRepresentation(rep);
        }
        else
        {
            if (eCode == OC_STACK_OK)
            {
                cout << "Observe registration failed or de-registration action failed/succeeded" << endl;
            }
            else
            {
                cout << "onObserve Response error: " << eCode << endl;
                exit(-1);
            }
        }
    }
    catch (exception &e)
    {
        cout << "Exception: " << e.what() << " in onObserve" << endl;
    }
}

void onDelete(const HeaderOptions & /*headerOptions*/,
              const int eCode)
{
    if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_DELETED)
    {
        cout << "\tDelete was successful" << endl;
    }
    else
    {
        cout << "\tDelete Response error: " << eCode << endl;
    }
}

condition_variable g_callbackLock;
string             g_uid;
string             g_accesstoken;

void handleLoginoutCB(const HeaderOptions &,
                      const OCRepresentation &rep, const int ecode)
{
    cout << "Auth response received code: " << ecode << endl;

    if (rep.getPayload() != NULL)
    {
        printRepresentation(rep);
    }

    if (ecode == 4)
    {
        g_accesstoken = rep.getValueToString("accesstoken");

        g_uid = rep.getValueToString("uid");
    }

    g_callbackLock.notify_all();
}

int main(int argc, char *argv[])
{
    if (argc != 4 && argc != 5)
    {
        cout << "Put \"[host-ipaddress:port] [authprovider] [authcode]\" for sign-up and sign-in"
             << endl;
        cout << "Put \"[host-ipaddress:port] [uid] [accessToken] 1\" for sign-in" <<
             endl;
        return 0;
    }

    PlatformConfig cfg
    {
        ServiceType::InProc,
        ModeType::Both,
        "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
        0,         // Uses randomly available port
        QualityOfService::LowQos
    };

    OCPlatform::Configure(cfg);

    OCStackResult result = OC_STACK_ERROR;

    string host = "coap+tcp://";
    host += argv[1];

    OCAccountManager::Ptr accountMgr = OCPlatform::constructAccountManagerObject(host,
                                       CT_ADAPTER_TCP);

    mutex blocker;
    unique_lock<mutex> lock(blocker);

    if (argc == 5)
    {
        accountMgr->signIn(argv[2], argv[3], &handleLoginoutCB);
        g_callbackLock.wait(lock);
    }
    else
    {
        accountMgr->signUp(argv[2], argv[3], &handleLoginoutCB);
        g_callbackLock.wait(lock);
        accountMgr->signIn(g_uid, g_accesstoken, &handleLoginoutCB);
        g_callbackLock.wait(lock);
    }

    cout << "---Group & Invite sample---" << endl;
    cout << "     1 - searchUser using user UUID" << endl;
    cout << "     2 - searchUser using email" << endl;
    cout << "     3 - searchUser using phone" << endl;
    cout << "     4 - deleteDevice" << endl;
    cout << "     5 - createGroup" << endl;
    cout << "     6 - observeGroup" << endl;
    cout << "     7 - getGroupList" << endl;
    cout << "     8 - deleteGroup" << endl;
    cout << "     9 - joinGroup" << endl;
    cout << "    10 - addDeviceToGroup" << endl;
    cout << "    11 - getGroupInfo" << endl;
    cout << "    12 - leaveGroup" << endl;
    cout << "    13 - deleteDeviceFromGroup" << endl;
    cout << "    14 - observeInvitation" << endl;
    cout << "    15 - sendInvitation" << endl;
    cout << "    16 - cancelInvitation" << endl;
    cout << "    17 - deleteInvitation" << endl;
    cout << "    18 - cancelObserveGroup" << endl;
    cout << "    19 - cancelObserveInvitation" << endl;
    cout << "    20 - exit" << endl;

    string cmd;
    string cmd2;

    while (true)
    {
        cin >> cmd;

        try
        {
            QueryParamsMap query;
            OCRepresentation rep;

            switch (atoi(cmd.c_str()))
            {
                case 1:
                    cout << "Put userUUID to search:" << endl;
                    cin >> cmd;
                    result = accountMgr->searchUser(cmd, &ocPost);
                    break;

                case 2:
                    cout << "Put email to search:" << endl;
                    cin >> cmd;
                    query["email"] = cmd;
                    result = accountMgr->searchUser(query, &ocPost);
                    break;

                case 3:
                    cout << "Put phone number to search:" << endl;
                    cin >> cmd;
                    query["phone"] = cmd;
                    result = accountMgr->searchUser(query, &ocPost);
                    break;

                case 4:
                    cout << "PUT deviceID to delete:";
                    cin >> cmd;
                    result = accountMgr->deleteDevice(cmd, &onDelete);
                    break;

                case 5:
                    result = accountMgr->createGroup(OC::AclGroupType::PUBLIC, &ocPost);
                    break;

                case 6:
                    cout << "PUT groupId to observe:";
                    cin >> cmd;
                    result = accountMgr->observeGroup(cmd, &onObserve);
                    break;

                case 7:
                    result = accountMgr->getGroupList(&ocPost);
                    break;

                case 8:
                    cout << "PUT groupId to delete:";
                    cin >> cmd;
                    result = accountMgr->deleteGroup(cmd, &onDelete);
                    break;

                case 9:
                    cout << "PUT groupId to join:";
                    cin >> cmd;
                    result = accountMgr->joinGroup(cmd, &ocPost);
                    break;

                case 10:
                    cout << "PUT groupId to add device:";
                    cin >> cmd;
                    cout << "PUT deviceId to add to group:";
                    cin >> cmd2;
                    {
                        vector<string> deviceIds;
                        deviceIds.push_back(cmd2);
                        result = accountMgr->addDeviceToGroup(cmd, deviceIds, &ocPost);
                    }
                    break;

                case 11:
                    cout << "PUT groupId to get info:";
                    cin >> cmd;
                    result = accountMgr->getGroupInfo(cmd, &ocPost);
                    break;

                case 12:
                    cout << "PUT groupId to leave:";
                    cin >> cmd;
                    result = accountMgr->leaveGroup(cmd, &onDelete);
                    break;

                case 13:
                    cout << "PUT groupId to remove device:";
                    cin >> cmd;
                    cout << "PUT deviceId to remove from group:";
                    cin >> cmd2;
                    {
                        vector<string> deviceIds;
                        deviceIds.push_back(cmd2);
                        result = accountMgr->deleteDeviceFromGroup(cmd, deviceIds, &onDelete);
                    }
                    break;

                case 14:
                    result = accountMgr->observeInvitation(&onObserve);
                    break;

                case 15:
                    cout << "PUT groupId to invite:";
                    cin >> cmd;
                    cout << "PUT userUUID to invite:";
                    cin >> cmd2;
                    result = accountMgr->sendInvitation(cmd, cmd2, &ocPost);
                    break;

                case 16:
                    cout << "PUT groupId to cancel invitation:";
                    cin >> cmd;
                    cout << "PUT userUUID to cancel invitation:";
                    cin >> cmd2;
                    result = accountMgr->cancelInvitation(cmd, cmd2, &onDelete);
                    break;

                case 17:
                    cout << "PUT groupId to delete invitation:";
                    cin >> cmd;
                    result = accountMgr->deleteInvitation(cmd, &onDelete);
                    break;

                case 18:
                    cout << "PUT groupId to cancel observe:";
                    cin >> cmd;
                    result = accountMgr->cancelObserveGroup(cmd);
                    break;

                case 19:
                    result = accountMgr->cancelObserveInvitation();
                    break;

                case 20:
                    goto exit;
                    break;
            }

            if (result != OC_STACK_OK)
            {
                cout << "Error, return code: " << result << endl;
            }
        }
        catch (exception e)
        {
            cout << "Precondition failed." << endl;
        }
    }

exit:
    return 0;
}
