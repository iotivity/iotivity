//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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
package oic.ctt;

import oic.ctt.formatter.IotivityKey;

/**
 * Enumeration for Storing DUT Keys
 *
 */
public enum DutKey {

    ROOT(IotivityKey.ROOT.toString()), DI(IotivityKey.DI.toString()), HREF(
            IotivityKey.HREF.toString()), DIS(IotivityKey.DIS.toString()), OBS(
            IotivityKey.OBS.toString()), RT(IotivityKey.RT.toString()), IF(
            IotivityKey.IF.toString()), REP(IotivityKey.REP.toString()), DUT_TYPE(
            "dutType");

    private String key;

    private DutKey(String key) {
        this.key = key;
    }

    @Override
    public String toString() {
        return key;
    }
}
