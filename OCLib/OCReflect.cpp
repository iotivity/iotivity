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

#include <string>
#include <cstring>

#include "OCReflect.h"

OC::OCReflect::entity OC::OCReflect::remote_resource::operator()(const std::string& name)
{
 return OC::OCReflect::entity();
}

namespace OC { namespace OCReflect { namespace to_OCStack {

// Free a C array of char *:
void release(char **in)
{
 if(nullptr == in)
  return;

 for(char **cursor = in; nullptr != *cursor; ++cursor)
  {
    if(nullptr != *cursor)
     free(*cursor);
  }

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
		out[i] = to_OCStack::strdup(vs[i]);

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

std::string convert(const OC::OCReflect::named_property_binding& npb)
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
	case property_type::string:		
	case property_type::list:	
	case property_type::INVALID:	
									throw std::runtime_error("not implemented"); 
									break;

	case property_type::boolean:	os << "bt." << 'b';	
									break;

	case property_type::integer:	os << "bt." << 'i';	
									break;
  }

 return os.str();
}

std::vector<std::string> convert(const OC::OCReflect::named_property_binding_vector& psv)
{
 std::vector<std::string> out;

 for(const auto& ps : psv)
  out.emplace_back(convert(ps));

 return out;
}

char *flatten(const std::vector<std::string>& input, const std::string& delim)
{
 std::string out;

 for(size_t i = 0; input.size() != i; ++i)
  {
    out += input[i];

    if(i < input.size() - 1)
     out += ";";
  }

 return OC::OCReflect::to_OCStack::strdup(out);
}

}}} // namespace OC::OCReflect::to_OCStack
