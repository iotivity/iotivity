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

package OC.Cm.Connector;

public class HeadersList {

    private long mNativeHeaderList;

    private native boolean setHeader(long headersList, String name, String value);

    private native String getHeader(long headersList, String name);

    private native String[] getHeaderNameList(long headersList);

    public HeadersList(long nativeHeaderList) {
        mNativeHeaderList = nativeHeaderList;
    }

    public boolean setHeader(String name, String value) {
        return this.setHeader(mNativeHeaderList, name, value);
    }

    public String getHeader(String name) {
        return this.getHeader(mNativeHeaderList, name);
    }

    public String[] getHeaderNameList() {
        return this.getHeaderNameList(mNativeHeaderList);
    }
}