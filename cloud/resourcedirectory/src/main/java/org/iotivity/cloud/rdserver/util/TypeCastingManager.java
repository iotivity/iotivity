/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
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
package org.iotivity.cloud.rdserver.util;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Iterator;

import org.iotivity.cloud.rdserver.Constants;

public class TypeCastingManager<T> {

    public TypeCastingManager() {

    }

    public void callMethod(String methodName, T objClass) {
        try {
            Method method = objClass.getClass().getDeclaredMethod(methodName);
            method.invoke(objClass);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public HashMap<Object, Object> convertObjectToMap(T objClass) {

        HashMap<Object, Object> map = new HashMap<Object, Object>();

        try {
            Field[] fieldList = objClass.getClass().getDeclaredFields();

            for (Field field : fieldList) {
                field.setAccessible(true);
                Object value = field.get(objClass);
                if (value != null) {
                    String fieldName = field.getName();
                    if (fieldName.equals("itf")) {
                        fieldName = Constants.INTERFACE;
                    }
                    map.put(fieldName, value);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return map;
    }

    public T convertMaptoObject(HashMap<Object, Object> map, T objClass) {

        String keyAttribute = null;
        String methodName = null;
        Iterator<Object> iter = map.keySet().iterator();
        String prefixName = "set";
        while (iter.hasNext()) {
            keyAttribute = iter.next().toString();
            methodName = makeMethodName(keyAttribute, prefixName);

            Method[] methodList = objClass.getClass().getDeclaredMethods();

            for (Method method : methodList) {
                if (methodName.equals(method.getName())) {
                    try {
                        method.invoke(objClass, map.get(keyAttribute));
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }
        }
        return objClass;
    }

    private String makeMethodName(String keyAttribute, String prefixName) {

        // Exception case
        if (keyAttribute.equals(Constants.INTERFACE)) {
            keyAttribute = "itf";
        }

        String methodName = null;

        methodName = prefixName + keyAttribute.substring(0, 1).toUpperCase()
                + keyAttribute.substring(1);

        return methodName;
    }
}
