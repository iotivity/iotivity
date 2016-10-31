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

class ICTemperatureResource : public ICResource //oic.r.temperature
{
private:
        int m_temperature;
        string m_range;
        string m_units;

public:
    ICTemperatureResource(string uri, vector<string> rt, vector<string> itf);
    void setTemperatureRepresentation(OCRepresentation &rep);
    OCEntityHandlerResult entityHandler(shared_ptr<OCResourceRequest> request);
};
