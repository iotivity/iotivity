/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 ******************************************************************/

package org.iotivity.test.rfw.common.devicecontroller.android;

import com.android.ddmlib.IShellOutputReceiver;

/**
 * Shell Output Receiver that captures shell output into a String for later
 * retrieval.
 */
class CommandOutputCapture implements IShellOutputReceiver {
    private volatile StringBuffer builder = new StringBuffer();

    @Override
    public boolean isCancelled() {
        return false;
    }

    @Override
    public void addOutput(byte[] data, int offset, int length) {
        String message = new String(data, offset, length);
        builder.append(message);
        // System.out.println(message);
    }

    @Override
    public String toString() {
        return builder.toString();
    }

    @Override
    public void flush() {
    }

}
