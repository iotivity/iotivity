//******************************************************************
//
// Copyright 2017 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <sstream>
#include <atomic>
#include <csignal>
#include <messageHandler.h>
#include "sqlite3.h"
#include <algorithm>

#include "miniPluginManager.h"

using namespace std;

#define TERMINAL_BOLD_TEXT_RED            "\033[1;31m"
#define TERMINAL_ATTRIBUTE_RESET          "\033[0m"

#define BIND_INDEX_FIRST 1
#define BIND_INDEX_SECOND 2
#define BIND_INDEX_THIRD 3
#define SQLITE_INSERT_T "INSERT INTO RECONNECT VALUES(?,?,?)"
#define SQLITE_GET_RECONNECT_FILE "SELECT RECONNECT_FILE from RECONNECT where PLUGIN_NAME=?"
#define SQLITE_DELETE_DEVICE "DELETE FROM RECONNECT WHERE URI=?"
#define SQLITE_SELECT_RECONNECT_FILE "SELECT RECONNECT_FILE from RECONNECT where URI=?"

#define RECONNECT_FILE_LENGTH 260

void scanCommand(MPMPluginHandle handle);

map<string, MPMPluginHandle> g_loadedPlugins;
std::atomic_bool keepEventLoop(true);
bool autoScanMode = false;
bool autoAddMode = false;

std::string rawUsageMessage =
    R"(
Usage: sample_app [OPTION]... PLUGINS_TO_LOAD...

Load and unload plugins, send them commands after loading etc.
Give valid inputs! This is just an example client and input is not sanitized to keep
the client minimal.

-n          Start plugins in NON secure mode. Default is secure.

-s          Start this client in auto scan mode.
            This will cause the client send a SCAN message at startup to each loaded
            plugin. If used in conjunction with '-a', then no user interaction required.

-a          Start this client in auto add mode.
            This will cause the client send an ADD message with the same message as in
            the scan response.

-h          View this message.
)";

template <typename T>
void log(T t)
{
    cout << TERMINAL_BOLD_TEXT_RED << t << TERMINAL_ATTRIBUTE_RESET;
}
template<typename T, typename... Args>
void log(T t, Args... args)
{
    cout << TERMINAL_BOLD_TEXT_RED << t << TERMINAL_ATTRIBUTE_RESET;
    log(args...);
}

enum pluginCommands { SCAN, ADD, REMOVE, UNLOAD, MAX_COMMANDS };

/**
 * Macro to verify sqlite success.
 * eg: VERIFY_NON_NULL(TAG, ptrData, ERROR,OC_STACK_ERROR);
 */
#define PDM_VERIFY_SQLITE_OK(tag, arg, logLevel) do{ if (SQLITE_OK != (arg)) \
            { log("Error in " #arg ", Error Message: ", \
               sqlite3_errmsg(db), "\n"); return; }}while (0)

sqlite3 *db;

void printUsage()
{
    std::cout << rawUsageMessage << "\n\n";
}

static int sqlCallback(void *data, int argc, char **argv, char **azColName)
{
    int i;

    log("Data: ", (const char*)data, "\n");
    for(i=0; i<argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void fileWriteAndDbUpdate(const char * uri, const char * pluginName, const char * metadata)
{
    FILE *fp = NULL;
    char filename[RECONNECT_FILE_LENGTH];
    const char* data = "Callback function called";
    int rc;
    char *zErrMsg = 0;
    std::string sql;
    std::string changedUri = uri;
    sqlite3_stmt *stmt = 0;

    replace(changedUri.begin(), changedUri.end(), '/', '_' );
    changedUri.erase(0,1);

    memset(filename, 0, RECONNECT_FILE_LENGTH);
    strncpy(filename, changedUri.c_str(), changedUri.length());
    strncat(filename, ".txt", strlen(".txt")+1);

    printf("filename: %s\n", filename);
    fp = fopen(filename, "w");
    if (!fp)
    {
        printf("Unable to open file");
        return;
    }
    fwrite(metadata, 1, MPM_MAX_METADATA_LEN, fp);
    fputs("\n", fp);
    fclose(fp);

    /* Create SQL statement */
    rc = sqlite3_prepare_v2(db, SQLITE_INSERT_T,
                              strlen(SQLITE_INSERT_T) + 1, &stmt, NULL);
    PDM_VERIFY_SQLITE_OK(TAG, rc, ERROR);

    rc = sqlite3_bind_text(stmt, BIND_INDEX_FIRST, uri, MPM_MAX_URI_LEN, SQLITE_STATIC);
    PDM_VERIFY_SQLITE_OK(TAG, rc, ERROR);

    rc = sqlite3_bind_text(stmt, BIND_INDEX_SECOND, pluginName, MAX_SHARED_OBJECT_NAME_LENGTH, SQLITE_STATIC);
    PDM_VERIFY_SQLITE_OK(TAG, rc, ERROR);

    rc = sqlite3_bind_text(stmt, BIND_INDEX_THIRD, filename, RECONNECT_FILE_LENGTH, SQLITE_STATIC);
    PDM_VERIFY_SQLITE_OK(TAG, rc, ERROR);

    rc = sqlite3_step(stmt);
    if (SQLITE_DONE != rc)
    {
        if (SQLITE_CONSTRAINT == rc)
        {
            //new OCStack result code
            log("Error Occured: ",sqlite3_errmsg(db), "\n");
            sqlite3_finalize(stmt);
            return;
        }
        log("Error Occured: ",sqlite3_errmsg(db), "\n");
        sqlite3_finalize(stmt);
        return ;
    }
    sqlite3_finalize(stmt);

    /* Create SQL statement */
    sql = "SELECT * from RECONNECT";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), sqlCallback, (void*)data, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        log("SQL error: ", zErrMsg, "\n");
        sqlite3_free(zErrMsg);
    }
    else
    {
        log("Operation done successfully\n");
    }

}

void deleteReconnectFile(char * uri)
{
    sqlite3_stmt *stmt = 0;
    int res = 0;

    res = sqlite3_prepare_v2(db, SQLITE_SELECT_RECONNECT_FILE,
                              strlen(SQLITE_SELECT_RECONNECT_FILE) + 1, &stmt, NULL);
    PDM_VERIFY_SQLITE_OK(TAG, res, ERROR);
    res = sqlite3_bind_text(stmt, BIND_INDEX_FIRST, uri, MPM_MAX_URI_LEN, SQLITE_STATIC);
    PDM_VERIFY_SQLITE_OK(TAG, res, ERROR);

    if (SQLITE_ROW == sqlite3_step(stmt))
    {
        char reconnect_file[RECONNECT_FILE_LENGTH];
        const unsigned char *ptr = sqlite3_column_text(stmt, 0);
        if (ptr == NULL)
        {
            log("An internal error occured\n");
            return;
        }
        memcpy(reconnect_file, ptr, RECONNECT_FILE_LENGTH);
        log("reconnect file to be deleted is: ", reconnect_file, "\n");
        remove(reconnect_file);
    }
}

void deleteRecordFromDb(char * uri)
{
    int res = 0;
    sqlite3_stmt *stmt = 0;

    res = sqlite3_prepare_v2(db, SQLITE_DELETE_DEVICE, strlen(SQLITE_DELETE_DEVICE) + 1, &stmt, NULL);
    PDM_VERIFY_SQLITE_OK(TAG, res, ERROR);

    res = sqlite3_bind_text(stmt, BIND_INDEX_FIRST, uri, MPM_MAX_URI_LEN, SQLITE_STATIC);
    PDM_VERIFY_SQLITE_OK(TAG, res, ERROR);

    if (SQLITE_DONE != sqlite3_step(stmt))
    {
        log("Delete error: ", sqlite3_errmsg(db), "\n");
        sqlite3_finalize(stmt);
        return;
    }
    log("successful", "\n");
    sqlite3_finalize(stmt);
}

void clearDevice(char * device)
{
    char * uri = (char *) calloc(1, MPM_MAX_URI_LEN);
    if (!uri)
    {
        log(std::string("calloc failed"), "\n");
        return;
    }
    memcpy(uri, device, strlen(device));

    deleteReconnectFile(uri);
    deleteRecordFromDb(uri);

    free(uri);
}

MPMCbResult onCallback(uint32_t msgType, MPMMessage message, size_t size, const char * plugin_name)
{
    log("Message from:", std::string(plugin_name), "\n");
    log("Interpreting payload as string:\n");

    char* message_char = (char*) calloc(1, size + 1);
    if (message_char == NULL)
    {
        log("Message_char is null\n");
        return MPM_CB_RESULT_ERROR;
    }
    memset(message_char, 0, size + 1);
    memcpy(message_char, message, size);
    log(std::string(message_char), "\n");

    if (msgType == MPM_SCAN && autoAddMode)
    {
        MPMAddDevice(g_loadedPlugins[std::string(plugin_name)], message_char, size + 1);
    }
    else if (msgType == MPM_ADD)
    {
        MPMAddResponse addResponse;
        memset(&addResponse, 0, sizeof(MPMAddResponse));
        memcpy(&addResponse, message_char, sizeof(MPMAddResponse));

        log("uri: ", addResponse.uri, "\n");

        fileWriteAndDbUpdate(addResponse.uri, plugin_name, addResponse.metadata);
    }
    else if (msgType == MPM_REMOVE)
    {
        clearDevice(message_char);
    }

    free(message_char);

    return MPM_CB_RESULT_OK;
}

void loadPlugin(const std::string pluginPath)
{
    MPMPluginHandle pluginHandle;

    MPMResult result = MPMLoad(&pluginHandle, pluginPath.c_str(), onCallback, NULL);

    if (result == MPM_RESULT_OK)
    {
        g_loadedPlugins[pluginPath] = pluginHandle;
        printf("%p\n", pluginHandle);
        log(pluginPath, " is loaded.\n");
        if(autoScanMode)
        {
            log(pluginPath, " performing auto scan.\n");
            scanCommand(pluginHandle);
        }
    }
    else
    {
        std::cout << pluginPath << " failed to load." << std::endl;
    }
}

void printPlugins()
{
    log("Plugins:\n");
    int no = 0;
    for (map<std::string, MPMPluginHandle >::iterator itr = g_loadedPlugins.begin(); itr != g_loadedPlugins.end();
            ++itr)
    {
        log("\t", no, ":", itr->first, "\n");
        ++no;
    }
}

void printCommands()
{
    log("Commands:", "\t" , SCAN , ":" , "SCAN  ", ADD , ":" , "ADD  ", REMOVE , ":" , "REMOVE  "
    , UNLOAD , ":" , "UNLOAD" , "\n");
}

void unloadCommand(MPMPluginHandle handle)
{
    std::string uri;
    for (map<std::string, MPMPluginHandle >::iterator itr = g_loadedPlugins.begin(); itr != g_loadedPlugins.end();
         ++itr)
    {
        if (itr->second == handle)
        {
            uri = itr->first;
        }
    }
    log("Unloading ", uri, "\n");
    MPMUnload(handle);
    g_loadedPlugins.erase(uri);
}

void addCommand(MPMPluginHandle handle)
{
    std::string message;
    log("What message to send to ADD (Usually URI of a \"scanned\" resource) : ");
    std::cin >> message;

    log("Sending ", message.c_str(), "\n");
    MPMAddDevice(handle, const_cast<char *> (message.c_str()), message.size() + 1);
}

void removeCommand(MPMPluginHandle handle)
{
    std::string message;
    log("What message to send to REMOVE (Usually URI of a \"added\" resource) : ");
    std::cin >> message;

    log("Sending ", message.c_str(), "\n");
    MPMRemoveDevice(handle, const_cast<char *> (message.c_str()), message.size() + 1);
}

void scanCommand(MPMPluginHandle handle)
{
    MPMScan(handle, NULL, 0);
}


void processCommand (MPMPluginHandle handle, int commandNo)
{
    switch(commandNo)
    {
        case UNLOAD:
            unloadCommand(handle);
            break;
         case SCAN:
            scanCommand(handle);
            break;
         case ADD:
            addCommand(handle);
            break;
        case REMOVE:
            removeCommand(handle);
            break;
         default:
            log("Invalid command\n");
            break;
    }
}
void getCommandEventLoop()
{
    while (keepEventLoop)
    {
        std::string input;
        int plugin_no = -1, commandNo = -1;

        std::cout << endl << endl;
        if (g_loadedPlugins.empty())
        {
            log("No plugins loaded. We're done here. Bye!\n");
            break;
        }
        printPlugins();
        std::cout << "\n";
        printCommands();
        log("Enter blank line to see more logs.\n");
        log("Enter plugin_no command_no : ");
        while (getline(cin, input ))
        {
            if (input != "\n")
            {
                stringstream ss(input);
                ss >> plugin_no >> commandNo;
                break;
            }
        }

        if (commandNo != -1 || plugin_no != -1)
        {
            MPMPluginHandle selectedPlugin = NULL;
            int i = 0;

            for (map<std::string, MPMPluginHandle >::iterator itr = g_loadedPlugins.begin();
                 itr != g_loadedPlugins.end();
                 ++itr)
            {
                if (i == plugin_no)
                {
                    selectedPlugin = itr->second;
                    break;
                }
                ++i;
            }
            processCommand (selectedPlugin, commandNo);
        }
    }
    for (map<std::string, MPMPluginHandle >::iterator itr = g_loadedPlugins.begin();
         itr != g_loadedPlugins.end();
         ++itr)
    {
        unloadCommand(itr->second);
    }
}

void loadCommandLinePlugins (std::vector<string> &pluginsToLoad)
{
    for (uint32_t i = 0; i < pluginsToLoad.size(); ++i)
    {
        log("Loading ", pluginsToLoad[i], "\n");
        loadPlugin(pluginsToLoad[i]);
    }
}

void signalHandler(int signal)
{
    (void) signal;
    keepEventLoop = false;
}

void openDatabase()
{
    int rc;
    char *zErrMsg = 0;
    std::string sql;
    rc = sqlite3_open("reconnect.db", &db);
    if (rc)
    {
        log("Can't open database: ", sqlite3_errmsg(db), "\n");
        return;
    }
    else
    {
        log("Opened database successfully\n");
    }
    /* Create SQL statement */
    sql = "CREATE TABLE IF NOT EXISTS RECONNECT("  \
          "URI TEXT PRIMARY KEY NOT NULL," \
          "PLUGIN_NAME TEXT NOT NULL," \
          "RECONNECT_FILE TEXT NOT NULL);";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        log("SQL error: ", zErrMsg, "\n");
        sqlite3_free(zErrMsg);
    }
    else
    {
       log("Table created successfully\n");
    }
}

void reconnect()
{
    FILE *fp = NULL;

    size_t len = -1;
    int ret = 0;
    char * metadata = NULL;
    sqlite3_stmt *stmt = 0;
    int res = 0;
    for (map<std::string, MPMPluginHandle >::iterator itr = g_loadedPlugins.begin();
             itr != g_loadedPlugins.end();
             ++itr)
    {
        res = sqlite3_prepare_v2(db, SQLITE_GET_RECONNECT_FILE,
                                      strlen(SQLITE_GET_RECONNECT_FILE) + 1, &stmt, NULL);
        PDM_VERIFY_SQLITE_OK(TAG, res, ERROR);

        char * plugin_name = (char *) calloc( 1, MAX_SHARED_OBJECT_NAME_LENGTH);
        if (plugin_name == NULL)
        {
            log("calloc failed","\n");
            return ;
        }
        memcpy(plugin_name, (*itr).first.c_str(), (*itr).first.length());

        res = sqlite3_bind_text(stmt, BIND_INDEX_FIRST, plugin_name, MAX_SHARED_OBJECT_NAME_LENGTH, SQLITE_STATIC);
        PDM_VERIFY_SQLITE_OK(TAG, res, ERROR);
        while (SQLITE_ROW == sqlite3_step(stmt))
        {
            char filename[RECONNECT_FILE_LENGTH];
            const unsigned char *ptr = sqlite3_column_text(stmt, 0);
            if (ptr == NULL)
            {
                log("An internal error occured\n");
                continue;
            }
            memcpy(filename, ptr, RECONNECT_FILE_LENGTH);
            log("reconnect filename is: ", filename, "\n");
            fp = fopen(filename, "r");
            if (fp == NULL)
            {
                log("File could not be opened\n");
                continue;
            }
            ret = getline(&metadata, &len, fp);
            if (ret == -1)
            {
                fclose(fp);
                continue;
            }
            fclose(fp);
            MPMReconnectDevice(itr->second, metadata, MPM_MAX_METADATA_LEN);
        }
        printf("no columns returned\n");
        free(metadata);
        metadata = NULL;
        free(plugin_name);
        sqlite3_finalize(stmt);
    }
}


int main(int argc, char const *argv[])
{
    vector<string> pluginsToLoad;
    pluginsToLoad.clear();

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-n") == 0)
        {
            setenv("NONSECURE", "true", 1);
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            autoScanMode = true;
        }
        else if (strcmp(argv[i], "-a") == 0)
        {
            autoAddMode = true;
        }
        else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
        {
            printUsage();
            return 0;
        }
        else
        {
            pluginsToLoad.push_back(std::string(argv[i]));
        }
    }

    if (!pluginsToLoad.empty())
    {
        loadCommandLinePlugins(pluginsToLoad);
    }
    else
    {
        log("At least one plugin needed\n");
        return 0;
    }

    openDatabase();

    std::signal(SIGINT, signalHandler);

    reconnect();

    std::thread eventLoopThread = std::thread(getCommandEventLoop);
    eventLoopThread.join();

    sqlite3_close(db);
    return 0;
}
