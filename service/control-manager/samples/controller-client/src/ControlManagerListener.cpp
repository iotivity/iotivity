/**
 * @copyright
 * This program is subject to copyright protection in accordance with the
 * applicable law. It must not, except where allowed by law, by any means or
 * in any form be reproduced, distributed or lent. Moreover, no part of the
 * program may be used, viewed, printed, disassembled or otherwise interfered
 * with in any form, except where allowed by law, without the express written
 * consent of the copyright holder.@n
 * Copyright (C) 2014 Samsung Electronics Co., Ltd. All Rights Reserved.
 *
 * @file        ControlManagerListener.cpp
 * @brief       This file includes implementation of all class methods and class members which are declared in ControlManagerListener.h
 * @author      Auto-Generated
 */

#include "ControlManagerListener.h"
#include "ProgramUtils.h"

ControlManagerListener::ControlManagerListener()
{
    m_cmNotification = 0;
}

ControlManagerListener::~ControlManagerListener()
{

}


void
ControlManagerListener::onStarted(std::string address)
{
    std::cout << "ControlManagerListener::onStarted(): => " <<
              "Control Manager Framework started with IP-Address " << address << std::endl;

    /**
     * TODO:    Application developers can extend this method for doing any specific action upon Start of Control Manager-Framework
     */
}

void
ControlManagerListener::onStopped()
{
    std::cout << "ControlManagerListener::onStopped(): => " <<
              "Control Manager Framework stopped successfully" << std::endl;

    /**
     * TODO:    Application developers can extend this method for doing any specific action upon Stop of Control Manager-Framework.<br><br>For example, cleanup operations.
     */
}

void
ControlManagerListener::onError(OC::Cm::CMError error)
{
    std::cout << "ControlManagerListener::onError(): => " << "ERROR: Received Error from Framework" <<
              std::endl;

    /**
     * TODO:    Application developers can extend this method for doing any specific action upon receiving an Error from Control Manager-Framework
     */
}

