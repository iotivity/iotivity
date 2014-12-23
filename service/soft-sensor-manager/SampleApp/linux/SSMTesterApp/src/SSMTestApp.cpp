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

using namespace std;

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
    int qid;
    SSMRESULT rtn = SSM_E_FAIL;

    if (queryString.size() == 0)
    {
        printf("   Please Enter query string: ");
        cin.ignore();
        getline(cin, queryString);
    }

    rtn = m_SSMClient.registerQuery(queryString, this, qid);

    if (rtn == SSM_S_OK)
    {
        printf("The query has been registered!\n");
        printf("QID : %d\n", qid);
    }
    else
        printf("Error occured(%d)", rtn);
}

/* unRegister Query.*/
void SSMTestApp::unregisterQuery(void)
{
    std::string qid;
    SSMRESULT rtn = SSM_E_FAIL;

    printf("   Please Enter query string: ");
    cin.ignore();
    getline(cin, qid);

    rtn = m_SSMClient.unregisterQuery(atoi(qid.c_str()));

    if (rtn == SSM_S_OK)
        printf("The query has been unregistered!\n");
    else
        printf("Error occured(%d)\n", (int) rtn);
}

/* APP. Level Callback Function for Observer of client. */
SSMRESULT SSMTestApp::onQueryEngineEvent(int cqid, IDataReader *pResult)
{
    int     dataCount = 0;
    IModelData      *pModelData = NULL;
    std::vector<std::string>        affectedModels;

    cout << "Event received! cqid = " << cqid << endl;

    pResult->getAffectedModels(&affectedModels);

    for (std::vector<std::string>::iterator itor = affectedModels.begin();
         itor != affectedModels.end(); ++itor)
    {
        cout << "Printing " << *itor << " model" << endl;
        pResult->getModelDataCount(*itor, &dataCount);
        for (int i = 0; i < dataCount; i++)
        {
            pResult->getModelData(*itor, i, &pModelData);
            cout << "dataId: " << pModelData->getDataId() << endl;
            for (int j = 0; j < pModelData->getPropertyCount(); j++)
            {
                cout << "Type: " << pModelData->getPropertyName(j) << " Value: " << pModelData->getPropertyValue(
                         j) << endl;
            }
        }
    }

    return SSM_S_OK;
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

