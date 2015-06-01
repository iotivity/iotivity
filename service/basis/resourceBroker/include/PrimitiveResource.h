/*
 * PrimitiveResource.h
 *
 *  Created on: May 20, 2015
 *      Author: root
 */

#ifndef PRIMITIVERESOURCE_H_
#define PRIMITIVERESOURCE_H_

class PrimitiveResource {
public:
    PrimitiveResource();
    PrimitiveResource(std::string _Uri, std::string _Address):Uri(_Uri), Address(_Address){}
    ~PrimitiveResource();

    const std::string getUri();
    const std::string getAddress();

    const std::string Uri;
    const std::string Address;
};

#endif /* PRIMITIVERESOURCE_H_ */
