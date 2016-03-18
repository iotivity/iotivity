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

package oic.ctt.network.control;

import oic.ctt.network.control.SecurityConstants.ArgumentType;

/**
 * The MessageArgument class. The arguments contained by the ControlMessage
 * object are of this type.
 *
 * @author Tanvir Ferdousi (tanvir.f@samsung.com)
 *
 */
public class MessageArgument {

    private ArgumentType mType;
    private int          mLength;
    private byte[]       mValue;

    /**
     * Gets the argument type
     *
     * @return Object of enum type ArgumentType
     */
    public ArgumentType getType() {
        return mType;
    }

    /**
     * Sets the argument type
     *
     * @param type
     *            Enum of type ArgumentType
     */
    public void setType(ArgumentType type) {
        this.mType = type;
    }

    /**
     * Gets the size of the argument value
     *
     * @return Integer depicting the size in bytes
     */
    public int getLength() {
        return mLength;
    }

    /**
     * Gets the argument value
     *
     * @return Array of byte
     */
    public byte[] getValue() {
        return mValue;
    }

    /**
     * Sets the argument value
     *
     * @param Array
     *            of byte
     */
    public void setValue(byte[] value) {
        this.mValue = value;
        this.mLength = value.length;
    }
}