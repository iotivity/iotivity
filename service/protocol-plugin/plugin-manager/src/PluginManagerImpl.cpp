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

/// @file PluginManagerImpl.cpp

/// @brief

#include "PluginManagerImpl.h"

using namespace OIC;

PluginManagerImpl *PluginManagerImpl::s_pinstance = nullptr;

extern "C" PluginManagerImpl *create_object()
{
    PluginManagerImpl *newobj;
    newobj =  new PluginManagerImpl;
    return newobj;
}

extern "C" void destroy_object( PluginManagerImpl *object )
{
    delete object;
}

PluginManagerImpl::PluginManagerImpl()
{
    cppm = CpluffAdapter::Getinstance();
    javappm = FelixAdapter::Getinstance();
    refreshPluginInfo();
}

PluginManagerImpl::~PluginManagerImpl()
{

}

int PluginManagerImpl::registerPlugin(std::string path)
{
    int flag = 0;
    flag = cppm->registerPlugin(path);
    flag = javappm->registerPlugin(path);
    refreshPluginInfo();
    return flag;
}

int PluginManagerImpl::registerAllPlugin(std::string path)
{
    int flag = 0;
    flag = cppm->registerAllPlugin(path);
    flag = javappm->registerAllPlugin(path);
    refreshPluginInfo();
    return flag;
}

int PluginManagerImpl::unregisterPlugin(std::string id)
{
    int flag = 0;

    for (unsigned int i = 0 ; i < m_plugins.size(); i++)
    {
        if (!m_plugins[i].getID().compare(id))
        {
            if (!m_plugins[i].getValueByAttribute("Language").compare("CPP"))
            {
                if ((flag = cppm->unregisterPlugin(&m_plugins[i])))
                {
                    m_plugins.erase(m_plugins.begin() + i);
                }
            }
            else if (!m_plugins[i].getValueByAttribute("Language").compare("JAVA"))
            {
                if ((flag = javappm->unregisterPlugin(&m_plugins[i])))
                {
                    m_plugins.erase(m_plugins.begin() + i);
                }
            }
        }
    }

    return flag;
}

int PluginManagerImpl::unregisterAllPlugin()
{
    int flag = 0;
    flag = cppm->unregisterAllPlugin();
    flag = javappm->unregisterAllPlugin();
    m_plugins.clear();
    return flag;
}

int PluginManagerImpl::rescanPlugin()
{
    Config *config = Config::Getinstance();
    std::string pluginpath = config->getPluginPath();
    if (pluginpath != "")
    {
        printf("Current path is %s\n", pluginpath.c_str());
    }
    else
    {
        fprintf(stderr, "Pluing path does not exist\n");
        pluginpath = "";
    }
    int result = registerAllPlugin(pluginpath);
    return result;
}


std::vector<Plugin> &PluginManagerImpl::getAllPlugins(void)
{
    return m_plugins;
}

std::vector<Plugin> *PluginManagerImpl::findPlugins(const std::string key,
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

Plugin *PluginManagerImpl::getPlugin(const std::string pluginID)
{
    for (unsigned int i = 0; i < m_plugins.size(); i++)
    {
        if (!(m_plugins[i].getID().compare(pluginID)))
        {
            return &(m_plugins[i]);
        }
    }

    return nullptr;
}
int PluginManagerImpl::startPlugins(const std::string key, const std::string value)
{
    int flag = FALSE;
    std::string resourcetype = "";
    int size = value.size();

    //Parsing Resource_Type
    for (int i = size - 1; i > -1 ; i--)
    {
        if (value.at(i) == '.' && value.at(i) == '/')
        {
            break;
        }
        resourcetype += value.at(i);
    }

    //Reverse string(Resource value)
    reverse(resourcetype.begin(), resourcetype.end());

    //Search Plugin
    std::vector<Plugin> *resource_plugin = findPlugins(key, resourcetype);

    //start Plugin
    std::vector<Plugin>::iterator it;
    for (it = resource_plugin->begin(); it != resource_plugin->end(); it++)
    {
        if (!isStarted(&(*it))) // started : 1, stop : 0
        {
            flag = startPlugins(&(*it));//TODO PASS the OCPlatform
        }
    }
    delete(resource_plugin);
    resource_plugin = nullptr;
    return flag;
}

int PluginManagerImpl::startPlugins(Plugin *const plugin)
{
    int flag = FALSE;
    void *arg  = nullptr;

    flag = startbyPlatform(plugin, arg);

    return flag;
}

int PluginManagerImpl::stopPlugins(const std::string key, const std::string value)
{
    int flag = FALSE;
    std::string resourcetype = "";
    int size = value.size();

    //Parsing Resource_Type
    for (int i = size - 1; i > -1 ; i--)
    {
        if (value.at(i) == '.' && value.at(i) == '/')
        {
            break;
        }
        resourcetype += value.at(i);
    }

    //Reverse string(Resource value)

    reverse(resourcetype.begin(), resourcetype.end());

    // printf("resource value = %s\n", resourcetype.c_str());

    //Search Plugin
    std::vector<Plugin> *resource_plugin = findPlugins(key, resourcetype);

    //stop Plugin
    std::vector<Plugin>::iterator it;
    for (it = resource_plugin->begin(); it != resource_plugin->end(); it++)
    {
        if (isStarted(&(*it))) // started : 1, stop : 0
        {
            flag = stopPlugins(&(*it));//TODO PASS the OCPlatform
        }
    }
    delete(resource_plugin);
    resource_plugin = nullptr;
    return flag;
}

int PluginManagerImpl::stopPlugins(Plugin *const plugin)
{
    int flag = FALSE;

    flag = stopbyPlatform(plugin);

    return flag;
}

int PluginManagerImpl::startbyPlatform(Plugin *const plugin, void *const arg)
{
    int flag = 0;

    for (unsigned int i = 0 ; i < m_plugins.size(); i++)
    {
        if (!m_plugins[i].getID().compare(plugin->getID()))
        {
            if (!m_plugins[i].getValueByAttribute("Language").compare("CPP"))
            {
                flag = cppm->start(plugin, arg);
            }
            else if (!m_plugins[i].getValueByAttribute("Language").compare("JAVA"))
            {
                flag = javappm->start(plugin, arg);
            }
        }
    }

    if (flag)
    {
        refreshPluginInfo();
    }
    return flag;
}

int PluginManagerImpl::stopbyPlatform(Plugin *const plugin)
{
    int flag = 0;

    for (unsigned int i = 0 ; i < m_plugins.size(); i++)
    {
        if (!m_plugins[i].getID().compare(plugin->getID()))
        {
            if (!m_plugins[i].getValueByAttribute("Language").compare("CPP"))
            {
                flag = cppm->stop(plugin);
            }
            else if (!m_plugins[i].getValueByAttribute("Language").compare("JAVA"))
            {
                flag = javappm->stop(plugin);
            }
        }
    }

    if (flag)
    {
        refreshPluginInfo();
    }
    return flag;
}

bool PluginManagerImpl::isStarted(Plugin *plugin)
{
    bool flag = 0;
    if (cppm->isStarted(plugin))
    {
        flag = TRUE;
        return flag;
    }

    if (javappm->isStarted(plugin))
    {
        flag = TRUE;
        return flag;
    }
    return flag;
}

std::string PluginManagerImpl::getState(std::string plugID)
{
    std::string str = "";

    for (unsigned int i = 0 ; i < m_plugins.size(); i++)
    {
        if (!m_plugins[i].getID().compare(plugID))
        {
            if (!m_plugins[i].getValueByAttribute("Language").compare("CPP"))
            {
                str = cppm->getState(plugID);
            }
            else if (!m_plugins[i].getValueByAttribute("Language").compare("JAVA"))
            {
                str = javappm->getState(plugID);
            }
        }
    }

    return str;
}

std::vector<Plugin> PluginManagerImpl::refreshPluginInfo()
{
    m_plugins.clear();
    m_plugins = cppm->getAllPlugins();

    std::vector<Plugin> java_plugins = javappm->getAllPlugins();
    int size = java_plugins.size();

    for (int i = 0 ; i < size ; i++)
    {
        m_plugins.push_back(java_plugins[i]);
    }

    return m_plugins;
}