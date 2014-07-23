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


#include <iostream>
#include <functional>

#include "OCServer.h"
#include "OCReflect.h"
#include "OCException.h"
#include "OCProperties.h"

namespace OC { namespace OCReflect {

// JFW:
/*
template <class BoundF>
struct call_through
{
 BoundF& f;

 const property_type		return_type;
 const property_type_vector	param_types;

 call_through(BoundF& f_, const property_type& return_type_, const property_type_vector param_types_)
  : f(f_), return_type(return_type_), param_types(param_types_)
 {}

 property operator()()
 {
	return make_property(f(), "return value"); // JFW: need a version of this for anonymous properties
 }

 template <class ...XS>
 property operator()(const XS& ...params)
 {
	// switch(p0_type)
	//return make_property(f(), "foo");

	f(params...);

//	return make_property(f(params...), "fancy result name");

	return make_property("Hello, World!", "fancy result name");
 }
};
*/

template <typename InstanceT, typename MethodT>
OC::OCReflect::bound_function bind_function(InstanceT *self, MethodT const& method)
{
 return [self, method](const property_vector& args) -> property
        {
            auto b = std::bind(method, self); 
            return OC::OCReflect::make_property::apply(b(args), "_ret_"); 
        };
}

template <typename InstanceT, typename MethodT, typename ...ParameterT>
void bind_service(OC::OCServer& server, InstanceT *self, MethodT const& method, const std::string& binding_name, const property_type& return_type, const ParameterT& ...param_types)
{
 OC::OCReflect::bound_function f { 
    [method, self](const property_vector& args) -> property 
    { 
        // JFW: auto b = std::bind(method, self); 

/* JFW: eventually, map parameters in to e return OC::OCReflect::make_property::apply(b(args), "_ret_"); 
        ...this is tricky, so we' punting on it for the Tuesday release. */

        return OC::OCReflect::make_property::apply("hello from your friendly neighborhood lambda", "return value");
    }
 };

 OC::OCReflect::method_binding fmb(binding_name, f,
                                   return_type, property_type_vector { param_types... });

 server.bind(fmb);
 
/*
 call_through<decltype(b)> ct(b, return_type, ptypes);

 // then, at the call site...
 auto r = ct(1); // call the function
 
 std::function<property_type(ParameterT...)> f = ct;
*/
}

template <typename PropertyT>
void bind_property(OC::OCServer& server, PropertyT& property, const std::string& binding_name, const property_type& pt, const property_attribute& pas)
{
}

}} // namespace OC::OCReflect

class light
{
 bool m_powered; 
 int m_level;
 int m_color;

 public:
 std::string manufacturer;

 public:
 light()
  : m_powered(false),
    m_level(0),
    m_color(0),
    manufacturer("Integrated Electronics")
 {}

 public:
 void update_things()								{ std::cout << "light::update_things()\n"; }

 void set_powered(const bool new_state)             { m_powered = new_state; }
 bool get_powered() const                           { return m_powered; }

 void set_level(const int new_level, int color)     { m_level = new_level; m_color = color; }
 int get_level() const                              { return m_level; }

 /* An example of server-side service binding. Note that this does not need to be a 
 member function: for classes you do not have access to, you can accomplish this with 
 a free function: */
 public:
 void bindTo(OC::OCServer& server, const std::string& base_URI)
 {
                using OC::OCReflect::method_signature;
                using OC::OCReflect::method_binding;
                using OC::OCReflect::property_signature;
                using OC::OCReflect::property_binding;
                using OC::OCReflect::bind_service;
                using OC::OCReflect::bind_property;

                using OC::OCReflect::property_type;

                server.registerResource(this, base_URI);

				// Bind a nullary method:
				bind_service(   server,	
								this,
								&light::update_things,
								"updateThings",
								property_type::nil,
								property_type::nil);

				// Bind an unary method:
				bind_service(	server,
								this, &light::set_powered,
								"setPowered",
								property_type::nil,
								property_type::boolean);

                // Example to map setLevel method using the canonical way in one step:
                bind_service(
                                server,                         // server to bind with
                                this,                           // instance to bind
                                &light::set_level,              // method to bind
                                "setLevel",                     // service name
                                property_type::nil,             // input type
                                property_type::integer,         // parameter list starts (level)
                                property_type::integer);        // parameter list (color) 


                bind_service(server, this, &light::get_level, "getLevel",
                                           property_type::integer);

                bind_service(server, this, &light::get_powered, "isPowered",
                                           property_type::boolean);

                bind_service(server, this, &light::set_powered, "setPowered",
                                           property_type::nil,
                                           property_type::boolean);

                // Map powered() as a service:
                method_signature m_setPowered(
                        property_type::nil,                     // return type
                        property_type::boolean                  // parameter list (here, just arity 1)
                );

                // JFW: below is to say a wrapper for: std::bind(&light::set_powered, this); we'll need
                // to pretty these up:
                OC::OCReflect::bound_function f = 
                        [](const OC::OCReflect::property_vector& args) -> OC::OCReflect::property 
                            { return OC::OCReflect::make_property::apply(nullptr, "ret"); };

                method_binding b_setPowered(
                        "setPowered",           // name to bind with service URL
                        f,                      // function to bind to the service
                        m_setPowered);          // method signature (metadata)

                server.bind(b_setPowered);      // request binding
/*
                // Example to bind a read-only property in individual steps:
                property_signature p_manufacturer_signature(property_type::string, OC::OCReflect::property_attribute::r); // read-only

                OC::OCReflect::named_property_binding p_manufacturer("manufacturer", p_manufacturer_signature);

                property_binding b_manufacturer(
                        this->manufacturer,
                        p_manufacturer);

                server.bind(b_manufacturer);

                // The canonical way to bind a property to a server in one step:
                bind_property(
                                server,                                      // server to bind to
                                this->manufacturer,                          // pointer to property
                                "manufacturer",                              // property binding name
                                OC::OCReflect::property_type::string,        // property
                                OC::OCReflect::property_attribute::r         // type decoration
                );
*/
        }
};

void show_bindings(const OC::OCServer& server)
{
 using namespace std;

 const auto& methods = server.methods();

 cout << "Bound methods:\n";
 for(const auto& m : methods)
  {
    cout << m.first << ':' << m.second << '\n'; 
  }
}

int main()
{
 OC::OCServer server;

 try
  {
	light l;
	l.bindTo(server, "/foo/");
  }
 catch(OC::OCReflect::reflection_exception& e)
  {
	std::cerr << "Oops: " << e.what() << '\n';
  }

 show_bindings(server);
}
