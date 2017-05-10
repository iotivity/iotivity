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
#include <cstdlib>
#include <BlindDevice.h>

using namespace std;
using namespace OIC::Service::SH;

class MyOpenLevelResourceDelegate: public OpenLevelResourceDelegate
{
public:
    virtual ~MyOpenLevelResourceDelegate() {};
public:
    virtual ResultCode onChangeLevel(int level);
};

ResultCode MyOpenLevelResourceDelegate::onChangeLevel(int level)
{
    std::cout << "[BlindDevice][OpenLevelResource] onChangeLevel.." << endl;
    return SUCCESS;
}

static BlindDevice *myDevice = NULL;
static MyOpenLevelResourceDelegate *delegate = NULL;

void print()
{
    std::cout << "==============================================================" << std::endl;
    std::cout << "0. print menu" << std::endl;
    std::cout << "1. change open level" << std::endl;
    std::cout << "2. change increment" << std::endl;
    std::cout << "3. change range" << std::endl;
    std::cout << "9. Quit" << std::endl;
    std::cout << "===============================================================" << std::endl;
    std::cout << "cmd>";
}

int main()
{
    std::cout << "#1. Create Predefined Blind Device" << endl;
    myDevice = new BlindDevice;

    std::cout << "#2. Set MyOpenLevelResourceDelegate" << endl;
    delegate = new MyOpenLevelResourceDelegate;
    myDevice->m_openLevel.setDelegate(delegate);

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
            int level;
            std::cout << "Level: ";
            std::cin >> level;
            myDevice->m_openLevel.setLevel(level);
        }
        else if (2 == cmd)
        {
            int increment;
            std::cout << "Increment: ";
            std::cin >> increment;
            myDevice->m_openLevel.setIncrement(increment);
        }
        else if (3 == cmd)
        {
            int min;
            int max;
            std::cout << "Min Value: ";
            std::cin >> min;
            std::cout << "Max Value: ";
            std::cin >> max;
            myDevice->m_openLevel.setRange(min, max);
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

    delete (myDevice);
    delete (delegate);

    return 0;
}