/*
 * resource.h
 */
#ifndef EXAMPLE_OCF_LIGHT_RESOURCE_H_
#define EXAMPLE_OCF_LIGHT_RESOURCE_H_

#include "octypes.h"
#include "OCRepresentation.h"
#include "OCResourceRequest.h"

/*
* default class, so that we have to define less variables/functions.
*/
class Resource
{
public:
    virtual ~Resource() { };
protected:
    OCResourceHandle m_resourceHandle;
    OC::OCRepresentation m_rep;
    virtual OCEntityHandlerResult entityHandler(std::shared_ptr<OC::OCResourceRequest> request)=0;
};

#endif /* EXAMPLE_OCF_LIGHT_RESOURCE_H_ */
