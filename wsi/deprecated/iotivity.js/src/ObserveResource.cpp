#include <node.h>

#include "common.h"

using namespace v8;
using namespace OC;
using namespace std;

struct ObserveBaton
{
    uv_work_t request;
  
    string resourceId;
    unsigned int observeType;
    string returnValue;
    Persistent<Function, CopyablePersistentTraits<Function> > cb;
};

ObserveBaton *observeB;
static ObserveType OBSERVE_TYPE_TO_USE = ObserveType::Observe;

static void onObserveAsyncAfter(uv_work_t *req, int status)
{   
    ObserveBaton *baton = static_cast<ObserveBaton*>(req->data);
    Isolate* isolate = Isolate::GetCurrent();

    Local<Value> argv[] = 
    {
      (String::NewFromUtf8(isolate, observeB->returnValue.c_str()))
    };
    Local<Function> fn = Local<Function>::New(isolate, baton->cb);
    fn->Call(isolate->GetCurrentContext()->Global(), 1, argv);
}

void onObserve(const HeaderOptions /*headerOptions*/, const OCRepresentation& rep,
                    const int& eCode, const int& sequenceNumber)
{
    try
    {
        if(eCode == OC_STACK_OK && sequenceNumber != OC_OBSERVE_NO_OPTION)
        {
            if(sequenceNumber == OC_OBSERVE_REGISTER)
            {
                std::cout << "Observe registration action is successful" << std::endl;
            }
            else if(sequenceNumber == OC_OBSERVE_DEREGISTER)
            {
                std::cout << "Observe De-registration action is successful" << std::endl;
            }

            std::cout << "OBSERVE RESULT:"<<std::endl;
            std::cout << "\tSequenceNumber: "<< sequenceNumber << std::endl;
            observeB->returnValue = make_json(rep, eCode, observeB->resourceId);            
        }
        else
        {
            if(sequenceNumber == OC_OBSERVE_NO_OPTION)
            {
                std::cout << "Observe registration or de-registration action is failed" << std::endl;
                observeB->returnValue = "{\"id\":\"" + observeB->resourceId +  "\"," + "\"message\":"+ "\"Observe registration or de-registration action is failed\"}";                
            }
            else
            {
                std::cout << "onObserve Response error: " << eCode << std::endl;
                observeB->returnValue = "{\"id\":\"" + observeB->resourceId +  "\"," + "\"message\":"+ "\"onObserve Response error\"}";                
            }
        }
    }
    catch(std::exception& e)
    {
        std::cout << "Exception: " << e.what() << " in onObserve" << std::endl;
        observeB->returnValue = "{\"id\":\"" + observeB->resourceId +  "\"," + "\"message\":"+ "\"" + "Exception: " + e.what() +  " in onObserve" + "\"}";
    }
    uv_queue_work(uv_default_loop(), &observeB->request, dummyAsync, onObserveAsyncAfter);
}

ObserveType getObserveType(int type)
{
    switch (type) {
    case 0:
        cout << "<===Setting ObserveType to Observe===>\n\n";
        return OC::ObserveType::Observe;
    case 1:
        cout << "<===Setting ObserveType to ObserveAll===>\n\n";
        return OC::ObserveType::ObserveAll;
    default:        
        cout << "<===Setting ObserveType to ObserveAll===>\n\n";
        return OC::ObserveType::ObserveAll;
    };
}

void ObserveRepresentation(shared_ptr<OCResource> resource)
{   
    cout << "segfault check 3 : " <<  resource << endl;

    if(resource)
    {
        cout << "Observing Resource Representation... "<< endl;         
        // Observe the resource host address 

        OBSERVE_TYPE_TO_USE = getObserveType(observeB->observeType);        
        resource->observe(OBSERVE_TYPE_TO_USE, QueryParamsMap(), &onObserve);
    }
}

static void observeResource(uv_work_t *req)
{  
    ObserveBaton *baton = static_cast<ObserveBaton*>(req->data);
    DiscoveredResourceMap::iterator it = discoveredResources.find(baton->resourceId);

    cout << "observeValueAsync   " << baton->resourceId << ":" << discoveredResources.size() <<endl;

    if(it == discoveredResources.end())
        return;        
    
    observeB = baton; 
    ObserveRepresentation(discoveredResources[baton->resourceId]);        
}

void observeValueFunc(const FunctionCallbackInfo<Value>& info)
{    
    if(info.Length() != 3) {
        return;
    }
    
    if(!info[0]->IsString() || !info[1]->IsUint32() || !info[2]->IsFunction()) {
        return;
    }

    ObserveBaton *baton = new ObserveBaton;

    String::Utf8Value arg0(info[0]->ToString());
    baton->resourceId = string(*arg0);    
    baton->observeType = info[1]->Uint32Value(); 

    Persistent<Function> pcb(info.GetIsolate(), Handle<Function>::Cast(info[2]));
    baton->cb = pcb;

    baton->request.data = baton;
    uv_queue_work(uv_default_loop(), &baton->request, observeResource, dummyAsyncAfter);    
}


