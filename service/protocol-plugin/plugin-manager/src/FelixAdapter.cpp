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

/// @file FelixAdapter.cpp

/// @brief

#include "FelixAdapter.h"
#include <sstream>

using namespace OIC;

FelixAdapter *FelixAdapter::s_pinstance;

FelixAdapter::FelixAdapter(JavaVM *args)
{
    jvm = args;

    config = Config::Getinstance(args);
    std::string pluginpath = config->getPluginPath();
    if (pluginpath != "")
    {
        printf("Current path is %s\n", pluginpath.c_str());
    }
    else
    {
        fprintf(stderr, "Pluin path does not exist\n");
        pluginpath = "";
    }
    registerAllPlugin(pluginpath);
}

FelixAdapter::~FelixAdapter(void)
{

    s_pinstance->deleteinstance();
    s_pinstance = NULL;
}

int FelixAdapter::installPlugin(const std::string path)
{
    return 1;
}

int FelixAdapter::findPluginRecursive(const std::string path)
{
    return 1;
}

int FelixAdapter::loadPluginInfoToManager(const std::string path)
{
    return 1;
}

int FelixAdapter::registerPlugin(const std::string path)
{
    if (path == "")
    {
        printf("FelixAdapter path == null\n");
        return 0;
    }

    JNIEnv *env;
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    jstring jpath = env->NewStringUTF(path.c_str());
    jclass cls = env->FindClass("org/iotivity/service/ppm/FelixManager");
    jmethodID mid = env->GetStaticMethodID(cls, "registerPlugin", "(Ljava/lang/String;)I");

    // call registerPlugin() function
    return  (int)env->CallStaticIntMethod(cls, mid, jpath);
}

int FelixAdapter::registerAllPlugin(const std::string path)
{
    if (path == "")
    {
        printf("FelixAdapter path == null\n");
        return 0;
    }

    JNIEnv *env;
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    jstring jpath = env->NewStringUTF(path.c_str());
    jclass cls = env->FindClass("org/iotivity/service/ppm/FelixManager");
    jmethodID mid = env->GetStaticMethodID(cls, "registerAllPlugin", "(Ljava/lang/String;)I");

    // call registerAllPlugin() function
    return (int)env->CallStaticIntMethod(cls, mid, jpath);
}

int FelixAdapter::unregisterPlugin(Plugin *const plugin)
{
    JNIEnv *env;
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    const char *cpath = plugin->getID().c_str();
    jstring jpath = env->NewStringUTF(cpath);
    jclass cls = env->FindClass("org/iotivity/service/ppm/FelixManager");
    jmethodID mid = env->GetStaticMethodID(cls, "unregisterPlugin", "(Ljava/lang/String;)I");

    // call unregisterPlugin() function
    return (int)env->CallStaticIntMethod(cls, mid, jpath);
}

int FelixAdapter::unregisterAllPlugin(void)
{
    JNIEnv *env;
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    jclass cls = env->FindClass("org/iotivity/service/ppm/FelixManager");
    jmethodID mid = env->GetStaticMethodID(cls, "unregisterAllPlugin", "()I");

    // call registerAllPlugin() function
    return (int)env->CallStaticIntMethod(cls, mid);
}

std::vector<Plugin> &FelixAdapter::getAllPlugins(void)
{
    JNIEnv *env;
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6);


    jclass cls = env->FindClass("org/iotivity/service/ppm/FelixManager");
    jmethodID mid = env->GetStaticMethodID(cls, "getAllPlugins", "()[Lorg/osgi/framework/Bundle;");

    // call getAllPlugins() function
    jobjectArray jresultArray = (jobjectArray)env->CallStaticObjectMethod(cls, mid);
    jint size = env->GetArrayLength(jresultArray);
    m_plugins.clear();

    for (int i = 0; i < (int)size; i++)
    {
        Plugin *plugin = new Plugin;

        jobject result = env->GetObjectArrayElement(jresultArray, i);
        cls = env->GetObjectClass(result);
        // set Language value
        plugin->setValue("Language", "JAVA");
        // set Id value
        mid = env->GetMethodID(cls, "getSymbolicName", "()Ljava/lang/String;");
        jstring jid = (jstring)env->CallObjectMethod(result, mid);
        std::string id = env->GetStringUTFChars(jid, 0);
        plugin->setValue("Id", id);
        // set Version value
        mid = env->GetMethodID(cls, "getVersion", "()Lorg/osgi/framework/Version;");
        jobject jversion_object = env->CallObjectMethod(result, mid);
        jclass vcls = env->GetObjectClass(jversion_object);
        mid = env->GetMethodID(vcls, "toString", "()Ljava/lang/String;");
        jstring jversion = (jstring)env->CallObjectMethod(jversion_object, mid);
        std::string version = env->GetStringUTFChars(jversion, 0);
        plugin->setValue("Version", version);
        // set Name value
        cls = env->FindClass("org/iotivity/service/ppm/FelixManager");
        mid = env->GetStaticMethodID(cls, "getValue",
                                     "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
        std::string key = "Name";
        jstring jname = (jstring)env->CallStaticObjectMethod(cls, mid, jid, 
                        env->NewStringUTF(key.c_str()));
        std::string name = env->GetStringUTFChars(jname, 0);
        plugin->setValue("Name", name);
        // set ResourceType value
        key = "ResourceType";
        jstring jresourcetype = (jstring)env->CallStaticObjectMethod(cls, mid, jid,
                                env->NewStringUTF(key.c_str()));
        std::string resourcetype = env->GetStringUTFChars(jresourcetype, 0);
        plugin->setValue("ResourceType", resourcetype);
        // push the plugin into the vector
        m_plugins.push_back(*plugin);
    }

    return m_plugins;
}

std::vector<Plugin> *FelixAdapter::findPlugins(const std::string key, const std::string value)
{
    JNIEnv *env;
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    jstring jkey = env->NewStringUTF(key.c_str());
    jstring jvalue = env->NewStringUTF(value.c_str());

    jclass cls = env->FindClass("org/iotivity/service/ppm/FelixManager");
    jmethodID mid = env->GetStaticMethodID(cls, "findPlugins",
                        "(Ljava/lang/String;Ljava/lang/String;)[Lorg/osgi/framework/Bundle;");

    // call findPlugins() function
    jobjectArray jresultArray = (jobjectArray)env->CallStaticObjectMethod(cls, mid, (jstring)jkey,
                                (jstring)jvalue);

    jint size = env->GetArrayLength(jresultArray);

    std::vector<Plugin> *re_plugins;
    re_plugins = new std::vector<Plugin>;

    for (int i = 0; i < (int)size; i++)
    {
        Plugin *plugin = new Plugin;

        jobject result = env->GetObjectArrayElement(jresultArray, i);
        cls = env->GetObjectClass(result);
        // set Language value
        plugin->setValue("Language", "JAVA");
        // set Id value
        mid = env->GetMethodID(cls, "getSymbolicName", "()Ljava/lang/String;");
        jstring jid = (jstring)env->CallObjectMethod(result, mid);
        std::string id = env->GetStringUTFChars(jid, 0);
        plugin->setValue("Id", id);
        // set Version value
        mid = env->GetMethodID(cls, "getVersion", "()Lorg/osgi/framework/Version;");
        jobject jversion_object = env->CallObjectMethod(result, mid);
        jclass vcls = env->GetObjectClass(jversion_object);
        mid = env->GetMethodID(vcls, "toString", "()Ljava/lang/String;");
        jstring jversion = (jstring)env->CallObjectMethod(jversion_object, mid);
        std::string version = env->GetStringUTFChars(jversion, 0);
        plugin->setValue("Version", version);
        // set Name value
        cls = env->FindClass("org/iotivity/service/ppm/FelixManager");
        mid = env->GetStaticMethodID(cls, "getValue",
                    "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
        std::string key = "Name";
        jstring jname = (jstring)env->CallStaticObjectMethod(cls, mid, jid, 
                        env->NewStringUTF(key.c_str()));
        std::string name = env->GetStringUTFChars(jname, 0);
        plugin->setValue("Name", name);
        // set ResourceType value
        key = "ResourceType";
        jstring jresourcetype = (jstring)env->CallStaticObjectMethod(cls, mid, jid,
                                env->NewStringUTF(key.c_str()));
        std::string resourcetype = env->GetStringUTFChars(jresourcetype, 0);
        plugin->setValue("ResourceType", resourcetype);
        // push the plugin into the vector
        m_plugins.push_back(*plugin);
    }

    return re_plugins;
}
/*
Plugin *FelixAdapter::getPlugin(const std::string plugID)
{
    return nullptr;
}
*/
bool FelixAdapter::getFileList(File_list &list, const std::string strDir)
{
    return true;
}


void FelixAdapter::printPluginList(cp_plugin_info_t **plugins)
{

}

int FelixAdapter::start(Plugin *const plugin, void *const arg)
{
    JNIEnv *env;
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    const char *cid = plugin->getID().c_str();
    jstring jid = env->NewStringUTF(cid);
    jclass cls = env->FindClass("org/iotivity/service/ppm/FelixManager");
    jmethodID mid = env->GetStaticMethodID(cls, "start", "(Ljava/lang/String;)I");

    // call start() function
    return (int)env->CallStaticIntMethod(cls, mid, jid);
}

int FelixAdapter::stop(Plugin *const plugin)
{
    JNIEnv *env;
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    const char *cid = plugin->getID().c_str();
    jstring jid = env->NewStringUTF(cid);
    jclass cls = env->FindClass("org/iotivity/service/ppm/FelixManager");
    jmethodID mid = env->GetStaticMethodID(cls, "stop", "(Ljava/lang/String;)I");

    // call stop() function
    return (int)env->CallStaticIntMethod(cls, mid, jid);
}

bool FelixAdapter::isStarted(Plugin *plugin)
{
    JNIEnv *env;
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    const char *cid = plugin->getID().c_str();
    jstring jid = env->NewStringUTF(cid);
    jclass cls = env->FindClass("org/iotivity/service/ppm/FelixManager");
    jmethodID mid = env->GetStaticMethodID(cls, "isStarted", "(Ljava/lang/String;)Z");

    // call isStarted() function
    return (bool)env->CallStaticBooleanMethod(cls, mid, jid);
}

/*
void FelixAdapter::observePluginPath(void *str)
{

}
*/

const std::string FelixAdapter::getState(const std::string plugID)
{
    JNIEnv *env;
    jvm->GetEnv((void **)&env, JNI_VERSION_1_6);

    jstring jplugID = env->NewStringUTF(plugID.c_str());
    jclass cls = env->FindClass("org/iotivity/service/ppm/FelixManager");
    jmethodID mid = env->GetStaticMethodID(cls, "getState", 
                    "(Ljava/lang/String;)Ljava/lang/String;");

    // call getState() function
    jstring jresult = (jstring)env->CallStaticObjectMethod(cls, mid, jplugID);
    return env->GetStringUTFChars(jresult, 0);
}