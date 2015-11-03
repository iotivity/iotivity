#include <uv.h>
#include <string>

#include "OCPlatform.h"

typedef std::map<std::string, std::shared_ptr<OC::OCResource>> DiscoveredResourceMap;
extern DiscoveredResourceMap discoveredResources;


static void dummyAsync(uv_work_t *req){}
static void dummyAsyncAfter(uv_work_t *req, int status){}


std::string make_json(const OC::OCRepresentation& rep, const int eCode, std::string resourceID); 
