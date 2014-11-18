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

#include <string>
#include <stdio.h>
#include <iostream>

#include "SSMTestApp.h"
#include "SSMClient.h"

SSMTestApp::SSMTestApp()
{
}

void SSMTestApp::displayMenu()
{
    printf("===============================================\n");
    printf(" Iotivity Soft Sensor Manager Test Application \n");
    printf("===============================================\n");
    printf("   1. Register Query \n");
    printf("   2. Unregister Query \n");
    printf("   3. Register DiscomfortIndexSensor sample query \n");
    printf("   9. exit \n");
    printf("===============================================\n");
    printf("   Please Enter the NO: ");
}

/* Register Query.*/
void SSMTestApp::registerQuery(std::string queryString)
{
    std::string qid;
    SSMReturn rtn = SSM_ERROR;

    if (queryString.size() == 0)
    {
        printf("   Please Enter query string: ");
        cin.ignore();
        getline(cin, queryString);
    }

    rtn = m_SSMClient.registerQuery(queryString, this, qid);

    if (rtn == SSM_SUCCESS)
    {
        printf("The query has been registered!\n");
        printf("QID : %s\n", qid.c_str());
    }
    else
        printf("Error occured(%d)", rtn);
}

/* unRegister Query.*/
void SSMTestApp::unregisterQuery(void)
{
    std::string qid;
    SSMReturn rtn = SSM_ERROR;

    printf("   Please Enter query string: ");
    cin.ignore();
    getline(cin, qid);

    rtn = m_SSMClient.unregisterQuery(qid);

    if (rtn == SSM_SUCCESS)
        printf("The query has been unregistered!\n");
    else
        printf("Error occured(%d)\n", (int) rtn);
}

/* APP. Level Callback Function for Observer of client. */
void SSMTestApp::onRegisterQuery(const AttributeMap &attributeMap, SSMReturn &eCode)
{
    if (eCode == SSM_SUCCESS)
    {
        printf("onListener!!!!\n");
        printf("\n");
        printf("T : dry bult temperature. (℃)\n");
        printf("H : relative humidity. (%%)\n");
        printf("Discomport level = 9/5 * T + 32 - 0.55*(1 - H/100)*(9/5 * T - 26) \n");
        printf("\n");
        printf("**************************************\n");
        printf("* All    Discomport level : 80 over. *\n");
        printf("* Half   Discomport level : 75 over. *\n");
        printf("* Little Discomport level : 68 over. *\n");
        printf("* All    Comport    level : 67 under.*\n");
        printf("**************************************\n");
        printf("\n");

        for (AttributeMap::const_iterator itor = attributeMap.begin(); itor != attributeMap.end();
             ++itor)
        {
            if (strcmp(itor->first.c_str(), "temperature") == 0)
            {
                std::cout << "* Temperature : " << itor->second.front().c_str() << "℃" << std::endl;
            }
            else if (strcmp(itor->first.c_str(), "humidity") == 0)
            {
                std::cout << "* Humidity : " << itor->second.front().c_str() << "%" << std::endl;
            }
            else if (strcmp(itor->first.c_str(), "discomfortIndex") == 0)
            {
                int DI = std::stoi(itor->second.front().c_str());

                std::cout << "* DiscomfortIndex : " << DI << "%" << std::endl;

                switch (DI)
                {
                    case ALL_DISCOMPORT:
                        std::cout << "* [Result] All person Discomfort." << std::endl;
                        break;
                    case HALF_DISCOMPORT:
                        std::cout << "* [Result] Half person Discomfort." << std::endl;
                        break;
                    case LITTLE_DISCOMPORT:
                        std::cout << "* [Result] Little person Discomfort." << std::endl;
                        break;
                    case ALL_COMPORT:
                        std::cout << "* [Result] All person Comfort." << std::endl;
                        break;
                }
            }
        }
    }
    else
    {
        std::cout << "Response error: " << eCode << std::endl;
    }
}

/**
 * APP. Main Function.
 */
int main()
{
    printf("program start.\n");
    printf("searching SSMResource\n");
    SSMTestApp *SSMApp = new SSMTestApp();
    APPMenu::APPMenu menu = APPMenu::NONE;

    while (menu != APPMenu::EXIT)
    {
        SSMApp->displayMenu();

        menu = (APPMenu::APPMenu) (getchar() - '0');
        if ((APPMenu::APPMenu) 0 > menu || menu > APPMenu::EXIT)
            menu = (APPMenu::APPMenu) (getchar() - '0');

        switch (menu)
        {
            case APPMenu::REGISTER:
                std::cout << "register operate." << std::endl;
                SSMApp->registerQuery("");
                break;

            case APPMenu::UNREGISTER:
                std::cout << "unregister operate." << std::endl;
                SSMApp->unregisterQuery();
                break;

            case APPMenu::DISCOMFORT_SAMPLE:
                SSMApp->registerQuery("subscribe Device.DiscomfortIndexSensor "\
                                      "if Device.DiscomfortIndexSensor.discomfortIndex > 0");
                break;

            case APPMenu::EXIT:
                std::cout << "program exit." << std::endl;
                break;

            default:
                std::cout << "Not supported yet." << std::endl;
        }
    } // while

    delete SSMApp;
}

