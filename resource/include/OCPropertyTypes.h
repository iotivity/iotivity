
#ifndef __INTEL_OCPROPERTY_TYPES_H_20140723
 #define __INTEL_OCPROPERTY_TYPES_H_20140723

#include <tuple>
#include <vector>
#include <algorithm>
#include <functional>

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

typedef std::tuple<std::string, property_signature>         property_binding;        // ie. name + signature
typedef std::vector<property_binding>					    property_binding_vector;

typedef std::tuple<property_binding, property_data>         property;
typedef std::tuple<property_signature, property_data>       tagged_property;         // eg. a return value

typedef std::vector<property>                               property_vector;

typedef std::function<property(property_vector)>            bound_function;


}} // namespace OC::OCReflect

namespace OC { namespace OCReflect {

namespace detail {

// zip a property type vector against an attribute:
inline property_signature_vector typev2signaturev(const property_type_vector& ptv, const property_attribute pa = property_attribute::rw)
{
 property_signature_vector psv(ptv.size());

 std::transform(std::begin(ptv), std::end(ptv), std::back_inserter(psv),
                [&pa](const property_type& pt) -> property_signature { return property_signature(pt, pa); });

 return psv;
}

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

}} // namespace OC::OCReflect


// Map concerete to property type enumerations (used for returning runtime types from compile time types):
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

}} // namespace OC::OCReflect

#endif
