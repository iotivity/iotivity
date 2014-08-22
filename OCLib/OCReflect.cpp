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


#include <string>
#include <vector>
#include <cstring>
#include <iterator>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include "OCReflect.h"

OC::OCReflect::entity OC::OCReflect::remote_resource::operator()(const std::string& name)
{
 return OC::OCReflect::entity();
}

namespace OC { namespace OCReflect { namespace OCStack {

// Free a char *:
void release(char *in)
{
 if(nullptr == in)
  return;

 free(in);
}

// Free a C array of char *:
void release(char **in)
{
 if(nullptr == in)
  return;

 for(char **cursor = in; nullptr != *cursor; ++cursor)
  release(*cursor);

 free(in);
}

char *strdup(const char *s)
{
 if(nullptr == s)
  return nullptr;

 char *ret = static_cast<char *>(malloc(1 + strlen(s)));

 if(nullptr == ret)
  return nullptr;

 return strcpy(ret, s);
}

char *strdup(const std::string& s)
{
 char *ret = static_cast<char *>(malloc(1 + s.length()));

 if(nullptr == ret)
  return nullptr;

 return strcpy(ret, s.c_str());
}

// Count the number of elements in a NULL-terminated C array of char*:
size_t length(char **in)
{
 if(nullptr == in)
  return 0;

 size_t ret = 0;

 for(char **cursor = in; nullptr != *cursor; ++cursor)
  ++ret;

 return ret;
}

// Note: caller is responsible for the returned memory:
char **convert(const std::vector<std::string>& vs)
{
 char **out;

 try
  {
 	out = (char **)malloc(vs.size()*sizeof(char *));

	if(nullptr == out)
	 throw;

	size_t i = 0;
	for(; vs.size() != i; ++i)
	 {
		out[i] = OCStack::strdup(vs[i]);

		if(nullptr == out[i])
		 throw;
	 }

	out[i] = nullptr;

	return out;
  }
 catch(...)
  {
	if(nullptr != out)
	 release(out);
  }

 return nullptr;
}

std::string convert(const OC::OCReflect::property_binding& npb)
{
 const std::string& name 					= std::get<0>(npb);
 const OC::OCReflect::property_signature ps	= std::get<1>(npb);

 std::ostringstream os;

 os << name << ':' << "oc.";

 using OC::OCReflect::property_type;

 switch(ps.type)
  {
	case property_type::nil:
	case property_type::rational:
	case property_type::list:
	case property_type::string:
                                    // Sashi commented this line to fix the build error
									//os << "convert(): request to convert valid, but unimplemented type: " << ps.type;
									throw std::runtime_error(os.str());
									break;

	case property_type::INVALID:
									throw std::runtime_error("convert(): request to convert invalid type");
									break;

	case property_type::boolean:	os << "bt." << 'b';
									break;

	case property_type::integer:	os << "bt." << 'i';
									break;
  }

 return os.str();
}

OC::OCReflect::property_type as_property_type(const std::string& pb_code)
{
 // We expect: "oc.bt.<char typecode>":
 const size_t code_length   = 7,
              code_pos      = 6;

 if(code_length != pb_code.length())
  return OC::OCReflect::property_type::INVALID;

 switch(pb_code[code_pos])
 {
    default:    break;

    case 'b':   return OC::OCReflect::property_type::boolean;
    case 'i':   return OC::OCReflect::property_type::integer;

/* These don't have codes yet:
	case property_type::nil:
	case property_type::rational:
	case property_type::string:
	case property_type::list:
*/
 }

 return OC::OCReflect::property_type::INVALID;
}

OC::OCReflect::property_binding as_property_binding(const std::string& pb_rep)
{
 auto delim_pos = pb_rep.find_first_of(':');

 if(pb_rep.length() == delim_pos)
  throw OC::reflection_exception("convert(): invalid property string (missing delimiter)");

 std::string pname { pb_rep.substr(0, delim_pos) };

 auto ptype = as_property_type(pb_rep.substr(1 + delim_pos));

 auto pattr = OC::OCReflect::property_attribute::rw;    // We aren't handling attributes right now...

 return OC::OCReflect::property_binding { pname, { ptype, pattr } };
}

OC::OCReflect::property_binding_vector as_property_binding_vector(const std::vector<std::string>& pb_reps)
{
 OC::OCReflect::property_binding_vector pbv;

 for(const auto& s : pb_reps)
  pbv.emplace_back(as_property_binding(s));

 return pbv;
}

std::vector<std::string> convert(const OC::OCReflect::property_binding_vector& psv)
{
 std::vector<std::string> out;

 for(const auto& ps : psv)
  out.push_back(convert(ps));

 return out;
}

char *flatten(const std::vector<std::string>& input, const std::string& delim)
try
{
 std::string out;

 for(size_t i = 0; input.size() != i; ++i)
  {
    out += input[i];

    if(i < input.size() - 1)
     out += delim;
  }

 return OC::OCReflect::OCStack::strdup(out);
}
catch(...)
{
 return nullptr;
}

// Note: expects output of flatten():
std::vector<std::string> expand(const char *flattened_string, const std::string& delim)
{
 if(nullptr == flattened_string)
  throw OC::reflection_exception("nullptr passed to expand()");

  std::vector<std::string> ret;
  std::string flattened = flattened_string;
  boost::split(ret, flattened, boost::is_any_of(delim));

  return ret;
}

}}} // namespace OC::OCReflect::OCStack
