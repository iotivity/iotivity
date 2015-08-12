/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "simulator_manager.h"

class AppLogger : public ILogger
{
    public:
        void write(std::string time, ILogger::Level level, std::string message)
        {
            std::cout << "[APPLogger] " << time << " " << ILogger::getString(level) << " " << message;
        }
};
std::shared_ptr<AppLogger> gAppLogger(new AppLogger());

class ClientController
{
    public:
        void startTest()
        {
            printMenu();
            bool cont = true;
            while (cont)
            {
                int choice = -1;
                std::cout << "Enter your choice: ";
                std::cin >> choice;
                if (choice < 0 || choice > 8)
                {
                    std::cout << "Invaild choice !" << std::endl; continue;
                }

                switch (choice)
                {
                    case 1: findResource(); break;
                    case 2: displayResource(); break;
                    case 3: observeResource(); break;
                    case 4: cancelObserving(); break;
                    case 5: sendGet(); break;
                    case 6: sendPut(); break;
                    case 7: sendPost(); break;
                    case 8: printMenu(); break;
                    case 0: cont = false;
                }
            }
        }

    private:
        void printMenu()
        {
            std::cout << "########### SIMULATOR CLIENT CONTROLLER ###########" << std::endl;
            std::cout << "1. Find resource" << std::endl;
            std::cout << "2. Display resource information" << std::endl;
            std::cout << "3. Observe for resource change" << std::endl;
            std::cout << "4. Cancel observation" << std::endl;
            std::cout << "5. Send GET message" << std::endl;
            std::cout << "6. Send PUT message" << std::endl;
            std::cout << "7. Send POST message" << std::endl;
            std::cout << "8: Help" << std::endl;
            std::cout << "0. Exit" << std::endl;
            std::cout << "###################################################" << std::endl;
        }

        int selectResource(std::vector<SimulatorRemoteResourcePtr> resourceList)
        {
            if (0 == resourceList.size())
            {
                std::cout << "No resouces!" << std::endl;
                return -1;
            }

            int index = 1;
            for (auto & resource : resourceList)
            {
                std::cout << index++ << ": " << resource->getURI() << "[" << resource->getHost()  << "]" << std::endl;
            }

            int choice = -1;
            std::cout << "Choose the resource: ";
            std::cin >> choice;

            if (choice < 1 || choice > index - 1)
            {
                std::cout << "Invalid choice !" << std::endl;
                choice = -1;
            }

            return choice;
        }

        void findResource()
        {
            std::string resourceType;
            std::cout << "Enter resource type : ";
            std::cin >> resourceType;

            ResourceFindCallback callback = [this](std::shared_ptr<SimulatorRemoteResource> resource)
            {
                std::cout << "Resource found ######" << std::endl;
                displayResource(resource);
            };

            SimulatorResult result = SimulatorManager::getInstance()->findResource
                                          (resourceType, callback);
            if (SIMULATOR_SUCCESS != result)
                std::cout << "SimulatorManager::findResource returned error : " << result << std::endl;
        }

        void displayResource()
        {
            std::vector<SimulatorRemoteResourcePtr> resourceList =
                SimulatorManager::getInstance()->getFoundResources();

            int index = selectResource(resourceList);
            if (-1 == index)
                return;

            displayResource(resourceList[index - 1]);
        }

        void displayResource(SimulatorRemoteResourcePtr resource)
        {
            std::cout << "#############################" << std::endl;
            std::cout << "URI: " << resource->getURI().c_str() << std::endl;
            std::cout << "Host: " << resource->getHost().c_str() << std::endl;
            std::cout << "Resource Types: ";
            for (auto &type : resource->getResourceTypes())
                std::cout << type << " ";
            std::cout << "\nInterface Types: ";
            for (auto &type : resource->getResourceInterfaces())
                std::cout << type << " ";
            std::cout << std::boolalpha << "\nisObservable : " << resource->isObservable() << std::endl;
            std::cout << "#############################" << std::endl;
        }

        void observeResource()
        {
            std::vector<SimulatorRemoteResourcePtr> resourceList =
                SimulatorManager::getInstance()->getFoundResources();

            int index = selectResource(resourceList);
            if (-1 == index)
                return;

            SimulatorRemoteResourcePtr resource = resourceList[index - 1];

            // callback implementaion
            SimulatorRemoteResource::RepresentationChangeCallback callback =
            [](int errorCode, const SimulatorResourceModel &rep, int seq)
            {
                std::cout << "\nObserve notificatoin received ###[errorcode:  " << errorCode << " seq:  " << seq << "]" << std::endl;
                std::map<std::string, SimulatorResourceModel::Attribute> attributes = rep.getAttributes();
                for (auto & attribute : attributes)
                {
                    std::cout << (attribute.second).getName() << " :  {" << std::endl;
                    std::cout << "value: " << (attribute.second).valueToString().c_str() << std::endl;
                    std::cout << "}" << std::endl;
                }
                std::cout << std::endl;
            };

            std::map <std::string, std::string> queryParams;
            SimulatorResult result = resource->observe(SimulatorRemoteResource::OBSERVE, queryParams, callback);
            if ( SIMULATOR_SUCCESS == result)
                std::cout << "Observe is successfull!" << std::endl;
            else
                std::cout << "Observe is failed!error: " << result << std::endl;
        }

        void cancelObserving()
        {
            std::vector<SimulatorRemoteResourcePtr> resourceList =
                SimulatorManager::getInstance()->getFoundResources();

            int index = selectResource(resourceList);
            if (-1 == index)
                return;

            SimulatorRemoteResourcePtr resource = resourceList[index - 1];
            SimulatorResult result = resource->cancelObserve();
            if ( SIMULATOR_SUCCESS == result)
                std::cout << "Cancelling observe is successfull!" << std::endl;
            else
                std::cout << "Cancelling observe is failed!error: " << result << std::endl;
        }

        void sendGet()
        {
            std::vector<SimulatorRemoteResourcePtr> resourceList =
                SimulatorManager::getInstance()->getFoundResources();

            int index = selectResource(resourceList);
            if (-1 == index)
                return;

            SimulatorRemoteResourcePtr resource = resourceList[index - 1];

            // callback implementaion
            SimulatorRemoteResource::ResponseCallback callback =
            [](int errorCode, const SimulatorResourceModel & rep)
            {
                std::cout << "\nGET Response received ### [errorcode:  " << errorCode << "]" << std::endl;
                std::cout << "Representation is: " << std::endl;
                std::map<std::string, SimulatorResourceModel::Attribute> attributes = rep.getAttributes();
                for (auto & attribute : attributes)
                {
                    std::cout << (attribute.second).getName() << " :  {" << std::endl;
                    std::cout << "value: " << (attribute.second).valueToString().c_str() << std::endl;
                    std::cout << "}" << std::endl;
                }
                std::cout << std::endl;
            };

            std::map <std::string, std::string> queryParams;
            SimulatorResult result = resource->get(queryParams, callback);
            if ( SIMULATOR_SUCCESS == result)
                std::cout << "GET is successfull!" << std::endl;
            else
                std::cout << "GET is failed!error: " << result << std::endl;
        }

        void sendPut()
        {
            std::vector<SimulatorRemoteResourcePtr> resourceList =
                SimulatorManager::getInstance()->getFoundResources();

            int index = selectResource(resourceList);
            if (-1 == index)
                return;

            SimulatorRemoteResourcePtr resource = resourceList[index - 1];

            // callback implementaion
            SimulatorRemoteResource::ResponseCallback callback =
            [](int errorCode, const SimulatorResourceModel & rep)
            {
                std::cout << "\nPUT Response received ![errorcode:  " << errorCode << "]" << std::endl;
                std::cout << "Representation is: " << std::endl;
                std::map<std::string, SimulatorResourceModel::Attribute> attributes = rep.getAttributes();
                for (auto & attribute : attributes)
                {
                    std::cout << (attribute.second).getName() << " :  {" << std::endl;
                    std::cout << "value: " << (attribute.second).valueToString().c_str() << std::endl;
                    std::cout << "}" << std::endl;
                }
                std::cout << std::endl;
            };

            std::map <std::string, std::string> queryParams;
            SimulatorResourceModel rep;
            rep.addAttribute("power", "off");
            rep.addAttribute("intensity", 5);

            SimulatorResult result = resource->put(rep, queryParams, callback);
            if ( SIMULATOR_SUCCESS == result)
                std::cout << "PUT is successfull!" << std::endl;
            else
                std::cout << "PUT is failed!error: " << result << std::endl;
        }

        void sendPost()
        {
            std::vector<SimulatorRemoteResourcePtr> resourceList =
                SimulatorManager::getInstance()->getFoundResources();

            int index = selectResource(resourceList);
            if (-1 == index)
                return;

            SimulatorRemoteResourcePtr resource = resourceList[index - 1];

            // callback implementaion
            SimulatorRemoteResource::ResponseCallback callback =
            [](int errorCode, const SimulatorResourceModel & rep)
            {
                std::cout << "\nPOST Response received ![errorcode:  " << errorCode << "]" << std::endl;
                std::cout << "Representation is: " << std::endl;
                std::map<std::string, SimulatorResourceModel::Attribute> attributes = rep.getAttributes();
                for (auto & attribute : attributes)
                {
                    std::cout << (attribute.second).getName() << " :  {" << std::endl;
                    std::cout << "value: " << (attribute.second).valueToString().c_str() << std::endl;
                    std::cout << "}" << std::endl;
                }
                std::cout << std::endl;
            };

            std::map <std::string, std::string> queryParams;
            SimulatorResourceModel rep;
            rep.addAttribute("power", "on");
            rep.addAttribute("intensity", 7);

            SimulatorResult result = resource->post(rep, queryParams, callback);
            if ( SIMULATOR_SUCCESS == result)
                std::cout << "POST is successfull!" << std::endl;
            else
                std::cout << "POST is failed!error: " << result << std::endl;
        }
};

void printMainMenu()
{
    std::cout << "############### MAIN MENU###############" << std::endl;
    std::cout << "1. Client Controller Test" << std::endl;
    std::cout << "2. Set Logger" << std::endl;
    std::cout << "3. Help" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "######################################" << std::endl;
}

void setLogger()
{
    std::cout << "1. Default console logger" << std::endl;
    std::cout << "2. Default file logger" << std::endl;
    std::cout << "3. custom logger" << std::endl;

    int choice = -1;
    std::cin >> choice;
    if (choice <= 0 || choice > 3)
    {
        std::cout << "Invalid selection !" << std::endl;
        return;
    }

    switch (choice)
    {
        case 1:
            {
                if (false == SimulatorManager::getInstance()->setDefaultConsoleLogger())
                    std::cout << "Failed to set the default console logger" << std::endl;
            } break;
        case 2:
            {
                std::string filePath;
                std::cout << "Enter the file path (without file name) : ";
                std::cin >> filePath;
                if (false == SimulatorManager::getInstance()->setDefaultFileLogger(filePath))
                    std::cout << "Failed to set default file logger" << std::endl;
            } break;
        case 3: SimulatorManager::getInstance()->setLogger(gAppLogger);
    }
}

int main(void)
{
    ClientController clientController;
    printMainMenu();
    bool cont = true;
    while (cont == true)
    {
        int choice = -1;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        if (choice < 0 || choice > 3)
        {
            std::cout << "Invaild choice !" << std::endl; continue;
        }

        switch (choice)
        {
            case 1: clientController.startTest();
                std::cout << "Welcome back to main menu !" << std::endl;
                break;
            case 2: setLogger(); break;
            case 3: printMainMenu(); break;
            case 0: cont = false;
        }
    }

    std::cout << "Terminating test !!!" << std::endl;
}
