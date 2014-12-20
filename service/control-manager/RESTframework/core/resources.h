/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef WEBSERVICE_RESOURCES_H_
#define WEBSERVICE_RESOURCES_H_

namespace webservice
{

    class Resources
    {
        public:
            static std::string GetSupportableVersionMin(void);
            static std::string GetSupportableVersionMax(void);
            static void RegisterResources(void);

        private:
            /**
              * This is only for SHP spec.
              * This function will be created by python script using SHP.xsd file.
              * So, You can not modify anything.
              * If you need adding any URIs, you can use it to UserDefinedResources function.
              */
            static void RegisterSHPResources(void);

            /**
              * This is only for NonSHP spec.
              * This function will be created by python script using NonSHP.xsd file.
              * So, You can not modify anything.
              * If you need adding any URIs, you can use it to UserDefinedResources function.
              */
            static void RegisterNonSHPResources(void);

            /**
              * This is only for 3rd party or user defined resource.
              * So, you can use this function to define anything.
              */
            static void RegisterUserDefinedResources(void);
    };
}

#endif /* WEBSERVICE_RESOURCES_H_ */

