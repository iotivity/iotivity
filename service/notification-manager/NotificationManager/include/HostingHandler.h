//******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
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

#ifndef HOSTINGHANDLER_H_
#define HOSTINGHANDLER_H_

#include "NotificationManager.h"

class HostingHandler
{
    public:

        static void initialize();
        static void initialize(HostingConfig cfg);
        static HostingHandler *getInstance();

        void setHostingConfig(HostingConfig cfg);

        void changeHostingMode(HostingMode hostingMode, AutomaticMethod autoMethod = AutomaticMethod::None);
        void changeAutomaticHostingMethod(AutomaticMethod autoMethod);
        void changeNotifiyMethod(NotifyMethod notifyMethod);
        void changeNotifyFrequencyType(NotifyFrequency notifyFrequency);

    private:

        HostingHandler();
        ~HostingHandler();

        static HostingHandler *s_instance;
        static mutex s_mutexForCreation;

        HostingConfig hostingCfg;

        static std::function< void(bool isHosting) > s_findHostingCandidate;
        static std::function< void(std::string) > s_addExtraStr;
        static std::function< void(std::shared_ptr< OCResource > resource) > s_startHosting;
        static std::function< void(OCResourceHandle resourceHandle) > s_notify;

        void startFindHost();
        void onObserve(AttributeMap &AttMap, OCResourceHandle resourceHandle);
        void onFoundCandidate(std::shared_ptr< OCResource > resource);

        void runAutomaticHosting(AutomaticMethod autoMethod);
        void stopAutomaticHosting();

        void notifyFrequence(OCResourceHandle resourceHandle);

};

#endif /* HOSTINGHANDLER_H_ */
