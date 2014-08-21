//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
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

/* To avoid confusion: These are not unit tests, these are "test programs". */

#include <map>
#include <string>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <stdexcept>

#include "OCReflect.h"

using namespace OC::OCReflect;

using std::cout;

template <class InputT>
void convert_and_compare(const InputT& input, const std::string name = "property")
{
 auto p = make_property::apply(input, name);

 cout << p << '\n';

 auto x = concretize<InputT>(p);

 cout << x << '\n';

 assert(input == x);
}

// Convert a native type to a property:
void convert_test()
{
 using std::cout;
 using namespace OC::OCReflect;
/*
 // Nil (JSON: null):
 {
    property_signature ps(property_type::nil);
    property_binding npb("nil_property", ps); 
 }
*/

 // Boolean:
 {
    convert_and_compare(true, "bool");
    convert_and_compare(false, "bool");
 }

 // Integer:
 {
    const int64_t input { 1024 };
    convert_and_compare(input, "int64_t");
 }

 // Rational (JSON: number):
 {
	const double input { 1024.0 };
    convert_and_compare(input, "double");
 }

 // String:
 {
	const std::string input { "Hello, World!" };
    convert_and_compare(input, "string");
 }

/*
 // List (JSON: array):
 {
    property_signature(property_type::list);    
 }
*/
/* JFW:
 // Dictionary (JSON: object):
 {
    property_signature(property_type::dictionary);  
 }
*/

 // test with attributes:
}

typedef property return_property;

template <class InT>
return_property make_return_property(const InT& in)
{
 return OC::OCReflect::make_property::apply(in, "__RETURN_VALUE__"); // ugly-- avert your eyes!
}

typedef std::function<return_property(const property_vector&)>	bound_call;

struct server
{
 std::map<std::string, bound_call>	methods;
};

std::string test_fn(const std::string s0, const std::string s1)
{
 return s0 + " " + s1;
}

/*
template <class ...ParamsT>
void check_signature(const property_signature_vector& param_types, ParamsT ...params)
{
 
}

struct call
{
 template <class ...ParamsT>
 static return_property operator()(bound_call& f, const property_signature return_type, const property_signature_vector& param_types, ParamsT ...params)
 {
	check_signature(param_types, params...);

	auto result = f(params...);

	check_signature(return_type, result);

	return result;
 }
*/

/* - match type
   - if type is concrete type
   - return curried function for that thing
   - this binds eventually to property(property_list), but calls underlying fn
*/

OC::OCReflect::property_signature head(const OC::OCReflect::property_signature_vector& psv)
{
 return psv[0];
}

// sloooow, but it will suit our purpose:
OC::OCReflect::property_signature_vector tail(const OC::OCReflect::property_signature_vector psv)
{
 return OC::OCReflect::property_signature_vector(1 + psv.begin(), psv.end());
}

bool type_matches(const OC::OCReflect::property_type& expected_pt, const OC::OCReflect::property_data& in)
{
 auto begin = in.begin();
 
 auto found_pt = static_cast<OC::OCReflect::property_type>(*begin);

 std::cout << "val ept=" << (int)expected_pt << ", ept: " << (int)found_pt << '\n';

 return expected_pt == found_pt;
}

bool type_matches(const OC::OCReflect::property_type expected_pt, const OC::OCReflect::property& in)
{
 return type_matches(expected_pt, std::get<1>(in));
}

bool signature_matches(const OC::OCReflect::property_signature&)
{
 return true;
}

/* JFW: tmp
template <class X>
bool signature_matches(const OC::OCReflect::property_signature& expected, X x)
{
 return type_matches(expected.type, OC::OCReflect::make_property::apply(x, "_")); 
}

template <class X, class ...XS>
bool signature_matches(const OC::OCReflect::property_signature_vector expected, X x, XS ...xs)
{
 return signature_matches(head(expected), x) && signature_matches(tail(expected), xs...);
}

template <class ...XS>
bool signature_matches(const OC::OCReflect::property_signature_vector expected, XS ...xs)
{
 return signature_matches(head(expected), x) && signature_matches(tail(expected), xs...);
 // return signature_matches(expected, xs...);
}
*/

void call_test()
{
 server s;

 s.methods["test_fn"] = 
	[](const property_vector& in) -> return_property
	{
		 for(const auto& p : in)
		 std::cout << p << '\n'; 

		auto s0 = concretize<std::string>(in[0]);
		auto s1 = concretize<std::string>(in[1]);

		return make_return_property(test_fn(s0, s1));
	};

 property_vector pv;
 pv.emplace_back(make_property::apply("Hello", "s0"));
 pv.emplace_back(make_property::apply("World", "s1"));

 std::cout << "INPUT:\n" << pv << '\n';

 auto result = s.methods["test_fn"](pv);

 std::cout << concretize<std::string>(std::get<1>(result));

 using namespace OC::OCReflect;

 // typechecking:
 {
 auto foo = make_property::apply("Hi there", "s0");
 assert(false == type_matches(property_type::boolean, foo));
 assert(true == type_matches(property_type::string, foo));
 }

/* JFW: tmp
 // signature typechecking:
 {
	property_signature_vector sig {
		{ property_type::string },
		{ property_type::integer }
	};

	assert(false == signature_matches(sig, 5, "Hello"));
	assert(true == signature_matches(sig, "Hello", 5));
 }
*/

/*
 auto sig = make_fun_sig(property_type::string,			// result
						 property_type::string, property_type::string);	// params
 std::cout << sig << '\n';
*/
/*
 auto f = make_fun(test_fn, 
				   property_type::string, 						  // result
				   property_type::string, property_type::string); // params
*/
}

// Demo of how to generate OCStack stuff (talk to the C library):
void rep_test()
{
 using OC::OCReflect::property_type;
 using OC::OCReflect::property_binding;

 property_binding_vector sigs {
	property_binding("state", property_type::boolean),
	property_binding("power", property_type::integer)
 };

 using namespace OC::OCReflect::OCStack;

 /* JFW: note:
 char *LEDrep[] = { "state;b", "power;i", NULL};

 char **LEDrep = convert(convert(sigs));

 if(0 == LEDrep)
  error();

 CREATE_RESOURCE_TYPE(LED, "core.led", LEDrep);
 */

 std::vector<std::string> reps { convert(sigs) }; 
 for(const auto& r : reps)
  std::cout << "converted representation: " << r << '\n';

 // Example of flattening to a single string:
 char *flattened = flatten(reps);
 if(nullptr == flattened)
  throw std::runtime_error("couldn't flatten");

 std::cout << "FLATTENED: " << flattened << '\n';

 // Example of converting to an array of char*s:
 char **handle = convert(reps);	

 if(nullptr == handle)
  {
        release(flattened);
        throw std::runtime_error("handle is a nullptr"); 
  }

 std::for_each(handle, handle + length(handle), [](const char *s) { std::cout << s << '\n'; });

 /* ...or, you could do it this way (sad panda!):
 for(char **cursor = handle; nullptr != *cursor; ++cursor)
  printf("%s\n", *cursor);
 */

 release(handle); // remember to free the memory! (Note: nobody said you couldn't wrap this in a smart ptr --JFW)
 release(flattened);
}

// Convert to and from OCStack representation:
void from_rep_test()
{
 property_binding_vector sigs {
	property_binding("state", property_type::boolean),
	property_binding("power", property_type::integer)
 };

 using OC::OCReflect::OCStack::length;
 using OC::OCReflect::OCStack::convert;

 using OC::OCReflect::OCStack::expand;
 using OC::OCReflect::OCStack::flatten;

 using OC::OCReflect::OCStack::as_property_binding_vector;

 std::vector<std::string> reps { convert(sigs) };   // convert a property_binding_vector to a std::vector of std::string

 /* First, let's make some C data from OCReflect stuff: 
    IMPORTANT: error checking is ignored below, DO NOT do this in real code. :-) */
 char *flattened = flatten(reps);   // convert a vector of std::string to a single C string
 char **handle   = convert(reps);   // convert a vector of std::string into a C array of C strings

 /* Great, now let's convert back to OCReflect: */ 
 // C array to std::vector of std::string (just normal C++):
 std::vector<std::string> reps_in1 { handle, length(handle) + handle };

 std::cout << "Retreived from C array:\n";
 for(const auto& s : reps_in1)
  std::cout << s << '\n';

 // C string into std::vector of std::string:
 std::vector<std::string> reps_in2 { expand(flattened) };

 std::cout << "Retrieved from flattened C string:\n";
 for(const auto& s : reps_in2)
  std::cout << s << '\n';

 /* Now, convert a std::vector of std::string into a vector of property bindings: */
 property_binding_vector sigs_in1 { as_property_binding_vector(reps_in1) };
 std::cout << "Retreived properties from vector<string>:\n";
 for(const auto sig : sigs_in1)
  std::cout << sig << '\n';

 // Notice that either representation /source/ is fine, it's just a vector of strings! 
 property_binding_vector sigs_in2 { as_property_binding_vector(reps_in2) };
 for(const auto sig : sigs_in1)
  std::cout << sig << '\n';
}

void simple_test()
{
 using namespace std;
 
 OC::OCReflect::property p = make_property::apply("Sashi", "p_first_name");

 string lname = "Penta";

 auto p2 = make_property::apply(lname, "p_last_name");

 cout << p << '\n'; 

 std::string fname = concretize<std::string>(p);
 cout << "string back from property: " << fname << '\n'; 
}

/* Note:
template <class OutT>
void concretize(const property& p, OutT& out)
{
 out = concretize<OutT>(p);
}
*/

int main()
{

 convert_test(); 

 call_test();

 simple_test();

 rep_test();

 from_rep_test();
}
