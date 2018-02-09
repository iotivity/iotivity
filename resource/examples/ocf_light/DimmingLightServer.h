/*
 * DimmingLightServer.h
 */

#ifndef EXAMPLE_OCF_LIGHT_DIMMINGLIGHTSERVER_H_
#define EXAMPLE_OCF_LIGHT_DIMMINGLIGHTSERVER_H_

#include "BinarySwitchResource.h"
#include "DimmingResource.h"

class DimmingLightServer
{
public:
    /**
     *  constructor
     *  creates all resources from the resource classes.
     */
    DimmingLightServer(void);

    /**
     *  destructor
     *
     */
    ~DimmingLightServer(void);

private:
    // Resources that are part of this server
    BinarySwitchResource  m_binaryswitchInstance;
    DimmingResource  m_dimmingInstance;
};

#endif /* EXAMPLE_OCF_LIGHT_DIMMINGLIGHTSERVER_H_ */
