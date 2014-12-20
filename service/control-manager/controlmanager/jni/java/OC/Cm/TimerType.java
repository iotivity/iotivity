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
 * This enum contains list of Types of Timers Supported by framework.
 */
public enum TimerType {

    EASYSETUP_TIMER(0), DEVICE_TOKEN_WAITING_TIMER(1);

    /**
     * Timer type variable
     */
    private int value;

    /**
     * Constructor of TimerType enum.
     * 
     * @param value
     *            Type of Timer desired to control
     */
    private TimerType(int value) {
        this.value = value;
    }

    /**
     * Default constructor of TimerType enum.
     */
    private TimerType() {
    }

    /**
     * Returns Timer type value which represents enum itself.
     * 
     * @return Current Timer type value
     */
    public int getValue() {
        return value;
    }
}
