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
 * RCSResourceObject is a part of the server builder module.
 */
#ifndef SERVER_RCSRESOURCEOBJECT_H
#define SERVER_RCSRESOURCEOBJECT_H

#include <string>
#include <mutex>
#include <thread>

#include <RCSResourceAttributes.h>
#include <RCSResponse.h>
#include <RCSRequest.h>

namespace OC
{
    class OCResourceRequest;
}

namespace OIC
{
    namespace Service
    {

        /**
         * @brief Thrown when lock has not been acquired.
         *
         * @see RCSResourceObject::LockGuard
         * @see RCSResourceObject::getAttributes
         */
        class NoLockException: public RCSException
        {
            public:
                NoLockException(std::string &&what) : RCSException { std::move(what) } {}
        };

        //! @cond
        template < typename T >
        class AtomicWrapper;
        //! @endcond

        /**
         * @brief  RCSResourceObject represents a resource. It handles any requests from
         *        clients automatically with attributes.
         *        It also provides an auto notification mechanism that notifies to the observers.
         *        <br/>
         *         Requests are handled automatically by defaultAction of RCSGetResponse and
         *        RCSSetResponse. You can override them and send your own response.
         *        <br/>
         *         For simple resources, you may want to know whenever attributes are changed
         *        by a set request. In this case, add an AttributeUpdatedListener
         *        with a key interested in instead of overriding SetRequestHandler.
         */
        class RCSResourceObject
        {
            private:
                class WeakGuard;

                typedef AtomicWrapper< std::thread::id > AtomicThreadId;

            public:
                /**
                 * @brief Represents the policy of auto-notify function.
                 *        In accord with this policy, observers are notified of attributes
                 *        when the attributes are set.
                 *
                 * @note Attributes are set according to execution of some functions
                 *       or receipt of 'set-request'.
                 *
                 * @see RCSResourceObject::setAttribute
                 * @see RCSResourceObject::removeAttribute
                 * @see RCSResourceObject::getAttributes
                 */
                enum class AutoNotifyPolicy
                {
                    NEVER,  /**< Never.*/
                    ALWAYS, /**< Always.*/
                    UPDATED /**< Only when attributes are changed.*/
                };

                /**
                 * @brief Represents the policy of set-request handler.
                 *        In accord with this policy, the RCSResourceObject decides whether
                 *        a set-request is acceptable or not.
                 */
                enum class SetRequestHandlerPolicy
                {
                    NEVER,     /**< Requests will be ignored if attributes of the request contain
                                    a new key or a value that has different type from the current
                                    value of the key. */
                    ACCEPTANCE /**< The whole attributes of the request will be applied
                                    even if there are name or type conflicts. */
                };

                typedef std::shared_ptr< RCSResourceObject > Ptr;
                typedef std::shared_ptr< const RCSResourceObject > ConstPtr;

                /**
                 * @brief   This class provides APIs for resource creation, setting properties &
                 *          attributes for the constructed resource.
                 */
                class Builder
                {
                    public:
                        /**
                         * @brief Sets the resource property values using initializers list.
                         *
                         * @param uri Resource URI value to be set
                         * @param type Resource type value to be set
                         * @param interface Resource interface value to be set
                         *
                         */
                        Builder(const std::string &uri, const std::string &type,
                                const std::string &interface);

                        /**
                         * Sets the discoverable property for the resource.
                         *
                         * @param discoverable Whether to be discovered.
                         *
                         */
                        Builder &setDiscoverable(bool discoverable);

                        /**
                         * Sets the observable property of the resource.
                         *
                         * @param observable Whether to be observed.
                         *
                         */
                        Builder &setObservable(bool observable);

                        /**
                         * Sets attribute of the resource.
                         *
                         * @param attributes Resource attributes to set
                         *
                         */
                        Builder &setAttributes(const RCSResourceAttributes &attributes);

                        /**
                         * @overload
                         */
                        Builder &setAttributes(RCSResourceAttributes &&attributes);

                        /**
                         * Construct a RCSResourceObject.
                         *
                         * @return Pointer to RCSResourceObject instance created.
                         *
                         * @throw PlatformException if resource registration is failed.
                         *
                         */
                        RCSResourceObject::Ptr build();

                    private:
                        std::string m_uri;
                        std::string m_type;
                        std::string m_interface;
                        uint8_t m_properties;
                        RCSResourceAttributes m_resourceAttributes;
                };

                class LockGuard;

                typedef std::function < RCSGetResponse(const RCSRequest&,
                                                       RCSResourceAttributes&) > GetRequestHandler;
                typedef std::function < RCSSetResponse(const RCSRequest&,
                                                       RCSResourceAttributes&) > SetRequestHandler;

                /**
                 * typedef for characterizing AttributeUpdatedListener.
                 *
                 * The first Value represents the old Value before being changed.
                 * The second Value means the new Value right after when it is used
                 */
                typedef std::function < void(const RCSResourceAttributes::Value&,
                                     const RCSResourceAttributes::Value &) > AttributeUpdatedListener;

            public:
                RCSResourceObject(RCSResourceObject&&) = delete;
                RCSResourceObject(const RCSResourceObject&) = delete;

                RCSResourceObject& operator=(RCSResourceObject&&) = delete;
                RCSResourceObject& operator=(const RCSResourceObject&) = delete;

                virtual ~RCSResourceObject();

                /**
                 * Sets a particular attribute value.
                 *
                 * @param key Key of attribute.
                 * @param value Attribute value to be mapped against the key.
                 *
                 * @note It guarantees thread-safety about attributes.
                 */
                void setAttribute(const std::string& key, const RCSResourceAttributes::Value& value);

                /**
                 * @overload
                 */
                void setAttribute(const std::string& key, RCSResourceAttributes::Value&& value);

                /**
                 * @overload
                 */
                void setAttribute(std::string&& key, const RCSResourceAttributes::Value& value);

                /**
                 * @overload
                 */
                void setAttribute(std::string&& key, RCSResourceAttributes::Value&& value);

                /**
                 * Returns an attribute value corresponding to a key.
                 *
                 * @param key Key of the attribute value.
                 *
                 * @note It guarantees thread-safety about attributes.
                 *
                 * @throws InvalidKeyException If key is invalid.
                 */
                RCSResourceAttributes::Value getAttributeValue(const std::string& key) const;

                /**
                 * Returns the attribute value as T.
                 *
                 * @param key Key of the attribute
                 *
                 * @throws BadGetException If type of the underlying value is not T.
                 * @throws InvalidKeyException If @a key doesn't match the key of any value.
                 *
                 * @note It guarantees thread-safety about attributes.
                 */
                template< typename T >
                T getAttribute(const std::string& key) const
                {
                    WeakGuard lock(*this);
                    return m_resourceAttributes.at(key).get< T >();
                }

                /**
                 * Removes a particular attribute of the resource.
                 *
                 * @param key Key of the attribute.
                 *
                 * @return True if the key exists and matched attribute is removed, otherwise false.
                 *
                 * @note It is guaranteed thread-safety about attributes.
                 */
                bool removeAttribute(const std::string& key);

                /**
                 * Checks whether a particular attribute exists or not.
                 *
                 * @param key Key of the attribute.
                 *
                 * @return True if the key exists, otherwise false.
                 *
                 * @note It guarantees thread-safety about attributes.
                 */
                bool containsAttribute(const std::string& key) const;

                /**
                 * Gets all the attributes of the RCSResourceObject.
                 * If it is not the owner then it throws an exception.
                 *
                 * @pre The call of this method must be guarded by LockGuard.
                 *
                 * @code
                   {
                      RCSResourceObject::LockGuard lock(*server);

                      auto &attributes = server->getAttributes();
                      ...
                   }
                 * @endcode
                 *
                 * @return Reference of the attributes of this RCSResourceObject.
                 *
                 * @throws NoLockException If the call is not guarded by LockGuard.
                 */
                RCSResourceAttributes& getAttributes();

                /**
                 * @overload
                 */
                const RCSResourceAttributes& getAttributes() const;

                /**
                 * Checks whether the particular resource is observable or not.
                 */
                virtual bool isObservable() const;

                /**
                 * Checks whether the particular resource is discoverable or not.
                 */
                virtual bool isDiscoverable() const;

                /**
                 * Sets the resource's get request handler.<br/>
                 * Default behavior builds a response with current attributes.<br/>
                 * If you want other handling, you can override your custom-handler.
                 *
                 * @param handler Request handler for get requests.
                 *
                 * @see RCSGetResponse
                 *
                 */
                virtual void setGetRequestHandler(GetRequestHandler handler);

                /**
                 * Sets the resource's set request handler.<br/>
                 * Default behavior sets the attributes according to set-request policy.<br/>
                 * If you want other handling, you can override your custom-handler.<br/>
                 * When receive set-request, this method most is called first.
                 *
                 * @param handler Request handler for set requests.
                 *
                 * @see RCSSetResponse
                 *
                 */
                virtual void setSetRequestHandler(SetRequestHandler handler);

                /**
                 * Sets the listener for a particular attribute update.
                 *
                 * @param key The interested attribute's key
                 * @param listener Listener for updation of the interested attribute
                 *
                 * @see AttributeUpdatedListener
                 */
                virtual void addAttributeUpdatedListener(const std::string& key,
                        AttributeUpdatedListener listener);

                /**
                 * @overload
                 */
                virtual void addAttributeUpdatedListener(std::string&& key,
                        AttributeUpdatedListener listener);

                /**
                 * Removes the listener for a particular attribute update.
                 *
                 * @param key The key of interested attribute's key
                 *
                 */
                virtual bool removeAttributeUpdatedListener(const std::string& key);

                /**
                 * Notifies all observers of the updated attributes.
                 */
                virtual void notify() const;

                /**
                 * Sets auto notify policy
                 *
                 * @param policy Policy to be set
                 *
                 */
                void setAutoNotifyPolicy(AutoNotifyPolicy policy);

                /**
                 * Returns notify policy
                 *
                 */
                AutoNotifyPolicy getAutoNotifyPolicy() const;

                /**
                 * Sets the policy for a setRequestHandler.
                 *
                 * @param policy Policy to be set
                 *
                 */
                void setSetRequestHandlerPolicy(SetRequestHandlerPolicy policy);

                /**
                 * Returns the SetRequestHandler Policy.
                 *
                 */
                SetRequestHandlerPolicy getSetRequestHandlerPolicy() const;

        private:
            RCSResourceObject(uint8_t, RCSResourceAttributes&&);

            OCEntityHandlerResult entityHandler(std::shared_ptr< OC::OCResourceRequest >);

            OCEntityHandlerResult handleRequest(std::shared_ptr< OC::OCResourceRequest >);
            OCEntityHandlerResult handleRequestGet(std::shared_ptr< OC::OCResourceRequest >);
            OCEntityHandlerResult handleRequestSet(std::shared_ptr< OC::OCResourceRequest >);
            OCEntityHandlerResult handleObserve(std::shared_ptr< OC::OCResourceRequest >);

            void expectOwnLock() const;

            std::thread::id getLockOwner() const noexcept;

            void setLockOwner(std::thread::id&&) const noexcept;

            void autoNotify(bool, AutoNotifyPolicy) const;
            void autoNotify(bool) const;

            bool testValueUpdated(const std::string&, const RCSResourceAttributes::Value&) const;

            template< typename K, typename V >
            void setAttributeInternal(K&&, V&&);

        private:
            const uint8_t m_properties;

            OCResourceHandle m_resourceHandle;
            RCSResourceAttributes m_resourceAttributes;

            GetRequestHandler m_getRequestHandler;
            SetRequestHandler m_setRequestHandler;
            AutoNotifyPolicy m_autoNotifyPolicy;
            SetRequestHandlerPolicy m_setRequestHandlerPolicy;

            std::unordered_map< std::string, AttributeUpdatedListener >
                    m_keyAttributesUpdatedListeners;

            mutable std::unique_ptr< AtomicThreadId > m_lockOwner;
            mutable std::mutex m_mutex;

            std::mutex m_mutexKeyAttributeUpdate;

        };

        /**
         * The class LockGuard owns a mutex for the duration of a scoped block.
         * When a LockGuard is created, it attempts to take ownership of the mutex it is given.
         * When control leaves the scope in which the LockGuard object was created,
         * the LockGuard is destructed and the mutex is released.
         */
        class RCSResourceObject::LockGuard
        {
        public:
           /**
            * The function locks the objects and ensures that all arguments are locked on return.
            * Working of AutoNotifyPolicy follows the current AutoNotifyPolicy status
            *
            * @param Object to be locked
            *
            */
            LockGuard(const RCSResourceObject&);

           /**
            * @overload
            */
            LockGuard(const RCSResourceObject::Ptr);

           /**
            * The function locks the objects and ensures that all arguments are locked on return.
            *
            * @param Object to be locked
            * @param AutoNotifyPolicy is set for sepcifying AutoNotifyPolicy status
            *
            * @see AutoNotifyPolicy
            */
            LockGuard(const RCSResourceObject&, AutoNotifyPolicy);

           /**
            * @overload
            */
            LockGuard(const RCSResourceObject::Ptr, AutoNotifyPolicy);
            ~LockGuard();

            LockGuard(const LockGuard&) = delete;
            LockGuard(LockGuard&&) = delete;

            LockGuard& operator=(const LockGuard&) = delete;
            LockGuard& operator=(LockGuard&&) = delete;

        private:
            void init();

        private:
            const RCSResourceObject& m_resourceObject;

            AutoNotifyPolicy m_autoNotifyPolicy;

            bool m_isOwningLock;

            std::function<void()> m_autoNotifyFunc;
        };

        class RCSResourceObject::WeakGuard
        {
        public:
            WeakGuard(const RCSResourceObject&);
            ~WeakGuard();

            WeakGuard(const WeakGuard&) = delete;
            WeakGuard(WeakGuard&&) = delete;

            WeakGuard& operator=(const WeakGuard&) = delete;
            WeakGuard& operator=(WeakGuard&&) = delete;

            bool hasLocked() const;

        private:
            bool m_isOwningLock;
            const RCSResourceObject& m_resourceObject;
        };
    }
}

#endif // SERVER_RCSRESOURCEOBJECT_H
