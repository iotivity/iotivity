//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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

#if defined(__linux__)
#include <unistd.h>
#endif

#include "RCSResourceContainer.h"
#include "oc_logger.hpp"
#include <iostream>

using namespace std;
using namespace OIC::Service;
using OC::oc_log_stream;

#define MAX_PATH 2048

/* Annother way to create a context: */
auto info_logger = []() -> boost::iostreams::stream<OC::oc_log_stream> &
{
  static OC::oc_log_stream ols(oc_make_ostream_logger);
  static boost::iostreams::stream<OC::oc_log_stream> os(ols);

  return os;
};

void getCurrentPath(std::string *pPath)
{
  char buffer[MAX_PATH];

#if defined(__linux__)
  char *strPath = NULL;
  int length = readlink("/proc/self/exe", buffer, MAX_PATH - 1);

  if (length != -1)
  {
    buffer[length] = '\0';
    strPath = strrchr(buffer, '/');

    if (strPath != NULL)
      *strPath = '\0';
  }
#endif
  pPath->append(buffer);
}

int main()
{
  std::string strConfigPath;
  getCurrentPath(&strConfigPath);
  strConfigPath.append("/examples/IASensorBundle/IAContainer/IAContainerConfig.xml");

  RCSResourceContainer *container = RCSResourceContainer::getInstance();
  container->startContainer(strConfigPath); 

  cout << "\nPress enter to stop container " << endl;
  getchar();
  container->stopContainer();

  cout << "Container stopped. Bye" << endl;
}
