#include <memory>
#include <iostream>
#include <stdexcept>
#include <condition_variable>
#include <map>
#include <vector>
#include <string>
#include <unistd.h>

#include "ocstack.h"
#include "ocpayload.h"
#include "rd_client.h"
#include "rd_server.h"
#include "rd_database.h"

#include <OCApi.h>
#include <OCPlatform.h>
#include "ICHelper.h"

using namespace OC;
using namespace std;

class ICAirconditionerResource : public ICResource //oic.d.airconditioner
{
public:
    ICAirconditionerResource(string uri, vector<string> rt, vector<string> itf);
    OCEntityHandlerResult entityHandler(shared_ptr<OCResourceRequest> request);
};
