/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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
 ******************************************************************/

package org.iotivity.service.server;

import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import org.iotivity.service.RcsException;
import org.iotivity.service.RcsObject;
import org.iotivity.service.RcsResourceAttributes;
import org.iotivity.service.RcsValue;

public final class RcsLockedAttributes extends RcsObject {

    private static native boolean nativeIsEmpty(RcsResourceObject resourceObj);

    private static native int nativeSize(RcsResourceObject resourceObj);

    private static native boolean nativeRemove(RcsResourceObject resourceObj,
            String key);

    private static native boolean nativeClear(RcsResourceObject resourceObj);

    private static native boolean nativeContains(RcsResourceObject resourceObj,
            String key);

    private static native void nativeAddKeys(RcsResourceObject resourceObj,
            Set<String> set);

    private static native RcsValue nativeAsJavaObject(
            RcsResourceObject resourceObj, String key);

    private static native void nativeApply(RcsResourceObject resourceObj,
            Map<String, RcsValue> cache);

    private native void nativeLock(RcsResourceObject resourceObj);

    private native void nativeUnlock();

    private final RcsResourceObject mResourceObject;

    private boolean mIsUnlocked;

    private Map<String, RcsValue> mCache = new HashMap<>();

    RcsLockedAttributes(RcsResourceObject resourceObject) throws RcsException {
        if (resourceObject == null) {
            throw new RcsException("Illegal opertaion!");
        }

        mResourceObject = resourceObject;

        nativeLock(resourceObject);
    }

    void setUnlockState() {
        mIsUnlocked = true;
        mCache = null;

        nativeUnlock();
    }

    void apply() {
        nativeApply(mResourceObject, mCache);
        mCache.clear();
    }

    private void ensureLocked() throws RcsException {
        if (mIsUnlocked) {
            throw new RcsUnlockedException("This attributes is unlocked!");
        }
    }

    public Set<String> keySet() throws RcsException {
        ensureLocked();

        final Set<String> keySet = new HashSet<>(mCache.keySet());

        nativeAddKeys(mResourceObject, keySet);

        return Collections.unmodifiableSet(keySet);
    }

    public RcsValue get(String key) throws RcsException {
        ensureLocked();

        if (key == null) throw new NullPointerException("key is null");

        if (!mCache.containsKey(key) && nativeContains(mResourceObject, key)) {
            mCache.put(key, nativeAsJavaObject(mResourceObject, key));
        }
        return mCache.get(key);
    }

    public RcsLockedAttributes putAll(RcsResourceAttributes attributes) {
        final Set<String> keys = attributes.keySet();

        for (final String k : keys) {
            mCache.put(k, attributes.get(k));
        }
        return this;
    }

    /**
     * set a attribute value.
     *
     * @param key
     *            - Key of the element to be added.
     *
     * @param value
     *            - value to be set.
     *
     */
    public RcsLockedAttributes put(String key, RcsValue value)
            throws RcsException {
        ensureLocked();

        if (key == null) throw new NullPointerException("key is null");
        if (value == null) throw new NullPointerException("value is null");

        mCache.put(key, value);

        return this;
    }

    /**
     * Returns whether attribute is empty.
     *
     * @return boolean
     */
    public boolean isEmpty() throws RcsException {
        ensureLocked();

        return mCache.isEmpty() && nativeIsEmpty(mResourceObject);
    }

    /**
     * Returns the number of elements.
     *
     */
    public int size() throws RcsException {
        ensureLocked();

        return mCache.size() + nativeSize(mResourceObject);
    }

    /**
     * Removes a single attribute
     *
     * @param key
     *            Key to be removed.
     *
     * @return true if an attribute is removed, false otherwise.
     */
    public boolean remove(String key) throws RcsException {
        ensureLocked();

        if (key == null) throw new NullPointerException("key is null");

        // XXX make sure both cache and native values to be removed.
        final boolean cacheRemove = mCache.remove(key) != null;
        final boolean nativeRemove = nativeRemove(mResourceObject, key);

        return cacheRemove || nativeRemove;
    }

    public void clear() {
        nativeClear(mResourceObject);
    }

    /**
     * Checks the container has an attribute with a Key equivalent to the
     * provided key.
     *
     * @param key
     *            Key to check.
     *
     * @return true if an attribute with requests key exists, false otherwise.
     */
    public boolean contains(String key) throws RcsException {
        ensureLocked();

        if (key == null) throw new NullPointerException("key is null");

        return mCache.containsKey(key) || nativeContains(mResourceObject, key);
    }
}
