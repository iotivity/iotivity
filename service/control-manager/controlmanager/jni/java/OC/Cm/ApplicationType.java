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
 * This enum contains list of Types of Applications Supported.
 */
public enum ApplicationType {
    ApplicationType_Controller(0), ApplicationType_Controllable(1), ApplicationType_Both(
            2);

    /**
     * Application type variable
     */
    private int value;

    /**
     * Constructor of ApplicationType enum.
     * 
     * @param value
     *            Type of Application desired to set
     */
    private ApplicationType(int value) {
        this.value = value;
    }

    /**
     * Default constructor of ApplicationType enum.
     */
    private ApplicationType() {
    }

    /**
     * Returns Application type value which represents enum itself.
     * 
     * @return Current Application type value
     */
    public int getValue() {
        return value;
    }

    /**
     * Returns ApplicationType object which corresponds to the specific
     * Application type value.
     * 
     * @param value
     *            Application type value which represents specific
     *            ApplicationType enum
     * 
     * @return ApplicationType object
     */
    public static ApplicationType getAppType(int value) {
        for (ApplicationType error : ApplicationType.values()) {
            if (error.getValue() == value) {
                return error;
            }
        }

        return ApplicationType_Controller;
    }

}
