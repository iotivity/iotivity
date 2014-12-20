/*
 * testDoor.h
 *
 *  Created on: Oct 14, 2014
 *  Author: kumar.vinay
 */

#ifndef TESTRESOURCE_H_
#define TESTRESOURCE_H_

#include "Device.h"
#include <iostream>

using namespace std;
class ITestResource
{
    public:

        virtual ~ITestResource() { cout << "~ITestResource\n" ;}

        virtual bool testGet() { cout << "testGet\n" ; return true;}
        virtual bool testPut()  { cout << "testPut\n" ; return true;}
        virtual bool testSubscription(OC::Cm::Device *cmDevice) { cout << "testSubscription\n" ; return true;}
        virtual bool testUnSubscribe() { cout << "testUnSubscribe\n" ; return true;}

};

#endif /* TESTRESOURCE_H_ */
