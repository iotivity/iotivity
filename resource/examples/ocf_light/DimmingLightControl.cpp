/*
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Copyright 2018 Intel Corporation All Rights Reserved.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */
#include "DimmingLightControl.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

//using namespace std;

DimmingLightControl::DimmingLightControl(DimmingLightServer& dimmingLightServer):
    m_quit{false},
    m_dimmingLightServer(dimmingLightServer)
{ }

DimmingLightControl::~DimmingLightControl(void) { }

void DimmingLightControl::process(void)
{
    help();
    std::string cmd;
    do
    {
        std::getline(std::cin, cmd);
        std::vector<std::string> token;
        std::stringstream ss(cmd);
        std::string tok;
        while (getline(ss, tok, ' '))
        {
            if (!tok.empty())
            {
                token.push_back(tok);
            }
        }
        if (token.size() == 1)
        {
            if ("ON" == token[0] || "on" == token[0]) {
                m_dimmingLightServer.m_binaryswitchInstance.setValue(true);
                if (OC_STACK_OK != m_dimmingLightServer.m_binaryswitchInstance.sendNotification())
                {
                    std::cerr << "NOTIFY failed." << std::endl;
                }
            }
            else if ("OFF" == token[0] || "off" == token[0]) {
                m_dimmingLightServer.m_binaryswitchInstance.setValue(false);
                if (OC_STACK_OK != m_dimmingLightServer.m_binaryswitchInstance.sendNotification())
                {
                    std::cerr << "NOTIFY failed." << std::endl;
                }
            }
            else if ("toggle" == token[0]) {
                bool tmp = m_dimmingLightServer.m_binaryswitchInstance.getValue();
                m_dimmingLightServer.m_binaryswitchInstance.setValue(!tmp);
                if (OC_STACK_OK != m_dimmingLightServer.m_binaryswitchInstance.sendNotification())
                {
                    std::cerr << "NOTIFY failed." << std::endl;
                }
            }
            else if ("info" == token[0]) {
                info();
            }
            else if ("help" == token[0] || "h" == token[0]) {
                help();
            }
            else if ("quit" == token[0] || "q" == token[0])
            {
                m_quit = true;
            }
        }
        else if (token.size() == 2)
        {
            if ("dim" == token[0]) {
                int value = atoi(token[1].c_str());
                if (value >= 0 && value <= 100)
                {
                    m_dimmingLightServer.m_dimmingInstance.setDimmingSetting(value);
                    if (OC_STACK_OK != m_dimmingLightServer.m_dimmingInstance.sendNotification())
                    {
                        std::cerr << "NOTIFY failed." << std::endl;
                    }
                }
                else
                {
                    std::cerr << "the dim value must between 0 and 100" << std::endl;
                }
            }
        }
        else {
            help();
        }
    } while (!m_quit);
}

void DimmingLightControl::info(void)
{
    std::cout <<
            "**************************************\n"
            " binary switch value: " << ((m_dimmingLightServer.m_binaryswitchInstance.getValue()) ? "ON" : "OFF") << std::endl;
    std::cout <<
            " dimming setting: " << m_dimmingLightServer.m_dimmingInstance.getDimmingSetting() << std::endl;
    std::cout <<
            "**************************************\n";
}

void DimmingLightControl::help(void)
{
    std::cout <<
            "**************************************\n"
            " Control the server locally:\n"
            " 'on': turn the light on\n"
            " 'off': turn the light off\n"
            " 'toggle': flip the light value\n"
            " 'dim [0-100]': change the dimming setting\n"
            " 'info': print current server values\n"
            " 'quit' or 'q': exit server\n"
            " 'help` or 'h': help\n"
            "**************************************\n";
}
