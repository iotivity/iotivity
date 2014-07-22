//******************************************************************
//
// Copyright 2014 Intel Corporation All Rights Reserved.
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

// Convert a native type to a property:
void convert_test()
{
 using std::cout;
 using namespace OC::OCReflect;
/*
 // Nil (JSON: null):
 {
    property_signature ps(property_type::nil);
    named_property_binding npb("nil_property", ps); 
 }
*/

 // Boolean:
 {
    OC::OCReflect::property my_bool     = OC::OCReflect::make_property::apply(true, "my_bool");
    OC::OCReflect::property my_bool_too = make_property::apply(false, "my_bool_too");

    cout << my_bool << '\n' << my_bool_too << '\n';

    auto x = concretize<bool>(my_bool);
    auto y = concretize<bool>(my_bool_too);

    cout << "got back: " << x << '\n';
    cout << "got back: " << y << '\n';

	assert(x == true);
	assert(y == false);
 }

 // Integer:
 {
    const int64_t input { 1024 };
    auto my_int = make_property::apply(input, "my int");

    cout << my_int << '\n';

    auto x = concretize<int64_t>(my_int);

    cout << x << '\n';

	assert(input == x);
 }

 // Rational (JSON: number):
 {
	const double input { 1024.0 };
	auto my_float = make_property::apply(input, "my float");

	cout << my_float << '\n';

	auto x = concretize<double>(my_float);

	cout << x << '\n';

	assert(input == x);
 }

 // String:
 {
	const std::string input { "Hello, World!" };

	auto my_string = make_property::apply(input, "my string");

	cout << my_string << '\n';

	auto x = concretize<std::string>(my_string);

	cout << x << '\n';

	assert(input == x);
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

// Demo of how to generate OCStack stuff:
void rep_test()
{
 using OC::OCReflect::property_type;
 using OC::OCReflect::named_property_binding;

 named_property_binding_vector sigs {
	named_property_binding("state", property_type::boolean),
	named_property_binding("power", property_type::integer)
 };

 using namespace OC::OCReflect::to_OCStack;

 /* JFW: note:
 char *LEDrep[] = { "state;b", "power;i", NULL};

 char **LEDrep = convert(convert(sigs));

 if(0 == LEDrep)
  error();

 CREATE_RESOURCE_TYPE(LED, "core.led", LEDrep);
 */
std::vector<std::string> reps { convert(sigs) }; 
 for(const auto& r : reps)
  std::cout << r << '\n';

 // Example of flattening to a single string:
 char *flattened = flatten(reps);
 std::cout << "FLATTENED: " << flattened << '\n';

 // Example of converting to an array of char*s:
 char **handle = convert(reps);	

 if(nullptr == handle)
  throw std::runtime_error("handle is a nullptr");

 std::for_each(handle, handle + length(handle), [](const char *s) { std::cout << s << '\n'; });

 /* ...or, you could do it this way (sad panda!):
 for(char **cursor = handle; nullptr != *cursor; ++cursor)
  printf("%s\n", *cursor);
 */

 release(handle); // remember to free the memory!
}

void from_rep_test()
{
 using std::cout;

 std::cout << "from_rep_test() doesn't do much yet :-)\n";
/*
 const char *LEDrep[] = { "state;b", "power;i", NULL };

 named_property_binding_vector npbv(LEDrep);

 cout << npbv << '\n'; 
*/
}

int main()
{
 using namespace OC::OCReflect;

 convert_test(); 

 call_test();

 rep_test();

 from_rep_test();
}
