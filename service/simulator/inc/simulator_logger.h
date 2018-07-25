/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

/**
 * @file   simulator_logger.h
 *
 * @brief   This file provides the interface for logging messages to different targets (console/file).
 */

#ifndef SIMULATOR_LOGGER_H_
#define SIMULATOR_LOGGER_H_

#include <iostream>
#include <memory>

/**
 * @class ILogger
 */
class ILogger
{
    public:
        enum Level
        {
            INFO = 0,    /**< information level log message */
            DEBUG,       /**< debug level log message */
            WARNING,     /**< warning level log message */
            ERROR        /**< error level log message */
        };

        /**
         * get the string value according to the level value
         * @param[in] level    log level value
         *
         * @return string message
         */
        static const char *getString(Level level)
        {
            switch (level)
            {
                case Level::INFO: return "INFO";
                case Level::DEBUG: return "DEBUG";
                case Level::WARNING: return "WARNING";
                case Level::ERROR: return "ERROR";
                default: return "UNKNOWN";
            }
        }

        virtual void write(std::string, Level, std::string) = 0;
};

/**
 * @class Logger
 */
class Logger
{
    public:
        /**
         * set the log target to console
         * @return true or false
         */
        bool setDefaultConsoleTarget();
        /**
         * set the log target to file
         * @param[in] path    file path
         * @return true or false
         */
        bool setDefaultFileTarget(const std::string &path);
        /**
         * set the log to custom target
         * @param[in] target    log target
         */
        void setCustomTarget(const std::shared_ptr<ILogger> &target);
        /**
         * write the log to given target
         * @param[in] level   type of log
         * @param[in] str     oc string stream
         */
        void write(ILogger::Level level, std::ostringstream &str);

    private:
        std::shared_ptr<ILogger> m_target;
};

auto simLogger() -> Logger &;

#ifndef SIM_LOG
#define SIM_LOG(LEVEL, MSG) { \
        simLogger().write(LEVEL, static_cast<std::ostringstream&>(std::ostringstream()<<MSG)); \
    }
#endif

#endif
