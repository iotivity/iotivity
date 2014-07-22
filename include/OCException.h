//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef __INTEL_OCEXCEPTION_H_2014_07_10
 #define __INTEL_OCEXCEPTION_H_2014_07_10

#include <stdexcept>

namespace OC { namespace OCReflect {

typedef std::runtime_error reflection_exception;

class OCException : public std::exception
{

};

}} // namespace OC::OCReflect

#endif
