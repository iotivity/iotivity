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

namespace OC { namespace OCReflect {

enum class property_type : uint8_t {
    nil			= 1,
    boolean,
    integer,
    rational,
    string,	
    list,

	INVALID
};

enum class property_attribute : uint8_t {
    r,
    w,
    rw
};

struct property_signature
{
 OC::OCReflect::property_type       type;
 OC::OCReflect::property_attribute  attribute;

 public:
 property_signature()
  : type(OC::OCReflect::property_type::nil),
    attribute(OC::OCReflect::property_attribute::rw)
 {}

 property_signature(const OC::OCReflect::property_type& type_)
  : type(type_),
    attribute(OC::OCReflect::property_attribute::rw)
 {}

 property_signature(const OC::OCReflect::property_type type_, const OC::OCReflect::property_attribute attribute_)
  : type(type_),
    attribute(attribute_)
 {}
};

typedef std::vector<property_type>							property_type_vector;

typedef std::vector<property_signature>                     property_signature_vector;

typedef std::vector<char>                                   property_data;

typedef std::tuple<std::string, property_signature>         named_property_binding;
typedef std::vector<named_property_binding>					named_property_binding_vector;

typedef std::tuple<named_property_binding, property_data>   property;
typedef std::tuple<property_signature, property_data>       tagged_property;         // eg. a return value

typedef std::vector<property>                               property_vector;

typedef std::function<property(property_vector)>            bound_function;

namespace detail {

// Great name, huh? Must be late in the day:
inline property_signature_vector typev2signaturev(const property_type_vector& ptv, const property_attribute pa = property_attribute::rw)
{
 property_signature_vector psv(ptv.size());

 std::transform(std::begin(ptv), std::end(ptv), std::back_inserter(psv),
                [&pa](const property_type& pt) -> property_signature { return property_signature(pt, pa); });

 return psv;
}

}

namespace detail {

// Helper for constructing sequences (like vector<property>) from a parameter pack:
template <typename SeqT, typename T>
SeqT make_seq(const T& x)
{
 return SeqT { x };
}

template <typename SeqT, typename T, typename ...TS>
SeqT make_seq(const T& x, const TS&... xs)
{
 SeqT s { x };

 auto vs = make_seq<SeqT>(xs...);

 s.insert(s.end(), std::make_move_iterator(vs.begin()), std::make_move_iterator(vs.end()));

 return s;
}

} // namespace OC::OCReflect::detail

}} // namespace OC::OCReflect

// Prettyprinters:
namespace OC { namespace OCReflect {

std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_type& pt);
std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_attribute& pa);
std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_signature& ps);
std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property& p);

std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_type_vector& pv);
std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_vector& p);

std::ostream& operator<<(std::ostream& os, const OC::OCReflect::method_binding& mb);

}} // namespace OC::OCReflect

/* JFW: I wonder if I can do this...
constexpr OC::OCReflect::property_type map_property_type(bool)
{
 return OC::OCReflect::property_type::boolean;
}
*/

// Map concerete to property type enumerations:
namespace OC { namespace OCReflect {

template <typename T>
struct map_property_type
{
 static const uint8_t value = static_cast<const uint8_t>(OC::OCReflect::property_type::INVALID);
};

template <>
struct map_property_type<bool>
{
 static const uint8_t value = static_cast<const uint8_t>(OC::OCReflect::property_type::boolean);
};

template <>
struct map_property_type<int64_t>
{
 static const uint8_t value = static_cast<const uint8_t>(OC::OCReflect::property_type::integer);
};

template <>
struct map_property_type<double>
{
 static const uint8_t value = static_cast<const uint8_t>(OC::OCReflect::property_type::rational);
};

template <>
struct map_property_type<std::string>
{
 static const uint8_t value = static_cast<const uint8_t>(OC::OCReflect::property_type::string);
};

template <>
struct map_property_type<const char *>
{
 static const uint8_t value = static_cast<const uint8_t>(OC::OCReflect::property_type::string);
};

}} // namespace OC::OCReflect

/*
// Sequence containers:
template <>
struct map_property_type< template <class T, class A> >
{
 static const uint8_t value = static_const<const uint8_t>(OC::OCReflect::property_type::list);
};

template <>
struct map_property_type<std::vector>
{
 static const uint8_t value = static_const<const uint8_t>(OC::OCReflect::property_type::list);
};

template <>
struct map_property_type<std::list>
{
 static const uint8_t value = static_const<const uint8_t>(OC::OCReflect::property_type::list);
};

template <>
struct map_property_type<std::forward_list>
{
 static const uint8_t value = static_const<const uint8_t>(OC::OCReflect::property_type::list);
};

template <>
struct map_property_type<std::deque>
{
 static const uint8_t value = static_const<const uint8_t>(OC::OCReflect::property_type::list);
};
*/

namespace OC { namespace OCReflect { namespace to_property {

OC::OCReflect::tagged_property convert(const bool& in);
OC::OCReflect::tagged_property convert(const int& in);
OC::OCReflect::tagged_property convert(const int64_t& in);
OC::OCReflect::tagged_property convert(const double& in);
OC::OCReflect::tagged_property convert(const std::string& in);

}}} // namespace OC::OCReflect::to_property

typedef std::tuple<OC::OCReflect::property_data::const_iterator, OC::OCReflect::property_data::const_iterator>  pd_iter_tuple;

pd_iter_tuple consume_typecheck(const OC::OCReflect::property_type expected_pt, const OC::OCReflect::property_data& in);

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

/*
template <template <class T, class AllocT> class SeqT>
static void convert(const pd_iter_pair& in, SeqT& out)
{
 std::copy(std::get<0>(in), std::get<1>(in), std::back_inserter(out));
}
*/

};

}}} // namespace OC::OCReflect::from_property

namespace OC { namespace OCReflect {

inline OC::OCReflect::named_property_binding make_named_property_binding(const std::string& name, const OC::OCReflect::property_type pt, const OC::OCReflect::property_attribute pa = OC::OCReflect::property_attribute::rw)
{
 return OC::OCReflect::named_property_binding { name, { pt, pa } };
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
/* JFW:
 static_assert(property_valid(InT, OC::OCReflect::property_type::INVALID),
               "invalid input type for make_property()");
*/

 OC::OCReflect::tagged_property         tp { OC::OCReflect::to_property::convert(in) };
 OC::OCReflect::named_property_binding npb { name, { std::get<0>(tp) } };

 OC::OCReflect::property_data& tp_data = std::get<1>(tp);

 // Encode the type, followed by the data:
 OC::OCReflect::property_data encoded_data;

 encoded_data.push_back(OC::OCReflect::map_property_type<InT>::value);
 std::copy(std::begin(tp_data), std::end(tp_data), std::back_inserter(encoded_data));

 return OC::OCReflect::property(npb, encoded_data);
}

/*
// Pretend you didn't see this, and I won't tell anyone where you hide the bodies:
template <class InT>
make_property(const InT& in, const std::string& name, OC::OCReflect::property& out)
{
 out = detail::make_property::apply(in, name, out);
}
*/

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

namespace OC { namespace OCReflect {

// Runtime dynamic cast from entities to concrete types:
template <class OutT>
OutT narrow(const entity& e)
{
 return OutT(); // placeholder
}

struct method_signature
{
 property_signature           ret_signature;
 property_signature_vector    param_signatures;

 public:
 method_signature(const property_type& return_type, const property_signature_vector& param_types_)
  : ret_signature(return_type, OC::OCReflect::property_attribute::r),
    param_signatures(param_types_)
 {}

 method_signature(const property_type& return_type, const property_type_vector& param_types_)
  : ret_signature(return_type, OC::OCReflect::property_attribute::r),
    param_signatures(detail::typev2signaturev(param_types_))
 {}

 template <typename ...ParameterT>
 method_signature(const property_type& return_type, const ParameterT& ...params_)
  : ret_signature(return_type, OC::OCReflect::property_attribute::r),
    param_signatures { detail::make_seq<property_signature_vector>(params_...) }
 {}
};

}} // namespace OC::OCReflect

namespace OC { namespace OCReflect {

struct method_binding
{
 friend std::ostream& operator<<(std::ostream& os, const OC::OCReflect::method_binding& mb);

 public:
 std::string            name;
 bound_function         f;
 method_signature       signature;

 private:
 static unsigned long long anon;

 public: 
 method_binding() 
  : name("__ANONYMOUS__"), 
    signature { property_type::nil, {{ property_type::nil, property_attribute::r }} }
 {}

 method_binding(bound_function& f,
                method_signature& signature_)
  : signature(signature_)
 {}

 method_binding(const std::string& name_, bound_function& f_, 
                const property_type& return_type_, const property_type_vector& param_types_)
  : name(name_),
    f(f_),
    signature { return_type_, param_types_ } 
 {}

 method_binding(const std::string& name_, bound_function& f_,
                const method_signature& signature_)
  : name(name_),
    f(f_),
   signature(signature_)
 {}
};

struct property_binding
{
 template <typename PropertyT>
 property_binding(PropertyT& property, const named_property_binding& pb)
 {
 }
};

}} // namespace OC::OCReflect

#endif
