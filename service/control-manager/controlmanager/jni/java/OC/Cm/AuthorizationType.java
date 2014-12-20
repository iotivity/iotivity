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

public enum AuthorizationType {
    UN_SET(0), AUTH_GRANT_TYPE_PASSWORD_CRED(1), AUTH_GRANT_TYPE_AUTH(2), AUTH_GRANT_TYPE_IMPLICIT(
            3), AUTH_GRANT_TYPE_EXTENDED_TOKEN(4);

    /**
     * Authorization type variable
     */
    private int value;

    /**
     * Constructor of AuthorizationType enum.
     * 
     * @param value
     *            Type of Authorization desired to set
     */
    private AuthorizationType(int value) {
        this.value = value;
    }

    /**
     * Default constructor of AuthorizationType enum.
     */
    private AuthorizationType() {
    }

    /**
     * Returns Authorization type value which represents enum itself.
     * 
     * @return Current Authorization type value
     */
    public int getValue() {
        return value;
    }

    /**
     * Returns AuthorizationType object which corresponds to the specific
     * Authorization type value.
     * 
     * @param value
     *            Authorization type value which represents specific
     *            Authorization enum
     * 
     * @return AuthorizationType object
     */
    public static AuthorizationType getAuthorizationType(int value) {
        for (AuthorizationType error : AuthorizationType.values()) {
            if (error.getValue() == value) {
                return error;
            }
        }

        return UN_SET;
    }
}
