/* *****************************************************************
*
* Copyright 2015 Samsung Electronics All Rights Reserved.
*
*----------------------------------------------------------------------
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************/


#include <iostream>

#include "RCSRemoteResourceObject.h"
#include "RCSDiscoveryManager.h"
#include "RCSResourceObject.h"
#include "RCSAddress.h"
#include "OCPlatform.h"
#include "RCSResourceAttributes.h"

#include "CommonUtil.h"
#include "AsyncWorkerThread.h"
#include "RECommon.h"

using namespace OIC::Service;
using namespace OC::OCPlatform;
using namespace OC;
using namespace std;

namespace REClient
{

    static Isolate *isolate = NULL;
    static Persistent<Function> discoveryCb;
    static Persistent<Function> resourceStateChangedCb;
    static Persistent<Function> attributesReceivedCb;
    static Persistent<Function> cacheUpdatedCb;

    int gPlatformInitialized = 0;

    void printAttribute(const std::string key, const RCSResourceAttributes::Value &value)
    {
        std::cout << "\tkey :  " << key << std::endl
                  << "\tvalue : " << value.toString() << std::endl;
    }

    void printAttributes(const RCSResourceAttributes &attributes)
    {
        if (attributes.empty())
        {
            std::cout << "\tattributes is empty" << std::endl;
        }
        for (const auto &attr : attributes)
        {
            printAttribute(attr.key(), attr.value());
        }
    }


    void onResourceDiscovered(shared_ptr<RCSRemoteResourceObject> discoveredResource)
    {
        cout << "[Discovery CB] Entry:: " << endl;

        if (NULL == discoveredResource.get())
        {
            cout << "discoveredResource is NULL" << endl;
        }

        string uri = discoveredResource->getUri();

        string address = discoveredResource->getAddress();

        cout << "[Discovery CB] resourceURI : " << uri.c_str() << endl;

        cout << "[Discovery CB] hostAddress : " << address.c_str() << endl;

        string jsHandle = CreateJSHandle(address, uri);

        cout <<  "[Discovery CB] Device Handle = " << jsHandle << endl;

        DiscoveryEventWork *work = new DiscoveryEventWork();

        work->request.data = work;

        work->context = jsHandle;

        work->callback.Reset(isolate, discoveryCb);

        SetRCSRemoteResourceObject(jsHandle, discoveredResource);

        uv_queue_work(uv_default_loop(), &work->request, WorkAsync, WorkResourceDiscoveredAsyncComplete);

        cout << "[Discovery CB] onResourceDiscovered callback Exit:: " << endl;

    }

    void onResourceStateChanged(const ResourceState &resourceState, std::string context)
    {
        std::cout << "[Resource Montior CB] Entry" << std::endl;
        std::cout << "[Resource Montior CB] Callback Context :: " << context << std::endl;

        int status = 0;
        switch (resourceState)
        {
            case ResourceState::NONE:
                std::cout << "\tState changed to : NOT_MONITORING" << std::endl;
                status = 0;
                break;

            case ResourceState::ALIVE:
                std::cout << "\tState changed to : ALIVE" << std::endl;
                status = 1;
                break;

            case ResourceState::REQUESTED:
                std::cout << "\tState changed to : REQUESTED" << std::endl;
                status = 2;
                break;

            case ResourceState::LOST_SIGNAL:
                std::cout << "\tState changed to : LOST_SIGNAL" << std::endl;
                status = 3;
                break;

            case ResourceState::DESTROYED:
                std::cout << "\tState changed to : DESTROYED" << std::endl;
                status = 4;
                break;
        }

        StatusEventWork *work = new StatusEventWork();
        work->request.data = work;

        work->context = context;

        work->status = status;

        work->callback.Reset(isolate , resourceStateChangedCb);

        uv_queue_work(uv_default_loop(), &work->request, WorkAsync, WorkResourceStateChangedAsyncComplete);

        std::cout << "[Resource Montior CB] Exit" << std::endl;

    }

    void onCacheUpdated(const RCSResourceAttributes &attributes, std::string context)
    {
        std::cout << "[Resource Caching CB] Entry" << std::endl;
        std::cout << "[Resource Caching CB] Callback Context :: " << context << std::endl;


        printAttributes(attributes);

        AttrChangeEventWork *work = new AttrChangeEventWork();
        work->request.data = work;

        work->context = context;

        Local<Object> attrMap = Object::New(isolate);

        if (attributes.empty())
        {
            std::cout << "\tattributes is empty" << std::endl;
        }

        for (const auto &attr : attributes)
        {
            attrMap->Set(String::NewFromUtf8(isolate, attr.key().c_str()), String::NewFromUtf8(isolate,
                         attr.value().toString().c_str()));
        }


        work->attrMap = attrMap;

        work->callback.Reset(isolate, cacheUpdatedCb);

        uv_queue_work(uv_default_loop(), &work->request, WorkAsync, WorkCacheUpdatedAsyncComplete);

        std::cout << "[Resource Caching CB] Exit" << std::endl;

    }

    void onRemoteAttributesReceived(const RCSResourceAttributes &attributes, int status,
                                    std::string context)
    {
        std::cout << "[Resource Attributes Received CB] Entry" << std::endl;

        std::cout << "[Resource Attributes Received CB] Callback Context :: " << context << std::endl;

        AttrChangeEventWork *work = new AttrChangeEventWork();
        work->request.data = work;

        work->context = context;

        Local<Object> attrMap = Object::New(isolate);

        if (attributes.empty())
        {
            std::cout << "\tattributes is empty" << std::endl;
        }

        for (const auto &attr : attributes)
        {


            RCSResourceAttributes::TypeId type = attr.value().getType().getId();


            if (type == RCSResourceAttributes::TypeId::INT)
            {

                attrMap->Set(String::NewFromUtf8(isolate, attr.key().c_str()), Integer::New(isolate,
                             attr.value().get<int>()));
            }
            else if (type == RCSResourceAttributes::TypeId::BOOL)
            {

                attrMap->Set(String::NewFromUtf8(isolate, attr.key().c_str()), Boolean::New(isolate,
                             attr.value().get<bool>()));

            }
            else if (type == RCSResourceAttributes::TypeId::STRING)
            {

                attrMap->Set(String::NewFromUtf8(isolate, attr.key().c_str()), String::NewFromUtf8(isolate,
                             attr.value().toString().c_str()));

            }
            else
            {
                cout << "[Resource Attributes Received CB] Attributes Data Type is not handled" << endl;
            }

        }

        work->attrMap = attrMap;

        work->callback.Reset(isolate, attributesReceivedCb);

        uv_queue_work(uv_default_loop(), &work->request, WorkAsync, WorkAttributesReceivedAsyncComplete);

        std::cout << "[Resource Attributes Received CB] Exit" << std::endl;


    }


    void platFormConfigure()
    {
        PlatformConfig config
        {
            OC::ServiceType::InProc, ModeType::Client, "0.0.0.0", 0, OC::QualityOfService::LowQos
        };

        OCPlatform::Configure(config);
    }

    void InitPlatform(const FunctionCallbackInfo<Value> &args)
    {
        cout << "InitPlatform Entry" << endl;

        if (gPlatformInitialized)
        {
            cout << "Platform is already intialized" << endl;
            return;
        }

        platFormConfigure();

        gPlatformInitialized = 1;

        cout << "InitPlatform Exit" << endl;
    }

    void DiscoverResourceByType(const FunctionCallbackInfo<Value> &args)
    {
        cout << "DiscoverResourceByType Entry" << endl;

        isolate = args.GetIsolate();

        v8::String::Utf8Value strUri(args[0]);
        const char *uri = ToCString(strUri);
        cout << uri << endl;

        v8::String::Utf8Value strRType(args[1]);
        const char *resourceType = ToCString(strRType);
        cout << resourceType << endl;


        Local<Function> callback = Local<Function>::Cast(args[2]);

        discoveryCb.Reset(isolate, callback);

        try
        {
            RCSDiscoveryManager::getInstance()->discoverResourceByType(RCSAddress::multicast(),
                    uri, resourceType , onResourceDiscovered);
        }
        catch (const RCSPlatformException &e)
        {
            std::cout << e.what() << std::endl;
        }

        cout << "DiscoverResourceByType Exit" << endl;

        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "Done"));

    }

    void DiscoverResource(const FunctionCallbackInfo<Value> &args)
    {
        cout << "DiscoverResource Entry" << endl;

        isolate = args.GetIsolate();

        v8::String::Utf8Value strUri(args[0]);
        const char *uri = ToCString(strUri);
        cout << uri << endl;


        Local<Function> callback = Local<Function>::Cast(args[1]);

        discoveryCb.Reset(isolate, callback);

        try
        {
            RCSDiscoveryManager::getInstance()->discoverResource(RCSAddress::multicast(),
                    uri, onResourceDiscovered);
        }
        catch (const RCSPlatformException &e)
        {
            std::cout << e.what() << std::endl;
        }

        cout << "DiscoverResource Exit" << endl;

        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "Done"));

    }


    void StartMonitoring(const FunctionCallbackInfo<Value> &args)
    {
        cout << "StartMonitoring Entry" << endl;

        isolate = args.GetIsolate();

        v8::String::Utf8Value strUri(args[0]);

        const char *uri = ToCString(strUri);

        cout << uri << endl;


        std::shared_ptr<RCSRemoteResourceObject> obj = GetRCSRemoteResourceObject(uri);

        if (obj.get() == NULL)
        {
            cout << "Remote Object is NULL" << endl;
            return;
        }

        Local<Function> callback = Local<Function>::Cast(args[1]);

        resourceStateChangedCb.Reset(isolate, callback);

        std::string context(uri);

        if (!obj->isMonitoring())
        {
            try
            {
                obj->startMonitoring(std::bind(&onResourceStateChanged, std::placeholders::_1, context));
                std::cout << "\tHosting Started..." << std::endl;
            }
            catch (const RCSPlatformException &e)
            {
                std::cout << e.what() << std::endl;
            }
        }
        else
        {
            std::cout << "\tAlready Started..." << std::endl;
        }

        cout << "StartMonitoring Exit" << endl;

        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "Done"));

    }

    void StopMonitoring(const FunctionCallbackInfo<Value> &args)
    {
        cout << "StopMonitoring Entry" << endl;

        isolate = args.GetIsolate();

        v8::String::Utf8Value strUri(args[0]);
        const char *uri = ToCString(strUri);
        cout << uri << endl;


        std::shared_ptr<RCSRemoteResourceObject> obj = GetRCSRemoteResourceObject(uri);
        if (obj.get() == NULL)
        {
            cout << "Remote Object is NULL" << endl;
            return;
        }

        if (obj->isMonitoring())
        {
            try
            {
                obj->stopMonitoring();
                std::cout << "\tHosting stopped..." << std::endl;
            }
            catch (const RCSPlatformException &e)
            {
                std::cout << e.what() << std::endl;
            }
        }
        else
        {
            std::cout << "\tHosting not started..." << std::endl;
        }

        cout << "StopMonitoring Exit" << endl;

        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "Done"));

    }

    void StartCachingWithCb(const FunctionCallbackInfo<Value> &args)
    {
        cout << "StartCaching Entry" << endl;

        isolate = args.GetIsolate();

        v8::String::Utf8Value strUri(args[0]);
        const char *uri = ToCString(strUri);
        cout << uri << endl;


        std::shared_ptr<RCSRemoteResourceObject> obj = GetRCSRemoteResourceObject(uri);
        if (obj.get() == NULL)
        {
            cout << "Remote Object is NULL" << endl;
            return;
        }

        Local<Function> callback = Local<Function>::Cast(args[1]);

        cacheUpdatedCb.Reset(isolate, callback);

        string context(uri);

        sleep(1);
        if (!obj->isCaching())
        {
            try
            {
                obj->startCaching(std::bind(&onCacheUpdated, std::placeholders::_1, context));
                std::cout << "\tCaching Started..." << std::endl;
            }
            catch (const RCSPlatformException &e)
            {
                std::cout << e.what() << std::endl;
            }
        }
        else
        {
            std::cout << "\tAlready Started Caching..." << std::endl;
        }

        cout << "StartCaching Exit" << endl;

        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "Done"));
    }

    void StartCachingWithoutCb(const FunctionCallbackInfo<Value> &args)
    {
        cout << "StartCaching Entry" << endl;

        isolate = args.GetIsolate();

        v8::String::Utf8Value strUri(args[0]);
        const char *uri = ToCString(strUri);
        cout << uri << endl;


        std::shared_ptr<RCSRemoteResourceObject> obj = GetRCSRemoteResourceObject(uri);
        if (obj.get() == NULL)
        {
            cout << "Remote Object is NULL" << endl;
            return;
        }

        string context(uri);

        sleep(1);
        if (!obj->isCaching())
        {
            try
            {
                obj->startCaching(NULL);
                std::cout << "\tCaching Started..." << std::endl;
            }
            catch (const RCSPlatformException &e)
            {
                std::cout << e.what() << std::endl;
            }
        }
        else
        {
            std::cout << "\tAlready Started Caching..." << std::endl;
        }

        cout << "StartCaching CB Exit" << endl;

        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "Done"));
    }


    void StopCaching(const FunctionCallbackInfo<Value> &args)
    {
        cout << "StopCaching Entry" << endl;

        isolate = args.GetIsolate();

        v8::String::Utf8Value strUri(args[0]);
        const char *uri = ToCString(strUri);
        cout << uri << endl;

        std::shared_ptr<RCSRemoteResourceObject> obj = GetRCSRemoteResourceObject(uri);
        if (obj.get() == NULL)
        {
            cout << "Remote Object is NULL" << endl;
            return;
        }

        if (obj->isCaching())
        {
            try
            {
                obj->stopCaching();
                std::cout << "\tCaching stopped..." << std::endl;
            }
            catch (const RCSPlatformException &e)
            {
                std::cout << e.what() << std::endl;
            }
        }
        else
        {
            std::cout << "\tCaching not started..." << std::endl;
        }

        cout << "StopCaching Exit" << endl;

        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "Done"));

    }

    void GetCachedAttribute(const FunctionCallbackInfo<Value> &args)
    {
        cout << "GetCachedAttribute Entry" << endl;

        isolate = args.GetIsolate();

        v8::String::Utf8Value strUri(args[0]);
        const char *uri = ToCString(strUri);
        cout << uri << endl;


        std::shared_ptr<RCSRemoteResourceObject> obj = GetRCSRemoteResourceObject(uri);

        if (obj.get() == NULL)
        {
            cout << "Remote Object is NULL" << endl;
            return;
        }
        RCSResourceAttributes attributes;
        try
        {
            attributes = obj->getCachedAttributes();
        }
        catch (const RCSBadRequestException &e)
        {
            std::cout << "Exception in getCachedAttributes : " << e.what() << std::endl;
        }
        catch (const RCSBadGetException &e)
        {
            std::cout << "Exception in getCachedAttributes : " << e.what() << std::endl;
        }
        catch (const RCSPlatformException &e)
        {
            std::cout << e.what() << std::endl;
        }


        printAttributes(attributes);

        Local<Object> attrMap = Object::New(isolate);

        if (attributes.empty())
        {
            std::cout << "\tattributes is empty" << std::endl;
        }

        for (const auto &attr : attributes)
        {


            RCSResourceAttributes::TypeId type = attr.value().getType().getId();


            if (type == RCSResourceAttributes::TypeId::INT)
            {

                attrMap->Set(String::NewFromUtf8(isolate, attr.key().c_str()), Integer::New(isolate,
                             attr.value().get<int>()));
            }
            else if (type == RCSResourceAttributes::TypeId::BOOL)
            {

                attrMap->Set(String::NewFromUtf8(isolate, attr.key().c_str()), Boolean::New(isolate,
                             attr.value().get<bool>()));

            }
            else if (type == RCSResourceAttributes::TypeId::STRING)
            {

                attrMap->Set(String::NewFromUtf8(isolate, attr.key().c_str()), String::NewFromUtf8(isolate,
                             attr.value().toString().c_str()));

            }
            else
            {
                cout << "[Resource Attributes Received CB] Attributes Data Type is not handled" << endl;
            }

        }

        args.GetReturnValue().Set(attrMap);
        cout << "GetCachedAttribute Exit" << endl;
    }

    void GetRemoteAttributes(const FunctionCallbackInfo<Value> &args)
    {
        cout << "GetRemoteAttributes Entry" << endl;

        isolate = args.GetIsolate();

        v8::String::Utf8Value strUri(args[0]);
        const char *uri = ToCString(strUri);
        cout << uri << endl;


        std::shared_ptr<RCSRemoteResourceObject> obj = GetRCSRemoteResourceObject(uri);

        if (obj.get() == NULL)
        {
            cout << "Remote Object is NULL" << endl;
            return;
        }

        Local<Function> callback = Local<Function>::Cast(args[1]);

        attributesReceivedCb.Reset(isolate, callback);

        string context(uri);

        try
        {
            obj->getRemoteAttributes(std::bind(&onRemoteAttributesReceived, std::placeholders::_1,
                                               std::placeholders::_2, context));
        }
        catch (const RCSPlatformException &e)
        {
            std::cout << e.what() << std::endl;
        }

        cout << "GetRemoteAttributes Exit" << endl;


        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "Done"));

    }

    void SetRemoteAttributes(const FunctionCallbackInfo<Value> &args)
    {
        cout << "SetRemoteAttributes Entry" << endl;

        isolate = args.GetIsolate();

        v8::String::Utf8Value strUri(args[0]);
        const char *uri = ToCString(strUri);
        cout << uri << endl;


        std::shared_ptr<RCSRemoteResourceObject> obj = GetRCSRemoteResourceObject(uri);

        if (obj.get() == NULL)
        {
            cout << "Remote Object is NULL" << endl;
            return;
        }
        //TODO: Need to check multiple attribute update using javascript map
        std::string key = " ";
        std::string value = " ";

        Local<Object> attrMap = args[1]->ToObject();

        Local<Array> propertyNames = attrMap ->GetOwnPropertyNames();

        int length = propertyNames->Length();

        cout << "Length of attributes " << length << endl;

        RCSResourceAttributes attributes;

        for (int i = 0; i < length; ++i)
        {
            Local<Value> key = propertyNames->Get(i);
            Local<Value> value = attrMap->Get(key);

            if (key->IsString() && value->IsString())
            {
                cout << "String attribute" << endl;
                String::Utf8Value utf8_key(key);
                String::Utf8Value utf8_value(value);

                std::string strKey(ToCString(utf8_key));
                std::string strValue(ToCString(utf8_value));

                cout << " key :: " << strKey << " Value :: " << strValue << endl;
                attributes[strKey] = strValue;
            }
            if (key->IsString() && value->IsNumber())
            {
                cout << "Number attribute" << endl;

                String::Utf8Value utf8_key(key);
                int utf8_value = value->IntegerValue();

                std::string strKey(ToCString(utf8_key));


                cout << " key :: " << strKey << " Value :: " << utf8_value << endl;
                attributes[strKey] = utf8_value;
            }
            if (key->IsString() && value->IsBoolean())
            {
                cout << "Boolean attribute" << endl;

                String::Utf8Value utf8_key(key);
                bool utf8_value = value->BooleanValue();

                std::string strKey(ToCString(utf8_key));


                cout << " key :: " << strKey << " Value :: " << utf8_value << endl;
                attributes[strKey] = utf8_value;
            }
        }


        Local<Function> callback = Local<Function>::Cast(args[2]);

        attributesReceivedCb.Reset(isolate, callback);

        string context(uri);

        try
        {
            obj->setRemoteAttributes(attributes, std::bind(&onRemoteAttributesReceived, std::placeholders::_1,
                                     std::placeholders::_2, context));
        }
        catch (const RCSPlatformException &e)
        {
            std::cout << e.what() << std::endl;
        }

        cout << "SetRemoteAttributes Exit" << endl;

        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "Done"));

    }


    void GetURI(const FunctionCallbackInfo<Value> &args)
    {
        cout << "GetURI Entry" << endl;

        isolate = args.GetIsolate();

        v8::String::Utf8Value strUri(args[0]);
        const char *uri = ToCString(strUri);
        cout << uri << endl;


        std::shared_ptr<RCSRemoteResourceObject> obj = GetRCSRemoteResourceObject(uri);

        if (obj.get() == NULL)
        {
            cout << "Remote Object is NULL" << endl;
            return;
        }

        std::string retUri;
        try
        {
            retUri = obj->getUri();
        }
        catch (const RCSPlatformException &e)
        {
            std::cout << e.what() << std::endl;
        }

        cout << "GetURIExit" << endl;

        args.GetReturnValue().Set(String::NewFromUtf8(isolate, retUri.c_str()));
    }

    void GetAddress(const FunctionCallbackInfo<Value> &args)
    {
        cout << "GetAddress Entry" << endl;

        isolate = args.GetIsolate();

        v8::String::Utf8Value strUri(args[0]);

        const char *uri = ToCString(strUri);

        cout << uri << endl;

        std::shared_ptr<RCSRemoteResourceObject> obj = GetRCSRemoteResourceObject(uri);

        if (obj.get() == NULL)
        {
            cout << "Remote Object is NULL" << endl;
            return;
        }

        std::string address;
        try
        {
            address = obj->getAddress();
        }
        catch (const RCSPlatformException &e)
        {
            std::cout << e.what() << std::endl;
        }

        //std::string address("107.108.71.175");

        cout << "GetAddress Exit" << endl;

        args.GetReturnValue().Set(String::NewFromUtf8(isolate, address.c_str()));
    }

    void GetResourceTypes(const FunctionCallbackInfo<Value> &args)
    {
        cout << "GetResourceTypes Entry" << endl;

        isolate = args.GetIsolate();

        v8::String::Utf8Value strUri(args[0]);
        const char *uri = ToCString(strUri);
        cout << uri << endl;


        std::shared_ptr<RCSRemoteResourceObject> obj = GetRCSRemoteResourceObject(uri);

        if (obj.get() == NULL)
        {
            cout << "Remote Object is NULL" << endl;
            return;
        }

        std::vector< std::string > resourceTypeList;

        try
        {
            resourceTypeList = obj->getTypes();
        }
        catch (const RCSPlatformException &e)
        {
            std::cout << e.what() << std::endl;
        }

        Local<Array> retList = Array::New(isolate);
        for (std::vector<int>::size_type i = 0; i != resourceTypeList.size(); i++)
        {

            std::cout << resourceTypeList[i] << endl;

            retList->Set(i, String::NewFromUtf8(isolate, resourceTypeList[i].c_str()));
        }

        cout << "GetResourceTypes Exit" << endl;

        args.GetReturnValue().Set(retList);
    }

    void GetInterfaces(const FunctionCallbackInfo<Value> &args)
    {
        cout << "GetInterfaces Entry" << endl;

        isolate = args.GetIsolate();

        v8::String::Utf8Value strUri(args[0]);
        const char *uri = ToCString(strUri);
        cout << uri << endl;


        std::shared_ptr<RCSRemoteResourceObject> obj = GetRCSRemoteResourceObject(uri);

        if (obj.get() == NULL)
        {
            cout << "Remote Object is NULL" << endl;
            return;
        }
        std::vector< std::string > interfaceList;

        try
        {
            interfaceList = obj->getInterfaces();
        }
        catch (const RCSPlatformException &e)
        {
            std::cout << e.what() << std::endl;
        }



        Local<Array> retList = Array::New(isolate);
        for (std::vector<int>::size_type i = 0; i != interfaceList.size(); i++)
        {

            std::cout << interfaceList[i] << endl;

            retList->Set(i, String::NewFromUtf8(isolate, interfaceList[i].c_str()));
        }

        cout << "GetInterfaces Exit" << endl;

        args.GetReturnValue().Set(retList);

    }

    void init(Local<Object> exports)
    {
        NODE_SET_METHOD(exports, "InitPlatform", InitPlatform);

        NODE_SET_METHOD(exports, "DiscoverResource", DiscoverResource);

        NODE_SET_METHOD(exports, "DiscoverResourceByType", DiscoverResourceByType);

        NODE_SET_METHOD(exports, "StartMonitoring", StartMonitoring);

        NODE_SET_METHOD(exports, "StopMonitoring", StopMonitoring);

        NODE_SET_METHOD(exports, "StartCachingWithCb", StartCachingWithCb);

        NODE_SET_METHOD(exports, "StartCachingWithoutCb", StartCachingWithoutCb);

        NODE_SET_METHOD(exports, "StopCaching", StopCaching);

        NODE_SET_METHOD(exports, "GetCachedAttribute", GetCachedAttribute);

        NODE_SET_METHOD(exports, "GetRemoteAttributes", GetRemoteAttributes);

        NODE_SET_METHOD(exports, "SetRemoteAttributes", SetRemoteAttributes);

        NODE_SET_METHOD(exports, "GetUri", GetURI);

        NODE_SET_METHOD(exports, "GetAddress", GetAddress);

        NODE_SET_METHOD(exports, "GetResourceTypes", GetResourceTypes);

        NODE_SET_METHOD(exports, "GetInterfaces", GetInterfaces);

    }

    NODE_MODULE(addon, init)


}
