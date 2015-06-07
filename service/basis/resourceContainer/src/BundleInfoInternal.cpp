//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#include "BundleInfoInternal.h"

namespace RC
{
    BundleInfoInternal::BundleInfoInternal()
    {

    }

    BundleInfoInternal::~BundleInfoInternal()
    {

    }

    void BundleInfoInternal::setID(string id)
    {
        m_ID = id;
    }

    string BundleInfoInternal::getID()
    {
        return m_ID;
    }

    void BundleInfoInternal::setPath(string path)
    {
        m_path = path;
    }
    string BundleInfoInternal::getPath()
    {
        return m_path;
    }

    void BundleInfoInternal::setVersion(string version)
    {
        m_version = version;
    }

    string BundleInfoInternal::getVersion()
    {
        return m_version;
    }

    void BundleInfoInternal::setLoaded(bool loaded)
    {
        m_loaded = loaded;
    }

    bool BundleInfoInternal::isLoaded()
    {
        return m_loaded;
    }

    void BundleInfoInternal::setActivated(bool activated)
    {
        m_activated = activated;
    }

    bool BundleInfoInternal::isActivated()
    {
        return m_activated;
    }

    int BundleInfoInternal::getId()
    {
        return m_id;
    }

    void BundleInfoInternal::setId(int id)
    {
        m_id = id;
    }

    void BundleInfoInternal::setBundleActivator(activator_t* activator)
    {
        m_activator = activator;
    }

    activator_t* BundleInfoInternal::getBundleActivator()
    {
        return m_activator;
    }

    void BundleInfoInternal::setBundleDeactivator(deactivator_t* deactivator)
    {
        m_deactivator = deactivator;
    }

    deactivator_t* BundleInfoInternal::getBundleDeactivator()
    {
        return m_deactivator;
    }

    void BundleInfoInternal::setBundleHandle(void* handle)
    {
        m_bundleHandle = handle;
    }

    void* BundleInfoInternal::getBundleHandle()
    {
        return m_bundleHandle;
    }
}
