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

package OC.Cm;

/**
 * This enum defines list of SHP Log Levels supported by SHP-Framework
 */
public enum CMLogType {
    CM_LOG_TYPE_SILENT(-3), // SILENT Level Logging - This disbles complete
                            // SHP-Logging itself
    CM_LOG_TYPE_ERROR(-2), // ERROR Level Logging - Which is default type of
                           // SHP-Logging
    CM_LOG_TYPE_WARN(-1), // WARNING Level Logging
    CM_LOG_TYPE_INFO(0), // INFO Level Logging
    CM_LOG_TYPE_DEBUG(1); // DEBUG Level Logging

    /**
     * Timer type variable
     */
    private int value;

    /**
     * Constructor of SHPLogType enum.
     * 
     * @param value
     *            Level of SHP-Log desired to be set
     */
    private CMLogType(int value) {
        this.value = value;
    }

    /**
     * Default constructor of SHPLogType enum.
     */
    private CMLogType() {
    }

    /**
     * Returns Timer type value which represents enum itself.
     * 
     * @return Current SHPLogType value
     */
    public int getValue() {
        return value;
    }
}
