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
 * @file        ControlManagerListener.h
 * @brief       This file includes all class methods and class members related to ControlManagerListener class.
 * @author      Auto-Generated
 */

#ifndef __ControlManagerListener__
#define __ControlManagerListener__

#include <iostream>
#include "IControlManagerListener.h"
#include "MyDevice.h"

/**
 * @class   ControlManagerListener
 * @brief   This class implements IControlManagerListener, this class is used for receiving notifications
 *          from Control Manager Framework and providing configuration data to Control Manager Framework.
 */
class ControlManagerListener: public OC::Cm::IControlManagerListener
{
    public:
        /**
         * Default constructor of ControlManagerListener
         */
        ControlManagerListener();
        /**
         * Default destructor of ControlManagerListener
         */
        virtual ~ControlManagerListener();

        /**
         * This method will be invoked by framework when framework is started. Framework passes IP-Address of
         * the server on which its been started. This callback indicates to the application that framework started
         * successfully and framework server listening on the IP address which has come along with this callback.
         *
         * @param[in]   address     IP address of the server
         */
        void onStarted(std::string address);

        /**
         * This method will be invoked by Control Manager-Framework once framework is stopped completely.
         */
        void onStopped();

        /**
         * This method will be invoked when framework encounters an error
         *
         * @param[in]   error   Error defined by Control Manager framework
         */
        void onError(OC::Cm::CMError error);


        int m_cmNotification;   /**< Represents Control Manager notification which has received by easySetupNotification() callback. */
};

#endif /* __ControlManagerListener__ */
