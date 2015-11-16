#include <node.h>

#include "common.h"

using namespace v8;
using namespace OC;
using namespace std;

struct DeleteBaton
{
    uv_work_t request;
  
    string resourceId;
    string returnValue;
    Persistent<Function, CopyablePersistentTraits<Function> > cb;
};

DeleteBaton *deleteB;


static void onDeleteAsyncAfter(uv_work_t *req, int status)
{   
    DeleteBaton *baton = static_cast<DeleteBaton*>(req->data);
    Isolate* isolate = Isolate::GetCurrent();

    Local<Value> argv[] = 
    {
      (String::NewFromUtf8(isolate, deleteB->returnValue.c_str()))
    };
    Local<Function> fn = Local<Function>::New(isolate, baton->cb);
    fn->Call(isolate->GetCurrentContext()->Global(), 1, argv);
}


void onDelete(const HeaderOptions& opts,const int eCode)
{
    cout << "onDelete..."<< endl;
    if (eCode == OC_STACK_RESOURCE_DELETED)
    {
        cout << "DELETE request was successful" << endl;
        deleteB->returnValue = "{\"id\":\"" + deleteB->resourceId +  "\"," + "\"message\":"+ "\"DELETE request was successful\"}";
    }
    else
    {
        cout << "onDelete Response error: " << eCode << endl; 
        deleteB->returnValue = "{\"id\":\"" + deleteB->resourceId +  "\"," + "\"message\":"+ "\"onDelete Response error\"}";
    }
    uv_queue_work(uv_default_loop(), &deleteB->request, dummyAsync, onDeleteAsyncAfter);        
}


void DeleteRepresentation(shared_ptr<OCResource> resource)
{    
    if(resource)
    {
        cout << "Deleteting Resource Representation..."<< endl;
        // Delete the resource host address 
        
        resource->deleteResource(&onDelete);
    }
}


static void deleteResource(uv_work_t *req)
{  
    DeleteBaton *baton = static_cast<DeleteBaton*>(req->data);
    DiscoveredResourceMap::iterator it = discoveredResources.find(baton->resourceId);

    cout << "deleteValueAsync   " << baton->resourceId << ":" << discoveredResources.size() <<endl;

    if(it == discoveredResources.end())
        return;

    deleteB = baton; 
    DeleteRepresentation(discoveredResources[baton->resourceId]);    
}


void deleteValueFunc(const FunctionCallbackInfo<Value>& info)
{
    if(info.Length() != 2) {
        return;
    }

    if(!info[0]->IsString() || !info[1]->IsFunction()) {
        return;
    }

    DeleteBaton *baton = new DeleteBaton;

    String::Utf8Value arg0(info[0]->ToString());
    baton->resourceId = string(*arg0);
   
    Persistent<Function> pcb(info.GetIsolate(), Handle<Function>::Cast(info[1]));
    baton->cb = pcb;

    baton->request.data = baton;
    uv_queue_work(uv_default_loop(), &baton->request, deleteResource, dummyAsyncAfter);    
}


