
/* Example client program (this is not a library header, don't include it in random programs): */

namespace Intel { namespace OCDemo { namespace client {

// Although not "done" here, this could be expanded into an interface to handle any sort of
// resource:
class resource_handle
{
 friend class resource_handler;

 private:
 const std::string                     URI;
 std::shared_ptr<OC::OCResource>       resource;

 public:
 resource_handle(const std::string& URI_, std::shared_ptr<OC::OCResource> resource_)
  : URI(URI_),
    resource(resource_)
 {}

 resource_handle(const std::string& URI_)
  : URI(URI_)
 {}

 // Callbacks (note that the signature after binding will match exactly:
 private:
 void onFoundResource(std::shared_ptr<OC::OCResource> in_resource);
 void onResourceGet(const OC::HeaderOptions& headerOptions,
                OC::OCRepresentation rep, const int error_code);
 void onResourcePut(const OC::HeaderOptions& headerOptions, const OC::OCRepresentation rep,
                const int error_code);
 void onObserve(const OC::HeaderOptions& headerOptions, const OC::OCRepresentation rep,
                const int error_code, const int& sequence_number);
};

class resource_handler
{

 static std::vector<std::shared_ptr<resource_handle>> resources;    // URI -> Maybe resource

 public:
 resource_handler(const std::vector<std::string>& resource_URIs_);
 resource_handler();

 public:
 bool has(const std::string& URI)
 {
    for(const auto& r : resources)
     {
      if(URI == r->URI)
       return true;
     }

    return false;
 }

 void add(const std::string& URI)
 {
    if(!has(URI))
     resources.emplace_back(std::make_shared<resource_handle>(URI));
 }

 void find_resources()
 {
        for(const auto& resource : resources)
         {
                std::cout << "* Finding resources \"" << resource->URI << "\".\n";

                call_timer.mark("find_resources");

                OC::OCPlatform::findResource("", resource->URI,
                                      std::bind(&resource_handle::onFoundResource, resource, std::placeholders::_1));
         }
 }
};

std::vector<std::shared_ptr<resource_handle>> resource_handler::resources;

resource_handler::resource_handler(const std::vector<std::string>& resource_URIs)
{
 for(const auto& URI : resource_URIs)
  add(URI);
}

void resource_handle::onFoundResource(std::shared_ptr<OC::OCResource> in_resource)
{
 using std::cout;

 cout << "* onFoundResource():\n";

 try
  {
       if(nullptr == in_resource)
        throw OC::OCException("invalid resource passed to client callback");

       call_timer.report_and_reset("find_resources");

       // Now, fixup our own representation ptr:
       resource = in_resource;

       /* Note: You can combine the host and URI to get a unique identifier, for
       example to filter out events you aren't interested in. Here, we just report the
       data: */
       cout << "resource URI: " << resource->uri() << "; "
            << "host address: " << resource->host() << '\n';

       call_timer.mark("get_resource");

       OC::QueryParamsMap qpm;

       resource->get(qpm, std::bind(&resource_handle::onResourceGet, this,
            std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
  }
 catch(OC::OCException& e)
  {
        std::cerr << "onFoundResource(): exception " << e.reason() << ": " << e.what() << '\n';
  }
 catch(std::exception& e)
  {
        std::cerr << "onFoundResource(): exception: " << e.what() << '\n';
  }
}

void resource_handle::onResourceGet(const OC::HeaderOptions& headerOptions,
                const OC::OCRepresentation rep, const int error_code)
{
 using std::cout;

 cout << "onResourceGet():\n";

 call_timer.report_and_reset("get_resource");

 if(error_code)
  {
        std::cerr << "onResourceGet(): error: " << error_code << '\n';
        return;
  }

 if(nullptr == resource)
  {
        std::cerr << "onResourceGet(): empty resource pointer.\n";
        return;
  }

 std::cout << "input attributes:\n";
 std::cout << "Attribute \"" << "state" << "\": "<< rep.getValue<bool>("state")<<"; ";
 std::cout << "Attribute \"" << "power" << "\": "<< rep.getValue<int>("power")<<"; \n";

 // Now, make a change to the light representation (replacing, rather than parsing):
 bool state = true;
 int power = 10;

 std::cout << "output attributes:\n";
 std::cout << "Attribute \"" << "state" << "\": "<< state<<"; ";
 std::cout << "Attribute \"" << "power" << "\": "<< power<<"; \n";

 call_timer.mark("put_resource");

 OC::OCRepresentation out_rep;
 out_rep.setValue("state", state);
 out_rep.setValue("power", power);

 resource->put(out_rep, OC::QueryParamsMap(),
               std::bind(&resource_handle::onResourcePut, this, std::placeholders::_1,
               std::placeholders::_2, std::placeholders::_3));
}

void resource_handle::onResourcePut(const OC::HeaderOptions& headerOptions,
            const OC::OCRepresentation rep, const int error_code)
{
 std::cout << "onResourcePut():\n";

 call_timer.report_and_reset("put_resource");

 if(0 != error_code)
  {
        std::ostringstream os;

        os << "onResourcePut(): error code " << error_code << " from server response.";

        throw OC::OCException(os.str());
  }

 std::cout << "input attributes:\n";
 std::cout << "Attribute \"" << "state" << "\": "<< rep.getValue<bool>("state")<<"; ";
 std::cout << "Attribute \"" << "power" << "\": "<< rep.getValue<int>("power")<<"; \n";

 call_timer.mark("observe_resource");

 // Start an observer:
 resource->observe(OC::ObserveType::Observe, OC::QueryParamsMap(),
                std::bind(&resource_handle::onObserve, this,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
                std::placeholders::_4));
}

void resource_handle::onObserve(const OC::HeaderOptions& headerOptions,
            const OC::OCRepresentation rep, const int error_code, const int& sequence_number)
{
 if(0 != error_code)
  {
    std::ostringstream os;
    os << "onObserve(): error " << error_code << " from callback.\n";
    throw OC::OCException(os.str());
  }

 std::cout << "onObserve(): sequence number: " << sequence_number << ":\n";

 call_timer.report_and_reset("observe_resource");

 std::cout << "Attribute \"" << "state" << "\": "<< rep.getValue<bool>("state")<<"; ";
 std::cout << "Attribute \"" << "power" << "\": "<< rep.getValue<int>("power")<<"; \n";

 const auto oc = observe_count();

 std::cout << "onObserve(): observation count is: " << oc << '\n';

 // We don't want to be observed forever for purposes of this demo:
 if(10 <= oc)
  {
    std::cout << "onObserve(): cancelling observation.\n";

    const auto result = resource->cancelObserve();

    std::cout << "onObserve(): result of cancellation: " << result << ".\n";

    this_thread::sleep_for(chrono::seconds(10));
  }
}

}}} // namespace Intel::OCDemo::client


