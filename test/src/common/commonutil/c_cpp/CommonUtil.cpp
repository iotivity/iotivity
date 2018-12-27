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
#ifdef HAVE_WINDOWS_H
#undef _WINSOCKAPI_
#define _WINSOCKAPI_
#include <windows.h>
#endif

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

char* CommonUtil::getOCStackResult(OCStackResult ocstackresult)
{
    char* resultString = NULL;

    switch (ocstackresult)
    {
        case OC_STACK_OK:
            resultString = (char*) "OC_STACK_OK";
            break;
        case OC_STACK_RESOURCE_CREATED:
            resultString = (char*) "OC_STACK_RESOURCE_CREATED";
            break;
        case OC_STACK_RESOURCE_DELETED:
            resultString = (char*) "OC_STACK_RESOURCE_DELETED";
            break;
        case OC_STACK_CONTINUE:
            resultString = (char*) "OC_STACK_CONTINUE";
            break;
        case OC_STACK_RESOURCE_CHANGED:
            resultString = (char*) "OC_STACK_RESOURCE_CHANGED";
            break;
        case OC_STACK_INVALID_URI:
            resultString = (char*) "OC_STACK_INVALID_URI";
            break;
        case OC_STACK_INVALID_QUERY:
            resultString = (char*) "OC_STACK_INVALID_QUERY";
            break;
        case OC_STACK_INVALID_IP:
            resultString = (char*) "OC_STACK_INVALID_IP";
            break;
        case OC_STACK_INVALID_PORT:
            resultString = (char*) "OC_STACK_INVALID_PORT";
            break;
        case OC_STACK_INVALID_CALLBACK:
            resultString = (char*) "OC_STACK_INVALID_CALLBACK";
            break;
        case OC_STACK_INVALID_METHOD:
            resultString = (char*) "OC_STACK_INVALID_METHOD";
            break;
        case OC_STACK_INVALID_PARAM:
            resultString = (char*) "OC_STACK_INVALID_PARAM";
            break;
        case OC_STACK_INVALID_OBSERVE_PARAM:
            resultString = (char*) "OC_STACK_INVALID_OBSERVE_PARAM";
            break;
        case OC_STACK_NO_MEMORY:
            resultString = (char*) "OC_STACK_NO_MEMORY";
            break;
        case OC_STACK_COMM_ERROR:
            resultString = (char*) "OC_STACK_COMM_ERROR";
            break;
        case OC_STACK_TIMEOUT:
            resultString = (char*) "OC_STACK_TIMEOUT";
            break;
        case OC_STACK_ADAPTER_NOT_ENABLED:
            resultString = (char*) "OC_STACK_ADAPTER_NOT_ENABLED";
            break;
        case OC_STACK_NOTIMPL:
            resultString = (char*) "OC_STACK_NOTIMPL";
            break;
        case OC_STACK_NO_RESOURCE:
            resultString = (char*) "OC_STACK_NO_RESOURCE";
            break;
        case OC_STACK_SLOW_RESOURCE:
            resultString = (char*) "OC_STACK_SLOW_RESOURCE";
            break;
        case OC_STACK_DUPLICATE_REQUEST:
            resultString = (char*) "OC_STACK_DUPLICATE_REQUEST";
            break;
        case OC_STACK_NO_OBSERVERS:
            resultString = (char*) "OC_STACK_NO_OBSERVERS";
            break;
        case OC_STACK_OBSERVER_NOT_FOUND:
            resultString = (char*) "OC_STACK_OBSERVER_NOT_FOUND";
            break;
        case OC_STACK_VIRTUAL_DO_NOT_HANDLE:
            resultString = (char*) "OC_STACK_VIRTUAL_DO_NOT_HANDLE";
            break;
        case OC_STACK_INVALID_OPTION:
            resultString = (char*) "OC_STACK_INVALID_OPTION";
            break;
        case OC_STACK_MALFORMED_RESPONSE:
            resultString = (char*) "OC_STACK_MALFORMED_RESPONSE";
            break;
        case OC_STACK_PERSISTENT_BUFFER_REQUIRED:
            resultString = (char*) "OC_STACK_PERSISTENT_BUFFER_REQUIRED";
            break;
        case OC_STACK_INVALID_REQUEST_HANDLE:
            resultString = (char*) "OC_STACK_INVALID_REQUEST_HANDLE";
            break;
        case OC_STACK_INVALID_DEVICE_INFO:
            resultString = (char*) "OC_STACK_INVALID_DEVICE_INFO";
            break;
        case OC_STACK_INVALID_JSON:
            resultString = (char*) "OC_STACK_INVALID_JSON";
            break;
        case OC_STACK_UNAUTHORIZED_REQ:
            resultString = (char*) "OC_STACK_UNAUTHORIZED_REQ";
            break;
        case OC_STACK_TOO_LARGE_REQ:
            resultString = (char*) "OC_STACK_TOO_LARGE_REQ";
            break;
        case OC_STACK_PDM_IS_NOT_INITIALIZED:
            resultString = (char*) "OC_STACK_PDM_IS_NOT_INITIALIZED";
            break;
        case OC_STACK_DUPLICATE_UUID:
            resultString = (char*) "OC_STACK_DUPLICATE_UUID";
            break;
        case OC_STACK_INCONSISTENT_DB:
            resultString = (char*) "OC_STACK_INCONSISTENT_DB";
            break;
        case OC_STACK_AUTHENTICATION_FAILURE:
            resultString = (char*) "OC_STACK_AUTHENTICATION_FAILURE";
            break;
        case OC_STACK_NOT_ALLOWED_OXM:
            resultString = (char*) "OC_STACK_NOT_ALLOWED_OXM";
            break;
        case OC_STACK_BAD_ENDPOINT:
            resultString = (char*) "OC_STACK_BAD_ENDPOINT";
            break;
        case OC_STACK_USER_DENIED_REQ:
            resultString = (char*) "OC_STACK_USER_DENIED_REQ";
            break;
        case OC_STACK_NOT_ACCEPTABLE:
            resultString = (char*) "OC_STACK_NOT_ACCEPTABLE";
            break;
        case OC_STACK_FORBIDDEN_REQ:
            resultString = (char*) "OC_STACK_FORBIDDEN_REQ";
            break;
        case OC_STACK_INTERNAL_SERVER_ERROR:
            resultString = (char*) "OC_STACK_INTERNAL_SERVER_ERROR";
            break;
        case OC_STACK_GATEWAY_TIMEOUT:
            resultString = (char*) "OC_STACK_GATEWAY_TIMEOUT";
            break;
        case OC_STACK_ERROR:
            resultString = (char*) "OC_STACK_ERROR";
            break;
        default:
            resultString = (char*) "UNKNOWN_STATE";
            break;
    }

    return resultString;
}

void CommonUtil::launchApp(std::string app, bool withGnome)
{
#if defined(__LINUX__) || defined(__TIZEN__)
    std::string cmd = "";

#ifdef __LINUX__
    if (withGnome)
    {
        cmd += "nohup gnome-terminal -x sh -c ";
    }
#endif

    cmd += app;
    cmd += " & echo $! >> pid_list.txt";
    printf("Launch App Command %s\nResult: %d\n", cmd.c_str(), system(cmd.c_str()));
#endif
}

void CommonUtil::killApp(std::string app)
{
#if defined(__LINUX__) || defined(__TIZEN__)
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

    std::string cmd = "";
    cmd = "killall " + appName;
    printf("KILL Command %s\nResult: %d\n",cmd.c_str(), system(cmd.c_str()));

    std::string prefix = "kill -9 $(pgrep ";
    std::string postfix = ")";
    cmd = prefix + appName + postfix;
    printf("KILL Command %s\nResult: %d\n",cmd.c_str(), system(cmd.c_str()));

    cmd = "pkill -9 " + appName;
    printf("pKILL Command %s\nResult: %d\n",cmd.c_str(), system(cmd.c_str()));

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
    sleep(seconds);
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

std::string CommonUtil::getLocalIPAll(InternetProtocolVersion ipVersion, std::string types)
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

                if (token.compare(types) == 0)
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
                if (token.compare(types) == 0)
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

string CommonUtil::getPIDFromPort(string port)
{
    FILE *fp;
    char pid[1035];
    port = PID_FIND_COMMAND + port;

    fp = popen(port.c_str(), "r");
    if (fp == NULL)
    {
        cout << "Failed to run command" << endl;
        exit(1);
    }

    while (fgets(pid, sizeof(pid) - 1, fp) != NULL)
    {
        cout << "Found PID is: " << pid << endl;
    }

    pclose(fp);
    return string(pid);
}

void CommonUtil::killPID(string pid)
{
    string cmd = KILL_ALL_COMMAND + pid;

#ifndef __ANDROID__
    string myPid = to_string(getpid());
#else
    std::stringstream ss;
    ss << getpid();
    string myPid = ss.str();
#endif

    if (myPid.compare(pid) != 0)
    {
        system(cmd.c_str());
    }
}
