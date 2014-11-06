
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>

#include <boost/program_options.hpp>

#include "exec.hpp"

namespace Intel { namespace OCDemo {

auto parse_options(boost::program_options::options_description& desc, int argc, char *argv[]) 
    -> boost::program_options::variables_map 
{
 namespace po = boost::program_options;

 po::positional_options_description popts;
 popts.add("uri", -1);

 po::variables_map vm;

 po::store(po::command_line_parser(argc,argv).options(desc).positional(popts).run(), 
           vm);

 po::notify(vm);

 return vm;
}

}} // namespace Intel::OCDemo

int main(int argc, char *argv[])
try
{
 using namespace std;
 
 auto desc = Intel::OCDemo::make_description();
 auto vm   = Intel::OCDemo::parse_options(desc, argc, argv);

 if(1 == argc || vm.count("help"))
  {
    std::cerr << desc << '\n';
    return 1;
  }

 return Intel::OCDemo::exec(vm);
}
catch(std::exception& e)
{
 std::cerr << "Unhandled exception: " << e.what() << '\n';
 return 1;
}
catch(...)
{
 std::cerr << "Unhandled exception.\n";
 return 1;
}
