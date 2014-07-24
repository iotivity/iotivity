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
#ifndef __INTEL_OCPROPERTIES_H_20140708
 #define __INTEL_OCPROPERTIES_H_20140708

#include <list>
#include <array>
#include <deque>
#include <vector>
#include <algorithm>
#include <functional>

#include <sstream>
#include <ostream>
#include <iostream>

#include <iterator>
#include <type_traits>

#include "OCException.h"
#include "OCPropertyTypes.h"

// Prettyprinters:
namespace OC { namespace OCReflect {

std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_type& pt);
std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_attribute& pa);
std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_signature& ps);
std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_binding& pb);
std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property& p);

std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_type_vector& pv);
std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_vector& p);

std::ostream& operator<<(std::ostream& os, const OC::OCReflect::method_binding& mb);

}} // namespace OC::OCReflect

// Convert from a concrete type to a property:
namespace OC { namespace OCReflect { namespace to_property {

OC::OCReflect::tagged_property convert(const bool& in);
OC::OCReflect::tagged_property convert(const int& in);
OC::OCReflect::tagged_property convert(const int64_t& in);
OC::OCReflect::tagged_property convert(const double& in);
OC::OCReflect::tagged_property convert(const std::string& in);

}}} // namespace OC::OCReflect::to_property

// Convert from a property to a concrete type:
namespace OC { namespace OCReflect { namespace from_property {

typedef OC::OCReflect::property_data::const_iterator pdci;
typedef std::tuple<pdci, pdci>  pd_iter_pair;

/* We need to use a struct to get around the fact that we can't partially specialize against a
free function's signature-- nothing else special happening here: */
struct conversion {

static void convert(const pd_iter_pair& in, bool& out)
{
 out = static_cast<bool>(static_cast<int>(*(std::get<0>(in))));
}

static void convert(const pd_iter_pair& in, int64_t& out)
{
 std::copy(std::get<0>(in), std::get<1>(in), reinterpret_cast<char *>(&out));
}

static void convert(const pd_iter_pair& in, double& out)
{
 std::copy(std::get<0>(in), std::get<1>(in), reinterpret_cast<char *>(&out));
}

static void convert(const pd_iter_pair& in, std::string& out)
{
 out.assign(std::get<0>(in), std::get<1>(in));
}

};

}}} // namespace OC::OCReflect::from_property

// Run-time typechecking:
namespace OC { namespace OCReflect {

typedef std::tuple<OC::OCReflect::property_data::const_iterator, OC::OCReflect::property_data::const_iterator>  pd_iter_tuple;
pd_iter_tuple consume_typecheck(const OC::OCReflect::property_type expected_pt, const OC::OCReflect::property_data& in);

}} // namespace OC::OCReflect

// Property binding factory:
namespace OC { namespace OCReflect {

inline OC::OCReflect::property_binding make_property_binding(const std::string& name, const OC::OCReflect::property_signature ps)
{
 return OC::OCReflect::property_binding { name, ps };
}

inline OC::OCReflect::property_binding make_property_binding(const std::string& name, const OC::OCReflect::property_type pt, const OC::OCReflect::property_attribute pa = OC::OCReflect::property_attribute::rw)
{
 return OC::OCReflect::property_binding { name, { pt, pa } };
}

}} // namespace OC::OCReflect

// ...probably not-so-efficent, but we want it "working" for now:
namespace OC { namespace OCReflect {

// ...a class just to facilitate the usual ADL trick:
struct make_property {

// ...work around array->bool conversion:
static OC::OCReflect::property apply(const char *in, const std::string& name)
{
 return apply(std::string(in), name);
}

template <class InT>
static OC::OCReflect::property apply(const InT& in, const std::string& name)
{
 OC::OCReflect::tagged_property  tp     { OC::OCReflect::to_property::convert(in) };
 OC::OCReflect::property_binding npb    { OC::OCReflect::make_property_binding(name, std::get<0>(tp)) }; 

 OC::OCReflect::property_data& tp_data = std::get<1>(tp);

 // Encode the type, followed by the data:
 OC::OCReflect::property_data encoded_data;

 encoded_data.push_back(OC::OCReflect::map_property_type<InT>::value);
 std::copy(std::begin(tp_data), std::end(tp_data), std::back_inserter(encoded_data));

 return OC::OCReflect::property(npb, encoded_data);
}

}; // struct make_property

}} // namespace OC::OCReflect

namespace OC { namespace OCReflect {

template <class OutT>
OutT concretize(const OC::OCReflect::property_data& pd)
{
 OutT ret;

 OC::OCReflect::from_property::conversion::convert(
	consume_typecheck(
		static_cast<OC::OCReflect::property_type>(
			OC::OCReflect::map_property_type<OutT>::value), pd), ret);

 return ret;
}

template <class OutT>
OutT concretize(const OC::OCReflect::tagged_property& tp)
{
 return concretize<OutT>(std::get<1>(tp));
}

template <class OutT>
OutT concretize(const OC::OCReflect::property& p)
{
 return concretize<OutT>(std::get<1>(p));
}

}} // namespace OC::OCReflect

/* JFW: We are using concretize<> to fill this niche-- candidate for removal: 
namespace OC { namespace OCReflect {

// Runtime dynamic cast from entities to concrete types:
template <class OutT>
OutT narrow(const entity& e)
{
 return OutT(); // placeholder
}

}} // namespace OC::OCReflect
*/

#endif
