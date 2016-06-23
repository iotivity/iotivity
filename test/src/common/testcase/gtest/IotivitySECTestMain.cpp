/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

#include "gtest/gtest.h"
#include "gtest_custom.h"
#include "IotivityTest_Logger.h"
#include "trace.h"

class IotivityTestEnv: public ::testing::Environment
{
protected:
    virtual void SetUp()
    {

        IOTIVITYTEST_LOG(DEBUG, "Global Set Up -- finished");
    }

    virtual void TearDown()
    {

        IOTIVITYTEST_LOG(DEBUG, "Global Tear Down -- finished");
    }
};

int main(int argc, char* argv[])
{
#if defined(__LINUX__)
    /*Call-Graph Test Region*/
    if (argc > 1)
    {
        std::string str = argv[1];
        if (str == "Test")
        {
            std::cout << "found argument" << argc;
            std::cout << argv[1] << "\n";
            std::cout << "Logfile: " << cygprofile_getfilename() << std::endl;
            std::cout << "enabling\n";
            cygprofile_enable();
            std::cout << "Done enabling\n";
        }
    }
    /*End Call-Graph Test Region*/
#endif

    IOTIVITYTEST_LOG(DEBUG, "Start testing Iotivity, Testable is built at %s %s", __DATE__,
            __TIME__);

    testing::InitGoogleTest(&argc, argv);
    testing::AddGlobalTestEnvironment(new IotivityTestEnv());

    return RUN_ALL_TESTS();
}
