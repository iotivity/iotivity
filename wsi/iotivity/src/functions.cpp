#include "OCPlatform.h"
#include "queue.hpp"

//Queue is stopgap for testing single resource sever. 
static Queue<std::string> listenQueue;

using namespace OC;

typedef std::map<OCResourceIdentifier, std::shared_ptr<OCResource>> DiscoveredResourceMap;

DiscoveredResourceMap discoveredResources;
std::shared_ptr<OCResource> curResource;
std::mutex curResourceLock;

// Callback handler on GET request
void onGet(const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode, std::string resourceID)
{    
    std::stringstream converter;
    
    try
    {
        std::string makeJson = "{\"id\":\"" + resourceID +  "\",";
        if(eCode == OC_STACK_OK)
        {
            std::cout << "GET request was successful" << std::endl;
            std::cout << "=================================================" << std::endl;

            OCRepPayload* paylod = rep.getPayload();
            OCRepPayloadValue* val = paylod->values;
            while(val)
            {
                switch(val->type)
                {
                    case OCREP_PROP_NULL:
                        //printf("\t\t%s: NULL", val->name);
                        makeJson = makeJson + "\"" + val->name + "\":"+ ":NULL,";
                        break;
                    case OCREP_PROP_INT:
                        printf("\t\t%s(int):%ld", val->name, val->i);
                        converter << val->i;
                        makeJson = makeJson + "\"" + val->name + "\":"+ "\"" + converter.str() + "\",";
                        break;
                    case OCREP_PROP_DOUBLE:
                        printf("\t\t%s(double):%f", val->name, val->d);
                        converter << val->d;
                        makeJson = makeJson + "\"" + val->name + "\":"+ "\"" + converter.str() + "\",";
                        break;
                    case OCREP_PROP_BOOL:
                        printf("\t\t%s(bool):%s", val->name, val->b ? "true" : "false");
                        converter << (val->b ? "true" : "false");
                        makeJson = makeJson + "\"" + val->name + "\":"+ "\"" + converter.str() + "\",";
                        break;
                    case OCREP_PROP_STRING:
                        printf("\t\t%s(string):%s", val->name, val->str);                        
                        makeJson = makeJson + "\"" + val->name + "\":"+ "\"" + val->str + "\",";
                        break;
                    case OCREP_PROP_OBJECT:
                        // Note: Only prints the URI (if available), to print further, you'll
                        // need to dig into the object better!
                        printf("\t\t%s(OCRep):%s", val->name, val->obj->uri);                        
                        makeJson = makeJson + "\"" + val->name + "\":"+ "\"" + val->obj->uri + "\",";
                        break;
                    case OCREP_PROP_ARRAY:
                        switch(val->arr.type)
                        {
                            case OCREP_PROP_INT:
                                printf("\t\t%s(int array):%ld x %ld x %ld",
                                        val->name,
                                        val->arr.dimensions[0], val->arr.dimensions[1],
                                        val->arr.dimensions[2]);
                                break;
                            case OCREP_PROP_DOUBLE:
                                printf("\t\t%s(double array):%ld x %ld x %ld",
                                        val->name,
                                        val->arr.dimensions[0], val->arr.dimensions[1],
                                        val->arr.dimensions[2]);
                                break;
                            case OCREP_PROP_BOOL:
                                printf("\t\t%s(bool array):%ld x %ld x %ld",
                                        val->name,
                                        val->arr.dimensions[0], val->arr.dimensions[1],
                                        val->arr.dimensions[2]);
                                break;
                            case OCREP_PROP_STRING:
                                printf("\t\t%s(string array):%ld x %ld x %ld",
                                        val->name,
                                        val->arr.dimensions[0], val->arr.dimensions[1],
                                        val->arr.dimensions[2]);
                                break;
                            case OCREP_PROP_OBJECT:
                                printf("\t\t%s(OCRep array):%ld x %ld x %ld",
                                        val->name,
                                        val->arr.dimensions[0], val->arr.dimensions[1],
                                        val->arr.dimensions[2]);
                                break;
                            default:
                                printf("\t\t%s <-- Unknown/unsupported array type!",
                                        val->name);
                                break;
                        }
                        break;
                    default:
                        printf("\t\t%s <-- Unknown type!", val->name);
                        break;
                }
                val = val -> next;
            }


            std::cout << "\n=================================================" << std::endl;
            std::cout << "Resource URI: " << rep.getUri() << std::endl;
        }
        else
        {
            std::cout << "onGET Response error: " << eCode << std::endl;
            std::exit(-1);
        }
        makeJson.erase(std::prev(makeJson.end()));
        makeJson = makeJson + "}";
        listenQueue.push(makeJson);
    }
    catch(std::exception& e)
    {
        std::cout << "Exception: " << e.what() << " in onGet" << std::endl;
    }
}

// Local function to get representation of light resource
void getRepresentation(std::shared_ptr<OCResource> resource)
{
    std::string hostAddress;

    if(resource)
    {
        std::cout << "Getting Resource Representation..."<<std::endl;
        // Get the resource host address
        hostAddress = resource->host();
        std::cout << "\tHost address of the resource: " << hostAddress << std::endl;

        std::string resourceID = hostAddress + "/oic/res";

        QueryParamsMap test;
        resource->get(test, [resourceID] (const HeaderOptions& headerOptions, const OCRepresentation& rep, const int eCode){
            onGet(headerOptions,rep,eCode, resourceID);
        });
    }
}
// callback handler on PUT request
void onPost(const HeaderOptions& /*headerOptions*/, const OCRepresentation& rep, const int eCode)
{
    try
    {
        if(eCode == OC_STACK_OK)
        {
            std::cout << "POST request was successful!!" << std::endl;
            listenQueue.push("POST request was successful");
        }
        else
        {
            std::cout << "onPost Response error: " << eCode << std::endl;
            std::exit(-1);
            listenQueue.push("onPost Response error");
        }
    }
    catch(std::exception& e)
    {
        std::cout << "Exception: " << e.what() << " in onPost" << std::endl;
        listenQueue.push("Exceptino onPost Response error");
    }
}

void foundResourceAndPost(std::shared_ptr<OCResource> resource,std::string property, std::string value)
{   
    if(resource)
    {
        OCRepresentation rep;

        std::cout << "Postting Resource representation..."<<std::endl;
        
        std::istringstream buffer(value);
        int m_vlaue;
        buffer >> m_vlaue;
        rep.setValue(property, m_vlaue);

        // Invoke resource's put API with rep, query map and the callback parameter
        resource->post(rep, QueryParamsMap(), &onPost);
    }
}
// Callback to found resources
void foundResource(std::shared_ptr<OCResource> resource)
{
    std::string resourceURI;
    try
    {
        std::lock_guard<std::mutex> lock(curResourceLock);         

        if(discoveredResources.find(resource->uniqueIdentifier()) == discoveredResources.end())
        {
            std::cout << "Found resource " << resource->uniqueIdentifier() <<
                " for the first time on server with ID: "<< resource->sid()<<std::endl;
            discoveredResources[resource->uniqueIdentifier()] = resource;
        }
        else
        {
            std::cout<<"Found resource "<< resource->uniqueIdentifier() << " again!"<<std::endl;
        }

        // Do some operations with resource object.
        if(resource)
        {            
            std::cout<<"DISCOVERED Resource:"<<std::endl;
            // Get the resource types
            std::cout << "\tList of resource types: " << std::endl;
            for(auto &resourceTypes : resource->getResourceTypes())
            {
                std::cout << "\t\t" << resourceTypes << std::endl;
            }
            // Get the resource URI
            resourceURI = resource->uri();

            if(resourceURI != "")
            {
                curResource = resource;
                // Call a local function which will internally invoke get API on the resource pointer
                getRepresentation(resource);
            }
        }
        else
        {
            // Resource is invalid
            std::cout << "Resource is invalid" << std::endl;
        }

    }
    catch(std::exception& e)
    {
        std::cerr << "Exception in foundResource: "<< e.what() << std::endl;
    }
}


std::string findResource(std::string resType)
{
    std::cout << resType << std::endl;
    std::ostringstream requestURI;

    //Create PlatformConfig object
    PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Both,
        "0.0.0.0",
        0,
        OC::QualityOfService::LowQos,
        NULL
    };

    OCPlatform::Configure(cfg);
    try
    {
        // makes it so that all boolean values are printed as 'true/false' in this stream
        std::cout.setf(std::ios::boolalpha);
        // Find all resources        
        requestURI << OC_RSRVD_WELL_KNOWN_URI << "?rt=" << resType; 
        
        OCPlatform::findResource("", requestURI.str(),CT_DEFAULT, &foundResource);
        
        std::cout<< "Finding Resource... " <<std::endl;        
        std::mutex blocker;
        std::unique_lock<std::mutex> lock(blocker);

    } catch(OCException& e)
    {
         oclog() << "Exception in main: "<<e.what();
    }

    std::string result;
    listenQueue.pop(result);    

    return result;
}
std::string postResource(std::string resourceId,std::string property,std::string value)
{ 
    std::cout << resourceId << std::endl;
    //Create PlatformConfig object
    PlatformConfig cfg {
        OC::ServiceType::InProc,
        OC::ModeType::Both,
        "0.0.0.0",
        0,
        OC::QualityOfService::LowQos,
        NULL
    };    

    OCPlatform::Configure(cfg);
    try
    {  
        // Find all resources        
        OCPlatform::findResource(resourceId, "",CT_DEFAULT, [property,value] (std::shared_ptr<OCResource> resource){
            foundResourceAndPost(resource, property, value);
        });
        
        std::cout<< "Finding Resource... " <<std::endl;        
        std::mutex blocker;
        std::unique_lock<std::mutex> lock(blocker);

    } catch(OCException& e)
    {
         oclog() << "Exception in main: "<<e.what();
    }

    std::string result;
    listenQueue.pop(result);    

    return result;
}
