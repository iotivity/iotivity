#include <chrono>
#include <string>
#include <iostream>
#include <algorithm>

#include "utility.hpp"

namespace Intel { namespace OCDemo {

int observe_count()
{
 static unsigned long long oc = 0;
 return ++oc;
}

}} // namespace Intel::OCDemo

// Helper for measuring call times:
namespace Intel { namespace OCDemo {

using std::cout;
using namespace std::chrono;

call_times call_timer;

void call_times::reset(const std::string& entry) 
{ 
 timings[entry] = make_pair(high_resolution_clock::now(), std::chrono::high_resolution_clock::time_point()); 
}

void call_times::mark(const std::string& name)
{
 auto e = timings.find(name);

    if(timings.end() == e)
     {
        reset(name);
        return;
     }

    auto& tp = (*e).second;

    if(tp.first > tp.second)
     timings[name].second = high_resolution_clock::now();
  }

void call_times::report()
{
 cout << "Time marks:\n";

 for_each(begin(timings), end(timings), 
 [](const std::pair<std::string, clock_interval>& tm) -> void 
 {
    const std::string& name     { tm.first };

    const time_point<high_resolution_clock>& request_time    { tm.second.first };
    const time_point<high_resolution_clock>& response_time   { tm.second.second };

    cout << '\t' << name << ": ";

    if(request_time > response_time)
     {
        cout << "<waiting>\n";
        return;
     }

    auto elapsed_ms = duration_cast<std::chrono::milliseconds>(response_time - request_time).count();
    cout << elapsed_ms << "ms (";

    auto elapsed_us = duration_cast<std::chrono::microseconds>(response_time - request_time).count();
    cout << elapsed_us << "us)\n";
 });
}

void call_times::report_and_reset(const std::string& name)
{
 mark(name), report(), reset(name);
}


}} // namespace Intel::OCDemo
