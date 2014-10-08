#ifndef _SIMPLE_PARSE_H_
#define _SIMPLE_PARSE_H_

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>

using namespace std;

bool readConfigFile(const char *cfgfilepath, const string &key, string &value);
vector<string> NormalizedOut(string &input);

#endif


