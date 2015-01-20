/*
 * //******************************************************************
 * //
 * // Copyright 2015 Intel Corporation.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base;

import java.util.HashMap;
import java.util.Map;

public class OcException extends Exception {

    private static final Map<String, String> errorCodes = new HashMap<String, String>();

    private static void addCode(String error, String message) {
        errorCodes.put(error, error + ": " + message);
    }

    static {
        /* Success status code - START HERE */
        addCode("OK", "");
        addCode("RESOURCE_CREATED", "");
        addCode("RESOURCE_DELETED", "");
        addCode("CONTINUE", "");
        /* Success status code - END HERE */
        /* Error status code - START HERE */
        addCode("INVALID_URI", "");
        addCode("INVALID_QUERY", "");
        addCode("INVALID_IP", "");
        addCode("INVALID_PORT", "");
        addCode("INVALID_CALLBACK", "");
        addCode("INVALID_METHOD", "");
        addCode("INVALID_PARAM", "");
        addCode("INVALID_OBSERVE_PARAM", "");
        addCode("NO_MEMORY", "");
        addCode("COMM_ERROR", "");
        addCode("NOTIMPL", "");
        addCode("NO_RESOURCE", "Resource not found");
        addCode("RESOURCE_ERROR", "Not supported method or interface");
        addCode("SLOW_RESOURCE", "");
        addCode("NO_OBSERVERS", "Resource has no registered observers");
        addCode("OBSERVER_NOT_FOUND", "");
        addCode("PRESENCE_STOPPED", "");
        addCode("PRESENCE_TIMEOUT", "");
        addCode("PRESENCE_DO_NOT_HANDLE", "");
        addCode("VIRTUAL_DO_NOT_HANDLE", "");
        addCode("INVALID_OPTION", "");
        addCode("MALFORMED_RESPONSE", "Remote reply contained malformed data");
        addCode("PERSISTENT_BUFFER_REQUIRED", "");
        addCode("INVALID_REQUEST_HANDLE", "");
        addCode("INVALID_DEVICE_INFO", "");
        addCode("ERROR", "Generic error");

        addCode("JNI_EXCEPTION", "Generic Java binder error");
        addCode("JNI_NO_NATIVE_OBJECT", "");
        addCode("JNI_INVALID_VALUE", "");
    }

    private OcException(String error, String errMessage) {
        super(errMessage + " (" + error + ": " + errorCodes.get(error) + ")");
    }

    private static void addStackTrace(Throwable throwable,
                                      String file,
                                      String functionName,
                                      int line) {
        StackTraceElement[] stack = throwable.getStackTrace();
        StackTraceElement[] newStack = new StackTraceElement[stack.length + 1];

        System.arraycopy(stack, 0, newStack, 1, stack.length);
        newStack[0] = new StackTraceElement("<native>", functionName, file, line);
        throwable.setStackTrace(newStack);
    }

    private void setNativeExceptionLocation(String file, String functionName, int line) {
        OcException.addStackTrace(this, file, functionName, line);
    }
}
