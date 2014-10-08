//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

/// @file PluginManager.cpp

/// @brief

#include "PluginManager.h"

using namespace OIC;

PluginManager::PluginManager()
{
    //printf("PluginManager construct\n");
    m_status = cp_init();
    m_context = cp_create_context(&m_status);
    m_cp_plugins = NULL;
    m_plugin = NULL;
}

PluginManager::~PluginManager(void)
{
    //printf("~PlugingManager\n");
    cp_release_info(m_context, m_cp_plugins);
    m_thread_g.interrupt_all();
    m_thread_g.join_all();
    s_pinstance->deleteinstance();
    s_pinstance = NULL;
}

int PluginManager::installPlugin(const std::string path)
{
    char *argv = const_cast<char *>(path.c_str());

    if ((m_plugin = cp_load_plugin_descriptor(m_context, argv, &m_status)) == NULL)
    {
        printf("cp_load_plugin_descriptor failed\n");
        return false;
    }
    else if ((m_status = cp_install_plugin(m_context, m_plugin)) != 0)
    {
        printf("cp_install_plugin failed\n");
        return false;
    }

    return true;
}

int PluginManager::findPluginRecursive(const std::string path)
{
    ////////FILELIST////////////
    File_list file_list;
    getFileList(file_list, path);
    File_list::iterator itr;
    int flag = FALSE;

    for (itr = file_list.begin(); itr != file_list.end(); itr++)
    {
        if (itr->second == true)
        {
            //printf("[Direcotry] %s\n",itr->first.c_str());
        }
        else
        {
            //printf("[FILE] = %s\n" , itr->first.c_str());
            std::string filepath = itr->first.c_str();
            int count = 0;
            for (unsigned int i = 0 ; i < filepath.size(); i++)
            {
                if (filepath.at(i) == '/')
                {
                    count = i;
                }
            }
            std::string filename = filepath.substr(0, count );
            //char *temp_char = const_cast<char *>(filename.c_str());
            flag = installPlugin(filename);
            //printf("plugin file path %s \n", plugin->plugin_path);
        }
    }

    return flag;
}

int PluginManager::loadPluginInfoToManager(const std::string path)
{
    //get plugins information.
    if ((m_cp_plugins = cp_get_plugins_info(m_context, &m_status, NULL)) == NULL)
    {
        printf("cp_get_plugins_infor failed\n");
        return FALSE;
    }
    else
    {
        printPluginList(m_cp_plugins);
    }

    for (int i = 0 ; m_cp_plugins[i] != NULL; i++)
    {
        Plugin *plugin = new Plugin;
        plugin->setValue("Path", m_cp_plugins[i]->plugin_path);
        //printf("add filepath %s\n",m_cp_plugins[i]->plugin_path);
        if (m_cp_plugins[i]->identifier != NULL)
        {
            plugin->setValue("Id", m_cp_plugins[i]->identifier );
        }
        else
        {
            plugin->setValue("Id", "");
        }

        if (m_cp_plugins[i]->url != NULL)
        {
            plugin->setValue("Url", m_cp_plugins[i]->url);
        }
        else
        {
            plugin->setValue("Url", "");
        }

        if (m_cp_plugins[i]->name != NULL)
        {
            plugin->setValue("Name", m_cp_plugins[i]->name);
        }
        else
        {
            plugin->setValue("Name", "");
        }

        if (m_cp_plugins[i]->resourcetype != NULL)
        {
            plugin->setValue("ResourceType", m_cp_plugins[i]->resourcetype);
        }
        else
        {
            plugin->setValue("ResourceType", "");
        }

        if (m_cp_plugins[i]->version != NULL)
        {
            plugin->setValue("Version", m_cp_plugins[i]->version);
        }
        else
        {
            plugin->setValue("Version", "");
        }

        bool plugin_compare_flag = true;
        for (unsigned int i = 0 ; i < m_plugins.size(); i++)
        {
            if (*plugin == m_plugins[i])
            {
                delete(plugin);
                plugin_compare_flag = false;
                break;
            }
        }
        if (plugin_compare_flag)
        {
            m_plugins.push_back(*plugin);
            try
            {
                boost::thread *t = new boost::thread(boost::bind(&PluginManager::observePluginPath,
                                                     //this, (void *)path.c_str()));
                                                     this, (void *)m_cp_plugins[i]->plugin_path));
                m_thread_g.add_thread(t);
            }
            catch (...)
            {
                printf("thread throw exception\n");
            }
            delete(plugin);
        }
        //printf("plugin size = %d\n",m_plugins.size());
    }

    return TRUE;
}

int PluginManager::registerPlugin(const std::string path)
{
    //printf("PluginManager register\n");
    int flag = FALSE;

    if (path == "")
    {
        printf("PluginManager path == null\n");
        return flag;
    }

    //non recursive

    flag = installPlugin(path);

    flag = loadPluginInfoToManager(path);

    return flag;
}

int PluginManager::registerAllPlugin(const std::string path)
{
    //printf("PluginManager register\n");
    int flag = FALSE;
    if (path == "")
    {
        printf("PluginManager path == null\n");
        return flag;
    }

    //recursive

    flag = findPluginRecursive(path);

    flag = loadPluginInfoToManager(path);

    return flag;
}

int PluginManager::unregisterPlugin(Plugin *const plugin)
{
    cp_status_t status;
    char *argv = NULL;
    std::string id = "";

    for (unsigned int i = 0; i < m_plugins.size(); i++)
    {

        if (*plugin == m_plugins[i])
        {
            id = m_plugins[i].getID().c_str();
            argv = const_cast<char *>(id.c_str());
            if ((status = cp_uninstall_plugin(m_context, argv)) != 0)
            {
                printf("unregister failed\n");
                return FALSE;
            }
            m_plugins.erase(m_plugins.begin() + i);
        }
    }
    return TRUE;
}

int PluginManager::unregisterAllPlugin(void)
{
    cp_status_t status;
    char *argv = NULL;
    std::string id = "";

    while (true)
    {
        id = m_plugins[0].getID().c_str();
        argv = const_cast<char *>(id.c_str());
        if ((status = cp_uninstall_plugin(m_context, argv)) != 0)
        {
            printf("unregister failed\n");
            return FALSE;
        }
        m_plugins.erase(m_plugins.begin());
        if (m_plugins.size() == 0)
        {
            break;
        }
    }

    return TRUE;
}

std::vector<Plugin> &PluginManager::getAllPlugins(void)
{
    return m_plugins;
}

std::vector<Plugin> *PluginManager::findPlugins(const std::string key,
        const std::string value)
{
    std::vector<Plugin> *re_plugins;

    re_plugins = new std::vector<Plugin>;

    for (unsigned int i = 0; i < m_plugins.size(); i++)
    {
        if (!m_plugins[i].getValueByAttribute(key).compare(value))
        {
            re_plugins->push_back(m_plugins[i]);
        }
    }

    return re_plugins;
}



Plugin *PluginManager::getPlugin(const std::string plugID)
{
    for (unsigned int i = 0; i < m_plugins.size(); i++)
    {
        if (!(m_plugins[i].getID().compare(plugID)))
        {
            return &(m_plugins[i]);
        }
    }

    return NULL;
}

bool PluginManager::getFileList(File_list &list, const std::string strDir)
{
    struct stat statinfo;
    memset(&statinfo, 0, sizeof(statinfo));
    std::string path = strDir;
    if (0 != lstat(path.c_str(), &statinfo))
    {
        printf("PlugManager lstat is 0\n");
        return false;
    }
    if (!S_ISDIR(statinfo.st_mode))
    {
        printf("%s is not directory", strDir.c_str());
        return false;
    }

    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(strDir.c_str())) == NULL)
    {
        printf("%s open error", strDir.c_str());
        return false;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        memset(&statinfo, 0, sizeof(statinfo));
        std::string strFilePath = strDir + "/" + entry->d_name;
        while (strFilePath.find("//") != std::string::npos)
            strFilePath.replace(strFilePath.find("//"), 2, "/");

        if (0 != lstat(strFilePath.c_str(), &statinfo))
        {
            printf("PlugManager lstat is 0\n");
            closedir(dir);
            return false;
        }

        if (S_ISDIR(statinfo.st_mode))
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            list.insert(std::pair<std::string, bool>(strFilePath, true));
            std::string strSubDir = strDir + "/" + entry->d_name;
            getFileList(list, strSubDir);
        }
        else
        {
            int size = strFilePath.size();
            std::string so = strFilePath.substr(size - 2, size - 1);
            if (so  == "so")
            {
                list.insert(std::pair<std::string, bool>(strFilePath, false));
            }
        }
    }
    closedir(dir);

    return true;
}

const char *PluginManager::state_to_string(int state)
{
    switch (state)
    {
        case 0:
            return "uninstalled";
        case 1:
            return "installed";
        case 2:
            return "resolved";
        case 3:
            return "starting";
        case 4:
            return "stopping";
        case 5:
            return "active";
        default:
            return "unknown";
    }
}

void PluginManager::printPluginList(cp_plugin_info_t **plugins)
{
    const char format[] = "  %-30s %-15s %-15s %-20s\n";
    printf("\n====== Plugins List ======\n");
    printf(format,
           "ID",
           "NAME",
           "STATE",
           "TYPE");

    for (int i = 0; m_cp_plugins[i] != NULL; i++)
    {
        printf(format,
               m_cp_plugins[i]->identifier,
               m_cp_plugins[i]->name != NULL ? m_cp_plugins[i]->name : "",
               state_to_string(cp_get_plugin_state(m_context, m_cp_plugins[i]->identifier)),
               m_cp_plugins[i]->resourcetype != NULL ? m_cp_plugins[i]->resourcetype : "");
    }
    printf("\n");
}

int PluginManager::start(Plugin *const plugin, void *const arg)
{
    //printf("start\n");
    std::string id;
    cp_status_t status;
    cp_context_t *ctx;

    id = plugin->getID();
    for (unsigned int i = 0 ; i < m_plugins.size(); i++)
    {
        if (*plugin == m_plugins[i])
        {
            ctx = cpi_new_context((cp_plugin_t *)hnode_get(hash_lookup(m_context->env->plugins, id.c_str())),
                                  m_context->env, &status);
            cp_define_symbol(ctx, "START_ARGUMENT", arg);
            //printf("start ocplatform address : %x\n", arg);

            if ((status = cp_start_plugin(m_context, (char *)id.c_str()) ) != CP_OK)
            {
                printf("API function PluginManager::start() faild with error code.\n");
                return FALSE;
            }
            else
            {
                printf("start plug-in %s.\n", (char *)id.c_str());
            }

            break;
        }
    }
    return TRUE;
}

int PluginManager::stop(Plugin *const plugin)
{
    std::string id;
    cp_status_t status;

    id = plugin->getID();
    printf("stop plugin id = %s\n", id.c_str());
    if ((status = cp_stop_plugin(m_context, (char *)id.c_str())) != CP_OK)
    {
        printf("API function PluginManager::stop() faild with error code.\n");
        return FALSE;
    }
    else
    {
        printf("Stop plug-in %s.\n", (char *)id.c_str());
    }

    return TRUE;
}

bool PluginManager::isStarted(Plugin *plugin)
{
    std::string id;
    cp_plugin_state_t state;

    id = plugin->getID();

    state = cp_get_plugin_state(m_context, (char *)id.c_str());
    if (state == CP_PLUGIN_STARTING || state == CP_PLUGIN_ACTIVE)
    {
        return TRUE;
    }
    return FALSE;
}

void PluginManager::observePluginPath(void *str)
{
    //printf("start observePluginPath\n");
    int length;
    int i = 0;
    int fd;
    int wd;
    char *str1 = (char *)str;
    std::string original_path(str1);
    //printf("Directory is %s\n",(char*)str1));
    char buffer[BUF_LEN];

    fd = inotify_init();

    if ( fd < 0 )
    {
        printf("inotify_init\n");
    }

    wd = inotify_add_watch( fd, str1,
                            IN_MODIFY | IN_CREATE | IN_DELETE | IN_DELETE_SELF
                            | IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO);

    while (true)
    {
        //printf("read start\n");
        i = 0;
        length = read( fd, buffer, BUF_LEN );

        if ( length < 0 )
        {
            printf("observePluginPath read\n");
        }
        std::string filepath = original_path;
        //printf("filepath = %s\n",filepath.c_str());
        while ( i < length )
        {
            if (i < (signed)(BUF_LEN  - ( sizeof( struct inotify_event) + 16)) && i > -1)
            {
                struct inotify_event *event = ( struct inotify_event *) &buffer[ i ];

                if ((event->mask & IN_CREATE) || (event->mask & IN_MODIFY) || (event->mask & IN_MOVED_TO))
                {
                    registerAllPlugin(str1);
                }
                else
                {
                    //filepath += "/";
                    //filepath += std::string(event->name);
                    std::vector<Plugin> *resource_plugin = findPlugins("Path", filepath.c_str()); //add foldername

                    //printf("plugin size is %d\n",resource_plugin->size());
                    //printf("plugin file path is %s\n",resource_plugin->());
                    if (resource_plugin->size() == 1)
                    {
                        unregisterPlugin(&(resource_plugin->at(0)));
                    }
                    else
                    {
                        registerAllPlugin(str1);
                    }
                    delete(resource_plugin);
                    resource_plugin = NULL;
                }
                //printf("observePluginPath path = %s \n",str1);
                //printf("observePluginPath directory name = %s \n",event->name);
                i += EVENT_SIZE + event->len;
            }
        }


    }
    ( void ) inotify_rm_watch( fd, wd );
    ( void ) close( fd );
    //printf("observePluginPath end\n");
}

const std::string PluginManager::getState(const std::string plugID)
{
    return state_to_string(cp_get_plugin_state(m_context, plugID.c_str()));
}
