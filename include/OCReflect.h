//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#ifndef __INTEL_OCREFLECT_H_20140708
 #define __INTEL_OCREFLECT_H_20140708

#include <iostream> 

/* Runtime reflection support. */

#include <tuple>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <cstdlib>
#include <functional>

#include "OCServer.h"
#include "OCProperties.h"

namespace OC { namespace OCReflect {

struct service;
class  remote_resource;
struct entity;
class  method;

struct service
{
 template <typename NarrowT>
 NarrowT property(const OC::OCReflect::entity& entity);

 OC::OCReflect::method method(const std::string& name);
};

/* This type originally represented a binding to a server-- I think it can probably
be factored out soon: */
class remote_resource
{
 OC::OCResource&    resource;

 std::string        resource_location;

 public:
 remote_resource(OC::OCResource& resource_, const std::string& resource_location_)
  : resource(resource_),
    resource_location(resource_location_)
 {}

 public:
 OC::OCReflect::entity operator()(const std::string& name);
};

struct entity
{
 // underlying type; data buffer; ctor may only be invoked from a remote resource
};

class method
{
 OC::OCResource *resource;  // observing ptr

 std::string name;

 public:
 /* This default ctor will go away once real narrowing is functional-- here to placate the compiler for now.
    - Ultimately, you should never be able to construct one of these in an invalid state. */
 method()
  : resource { nullptr }
 {}
 
 method(OC::OCResource& resource_, const std::string& name_)
  : resource(&resource_), 
    name(name_)
 {}

 public:
 /* Note that this declaration will likely change in the near future: */
 template <class ...TS>
 OC::OCReflect::tagged_property operator()(TS ...xs)
 {
    return OC::OCReflect::tagged_property();
 };
};

}} // namespace OC::OCReflect

// Convert to underlying OCStack C API (and, some handy C-wrangling utilities):
namespace OC { namespace OCReflect { namespace OCStack  {

void release(char *in);
void release(char **in);
char *strdup(const char *s);
char *strdup(const std::string& s);
size_t length(char **in);
char **convert(const std::vector<std::string>& vs);

std::string convert(const property_binding& npb);
std::vector<std::string> convert(const property_binding_vector& psv);

OC::OCReflect::property_type as_property_type(const std::string& pt_rep);
OC::OCReflect::property_binding as_property_binding(const std::string& pb_rep);
OC::OCReflect::property_binding_vector as_property_binding_vector(const std::vector<std::string>& pb_reps);

char *flatten(const std::vector<std::string>& input, const std::string& delim = ";");
std::vector<std::string> expand(const char *flattened_string, const std::string& delim = ";");

}}} // namespace OC::OCReflect::OCStack

#endif
