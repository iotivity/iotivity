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

std::vector<SimulatorSingleResourceSP> g_singleResources;
std::vector<SimulatorCollectionResourceSP> g_collectionResources;

std::string getPropertyTypeString(SimulatorResourceModel::AttributeProperty::Type type)
{
    switch(type)
    {
        case SimulatorResourceModel::AttributeProperty::Type::RANGE:
            return "RANGE";
        case SimulatorResourceModel::AttributeProperty::Type::VALUE_SET:
            return "VALUE_SET";
        default:
            break;
    }

    return "UNKNOWN";
}

class AppLogger : public ILogger
{
    public:
        void write(std::string time, ILogger::Level level, std::string message)
        {
            std::cout << "[APPLogger] " << time << " " << ILogger::getString(level) << " "
                      << message;
        }
};
std::shared_ptr<AppLogger> gAppLogger(new AppLogger());

int selectResource()
{
    if (0 == g_singleResources.size())
    {
        std::cout << "No resouces!" << std::endl;
        return -1;
    }

    int index = 1;
    for (auto & resource : g_singleResources)
    {
        std::cout << index++ << ": " << resource->getURI().c_str() << std::endl;
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

void simulateResource()
{
    try
    {
        // Resource model change callback
        SimulatorResource::ResourceModelChangedCallback modelChangeCB =
        [](const std::string &uri, SimulatorResourceModel &resModel)
        {
            std::cout << "[callback] Resource model is changed URI: " << uri.c_str() << std::endl;
            std::cout << "#### Modified attributes are ####" << std::endl;
            std::cout << "#### Updated resource model ####" << std::endl;
            std::cout << resModel.toString() << std::endl;
            std::cout << "########################" << std::endl;
        };

        // Observer added/removed callback
        SimulatorResource::ObserverCallback observerCB =
        [] (const std::string &uri, ObservationStatus state, const ObserverInfo &observerInfo)
        {
            std::cout << "[callback] Observer notification received..." << uri << std::endl;

            std::ostringstream out;
            out << "ID:  " << (int) observerInfo.id << std::endl;
            out << " [address: " << observerInfo.address << " port: " << observerInfo.port
                << "]" << std::endl;
            std::cout << out.str();
        };

        // Get the RAML file path from user
        std::string configPath;
        std::cout << "Enter RAML path: ";
        std::cin>>configPath;

        SimulatorResourceSP resource =
            SimulatorManager::getInstance()->createResource(configPath);

        // Add resource to appropriate list
        if (SimulatorResource::Type::SINGLE_RESOURCE == resource->getType())
        {
            std::cout << "Single type resource created [URI:  " << resource->getURI() << " ]" << std::endl;
            SimulatorSingleResourceSP singleRes =
                std::dynamic_pointer_cast<SimulatorSingleResource>(resource);
            singleRes->setModelChangeCallback(modelChangeCB);
            singleRes->setObserverCallback(observerCB);
            g_singleResources.push_back(singleRes);
        }
        else
        {
            std::cout << "Collection type resource created [URI:  " << resource->getURI() << " ]" << std::endl;
            SimulatorCollectionResourceSP collectionRes =
                std::dynamic_pointer_cast<SimulatorCollectionResource>(resource);
            collectionRes->setObserverCallback(observerCB);
            g_collectionResources.push_back(collectionRes);
        }
    }
    catch (InvalidArgsException &e)
    {
        std::cout << "InvalidArgsException occured [code : " << e.code() << " Details: "
                  << e.what() << "]" << std::endl;
    }
    catch (SimulatorException &e)
    {
        std::cout << "SimulatorException occured [code : " << e.code() << " Details: "
                  << e.what() << "]" << std::endl;
    }
}

void displayResource()
{
    int index = selectResource();
    if (-1 == index)
        return;

    SimulatorSingleResourceSP resource = g_singleResources[index - 1];

    std::cout << "#############################" << std::endl;
    std::cout << "Name: " << resource->getName() << std::endl;
    std::cout << "URI: " << resource->getURI() << std::endl;
    std::cout << "Resource type: " << resource->getResourceType() << std::endl;
    std::cout << "Interface type:";
    for (auto &interfaceType : resource->getInterface())
        std::cout << " " << interfaceType << std::endl;

    // Attributes
    SimulatorResourceModel resModel = resource->getResourceModel();
    std::map<std::string, SimulatorResourceModel::Attribute> attributes =
        resModel.getAttributes();
    std::cout << "##### Attributes [" << attributes.size() << "]" << std::endl;
    for (auto & attribute : attributes)
    {
        std::cout << (attribute.second).getName() << " :  {" << std::endl;
        std::cout << "value: " << (attribute.second).toString() << std::endl;
        SimulatorResourceModel::AttributeProperty prop = (attribute.second).getProperty();
        std::cout << "Supported values given by : " << getPropertyTypeString(prop.type()) << std::endl;
        if (SimulatorResourceModel::AttributeProperty::Type::RANGE == prop.type())
        {
            std::cout << "Min: " << prop.min() << std::endl;
            std::cout << "Max: " << prop.max() << std::endl;
        }
        else if (SimulatorResourceModel::AttributeProperty::Type::VALUE_SET == prop.type())
        {
            std::cout << "Value set: " << prop.valueSetToString() << std::endl;
        }

        std::cout << "}" << std::endl << std::endl;
    }
    std::cout << "#############################" << std::endl;
}

void startResource()
{
    int index = selectResource();
    if (-1 == index)
        return;

    SimulatorSingleResourceSP resource = g_singleResources[index - 1];
    resource->start();
    std::cout << "Resource started!" << std::endl;
}

void stopResource()
{
    int index = selectResource();
    if (-1 == index)
        return;

    SimulatorSingleResourceSP resource = g_singleResources[index - 1];
    resource->stop();
    std::cout << "Resource stopped!" << std::endl;
}

void automateResourceUpdate()
{
    updateCompleteCallback callback = [](const std::string &uri, const int id)
    {
        std::cout << "Update automation is completed [URI: " << uri
                  << "  AutomationID: " << id << "] ###" << std::endl;
    };

    int index = selectResource();
    if (-1 == index)
        return;

    AutomationType type = AutomationType::NORMAL;
    int choice = 0;
    std::cout << "Press 1 if you want recurrent automation: ";
    std::cin >> choice;
    if (1 == choice)
        type = AutomationType::RECURRENT;

    try
    {
        int id = g_singleResources[index - 1]->startResourceUpdation(type, -1, callback);

        std::cout << "startUpdateAutomation() returned succces : " << id << std::endl;
    }
    catch (SimulatorException &e)
    {
        std::cout << "SimulatorException occured [code : " << e.code() << " Details: " <<
                  e.what() << "]" << std::endl;
    }
}

void automateAttributeUpdate()
{
    updateCompleteCallback callback = [](const std::string &uri, const int id)
    {
        std::cout << "Update automation is completed [URI: " << uri
                  << "  AutomationID: " << id << "] ###" << std::endl;
    };

    int index = selectResource();
    if (-1 == index)
        return;

    SimulatorSingleResourceSP resource = g_singleResources[index - 1];
    SimulatorResourceModel resModel = resource->getResourceModel();
    std::map<std::string, SimulatorResourceModel::Attribute> attributes =
        resModel.getAttributes();
    int size = 0;
    for (auto & attribute : attributes)
    {
        std::cout << ++size << ": " << attribute.first.c_str() << std::endl;
    }

    if (0 == size)
    {
        std::cout << "This resource doest not contain any attributes!" << std::endl;
        return;
    }

    int choice = -1;
    std::cout << "Select the attribute which you want to automate for updation: " <<
              std::endl;
    std::cin >> choice;
    if (choice < 0 || choice > size)
    {
        std::cout << "Invalid selection!" << std::endl;
        return;
    }

    int count = 0;
    std::string attributeName;
    for (auto & attribute : attributes)
    {
        if (count == choice - 1)
        {
            attributeName = attribute.first;
            break;
        }

        count++;
    }

    AutomationType type = AutomationType::NORMAL;
    std::cout << "Press 1 if you want recurrent automation: ";
    std::cin >> choice;
    if (1 == choice)
        type = AutomationType::RECURRENT;

    std::cout << "Requesting attribute automation for " << attributeName <<
              std::endl;

    try
    {

        int id = resource->startAttributeUpdation(attributeName, type, -1, callback);
        std::cout << "startUpdateAutomation() returned succces : " << id << std::endl;
    }
    catch (SimulatorException &e)
    {
        std::cout << "SimulatorException occured [Error: " << e.code() << " Details: " <<
                  e.what() << "]" << std::endl;
    }
}

void stopAutomation()
{
    int index = selectResource();
    if (-1 == index)
        return;

    SimulatorSingleResourceSP resource = g_singleResources[index - 1];

    // Select the automation to stop
    std::vector<int> ids;
    {
        std::vector<int> rids = resource->getResourceUpdationIds();
        std::vector<int> aids = resource->getAttributeUpdationIds();
        ids.insert(ids.end(), rids.begin(), rids.end());
        ids.insert(ids.end(), aids.begin(), aids.end());
    }

    if (!ids.size())
    {
        std::cout << "No automation operation is going on this resource right now!" <<
                  std::endl;
        return;
    }

    for (auto & id : ids)
    {
        std::cout <<  id  << " ";
        resource->stopUpdation(id);
    }
}

void getObservers()
{
    int index = selectResource();
    if (-1 == index)
        return;

    SimulatorSingleResourceSP resource = g_singleResources[index - 1];

    std::vector<ObserverInfo> observersList = resource->getObserversList();

    std::cout << "##### Number of Observers [" << observersList.size() << "]" << std::endl;
    for (auto & observerInfo : observersList)
    {
        std::cout << " ID :  " << (int) observerInfo.id << " [address: " <<
                  observerInfo.address << " port: " << observerInfo.port << "]" << std::endl;
    }
    std::cout << "########################" << std::endl;
}

void printMainMenu()
{
    std::cout << "############### MAIN MENU###############" << std::endl;
    std::cout << "1. Simulate resource" << std::endl;
    std::cout << "2. Display resource information" << std::endl;
    std::cout << "3. Start resource" << std::endl;
    std::cout << "4. Stop resource" << std::endl;
    std::cout << "5. Automate resource update" << std::endl;
    std::cout << "6. Automate attributes update" << std::endl;
    std::cout << "7. Stop Automation" << std::endl;
    std::cout << "8. Get Observers of a resource" << std::endl;
    std::cout << "9. Set Logger" << std::endl;
    std::cout << "10. Set Device Info" << std::endl;
    std::cout << "11. Set Platform Info" << std::endl;
    std::cout << "12. Help" << std::endl;
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
                if (false == SimulatorManager::getInstance()->setConsoleLogger())
                    std::cout << "Failed to set the default console logger" << std::endl;
            } break;
        case 2:
            {
                std::string filePath;
                std::cout << "Enter the file path (without file name) : ";
                std::cin >> filePath;
                if (false == SimulatorManager::getInstance()->setFileLogger(filePath))
                    std::cout << "Failed to set default file logger" << std::endl;
            } break;
        case 3: SimulatorManager::getInstance()->setLogger(gAppLogger);
    }
}

void setDeviceInfo()
{
    SimulatorManager::getInstance()->setDeviceInfo("IoTivity Simulator Linux Sample");
    std::cout << "Setting Device Info is successful" << std::endl;
}

void setPlatformInfo()
{
    PlatformInfo pInfo;
    pInfo.setPlatformID("Samsung Platform Identifier");
    pInfo.setFirmwareVersion("FirwareVersion01");
    pInfo.setHardwareVersion("HardwareVersion01");
    pInfo.setManufacturerName("Samsung");
    pInfo.setManufacturerUrl("www.samsung.com");
    pInfo.setModelNumber("Samsung Model Num01");
    pInfo.setOSVersion("OSVersion01");
    pInfo.setPlatformVersion("PlatformVersion01");
    pInfo.setSupportUrl("http://www.samsung.com/support");
    pInfo.setSystemTime("2015-09-10T11:10:30Z");
    pInfo.setDateOfManfacture("2015-09-10T11:10:30Z");

    SimulatorManager::getInstance()->setPlatformInfo(pInfo);
    std::cout << "Setting Platform Info is successful" << std::endl;
}

int main(int argc, char *argv[])
{
    printMainMenu();
    bool cont = true;
    while (cont == true)
    {
        int choice = -1;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        if (choice < 0 || choice > 12)
        {
            std::cout << "Invaild choice !" << std::endl; continue;
        }

        switch (choice)
        {
            case 1 : simulateResource(); break;
            case 2 : displayResource(); break;
            case 3 : startResource(); break;
            case 4 : stopResource(); break;
            case 5 : automateResourceUpdate(); break;
            case 6 : automateAttributeUpdate(); break;
            case 7 : stopAutomation(); break;
            case 8 : getObservers(); break;
            case 9 : setLogger(); break;
            case 10: setDeviceInfo(); break;
            case 11: setPlatformInfo(); break;
            case 12: printMainMenu(); break;
            case 0: cont = false;
        }
    }

    std::cout << "Terminating test !!!" << std::endl;
}
