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
#include <stdlib.h>

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
    printf("   4. Register IndoorTrajectorySensor sample query \n");
    printf("   5. Register BMISensor sample query \n");
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
        getline(cin, queryString);
    }

    rtn = RegisterQuery(queryString, this, qid);

    if (rtn == SSM_S_OK)
    {
        printf("The query has been registered!\n");
        printf("QID : %d\n", qid);
    }
    else
        printf("Error occured(%d)\n", rtn);
}

/* unRegister Query.*/
void SSMTestApp::unregisterQuery(void)
{
    std::string qid;
    SSMRESULT rtn = SSM_E_FAIL;

    printf("   Please Enter query Id: ");
    getline(cin, qid);

    rtn = UnregisterQuery(atoi(qid.c_str()));

    if (rtn == SSM_S_OK)
        printf("The query has been unregistered!\n");
    else
        printf("Error occured(%d)\n", (int) rtn);
}


#define INPUT_EA        9
char input_name[INPUT_EA][10] = { "trackeeID", "timeT0", "ref01T0", "ref02T0", "ref03T0", "timeT1", "ref01T1", "ref02T1", "ref03T1" };

void SSMTestApp::TrajectoryDataOutput(IModelData *pModelData)
{
    std::string name = "";
    int l = 0;

    std::string trackeeID;
    std::string T0DateTime;
    std::string T0Ref01;
    std::string T0Ref02;
    std::string T0Ref03;
    std::string T1DateTime;
    std::string T1Ref01;
    std::string T1Ref02;
    std::string T1Ref03;

    for (int j = 0; j < pModelData->getPropertyCount(); j++)
    {
        name = pModelData->getPropertyName(j);

        for (l = 0; l < INPUT_EA; l++)
        {
            if (name.compare(input_name[l]) == 0)
                break;
        }

        switch (l)
        {
            case 0:
                trackeeID = pModelData->getPropertyValue(j);
                break;
            case 1:
                T0DateTime = pModelData->getPropertyValue(j);
                break;
            case 2:
                T0Ref01 = pModelData->getPropertyValue(j);
                break;
            case 3:
                T0Ref02 = pModelData->getPropertyValue(j);
                break;
            case 4:
                T0Ref03 = pModelData->getPropertyValue(j);
                break;
            case 5:
                T1DateTime = pModelData->getPropertyValue(j);
                break;
            case 6:
                T1Ref01 = pModelData->getPropertyValue(j);
                break;
            case 7:
                T1Ref02 = pModelData->getPropertyValue(j);
                break;
            case 8:
                T1Ref03 = pModelData->getPropertyValue(j);
                break;
            default:
                ;
        }
    }

    printf("===========================================\n");
    printf("        ITS Trajectory Data Output         \n");
    printf("===========================================\n");
    printf("\n");
    printf(" < Trackee Thing ID : %s > \n", trackeeID.c_str());
    printf("   - Trajectory 01         \n");
    printf("     0. Date, Time : %s    \n", T0DateTime.c_str());
    printf("     1. Ref. Thing : %s    \n", T0Ref01.c_str());
    printf("     2. Ref. Thing : %s    \n", T0Ref02.c_str());
    printf("     3. Ref. Thing : %s    \n", T0Ref03.c_str());
    printf("\n");
    printf("   - Trajectory 02         \n");
    printf("     0. Date, Time : %s    \n", T1DateTime.c_str());
    printf("     1. Ref. Thing : %s    \n", T1Ref01.c_str());
    printf("     2. Ref. Thing : %s    \n", T1Ref02.c_str());
    printf("     3. Ref. Thing : %s    \n", T1Ref03.c_str());
    printf("\n");
    printf("===========================================\n");
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

            //TrajectoryDataOutput(pModelData);
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
    std::string strMenu;

    std::string xmlDescription = "<SSMCore>"
                                 "<Device>"
                                 "<UDN>abcde123-31f8-11b4-a222-08002b34c003</UDN>"
                                 "<Name>MyPC</Name>"
                                 "<Type>PC</Type>"
                                 "</Device>"
                                 "</SSMCore>";

    if (InitializeSSM(xmlDescription) != SSM_S_OK)
        std::cout << "core init failed" << std::endl;

    while (menu != APPMenu::EXIT)
    {
        SSMApp->displayMenu();

        getline(cin, strMenu);
        menu = (APPMenu::APPMenu) (atoi(strMenu.c_str()));

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

            case APPMenu::ITS_SAMPLE:
                SSMApp->registerQuery("subscribe Device.IndoorTrajectorySensor "\
                                      "if Device.IndoorTrajectorySensor.trackeeID == \"9059AF16FEF7\"");
                break;

            case APPMenu::BMI_SAMPLE:
                SSMApp->registerQuery("subscribe Device.BMISensor "\
                                      "if Device.BMISensor.BMIresult > 0");

                break;





            case APPMenu::EXIT:
                std::cout << "program exit." << std::endl;
                break;

            default:
                std::cout << "Not supported yet." << std::endl;
        }
    } // while

    TerminateSSM();

    delete SSMApp;

    return 0;
}

