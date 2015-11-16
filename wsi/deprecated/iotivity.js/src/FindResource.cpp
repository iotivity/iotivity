#include <node.h>

#include "common.h"

using namespace v8;
using namespace OC;
using namespace std;

DiscoveredResourceMap discoveredResources;

struct FindResourceBaton
{
    uv_work_t request;
  
    string host;
    string resName;
    string conType;
    string returnValue;
    Persistent<Function, CopyablePersistentTraits<Function> > cb;
};

FindResourceBaton *frb;


static void findResource_AsyncAfter(uv_work_t *req, int status)
{
    FindResourceBaton *baton = static_cast<FindResourceBaton*>(req->data);
    Isolate* isolate = Isolate::GetCurrent();

    Local<Value> argv[] = 
    {
      (String::NewFromUtf8(isolate, frb->returnValue.c_str()))
    };
    Local<Function> fn = Local<Function>::New(isolate, baton->cb);
    fn->Call(isolate->GetCurrentContext()->Global(), 1, argv);
}


// Callback to found resources
void findResource_callback(shared_ptr<OCResource> resource)
{
    ostringstream stream;
    stream << resource->uniqueIdentifier();
    string resourceURI = stream.str();

    try
    {
        if(discoveredResources.find(resourceURI) == discoveredResources.end())
        {
            cout << "Found resource " << resourceURI <<
                " for the first time on server with ID: "<< resource->sid()<<endl;
            discoveredResources[resourceURI] = resource;
        }
        else
        {
            cout<<"Found resource "<< resource->uniqueIdentifier() << " again!"<<endl;
        }

        // Do some operations with resource object.
        if(resource)
        {
/*            cout<<"DISCOVERED Resource:"<<endl;
            // Get the resource types
            cout << "\tList of resource types: " << endl;
            for(auto &resourceTypes : resource->getResourceTypes())
            {
                cout << "\t\t" << resourceTypes << endl;
            }
            // Get the resource URI*/
        }
        else
        {
            // Resource is invalid
            cout << "Resource is invalid" << endl;
        }

    }
    catch(exception& e)
    {
        cerr << "Exception in foundResource: "<< e.what() << endl;
    }

    frb->returnValue = resourceURI;
    uv_queue_work(uv_default_loop(), &frb->request, dummyAsync, findResource_AsyncAfter);
}


void findResource(string host, string resName, string conType)
{
    cout << resName << endl;
    ostringstream requestURI;

    try
    {
        // makes it so that all boolean values are printed as 'true/false' in this stream
        cout.setf(ios::boolalpha);
        // Find all resources
        OCPlatform::findResource(host, resName ,CT_DEFAULT, &findResource_callback);

        cout<< "Finding Resource... " <<endl;

    } catch(OCException& e)
    {
         oclog() << "Exception in main: "<<e.what();
    }
}


static void findResourceAsync(uv_work_t *req)
{    
    FindResourceBaton *baton = static_cast<FindResourceBaton*>(req->data);
    findResource(baton->host, baton->resName, baton->conType);
    frb = baton;    
}


void findResourceFunc(const FunctionCallbackInfo<Value>& info)
{
    if(info.Length() != 4) {
        return;
    }

    if(!info[0]->IsString() || !info[1]->IsString() || !info[2]->IsString() || !info[3]->IsFunction()) {
        return;
    }

    FindResourceBaton *baton = new FindResourceBaton;

    String::Utf8Value arg0(info[0]->ToString());
    baton->host = string(*arg0);
    
    String::Utf8Value arg1(info[1]->ToString());
    baton->resName = string(*arg1);
    
    String::Utf8Value arg2(info[2]->ToString());
    baton->conType = string(*arg2);
    
    Persistent<Function> pcb(info.GetIsolate(), Handle<Function>::Cast(info[3]));
    baton->cb = pcb;

    baton->request.data = baton;
    uv_queue_work(uv_default_loop(), &baton->request, findResourceAsync, dummyAsyncAfter);
}
