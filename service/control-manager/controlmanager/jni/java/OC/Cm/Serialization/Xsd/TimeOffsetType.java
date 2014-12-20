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

package OC.Cm.Serialization.Xsd;

public class TimeOffsetType {

    public String value;

    public boolean validateContent() {
        this.value = this.value.trim();

        if (value.length() != 6) {
            return false;
        }

        for (int index = 0; index < value.length(); index++) {
            if (index == 0) {
                if (value.charAt(index) != '+' && value.charAt(index) != '-') {
                    return false;
                }
            } else if (index == 3) {
                if (value.charAt(index) != ':') {
                    return false;
                }
            } else {
                if (Character.isDigit(value.charAt(index)) == false) {
                    return false;
                }
            }
        }

        return true;
    }
}