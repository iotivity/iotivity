#include <node.h>

#include "common.h"

using namespace v8;
using namespace OC;
using namespace std;

typedef enum ValueType { BOOLEAN =0 , INTEGER, STRING } VALUETYPE;

struct PutBaton
{
    uv_work_t request;
  
    string resourceId;
    string property;
    string value;
    VALUETYPE valueType;
    string returnValue;
    Persistent<Function, CopyablePersistentTraits<Function> > cb;
};

PutBaton *putB;


static void onPut_AsyncAfter(uv_work_t *req, int status)
{
    //  FindResourceBaton *baton = frb; 
    PutBaton *baton = static_cast<PutBaton*>(req->data);
    Isolate* isolate = Isolate::GetCurrent();

    Local<Value> argv[] = 
    {
      (String::NewFromUtf8(isolate, putB->returnValue.c_str()))
    };
    Local<Function> fn = Local<Function>::New(isolate, baton->cb);
    fn->Call(isolate->GetCurrentContext()->Global(), 1, argv);  
}


// callback handler on POST request
void onPut(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{
    try
    {
        if(eCode == OC_STACK_OK)
        {
            cout << "POST request was successful" << endl;            
            putB->returnValue = "POST request was successful";
            uv_queue_work(uv_default_loop(), &putB->request, dummyAsync, onPut_AsyncAfter);
        }
        else
        {
            cout << "onPut Response error: " << eCode << endl;

            if(eCode == OC_STACK_RESOURCE_CREATED)
                putB->returnValue = "onPut Response: OC_STACK_RESOURCE_CREATED";
            else
                putB->returnValue = "onPut Response error";
            
            uv_queue_work(uv_default_loop(), &putB->request, dummyAsync, onPut_AsyncAfter);
    //        exit(-1);            
        }
    }
    catch(exception& e)
    {
        cout << "Exception: " << e.what() << " in onPut" << endl;      
        putB->returnValue = "onPut Exception";  
        uv_queue_work(uv_default_loop(), &putB->request, dummyAsync, onPut_AsyncAfter);
    }
}


static void putResource(uv_work_t *req)
{
    PutBaton *baton = static_cast<PutBaton*>(req->data);
    shared_ptr<OCResource> resource;
    OCRepresentation rep;
    DiscoveredResourceMap::iterator it = discoveredResources.find(baton->resourceId);

    cout << "putResource  " << baton->resourceId << ":" << discoveredResources.size() <<endl;

    if(it == discoveredResources.end())
        return;

    resource = discoveredResources[baton->resourceId];
    if(!resource)
        return;

    cout << "Puting Resource representation... : "<< baton->value << " ValueType : " << baton->valueType << endl;
    istringstream buffer(baton->value);

    switch(baton->valueType)
    {
        case BOOLEAN:
            bool boolean_value;
            buffer >> boolean_value;
            rep.setValue(baton->property, boolean_value);
            break;

       case INTEGER:
            int int_vlaue;
            buffer >> int_vlaue;
            rep.setValue(baton->property, int_vlaue);
            break;

       case STRING:
            rep.setValue(baton->property, baton->value);
            break;
    }

    // Invoke resource's put API with rep, query map and the callback parameter
    putB = baton;
    resource->put(rep, QueryParamsMap(), &onPut);    
}


void putValueFunc(const FunctionCallbackInfo<Value>& info)
{  
    if(info.Length() != 4) {
        return;
    }

    if(!info[0]->IsString() || !info[1]->IsString() || !info[3]->IsFunction()) {
        return;
    }

    PutBaton *baton = new PutBaton;

    String::Utf8Value arg0(info[0]->ToString());
    baton->resourceId = string(*arg0);
    
    String::Utf8Value arg1(info[1]->ToString());
    baton->property = string(*arg1);

    if(info[2]->IsBoolean()){
    	baton->valueType = BOOLEAN;
    }else if(info[2]->IsInt32()){    	
        baton->valueType = INTEGER;
    }else if(info[2]->IsString()){    	
        baton->valueType = STRING;
    }

    String::Utf8Value arg2(info[2]->ToString());
    baton->value = string(*arg2);
    
    Persistent<Function> pcb(info.GetIsolate(), Handle<Function>::Cast(info[3]));
    baton->cb = pcb;

    baton->request.data = baton;
    uv_queue_work(uv_default_loop(), &baton->request, putResource, dummyAsyncAfter);    
}


