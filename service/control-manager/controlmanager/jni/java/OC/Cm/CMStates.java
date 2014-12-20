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

public enum CMStates {
    CM_UNKNOWN(0), CM_STARTING(1), CM_STARTED(2), CM_STOPPING(3), CM_STOPPED(4), CM_FAILED(
            5), CM_INTERNAL_STOPPED(6);

    private int value;

    private CMStates(int value) {
        this.value = value;
    }

    private CMStates() {
    }

    public int getValue() {
        return value;
    }

    public static CMStates getCMState(int value) {
        for (CMStates error : CMStates.values()) {
            if (error.getValue() == value) {
                return error;
            }
        }

        return CM_UNKNOWN;
    }
}
