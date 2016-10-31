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

class ICBinarySwitchResource : public ICResource //oic.r.switch.binary
{
private:
    bool m_value;

public:
    ICBinarySwitchResource(string uri, vector<string> rt, vector<string> itf);
    void setBinarySwitchRepresentation(OCRepresentation &rep);
    OCEntityHandlerResult entityHandler(shared_ptr<OCResourceRequest> request);
};
