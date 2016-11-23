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

#include "CommonUtil.h"

using namespace boost::assign;

map< OCStackResult, string > CommonUtil::s_OCStackResultString = map_list_of(OC_STACK_OK,
        "OC_STACK_OK")(OC_STACK_RESOURCE_CREATED, "OC_STACK_RESOURCE_CREATED")(
        OC_STACK_RESOURCE_DELETED, "OC_STACK_RESOURCE_DELETED")(OC_STACK_CONTINUE,
        "OC_STACK_CONTINUE")(OC_STACK_INVALID_URI, "OC_STACK_INVALID_URI")(OC_STACK_INVALID_QUERY,
        "OC_STACK_INVALID_QUERY")(OC_STACK_INVALID_IP, "OC_STACK_INVALID_IP")(OC_STACK_INVALID_PORT,
        "OC_STACK_INVALID_PORT")(OC_STACK_INVALID_CALLBACK, "OC_STACK_INVALID_CALLBACK")(
        OC_STACK_INVALID_METHOD, "OC_STACK_INVALID_METHOD")(OC_STACK_INVALID_PARAM,
        "OC_STACK_INVALID_PARAM")(OC_STACK_INVALID_OBSERVE_PARAM, "OC_STACK_INVALID_OBSERVE_PARAM")(
        OC_STACK_NO_MEMORY, "OC_STACK_NO_MEMORY")(OC_STACK_COMM_ERROR, "OC_STACK_COMM_ERROR")(
        OC_STACK_TIMEOUT, "OC_STACK_TIMEOUT")(OC_STACK_ADAPTER_NOT_ENABLED,
        "OC_STACK_ADAPTER_NOT_ENABLED")(OC_STACK_NOTIMPL, "OC_STACK_NOTIMPL")(OC_STACK_NO_RESOURCE,
        "OC_STACK_NO_RESOURCE")(OC_STACK_RESOURCE_ERROR, "OC_STACK_RESOURCE_ERROR")(
        OC_STACK_SLOW_RESOURCE, "OC_STACK_SLOW_RESOURCE")(OC_STACK_DUPLICATE_REQUEST,
        "OC_STACK_DUPLICATE_REQUEST")(OC_STACK_NO_OBSERVERS, "OC_STACK_NO_OBSERVERS")(
        OC_STACK_OBSERVER_NOT_FOUND, "OC_STACK_OBSERVER_NOT_FOUND")(OC_STACK_VIRTUAL_DO_NOT_HANDLE,
        "OC_STACK_VIRTUAL_DO_NOT_HANDLE")(OC_STACK_INVALID_OPTION, "OC_STACK_INVALID_OPTION")(
        OC_STACK_MALFORMED_RESPONSE, "OC_STACK_MALFORMED_RESPONSE")(
        OC_STACK_PERSISTENT_BUFFER_REQUIRED, "OC_STACK_PERSISTENT_BUFFER_REQUIRED")(
        OC_STACK_INVALID_REQUEST_HANDLE, "OC_STACK_INVALID_REQUEST_HANDLE")(
        OC_STACK_INVALID_DEVICE_INFO, "OC_STACK_INVALID_DEVICE_INFO")(OC_STACK_INVALID_JSON,
        "OC_STACK_INVALID_JSON")(OC_STACK_UNAUTHORIZED_REQ, "OC_STACK_UNAUTHORIZED_REQ")(
        OC_STACK_PDM_IS_NOT_INITIALIZED, "OC_STACK_PDM_IS_NOT_INITIALIZED")(OC_STACK_DUPLICATE_UUID,
        "OC_STACK_DUPLICATE_UUID")(OC_STACK_INCONSISTENT_DB, "OC_STACK_INCONSISTENT_DB")(
        OC_STACK_AUTHENTICATION_FAILURE, "OC_STACK_AUTHENTICATION_FAILURE")(OC_STACK_ERROR,
        "OC_STACK_ERROR");

long CommonUtil::s_setUpDynamicMemoryUsage = 0;
long CommonUtil::s_tearDownDynamicMemoryUsage = 0;
long CommonUtil::s_memoryDiffSum = 0;

long CommonUtil::getCurrentAllocatedMemory()
{
    return mallinfo().uordblks;
}

void CommonUtil::runCommonTCSetUpPart()
{
    s_setUpDynamicMemoryUsage = getCurrentAllocatedMemory();
}

void CommonUtil::runCommonTCTearDownPart()
{
    s_tearDownDynamicMemoryUsage = getCurrentAllocatedMemory();
    s_memoryDiffSum += s_tearDownDynamicMemoryUsage - s_setUpDynamicMemoryUsage;

    //TODO: It will be printed using new logger
    //printf("[Memory] Dynamic Memory Diff Sum : %ld bytes\n", s_memoryDiffSum);

    ::testing::Test::RecordProperty("MemoryUsage", s_memoryDiffSum);
    s_tearDownDynamicMemoryUsage = s_setUpDynamicMemoryUsage = 0;
}

void CommonUtil::launchApp(std::string app, bool withGnome)
{
#ifdef __LINUX__
    std::string cmd = "";

    if (withGnome)
    {
        cmd += "nohup gnome-terminal -x sh -c ";
    }

    cmd += app;
    cmd += " &";
    system(cmd.c_str());
#endif
}

void CommonUtil::killApp(std::string app)
{
#ifdef __LINUX__
    int firstSpace = app.find_first_of(" ");
    std::string appName = "";
    if (firstSpace > 0)
    {
        appName = app.substr(app.find_last_of("/\\") + 1, firstSpace - 1);
    }
    else
    {
        appName = app.substr(app.find_last_of("/\\") + 1);
    }

    std::string prefix = "kill -9 $(pgrep ";
    std::string postfix = ")";
    std::string command = prefix + appName + postfix;
    system(command.c_str());

    command = "killall " + appName;
    system(command.c_str());

    command = "pkill -9 " + appName;
    system(command.c_str());

#endif
}

void CommonUtil::mkDir(std::string dir)
{
#ifdef __LINUX__
    try
    {
        boost::filesystem::create_directories(dir);
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }

#endif
#ifdef __TIZEN__
    boost::filesystem::create_directories(dir);
#endif
}

void CommonUtil::rmDir(std::string dir)
{
#ifdef __LINUX__
    try
    {
        boost::filesystem::remove_all(dir);
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }
#endif
#ifdef __TIZEN__
    boost::filesystem::remove_all(dir);
#endif
}

void CommonUtil::copyFile(std::string from, std::string to)
{
#ifdef __LINUX__
    try
    {
        boost::filesystem::copy_file(from,to,boost::filesystem::copy_option::overwrite_if_exists);
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }
#endif
#ifdef __TIZEN__
    try
    {
        boost::filesystem::copy_file(from,to,boost::filesystem::copy_option::overwrite_if_exists);
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }
#endif
}

void CommonUtil::rmFile(std::string file)
{
#ifdef __LINUX__
    boost::filesystem::remove(file);
#endif
#ifdef __TIZEN__
    boost::filesystem::remove(file);
#endif
}
bool CommonUtil::isExists(std::string file)
{
    bool isExist = false;
#ifdef __LINUX__
    isExist= boost::filesystem::exists(file);
#endif
#ifdef __TIZEN__
    isExist= boost::filesystem::exists(file);
#endif
    return isExist;
}

const char* CommonUtil::GetTimeStampString()
{
    static char time_string[128] = "";

    time_t now = time(0);
    struct tm* now_tm = localtime(&now);

    snprintf(time_string, sizeof(time_string), "%d-%02d-%02d %02d:%02d:%02d",
            now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday, now_tm->tm_hour,
            now_tm->tm_min, now_tm->tm_sec);

    return time_string;
}

void CommonUtil::waitInSecond(unsigned int seconds)
{
#ifdef __LINUX__
    sleep(seconds);
#endif

#ifdef __WINDOWS__
    sleep(seconds * 1000);
#endif
#ifdef __TIZEN__
    sleep(seconds);
#endif
}

bool CommonUtil::writeFile(std::string filePath, std::string fileName, std::string fileContent)
{
    try
    {
        if (!isExists(filePath))
        {
            mkDir(filePath);
        }

        std::ofstream xmlFile(filePath + "/" + fileName);

        if (xmlFile.is_open())
        {
            xmlFile << fileContent;
            xmlFile.close();
        }
        else
        {
            return false;
        }
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return true;
}

std::string CommonUtil::getLocalIP(InternetProtocolVersion ipVersion)
{
    std::string ip = "";

#ifdef __LINUX__

    struct ifaddrs * ifAddrStruct = NULL;
    struct ifaddrs * ifa = NULL;
    void * tmpAddrPtr = NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (!ifa->ifa_addr)
        {
            continue;
        }

        if (ipVersion == IPv4)
        {
            if (ifa->ifa_addr->sa_family == AF_INET)
            {
                tmpAddrPtr = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
                char addressBuffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
                std::string token = ifa->ifa_name;

                if (token.compare("eth0") == 0)
                {
                    ip = addressBuffer;
                }
            }
        }
        else if (ipVersion == IPv6)
        {
            if (ifa->ifa_addr->sa_family == AF_INET6)
            {
                tmpAddrPtr = &((struct sockaddr_in6 *) ifa->ifa_addr)->sin6_addr;
                char addressBuffer[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);

                std::string token = ifa->ifa_name;
                if (token.compare("eth0") == 0)
                {
                    ip = addressBuffer;
                }
            }
        }
    }

    if (ifAddrStruct != NULL)
    {
        freeifaddrs(ifAddrStruct);
    }

#endif

    return ip;
}

void CommonUtil::getCurrentTime(struct tm& currentTime)
{
    time_t t = time(0); // get time now
    struct tm * now = localtime(&t);

    currentTime.tm_isdst = 0;
    currentTime.tm_year = now->tm_year + 1900;
    currentTime.tm_mon = now->tm_mon + 1;
    currentTime.tm_mday = now->tm_mday;
    currentTime.tm_hour = now->tm_hour;
    currentTime.tm_min = now->tm_min;
    currentTime.tm_sec = now->tm_sec;

    std::cout << "Date = " << currentTime.tm_year << "-" << currentTime.tm_mon << "-"
            << currentTime.tm_mday << std::endl;
    std::cout << "Time = " << currentTime.tm_hour << ":" << currentTime.tm_min << ":"
            << currentTime.tm_sec << std::endl;
}

