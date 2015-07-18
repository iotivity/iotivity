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

/**
 * @file
 *
 * This file contains the resource object APIs provided to the developers.
 * ResourceObject is a part of the server builder module.
 */
#ifndef SERVERBUILDER_RESOURCEOBJECT_H
#define SERVERBUILDER_RESOURCEOBJECT_H

#include <string>
#include <mutex>
#include <thread>

#include <boost/atomic.hpp>

#include <ResourceAttributes.h>
#include <RCSResponse.h>
#include <RCSRequest.h>

namespace OC
{
    /**
     * Forward declaration
     */
    class OCResourceRequest;
}

namespace OIC
{
    namespace Service
    {

        /**
         * @class   NoLockException
         * @brief   This class is  used to throw exception to the upper layer if lock has not been acquired.
         *              It is inherited from PrimitiveException class.
         *
         */
        class NoLockException: public RCSException
        {
            public:
                NoLockException(std::string &&what) : RCSException { std::move(what) } {}
        };

        /**
         * @class ResourceObject
         * @brief This class provides APIs and subclasses for creation of resource(s), setting and getting
         *            resource properties and attributes, and for handling of requests.
         *            It provides resource locking feature for synchronized operations. On creating a resouce
         *            using the build() API, the developer gets a ResourceObject pointer
         *
         *@see build()
         */
        class ResourceObject
        {
            private:
                class WeakGuard;

            public:

                /**
                * enum class contains the different Auto notify policy
                */
                enum class AutoNotifyPolicy
                {
                    NEVER,
                    ALWAYS,
                    UPDATED
                };

                /**
                * enum class contains the different Request Handler policies
                */
                enum class SetRequestHandlerPolicy
                {
                    NEVER,
                    ACCEPTANCE
                };

                /**
                  * declaring member as a shared pointer to object of own class.
                  */
                typedef std::shared_ptr< ResourceObject > Ptr;
                /**
                  * declaring member as a shared pointer to constant object of own class.
                  */
                typedef std::shared_ptr< const ResourceObject > ConstPtr;

                /**
                 * @class   Builder
                 * @brief   This class provides APIs for resource creation, setting properties & attributes for the
                 *              constructed resource. It is a subclass of ResourceObject. It provides the build() API
                 *              which builds a resource and return pointer to ResourceObject class.
                 *
                 *@see build()
                 */
                class Builder
                {
                    public:
                        /**
                         * @brief Constructor.
                         *           Sets the resource property values using initializers list.
                         *
                         * @param uri - Resource URI value to be set
                         * @param type - Resource type value to be set
                         * @param interface - Interface value to be set
                         *
                         *NOTE : m_properties value is by default set to OC_DISCOVERABLE | OC_OBSERVABLE.
                         *           OC_DISCOVERABLE and OC_OBSERVABLE are defined in octypes.h.
                         */
                        Builder(const std::string &uri, const std::string &type,
                                const std::string &interface);

                        /**
                        * Sets the discoverable(OC_DISCOVERABLE) property for the resource.
                        *
                        * @param discoverable - true or false
                        *
                        * @return Builder& -reference of the builder class
                        *
                        *NOTE : OC_DISCOVERABLE is defined in octypes.h
                        */
                        Builder &setDiscoverable(bool discoverable);
                        /**
                        * Sets the observable(OC_OBSERVABLE) property of the resource.
                        *
                        * @param observable - true or false
                        *
                        * @return Builder& - reference of the builder class
                        *
                        *NOTE : OC_DISCOVERABLE is defined in octypes.h
                        */
                        Builder &setObservable(bool observable);

                        /**
                        * Sets attribute of the resource.
                        *
                        * @param attributes - Resource attributes to set
                        *
                        * @return Builder& - reference of the builder class
                        */
                        Builder &setAttributes(const ResourceAttributes &attributes);
                        /**
                        * API for setting attributes of the resource.
                        *
                        * @param attributes - Resource Attributes to set
                        *
                        * @return Builder& - reference of the builder class
                        */
                        Builder &setAttributes(ResourceAttributes &&attributes);

                        /**
                         * API for constructing a new resource.
                         * It internally invokes the ResourceObject constructor to create a
                         * new ResourceObject instance. It also binds an entity handler to the ResourceObject created.
                         * Then it registers a resource with set properties to obtain a resource handle.
                         * This handle is set as the handle of ResourceObject created.
                         * On successful execution the pointer to this object is returned.
                         *
                         * @return ResourceObject::Ptr - Pointer to ResourceObject instance created.
                         *
                         * @throw PlatformException
                         *       It catches exception from registerResource API of OCPlatform and throws it to developer.
                         *
                         */
                        ResourceObject::Ptr build();

                    private:

                        /**
                         *  Resource URI.
                         */
                        std::string m_uri;

                        /**
                         *  Resource type.
                         */
                        std::string m_type;

                        /**
                         *  Resource interface.
                         */
                        std::string m_interface;

                        /**
                         *  Resource property.
                         */
                        uint8_t m_properties;

                        /**
                         *  Attributes of the resource.
                         */
                        ResourceAttributes m_resourceAttributes;
                };

                class LockGuard;

                /**
                * Request handler for get requests
                */
                typedef std::function < RCSGetResponse(const RCSRequest &,
                                                       ResourceAttributes &) > GetRequestHandler;
                /**
                * Request handler for set requests
                */
                typedef std::function < RCSSetResponse(const RCSRequest &,
                                                       ResourceAttributes &) > SetRequestHandler;
                /**
                *  Listener for update of attribute
                */
                typedef std::function < void(const ResourceAttributes::Value &,
                                             const ResourceAttributes::Value &) > AttributeUpdatedListener;

            public:
                /**
                * Constructor
                */
                ResourceObject(ResourceObject&&) = delete;
                ResourceObject(const ResourceObject &) = delete;

                /**
                * overloading = operator
                */
                ResourceObject &operator=(ResourceObject && ) = delete;
                ResourceObject &operator=(const ResourceObject &) = delete;

                /**
                 * Virtual destructor.
                 */
                virtual ~ResourceObject();

                /**
                 * API for setting a particular attribute value.
                 * It uses mutex to invoke autoNotifyIfNeeded on updating the value. This notifies all observers
                 * with updated attributes value as per the notification policy(AutoNotifyPolicy).
                 *
                 * @param key - name of attribute(used to map the attribute value).
                 *
                 * @param value - attribute value to be mapped against the key.
                 *
                 *NOTE : AutoNotifyPolicy enum class is defined in ResourceObject class.
                 */
                void setAttribute(const std::string &key, const ResourceAttributes::Value & value);
                /**
                 * API for setting a particular attribute value.
                 * It uses mutex to invoke autoNotifyIfNeeded on updating the value. This notifies all observers
                 * with updated attributes value as per the notification policy(AutoNotifyPolicy).
                 *
                 * @param key - name of attribute(used to map the attribute value).
                 *
                 * @param value - attribute value to be mapped against the key.
                 *
                 *NOTE : AutoNotifyPolicy enum class is defined in ResourceObject class.
                 */
                void setAttribute(const std::string &key, ResourceAttributes::Value&& value);

                /**
                 * API for setting a particular attribute value.
                 * It uses mutex to invoke autoNotifyIfNeeded on updating the value. This notifies all observers
                 * with updated attributes value as per the notification policy(AutoNotifyPolicy).
                 *
                 * @param key - name of attribute(used to map the attribute value).
                 *
                 * @param value - attribute value to be mapped against the key.
                 *
                 *NOTE : AutoNotifyPolicy enum class is defined in ResourceObject class.
                 */
                void setAttribute(std::string&& key, const ResourceAttributes::Value & value);
                /**
                 * API for setting a particular attribute value.
                 * It uses mutex to invoke autoNotifyIfNeeded on updating the value. This notifies all observers
                 * with updated attributes value as per the notification policy(AutoNotifyPolicy).
                 *
                 * @param key - name of attribute(used to map the attribute value).
                 *
                 * @param value - attribute value to be mapped against the key.
                 *
                 *NOTE : AutoNotifyPolicy enum class is defined in ResourceObject class.
                 */
                void setAttribute(std::string&& key, ResourceAttributes::Value&& value);

                /**
                 * API for getting attribute value corresponding to a key(name of that attribute). Uses mutex.
                 *
                 *@param key - name of the attribute value to look for.
                 *
                 * @return ResourceAttributes::Value -value of the resource attribute.
                 *
                 */
                ResourceAttributes::Value getAttributeValue(const std::string &key) const;

                /**
                 *  API for retrieving the attribute value associated with the supplied name. Uses mutex.
                 *
                 *  @param key - Name of the attribute
                 *
                 *  @return  typename - resource attributes value.
                 */
                template< typename T >
                T getAttribute(const std::string &key) const
                {
                    WeakGuard lock(*this);
                    return m_resourceAttributes.at(key).get< T >();
                }

                /**
                 * API for removing a particular attribute of the resource.
                 * Uses mutex.
                 *
                 * @param key - Name of the attribute.
                 *
                 * @return bool - true or false
                 *
                 */
                bool removeAttribute(const std::string &key);

                /**
                 * API for checking whether a particular attribute is there for a resource or not.
                 *
                 * @param key - Name of the attribute.
                 *
                 * @return bool - true or false.
                 *
                 */
                bool containsAttribute(const std::string &key) const;

                /**
                 * API for getting all the attributes of the resource.
                 * It invokes the expectOwnLock() API to check the owner of the lock using the thread id.
                 * If it is not the owner then it throws exception.
                 *
                 * @return ResourceAttributes& - reference of the attributes of the resource.
                 *
                 *@see expectOwnLock()
                 *
                 * @throw NoLockException
                 *              It catches exception thrown from expectOwnLock() API and throws it to the developer.
                 */
                ResourceAttributes &getAttributes();
                /**
                 * API for getting all the attributes of the resource.
                 * It invokes the expectOwnLock() API to check the owner of the lock using the thread id.
                 * If it is not the owner then it throws exception.
                 *
                 * @return ResourceAttributes& - reference of the attributes of the resource.
                 *
                 *@see expectOwnLock()
                 *
                 * @throw NoLockException
                 *              It catches exception thrown from expectOwnLock() API and throws it to the developer.
                 */
                const ResourceAttributes &getAttributes() const;

                /**
                * API for checking whether the particular resource is observable or not
                *
                * @return bool - true or false.
                *
                */
                virtual bool isObservable() const;
                /**
                * API for checking whether the particular resource is discoverable or not
                *
                * @return bool - true or false.
                *
                */
                virtual bool isDiscoverable() const;

                /**
                 * API for setting the resource's get request handler by the developer/application.
                 * If developer set this handler then all get request will come to the application & developer can
                 * send the response to the client using APIs of RCSGetResponse class.
                 *
                 * @param handler - Request handler for get requests
                 *
                 * @see RCSGetResponse
                 *
                 */
                virtual void setGetRequestHandler(GetRequestHandler handler);
                /**
                 * API for setting the resource's set request handler by the developer/application.
                 * If developer set this handler then all set request for the resource  will come to the application
                 * & developer can send the response to the client using APIs of RCSSetResponse class.
                 *
                 * @param handler - Request handler for set requests
                 *
                 * @see RCSSetResponse
                 *
                 */
                virtual void setSetRequestHandler(SetRequestHandler handler);

                /**
                 * API for setting the Listener for a particular attribute update.
                 *
                 * @param key  - The intersted attribute's key
                 * @param listener - Listener for updation of the interested attribute
                 *
                 */
                virtual void addAttributeUpdatedListener(const std::string &key,
                        AttributeUpdatedListener listener);
                /**
                 * API for setting the Listener for a particular attribute update.
                 *
                 * @param key  - The intersted attribute's key
                 * @param listener - Listener for updation of the interested attribute
                 *
                 */
                virtual void addAttributeUpdatedListener(std::string &&key, AttributeUpdatedListener listener);
                /**
                * API for removing the handler for a particular attribute update.
                *
                * @param key - The intersted attribute's key
                *
                */
                virtual bool removeAttributeUpdatedListener(const std::string &key);

                /**
                 * API for notifying all observers of the resource with the updated attributes value
                 */
                virtual void notify() const;

                /**
                * API for setting Auto notify policy
                *
                * @param policy - Policy to be set
                *
                * Note: AutoNotifyPolicy is an enum class
                *
                * @see AutoNotifyPolicy
                *
                */
                void setAutoNotifyPolicy(AutoNotifyPolicy policy);
                /**
                * API for getting auto notify policy
                *
                * @returns AutoNotifyPolicy - AntoNotify policy
                *
                * Note: AutoNotifyPolicy is an enum class
                *
                * @see AutoNotifyPolicy
                */
                AutoNotifyPolicy getAutoNotifyPolicy() const;

                /**
                * API for setting the policy for a setRequestHandler.
                *
                * @param policy - policy to be set
                *
                * @see SetRequestHandlerPolicy
                *
                * Note: SetRequestHandlerPolicy is the enum class with two values : DEFAULT & ACCEPTANCE
                *
                */
                void setSetRequestHandlerPolicy(SetRequestHandlerPolicy policy);

                /**
                * API for getting the SetRequestHandler Policy.
                *
                * @returns  SetRequestHandlerPolicy - Property of setRequesthandler
                *
                * @see SetRequestHandlerPolicy
                *
                * Note: SetRequestHandlerPolicy is the enum class with two values : DEFAULT & ACCEPTANCE
                *
                */
                SetRequestHandlerPolicy getSetRequestHandlerPolicy() const;

        private:
            ResourceObject(uint8_t, ResourceAttributes&&);

            OCEntityHandlerResult entityHandler(std::shared_ptr< OC::OCResourceRequest >);

            OCEntityHandlerResult handleRequest(std::shared_ptr< OC::OCResourceRequest >);
            OCEntityHandlerResult handleRequestGet(std::shared_ptr< OC::OCResourceRequest >);
            OCEntityHandlerResult handleRequestSet(std::shared_ptr< OC::OCResourceRequest >);
            OCEntityHandlerResult handleObserve(std::shared_ptr< OC::OCResourceRequest >);

            void expectOwnLock() const;
            void autoNotify(bool, AutoNotifyPolicy) const;
            void autoNotifyIfNeeded(const std::string& , const ResourceAttributes::Value& );

        private:
            const uint8_t m_properties;

            OCResourceHandle m_resourceHandle;
            ResourceAttributes m_resourceAttributes;

            GetRequestHandler m_getRequestHandler;
            SetRequestHandler m_setRequestHandler;
            AutoNotifyPolicy m_autoNotifyPolicy;
            SetRequestHandlerPolicy m_setRequestHandlerPolicy;

            std::unordered_map< std::string, AttributeUpdatedListener >
                    m_keyAttributesUpdatedListeners;

            mutable boost::atomic< std::thread::id > m_lockOwner;
            mutable std::mutex m_mutex;

            std::mutex m_mutexKeyAttributeUpdate;

        };
        /**
         * This class enables mutex functionality for the ResourceObject instances.
         */
        class ResourceObject::LockGuard
        {
        public:
            LockGuard(const ResourceObject&);
            LockGuard(const ResourceObject::Ptr);
            LockGuard(const ResourceObject&, AutoNotifyPolicy);
            LockGuard(const ResourceObject::Ptr, AutoNotifyPolicy);
            ~LockGuard();

            LockGuard(const LockGuard&) = delete;
            LockGuard(LockGuard&&) = delete;

            LockGuard& operator=(const LockGuard&) = delete;
            LockGuard& operator=(LockGuard&&) = delete;

        private:
            void init();

        private:
            const ResourceObject& m_resourceObject;

            AutoNotifyPolicy m_autoNotifyPolicy;

            bool m_isOwningLock;

            std::function<void()> m_autoNotifyFunc;
        };
        /**
         * This class manages weak guard for the ResourceObject.
         */
        class ResourceObject::WeakGuard
        {
        public:
            WeakGuard(const ResourceObject&);
            ~WeakGuard();

            WeakGuard(const WeakGuard&) = delete;
            WeakGuard(WeakGuard&&) = delete;

            WeakGuard& operator=(const WeakGuard&) = delete;
            WeakGuard& operator=(WeakGuard&&) = delete;

            bool hasLocked() const;

        private:
            bool m_isOwningLock;
            const ResourceObject& m_resourceObject;
        };
    }
}

#endif // SERVERBUILDER_RESOURCEOBJECT_H
