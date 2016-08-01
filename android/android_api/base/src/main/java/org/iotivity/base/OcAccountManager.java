/*
 *******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base;

import java.util.EnumSet;
import java.util.List;
import java.util.Map;

public final class OcAccountManager {

    private OcAccountManager(long nativeHandle) {
        this.mNativeHandle = nativeHandle;
    }

    /**
     * Method to get the host address of account server
     *
     * @return String host address
     */
    public native String getHost();

    /**
     * Method to get the connectivity type for account server
     *
     * @return EnumSet<OcConnectivityType></OcConnectivityType> connectivity type set
     */
    public EnumSet<OcConnectivityType> getConnectivityTypeSet() {
        return OcConnectivityType.convertToEnumSet(
                this.getConnectivityTypeN()
        );
    }

    private native int getConnectivityTypeN();

    /**
     * Method for account registration to account server
     *
     * @param authProvider   Provider name used for authentication.
     * @param authCode       The authorization code obtained by using an authorization server
     *                       as an intermediary between the client and resource owner.
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     * @throws OcException if failure
     */
    public void signUp(String authProvider,
                       String authCode,
                       OnPostListener onPostListener) throws OcException {
        this.signUp0(authProvider, authCode, onPostListener);
    }

    private native void signUp0(String authProvider,
                                String authCode,
                                OnPostListener onPostListener) throws OcException;

    /**
     * overload
     *
     * @param authProvider   Provider name used for authentication.
     * @param authCode       The authorization code obtained by using an authorization server
     *                       as an intermediary between the client and resource owner.
     * @param options        The option values depends on auth provider.
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     * @throws OcException if failure
     */
    public void signUp(String authProvider,
                       String authCode,
                       Map<String, String> options,
                       OnPostListener onPostListener) throws OcException {
        this.signUp1(authProvider, authCode, options, onPostListener);
    }

    private native void signUp1(String authProvider,
                                String authCode,
                                Map<String, String> options,
                                OnPostListener onPostListener) throws OcException;

    /**
     * Method for sign-in to account server
     *
     * @param userUuid       Identifier of the user obtained by account registration.
     * @param accessToken    Identifier of the resource obtained by account registration.
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     * @throws OcException if failure
     */
    public void signIn(String userUuid,
                       String accessToken,
                       OnPostListener onPostListener) throws OcException {
        this.signIn0(userUuid, accessToken, onPostListener);
    }

    private native void signIn0(String userUuid,
                                String accessToken,
                                OnPostListener onPostListener) throws OcException;

    /**
     * Method for sign-out to account server
     *
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     * @throws OcException if failure
     */
    public void signOut(OnPostListener onPostListener) throws OcException {
        this.signOut0(onPostListener);
    }

    private native void signOut0(OnPostListener onPostListener) throws OcException;

    /**
     * Method to refresh access token to account server
     *
     * @param userUuid       Identifier of the user obtained by account registration.
     * @param refreshToken   Refresh token used for access token refresh.
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     * @throws OcException if failure
     */
    public void refreshAccessToken(String userUuid,
                                   String refreshToken,
                                   OnPostListener onPostListener) throws OcException {
        this.refreshAccessToken0(userUuid, refreshToken, onPostListener);
    }

    private native void refreshAccessToken0(String userUuid,
                                            String refreshToken,
                                            OnPostListener onPostListener) throws OcException;

    /**
     * Method to get information of the user to account server
     *
     * @param userUuid      Identifier of the user to get information.
     * @param onGetListener The event handler will be invoked with a map of attribute name and
     *                      values. The event handler will also have the result from this Get
     *                      operation This will have error codes
     * @throws OcException if failure
     */
    public void searchUser(String userUuid,
                           OnGetListener onGetListener) throws OcException {
        this.searchUser0(userUuid, onGetListener);
    }

    private native void searchUser0(String userUuid,
                                    OnGetListener onGetListener) throws OcException;

    /**
     * Overload
     *
     * @param queryMap       Map which can have the query key and value for specific users.
     *                       account server can response information of more than one user.
     * @param onGetListener  The event handler will be invoked with a map of attribute name and
     *                       values. The event handler will also have the result from this Get
     *                       operation This will have error codes
     * @throws OcException if failure
     */
    public void searchUser(Map<String, String> queryMap,
                           OnGetListener onGetListener) throws OcException {
        this.searchUser1(queryMap, onGetListener);
    }

    private native void searchUser1(Map<String, String> queryMap,
                                    OnGetListener onGetListener) throws OcException;

    /**
     * Method to delete the device registered on the account signed-in
     *
     * @param deviceId         Device ID to delete.
     * @param onDeleteListener event handler The event handler will have headerOptionList
     * @throws OcException if failure
     */
    public void deleteDevice(String deviceId,
                             OnDeleteListener onDeleteListener) throws OcException {
        this.deleteDevice0(deviceId, onDeleteListener);
    }

    private native void deleteDevice0(String deviceId,
                                      OnDeleteListener onDeleteListener) throws OcException;

    /**
     * An OnGetListener can be registered via the resource get call.
     * Event listeners are notified asynchronously
     */
    public interface OnGetListener {
        public void onGetCompleted(List<OcHeaderOption> headerOptionList,
                                   OcRepresentation ocRepresentation);

        public void onGetFailed(Throwable ex);
    }

    /**
     * An OnPostListener can be registered via the resource post call.
     * Event listeners are notified asynchronously
     */
    public interface OnPostListener {
            public void onPostCompleted(List<OcHeaderOption> headerOptionList,
                                        OcRepresentation ocRepresentation);

            public void onPostFailed(Throwable ex);
    }

    /**
     * An OnDeleteListener can be registered via the resource delete call.
     * Event listeners are notified asynchronously
     */
    public interface OnDeleteListener {
        public void onDeleteCompleted(List<OcHeaderOption> headerOptionList);

        public void onDeleteFailed(Throwable ex);
    }

    private long mNativeHandle;
}

