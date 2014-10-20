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

#include "OCReflect.h"
#include "OCProperties.h"

using OC::OCReflect::property_type;

// Prettyprinters:
namespace OC { namespace OCReflect {

std::ostream& operator<<(std::ostream& os, const property_type& pt)
{
 using ocpt = property_type;

 switch(pt)
 {
    case ocpt::nil:         os << "nil";        break;
    case ocpt::boolean:     os << "boolean";    break;
    case ocpt::integer:     os << "integer";    break;
    case ocpt::rational:    os << "rational";   break;
    case ocpt::string:      os << "string";     break;
    case ocpt::list:        os << "list";       break;

	case ocpt::INVALID:		os << "INVALID";	break;
 }

 return os;
}

std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_attribute& pa)
{
 using ocpa = OC::OCReflect::property_attribute;

 switch(pa)
 {
    case ocpa::r:   os << 'r';      break;
    case ocpa::w:   os << 'w';      break;
    case ocpa::rw:  os << "rw";     break;
 }

 return os;
}

std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_signature& ps)
{
 os << '(' << ps.type << ':' << ps.attribute << ')';
 return os;
}

std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_binding& pb)
{
 os << "property binding \"" << std::get<0>(pb) << "\": " << std::get<1>(pb);
 return os;
}

std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property& p)
{
 using std::get;

 const auto& npb = get<0>(p);
  const auto& npb_name = get<0>(npb);
  const auto& npb_sig  = get<1>(npb);

 const auto& pd  = std::get<1>(p);

 os << "property \"" << npb_name << "\": " << npb_sig << "\"\n";

 os << '\"' << npb_name << ' ' << npb_sig;

 os << "\nDATA {\n";

 for(const unsigned char c : pd)
  {
    os << static_cast<int>(c);

    if(std::isprint(c))
     os << ' ' << c;

    os << '\n';
  }

 os << "}\n";

 return os;
}

std::ostream& operator<<(std::ostream& os, const property_type_vector& ptv)
{
 for(const auto& pt : ptv)
  os << pt << "; ";

 return os;
}

std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_vector& pv)
{
 for(const auto& p : pv)
  os << p << "; ";

 return os;
}

std::ostream& operator<<(std::ostream& os, const OC::OCReflect::property_signature_vector& psv)
{
 for(const auto& ps : psv)
  os << '[' << ps.attribute << ']' << ps.type << "; ";

 return os;
}

std::ostream& operator<<(std::ostream& os, const OC::OCReflect::method_signature& ms)
{
 os << ms.param_signatures << " -> " << ms.ret_signature;
 return os;
}

std::ostream& operator<<(std::ostream& os, const OC::OCReflect::method_binding& mb)
{
 os << mb.name << " :: " << mb.signature;
 return os;
}

}} // namespace OC::OCReflect

namespace OC { namespace OCReflect { namespace to_property { namespace detail {

// Convert a memory representation to bytes:
template <class InT>
OC::OCReflect::property_data static_rep(const InT& in)
{
 return OC::OCReflect::property_data(reinterpret_cast<const char *>(&in),
                                     reinterpret_cast<const char *>(&in) + sizeof(in));
}

// Apply a typetag to a representation:
template <class RepT>
OC::OCReflect::tagged_property tag_rep(const property_type pt, const RepT& rep)
{
 return OC::OCReflect::tagged_property({ pt }, { rep });
}

template <class SeqIterT>
OC::OCReflect::tagged_property tag_rep(const property_type pt, const SeqIterT& begin, const SeqIterT& end)
{
 return OC::OCReflect::tagged_property({ pt },
                                       { begin, end });
}

}}}} // namespace OC::OCReflect::to_property::detail

// Convert concrete types into tagged properties (a representation with a typetag):
namespace OC { namespace OCReflect { namespace to_property { 

OC::OCReflect::tagged_property convert(const bool& in)
{
 return detail::tag_rep(property_type::boolean, static_cast<char>(in));
}

OC::OCReflect::tagged_property convert(const int64_t& in)
{
 return detail::tag_rep(property_type::integer, detail::static_rep(in));
}

// Convenience sugar for default platform int:
OC::OCReflect::tagged_property convert(const int& in)
{
 static_assert(sizeof(int) <= sizeof(int64_t), 
               "conversion to int64_t may be dangerous on your platform: use int64_t explicitly");

 return convert(static_cast<int64_t>(in));
}

OC::OCReflect::tagged_property convert(const double& in)
{
 return detail::tag_rep(property_type::rational, detail::static_rep(in));
}

OC::OCReflect::tagged_property convert(const std::string& in)
{
 return detail::tag_rep(property_type::string, 
                        in.begin(), in.end());
}

}}} // namespace OC::OCReflect::to_property

namespace OC { namespace OCReflect {

pd_iter_tuple consume_typecheck(const property_type expected_pt, const OC::OCReflect::property_data& in) 
{
 OC::OCReflect::property_data::const_iterator begin = in.begin();

 auto found_pt = static_cast<property_type>(*begin);

 std::cout << "val ept=" << (int)expected_pt << ", ept: " << (int)found_pt << '\n';

 if(expected_pt != found_pt)
  {
    std::ostringstream os;
    os << "type error: " << "expected " << expected_pt << ", found " << found_pt;
    throw OC::reflection_exception(os.str());
  }

 return std::forward_as_tuple(++begin, in.end());
}

}} // namespace OC::OCReflect


