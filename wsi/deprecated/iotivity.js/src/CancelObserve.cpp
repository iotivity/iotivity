#include <node.h>

#include "common.h"

using namespace v8;
using namespace OC;
using namespace std;

struct CancelObserveBaton
{
    uv_work_t request;
  
    string resourceId;
    string returnValue;
    Persistent<Function, CopyablePersistentTraits<Function> > cb;
};

CancelObserveBaton *cancelObserveB;

static void CancelObserveRepresentation(shared_ptr<OCResource> resource)
{
    Isolate* isolate = Isolate::GetCurrent();    
    if(resource)
    {
        cout << "CancelObserveting Resource Representation..."<< endl;
        // CancelObserve the resource host address 
        OCStackResult result = resource->cancelObserve();
        cout << "Cancel result: "<< result <<endl;
        sleep(2);        
        cancelObserveB->returnValue = "{\"id\":\"" + cancelObserveB->resourceId +  "\"," + "\"message\":"+ "\"Observe Cancel\"}";                           
        Local<Value> argv[] = 
        {
          (String::NewFromUtf8(isolate, cancelObserveB->returnValue.c_str()))
        };        
        Local<Function> fn = Local<Function>::New(isolate, cancelObserveB->cb);
        fn->Call(isolate->GetCurrentContext()->Global(), 1, argv);
    }    
}


static void cancelObserveResource(uv_work_t *req, int status)
{  
    CancelObserveBaton *baton = static_cast<CancelObserveBaton*>(req->data);
    DiscoveredResourceMap::iterator it = discoveredResources.find(baton->resourceId);

    cout << "cancelObserveValueAsync   " << baton->resourceId << ":" << discoveredResources.size() <<endl;

    if(it == discoveredResources.end())
        return;

    cancelObserveB = baton; 
    CancelObserveRepresentation(discoveredResources[baton->resourceId]);    
}


void cancelObserveValueFunc(const FunctionCallbackInfo<Value>& info)
{
    if(info.Length() != 2) {
        return;
    }

    if(!info[0]->IsString() || !info[1]->IsFunction()) {
        return;
    }

    CancelObserveBaton *baton = new CancelObserveBaton;

    String::Utf8Value arg0(info[0]->ToString());
    baton->resourceId = string(*arg0);
   
    Persistent<Function> pcb(info.GetIsolate(), Handle<Function>::Cast(info[1]));
    baton->cb = pcb;

    baton->request.data = baton;
    uv_queue_work(uv_default_loop(), &baton->request, dummyAsync, cancelObserveResource);    
}


