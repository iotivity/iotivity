/*
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Copyright 2018 Intel Corporation All Rights Reserved.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

#ifndef EXAMPLE_OCF_LIGHT_DIMMINGLIGHTCONTROL_H_
#define EXAMPLE_OCF_LIGHT_DIMMINGLIGHTCONTROL_H_
#include "DimmingLightServer.h"

/**
 * Used to controlling the Dimming light sample locally via command line.
 * This is a friend class to the DimmingLightServer.
 */
class DimmingLightControl {
public:
    DimmingLightControl(DimmingLightServer& dimmingLightServer);
    virtual ~DimmingLightControl(void);

    /**
     * Continually process commandline commands that can be used to control the
     * dimming light locally.
     *
     * Calling this function will block till user provides a commandline command
     * to exit.
     */
    void process(void);
private:
    /**
     * Print the values of the binary switch server and the dimming server
     */
    void info(void);

    /**
     * Print a help message to inform the user of valid commands.
     */
    void help(void);
    bool m_quit;
    DimmingLightServer& m_dimmingLightServer;
};

#endif /* EXAMPLE_OCF_LIGHT_DIMMINGLIGHTCONTROL_H_ */
