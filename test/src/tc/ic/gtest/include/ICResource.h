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

class ICResource
{
    public:
        ICResource(string uri, vector<string> rt, vector<string> itf);
        OCResourceHandle m_handle;
        string getResourceUri();
        vector<string> getResourceType();
        vector<string> getInterfaces();
        OCRepresentation getRepresentation(void);
        OCStackResult addChildResource(ICResource  *childResource);
        OCStackResult sendRepresentation(shared_ptr<OCResourceRequest> pRequest);
        OCStackResult propagate();
        virtual OCEntityHandlerResult entityHandler(shared_ptr<OCResourceRequest> request) = 0;

    protected:
        OCRepresentation    m_representation;
        vector<ICResource *>  m_childResources;
        ObservationIds      m_interestedObservers;
};
