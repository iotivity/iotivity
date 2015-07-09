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

class SimLightResource
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
                if (choice < 0 || choice > 9)
                {
                    std::cout << "Invaild choice !" << std::endl; continue;
                }

                switch (choice)
                {
                    case 1: simulateResource(); break;
                    case 2: displayResource(); break;
                    case 3: deleteResource(); break;
                    case 4: updateAttributePower(); break;
                    case 5: updateAttributeIntensity(); break;
                    case 6: automateResourceUpdate(); break;
                    case 7: automateAttributeUpdate(); break;
                    case 8: stopAutomation(); break;
                    case 9: printMenu(); break;
                    case 0: cont = false;
                }
            }
        }

    private:
        void printMenu()
        {
            std::cout << "########### LIGHT RESOURCE TESTING ###########" << std::endl;
            std::cout << "1. Simulate resource" << std::endl;
            std::cout << "2. Display resource information" << std::endl;
            std::cout << "3. Delete resource" << std::endl;
            std::cout << "4. Update attribute \"power\"" << std::endl;
            std::cout << "5. Update attribute \"intensity\"" << std::endl;
            std::cout << "6. Automate resource update" << std::endl;
            std::cout << "7. Automate attributes update" << std::endl;
            std::cout << "8. Stop Automation" << std::endl;
            std::cout << "9: Help" << std::endl;
            std::cout << "0. Exit" << std::endl;
            std::cout << "#######################################" << std::endl;
        }

        int selectResource()
        {
            int index = 1;
            for (auto & resource : m_resources)
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

        void onResourceModelChanged(const std::string &uri,
                                    const std::vector<SimulatorResourceModel::Attribute> &attributes)
        {
            std::cout << "[callback] Resource model is changed URI: " << uri.c_str() << "Count : " <<
                      attributes.size() << std::endl;
            std::cout << "#### Modified attributes are ####" << std::endl;
            for (auto & attribute : attributes)
            {
                std::cout << attribute.getName() << " :  " << attribute.valueToString().c_str() << std::endl;
            }
            std::cout << "########################" << std::endl;
        }

        void simulateResource()
        {
            SimulatorResource::ResourceModelChangedCB callback = std::bind(
                        &SimLightResource::onResourceModelChanged, this, std::placeholders::_1, std::placeholders::_2);
            SimulatorResourcePtr resource = SimulatorManager::getInstance()->createResource("", callback);
            if (NULL == resource.get())
                std::cout << "Failed to create resource" << std::endl;

            m_resources.push_back(resource);
            std::cout << "Resource created successfully! URI= " << resource->getURI().c_str() << std::endl;
        }

        void deleteResource()
        {
            int index = selectResource();
            if (-1 == index)
                return;

            SimulatorManager::getInstance()->deleteResource(m_resources[index - 1]);
            std::cout << "Resource deleted successfully! " << std::endl;
        }

        void updateAttributePower()
        {
            int index = selectResource();
            if (-1 == index)
                return;

            SimulatorResourcePtr resource = m_resources[index - 1];
            SimulatorResourceModel resModel = resource->getModel();
            SimulatorResourceModel::Attribute powerAttribute;
            resModel.getAttribute("power", powerAttribute);

            int allowedValuesSize = powerAttribute.getAllowedValuesSize();
            if (0 == allowedValuesSize)
            {
                std::cout << "This attribute does not have allowed values!" << std::endl;
                return;
            }

            std::cout << "Setting the new values from allowed values list to power attribute" << std::endl;
            // Update all possible values from allowed values
            for (int index = 0; index < allowedValuesSize; index++)
            {
                // Update the new value and display the resource model after modifying
                resource->updateAttributeFromAllowedValues("power", index);
                std::cout << "Attribute value is modified ####" << std::endl;

                // Display the resource to user to verify the changed attribute value
                displayResource(resource);
                std::cout << std::endl << std::endl;

                // Get user input for continuing this operation
                if ((index + 1) < allowedValuesSize)
                {
                    int choice;
                    std::cout << "Would you like to continue the attribute values changing process? (1/0): ";
                    std::cin >> choice;
                    if (0 == choice)
                        break;
                }
            }

            std::cout << "All the allowed values are tried!" << std::endl;
        }

        void updateAttributeIntensity()
        {
            int index = selectResource();
            if (-1 == index)
                return;

            SimulatorResourcePtr resource = m_resources[index - 1];
            SimulatorResourceModel resModel = resource->getModel();
            SimulatorResourceModel::Attribute intensityAttribute;
            resModel.getAttribute("intensity", intensityAttribute);

            int allowedValuesSize = intensityAttribute.getAllowedValuesSize();
            if (0 == allowedValuesSize)
            {
                std::cout << "This attribute does not have allowed values!" << std::endl;
                return;
            }

            std::cout << "Setting the new values from allowed values list to intensity attribute" << std::endl;
            // Update all possible values from allowed values
            for (int index = 0; index < allowedValuesSize; index++)
            {
                // Update the new value and display the resource model after modifying
                resource->updateAttributeFromAllowedValues("intensity", index);
                std::cout << "Attribute value is modified ####" << std::endl;

                // Display the resource to user to verify the changed attribute value
                displayResource(resource);
                std::cout << std::endl << std::endl;

                // Get user input for continuing this operation
                if ((index + 1) < allowedValuesSize)
                {
                    int choice;
                    std::cout << "Would you like to continue the attribute values changing process? (1/0): ";
                    std::cin >> choice;
                    if (0 == choice)
                        break;
                }
            }

            std::cout << "All the allowed values are tried!" << std::endl;
        }

        void displayResource()
        {
            int index = selectResource();
            if (-1 == index)
                return;

            SimulatorResourcePtr resource = m_resources[index - 1];
            displayResource(resource);
        }

        void displayResource(SimulatorResourcePtr resource)
        {
            std::cout << "#############################" << std::endl;
            std::cout << "Name: " << resource->getName().c_str() << std::endl;
            std::cout << "URI: " << resource->getURI().c_str() << std::endl;
            std::cout << "R. Type: " << resource->getResourceType().c_str() << std::endl;
            std::cout << "I. Type: " << resource->getInterfaceType().c_str() << std::endl;

            // Attributes
            SimulatorResourceModel resModel = resource->getModel();
            std::map<std::string, SimulatorResourceModel::Attribute> attributes = resModel.getAttributes();
            std::cout << "##### Attributes [" << attributes.size() << "]" << std::endl;
            for (auto & attribute : attributes)
            {
                std::cout << (attribute.second).getName() << " :  {" << std::endl;
                std::cout << "value: " << (attribute.second).valueToString().c_str() << std::endl;
                int min, max;
                (attribute.second).getRange(min, max);
                std::cout << "min: " << min << std::endl;
                std::cout << "max: " << max << std::endl;
                std::cout << "allowed values : " << (attribute.second).allowedValuesToString() << std::endl;
                std::cout << "}" << std::endl << std::endl;
            }
            std::cout << "#############################" << std::endl;
        }

        void automateResourceUpdate()
        {
            int index = selectResource();
            if (-1 == index)
                return;

            int id;
            if (SIMULATOR_SUCCESS != m_resources[index - 1]->startUpdateAutomation(AutomationType::NORMAL, id))
                std::cout << "startUpdateAutomation() returned error!" << std::endl;
            else
                std::cout << "startUpdateAutomation() returned succces : " << id << std::endl;
        }

        void automateAttributeUpdate()
        {
            int index = selectResource();
            if (-1 == index)
                return;

            SimulatorResourcePtr resource = m_resources[index - 1];
            SimulatorResourceModel resModel = resource->getModel();
            std::map<std::string, SimulatorResourceModel::Attribute> attributes = resModel.getAttributes();
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
            std::cout << "Select the attribute which you want to automate for updation: " << std::endl;
            std::cin >> choice;
            if (choice == -1 || choice > size)
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

            std::cout << "Requesting attribute automation for " << attributeName.c_str() << std::endl;
            int id;
            if (SIMULATOR_SUCCESS != resource->startUpdateAutomation(attributeName, AutomationType::NORMAL, id))
                std::cout << "startUpdateAutomation() returned error!" << std::endl;
            else
                std::cout << "startUpdateAutomation() returned succces : " << id << std::endl;
        }

        void stopAutomation()
        {
            int index = selectResource();
            if (-1 == index)
                return;

            SimulatorResourcePtr resource = m_resources[index - 1];
            int automationid;
            std::cout << "Enter automation id: " << std::endl;
            std::cin >> automationid;
            resource->stopUpdateAutomation(automationid);
        }

    private:
        std::vector<SimulatorResourcePtr> m_resources;
};

void printMainMenu()
{
    std::cout << "############### MAIN MENU###############" << std::endl;
    std::cout << "1. Test simulation of light resource" << std::endl;
    std::cout << "2. Help" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "######################################" << std::endl;
}

int main(void)
{
    SimLightResource lightResource;

    printMainMenu();
    bool cont = true;
    while (cont == true)
    {
        int choice = -1;
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        if (choice < 0 || choice > 2)
        {
            std::cout << "Invaild choice !" << std::endl; continue;
        }

        switch (choice)
        {
            case 1: lightResource.startTest();
                std::cout << "Welcome back to main menu !" << std::endl;
                break;
            case 2: printMainMenu(); break;
            case 0: cont = false;
        }
    }

    std::cout << "Terminating test !!!" << std::endl;
}
