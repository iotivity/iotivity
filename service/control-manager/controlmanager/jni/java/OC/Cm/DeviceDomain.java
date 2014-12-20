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

public enum DeviceDomain {
    DeviceDomain_Local(0), // !< Represents Local domain
    DeviceDomain_Remote(1); // !< Represents Remote domain

    private int value;

    private DeviceDomain(int value) {
        this.value = value;
    }

    private DeviceDomain() {
    }

    public int getValue() {
        return value;
    }

    public static DeviceDomain getDeviceDomain(int value) {
        for (DeviceDomain error : DeviceDomain.values()) {
            if (error.getValue() == value) {
                return error;
            }
        }

        return DeviceDomain_Local;
    }
}
