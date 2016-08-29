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

#define maxSequenceNumber 0xFFFFFF

void printRepresentation(OCRepresentation rep)
{
    for (auto itr = rep.begin(); itr != rep.end(); ++itr)
    {
        cout << "\t" << itr->attrname() << ":\t" << itr->getValueToString() << endl;
    }
}

void onSearchUser(const HeaderOptions & /*headerOptions*/,
                  const OCRepresentation &rep, const int eCode)
{
    if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CHANGED)
    {
        cout << "\tSearchUser request was successful" << endl;

        std::vector<OCRepresentation> userList = rep.getValue<std::vector<OCRepresentation>>("ulist");

        for (auto user : userList)
        {
            // Mandatory field
            cout << "\tuid: " << user.getValue<string>("uid") << endl;

            // Optional field
            cout << "\tuinfo: " << endl;
            OCRepresentation uinfo = user.getValue<OCRepresentation>("uinfo");
            for (auto itr = uinfo.begin(); itr != uinfo.end(); ++itr)
            {
                cout << "\t\t" << itr->attrname() << ":\t" << itr->getValueToString() << endl;
            }
            cout << endl;
        }
    }
    else
    {
        cout << "\tSearchUser Response error: " << eCode << endl;
    }
}

//tmp callback
void ocPost(const HeaderOptions & /*headerOptions*/,
            const OCRepresentation &rep, const int eCode)
{
    if (eCode == OC_STACK_OK || eCode == OC_STACK_RESOURCE_CHANGED)
    {
        std::cout << "\tRequest was successful: " << eCode << std::endl;

        printRepresentation(rep);
    }
    else
    {
        std::cout << "\tResponse error: " << eCode << std::endl;
    }
}

void onObserve(const HeaderOptions /*headerOptions*/, const OCRepresentation &rep,
               const int &eCode, const int &sequenceNumber)
{
    try
    {
        if (eCode == OC_STACK_OK && sequenceNumber != maxSequenceNumber + 1)
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
                std::cout << "Observe registration failed or de-registration action failed/succeeded" << std::endl;
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

std::condition_variable g_callbackLock;
std::string             g_uid;
std::string             g_accesstoken;

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
    unique_lock<std::mutex> lock(blocker);

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
    cout << "     1 - searchUser" << endl;
    cout << "     2 - deleteDevice" << endl;
    cout << "     3 - createGroup" << endl;
    cout << "     4 - observeGroup" << endl;
    cout << "     5 - getGroupList" << endl;
    cout << "     6 - deleteGroup" << endl;
    cout << "     7 - joinGroup" << endl;
    cout << "     8 - addDeviceToGroup" << endl;
    cout << "     9 - getGroupInfo" << endl;
    cout << "    10 - leaveGroup" << endl;
    cout << "    11 - deleteDeviceFromGroup" << endl;
    cout << "    12 - observeInvitation" << endl;
    cout << "    13 - sendInvitation" << endl;
    cout << "    14 - cancelInvitation" << endl;
    cout << "    15 - deleteInvitation" << endl;
    cout << "    16 - cancelObserveGroup" << endl;
    cout << "    17 - cancelObserveInvitation" << endl;
    cout << "    18 - exit" << endl;

    string cmd;
    string cmd2;

    while (true)
    {
        cin >> cmd;

        QueryParamsMap query;
        OCRepresentation rep;

        switch (atoi(cmd.c_str()))
        {
            case 1:
                cout << "Put userUUID to search:" << endl;
                cin >> cmd;
                result = accountMgr->searchUser(cmd, &onSearchUser);
                break;

            case 2:
                cout << "PUT deviceID to delete:";
                cin >> cmd;
                result = accountMgr->deleteDevice(cmd, &onDelete);
                break;

            case 3:
                result = accountMgr->createGroup(OC::AclGroupType::PUBLIC, &ocPost);
                break;

            case 4:
                cout << "PUT groupId to observe:";
                cin >> cmd;
                result = accountMgr->observeGroup(cmd, &onObserve);
                break;

            case 5:
                result = accountMgr->getGroupList(&ocPost);
                break;

            case 6:
                cout << "PUT groupId to delete:";
                cin >> cmd;
                result = accountMgr->deleteGroup(cmd, &onDelete);
                break;

            case 7:
                cout << "PUT groupId to join:";
                cin >> cmd;
                result = accountMgr->joinGroup(cmd, &ocPost);
                break;

            case 8:
                cout << "PUT groupId to add device:";
                cin >> cmd;
                cout << "PUT deviceId to add to group:";
                cin >> cmd2;
                {
                    std::vector<std::string> deviceIds;
                    deviceIds.push_back(cmd2);
                    result = accountMgr->addDeviceToGroup(cmd, deviceIds, &ocPost);
                }
                break;

            case 9:
                cout << "PUT groupId to get info:";
                cin >> cmd;
                result = accountMgr->getGroupInfo(cmd, &ocPost);
                break;

            case 10:
                cout << "PUT groupId to leave:";
                cin >> cmd;
                result = accountMgr->leaveGroup(cmd, &onDelete);
                break;

            case 11:
                cout << "PUT groupId to remove device:";
                cin >> cmd;
                cout << "PUT deviceId to remove from group:";
                cin >> cmd2;
                {
                    std::vector<std::string> deviceIds;
                    deviceIds.push_back(cmd2);
                    result = accountMgr->deleteDeviceFromGroup(cmd, deviceIds, &onDelete);
                }
                break;

            case 12:
                result = accountMgr->observeInvitation(&onObserve);
                break;

            case 13:
                cout << "PUT groupId to invite:";
                cin >> cmd;
                cout << "PUT userUUID to invite:";
                cin >> cmd2;
                result = accountMgr->sendInvitation(cmd, cmd2, &ocPost);
                break;

            case 14:
                cout << "PUT groupId to cancel invitation:";
                cin >> cmd;
                cout << "PUT userUUID to cancel invitation:";
                cin >> cmd2;
                result = accountMgr->cancelInvitation(cmd, cmd2, &onDelete);
                break;

            case 15:
                cout << "PUT groupId to delete invitation:";
                cin >> cmd;
                result = accountMgr->deleteInvitation(cmd, &onDelete);
                break;

            case 16:
                cout << "PUT groupId to cancel observe:";
                cin >> cmd;
                result = accountMgr->cancelObserveGroup(cmd);
                break;

            case 17:
                result = accountMgr->cancelObserveInvitation();
                break;

            //113 is q
            case 18:
                goto exit;
                break;
        }

        if (result != OC_STACK_OK)
        {
            cout << "Error, return code: " << result << endl;
        }
    }

exit:
    return 0;
}