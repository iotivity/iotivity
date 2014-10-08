/* Example program illustrating how to work with the ocicuc driver program: */

#include "exec.hpp"

#include <iostream>

namespace Intel { namespace OCDemo {

/* exec() is essentially main(), and is where the driver will start your
program after command-line options have been parsed: */
int exec(const boost::program_options::variables_map& vm)
{
 using std::cout;

 cout << "This is the start of my wonderful program!\n";

 cout << "My command-line options are:\n";

 for(const auto& o : vm)
  cout << o.first << " => " << o.second.as<std::string>() << '\n';

 return 0;
}

/* make_description() is your opportunity to describe your program's help screen and command
line parameter types. Refer to the boost::program_options library for details on how to
add different kinds of command-line options: */
auto make_description()
    -> boost::program_options::options_description
{
 namespace po = boost::program_options;     // because boost::program_options is a lot to type!

 po::options_description desc("My wonderful program's options! Run with \"--\" to simply use the defaults.");

 desc.add_options()
    ("param",   po::value<std::string>()->default_value("Hello, World!"),   "description of param")
    ;

 return desc;
}

}} // namespace Intel::OCDemo


