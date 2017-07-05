/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
#include <iostream>
#include <CommonApi.h>
#include <LightDevice.h>
#include <ResourceQuery.h>

using namespace std;
using namespace OIC::Service::SH;

class MyBinarySwitchDelegate: public BinarySwitchResourceDelegate
{
public:
    virtual ~MyBinarySwitchDelegate() {};
public:
    virtual ResultCode turnOnCallback();
    virtual ResultCode turnOffCallback();
};

ResultCode MyBinarySwitchDelegate::turnOnCallback()
{
    std::cout << "[LightServerSample][BinarySwitch] turnOnCallback.." << endl;
    return SUCCESS;
}

ResultCode MyBinarySwitchDelegate::turnOffCallback()
{
    std::cout << "[LightServerSample][BinarySwitch] turnOffHandler.." << endl;
    return SUCCESS;
}

void print()
{
    std::cout << "==============================================================" << std::endl;
    std::cout << "0. print menu" << std::endl;
    std::cout << "1. turn on switch" << std::endl;
    std::cout << "2. turn off switch" << std::endl;
    std::cout << "9. Quit" << std::endl;
    std::cout << "===============================================================" << std::endl;
    std::cout << "cmd>";
}

static LightDevice *myLight = NULL;
static MyBinarySwitchDelegate *switchDelegate = NULL;

int main()
{
    std::cout << "#1. Create Predefined Light Device" << endl;
    myLight = new LightDevice;
    std::cout << "#2. Set MyBinarySwitchDelegate" << endl;
    switchDelegate = new MyBinarySwitchDelegate;
    myLight->m_binarySwitch.setDelegate(switchDelegate);

    bool quitflag = false;
    int cmd;
    print();
    while (!quitflag)
    {
        std::cin >> cmd;

        if (0 == cmd)
        {
            print();
        }
        else if (1 == cmd)
        {
            myLight->m_binarySwitch.setState(true);
        }
        else if (2 == cmd)
        {
            myLight->m_binarySwitch.setState(false);
        }
        else if (9 == cmd)
        {
            quitflag = true;
        }
        else
        {
            std::cout << "Unknown cmd." << std::endl;
            cmd = 0;
            std::cin.clear();
            std::cin.ignore(1024, '\n');
        }
    }

    delete (myLight);
    delete (switchDelegate);

    return 0;
}
