#include <node.h>

#include "common.h"

using namespace v8;
using namespace OC;
using namespace std;

struct GetBaton
{
    uv_work_t request;
  
    string resourceId;
    string returnValue;
    Persistent<Function, CopyablePersistentTraits<Function> > cb;
};

GetBaton *getB;


static void onGetAsyncAfter(uv_work_t *req, int status)
{   
    GetBaton *baton = static_cast<GetBaton*>(req->data);
    Isolate* isolate = Isolate::GetCurrent();

    Local<Value> argv[] = 
    {
      (String::NewFromUtf8(isolate, getB->returnValue.c_str()))
    };
    Local<Function> fn = Local<Function>::New(isolate, baton->cb);
    fn->Call(isolate->GetCurrentContext()->Global(), 1, argv);
}


void onGet(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode)
{    
    try
    {
        getB->returnValue = make_json(rep, eCode, getB->resourceId);
        uv_queue_work(uv_default_loop(), &getB->request, dummyAsync, onGetAsyncAfter);
    }
    catch(exception& e)
    {
        cout << "Exception: " << e.what() << " in onGet" << endl;
    }
}


void specificGetRepresentation(shared_ptr<OCResource> resource)
{    
    if(resource)
    {
        cout << "Getting Resource Representation..."<<endl;
        // Get the resource host address
 
        QueryParamsMap queryParamsMap;
        resource->get(queryParamsMap, &onGet);
    }
}


static void getResource(uv_work_t *req)
{  
    GetBaton *baton = static_cast<GetBaton*>(req->data);
    DiscoveredResourceMap::iterator it = discoveredResources.find(baton->resourceId);

    cout << "getValueAsync   " << baton->resourceId << ":" << discoveredResources.size() <<endl;

    if(it == discoveredResources.end())
        return;

    getB = baton; 
    specificGetRepresentation(discoveredResources[baton->resourceId]);    
}


void getValueFunc(const FunctionCallbackInfo<Value>& info)
{
    if(info.Length() != 2) {
        return;
    }

    if(!info[0]->IsString() || !info[1]->IsFunction()) {
        return;
    }

    GetBaton *baton = new GetBaton;

    String::Utf8Value arg0(info[0]->ToString());
    baton->resourceId = string(*arg0);
   
    Persistent<Function> pcb(info.GetIsolate(), Handle<Function>::Cast(info[1]));
    baton->cb = pcb;

    baton->request.data = baton;
    uv_queue_work(uv_default_loop(), &baton->request, getResource, dummyAsyncAfter);    
}


