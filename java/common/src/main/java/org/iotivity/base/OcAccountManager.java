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
     * @return EnumSet OcConnectivityType connectivity type set
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
     * Method for account registration to account server
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
     * @param accessToken    Identifier of the resource obtained by account registration.
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     * @throws OcException if failure
     */
    public void signOut(String accessToken,
                        OnPostListener onPostListener) throws OcException {
        this.signOut0(accessToken, onPostListener);
    }

    private native void signOut0(String accessToken,
                                 OnPostListener onPostListener) throws OcException;

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
     * @param queryMap       Map which can have the query key and value for specific users.
     *                       account server can response information of more than one user.
     * @param onGetListener  The event handler will be invoked with a map of attribute name and
     *                       values. It will also have the result from this Get operation.
     * @throws OcException if failure
     */
    public void searchUser(Map<String, String> queryMap,
                           OnGetListener onGetListener) throws OcException {
        this.searchUser0(queryMap, onGetListener);
    }

    private native void searchUser0(Map<String, String> queryMap,
                                    OnGetListener onGetListener) throws OcException;

    /**
     * Method to delete the device registered on the account signed-in
     *
     * @param accessToken      Identifier of the resource obtained by account registration.
     * @param deviceId         Device ID to delete.
     * @param onDeleteListener event handler The event handler will have headerOptionList
     * @throws OcException if failure
     */
    public void deleteDevice(String accessToken,
                             String deviceId,
                             OnDeleteListener onDeleteListener) throws OcException {
        this.deleteDevice0(accessToken, deviceId, onDeleteListener);
    }

    private native void deleteDevice0(String accessToken,
                                      String deviceId,
                                      OnDeleteListener onDeleteListener) throws OcException;

    /**
     * Method to delete the device registered on the account signed-in
     *
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     * @throws OcException if failure
     */
    public void createGroup(OnPostListener onPostListener) throws OcException {
        this.createGroup0(onPostListener);
    }

    private native void createGroup0(OnPostListener onPostListener) throws OcException;

    /**
     * Method to delete the device registered on the account signed-in
     *
     * @param queryMap       Map that has optional properties and values to create a group.
     *                       Defined properties on the OCF spec are [gname, parent] so far.
     *                       (2016/10/19)
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     * @throws OcException if failure
     */
    public void createGroup(Map<String, String> queryMap,
                            OnPostListener onPostListener) throws OcException {
        this.createGroup1(queryMap, onPostListener);
    }

    private native void createGroup1(Map<String, String> queryMap,
                                     OnPostListener onPostListener) throws OcException;

    /**
     * Method to delete the group from account server.
     *
     * @param groupId          Group ID to delete.
     * @param onDeleteListener event handler The event handler will have headerOptionList.
     * @throws OcException if failure
     */
    public void deleteGroup(String groupId,
                            OnDeleteListener onDeleteListener) throws OcException {
        this.deleteGroup0(groupId, onDeleteListener);
    }

    private native void deleteGroup0(String groupId,
                                     OnDeleteListener onDeleteListener) throws OcException;

    /**
     * Method to get infomation of all your group from account server.
     *
     * @param onGetListener  The event handler will be invoked with a map of attribute name and
     *                       values. It will also have the result from this Get operation.
     * @throws OcException if failure
     */
    public void getGroupInfoAll(OnGetListener onGetListener) throws OcException {
        this.getGroupInfoAll0(onGetListener);
    }

    private native void getGroupInfoAll0(OnGetListener onGetListener) throws OcException;

    /**
     * Method to get information of the specific group from account server.
     *
     * @param groupId        Group ID to get information.
     * @param onGetListener  The event handler will be invoked with a map of attribute name and
     *                       values. It will also have the result from this Get operation.
     * @throws OcException if failure
     */
    public void getGroupInfo(String groupId, OnGetListener onGetListener) throws OcException {
        this.getGroupInfo0(groupId, onGetListener);
    }

    private native void getGroupInfo0(String groupId,
                                      OnGetListener onGetListener) throws OcException;

    /**
     * Method to add values for properties to the group on account server.
     *
     * @param groupId        Group ID to add property values.
     * @param propertyValue  OCRepresentation info that has pairs of property and value.
     *                       Defined properties on the OCF spec are [members, masters, devices,
     *                       resources, links] so far. (2016/10/19)
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     * @throws OcException if failure
     */
    public void addPropertyValueToGroup(String groupId,
                                        OcRepresentation propertyValue,
                                        OnPostListener onPostListener) throws OcException {
        this.addPropertyValueToGroup0(groupId, propertyValue, onPostListener);
    }

    private native void addPropertyValueToGroup0(String groupId,
                                                 OcRepresentation propertyValue,
                                                 OnPostListener onPostListener) throws OcException;

    /**
     * Method to delete values for properties from the group on account server.
     *
     * @param groupId        Group ID to add property values.
     * @param propertyValue  OCRepresentation info that has pairs of property and value.
     *                       Defined properties on the OCF spec are [members, masters, devices,
     *                       resources, links] so far. (2016/10/19)
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     * @throws OcException if failure
     */
    public void deletePropertyValueFromGroup(String groupId,
                                             OcRepresentation propertyValue,
                                             OnPostListener onPostListener) throws OcException {
        this.deletePropertyValueFromGroup0(groupId, propertyValue, onPostListener);
    }

    private native void deletePropertyValueFromGroup0(String groupId,
                                                      OcRepresentation propertyValue,
                                                      OnPostListener onPostListener) throws OcException;

    /**
     * Method to update values for properties on the group on account server.
     * It completely replaces existing values for specific properties.
     *
     * @param groupId        Group ID to add property values.
     * @param propertyValue  OCRepresentation info that has pairs of property and value.
     *                       Defined properties on the OCF spec are [members, gname, owner,
     *                       masters, devices, resources, latitude, longitude, radius,
     *                       backgroundImage] so far. (2016/10/19)
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     * @throws OcException if failure
     */
    public void updatePropertyValueOnGroup(String groupId,
                                           OcRepresentation propertyValue,
                                           OnPostListener onPostListener) throws OcException {
        this.updatePropertyValueOnGroup0(groupId, propertyValue, onPostListener);
    }

    private native void updatePropertyValueOnGroup0(String groupId,
                                                    OcRepresentation propertyValue,
                                                    OnPostListener onPostListener) throws OcException;

    /**
     * Method to register observe to group resource on account server.
     * You can receive a notification when any value of property get changed in the group you
     * joined.
     *
     * @param onObserveListener event handler The handler method will be invoked with a map
     *                          of attribute name and values.
     * @throws OcException if failure
     */
    public void observeGroup(OnObserveListener onObserveListener) throws OcException {
        this.observeGroup0(onObserveListener);
    }

    private native void observeGroup0(OnObserveListener onObserveListener) throws OcException;

    /**
     * Method to cancel observe to group resource on account server.
     *
     * @throws OcException if failure
     */
    public void cancelObserveGroup() throws OcException {
        this.cancelObserveGroup0();
    }

    private native void cancelObserveGroup0() throws OcException;

    /**
     * Method to register observe to invitation resource on account server.
     * You can receive a notification when you send or receive a invitation.
     * Sending a invitation will be notified as 'invite' and Receiving will be as 'invited'.
     * If you receive a invitation from other user, you should call 'replyToInvitation' to
     * delete the invitation on account server, otherwise it will remain on the server.
     *
     * @param onObserveListener event handler The handler method will be invoked with a map
     *                          of attribute name and values.
     * @throws OcException if failure
     */
    public void observeInvitation(OnObserveListener onObserveListener) throws OcException {
        this.observeInvitation0(onObserveListener);
    }

    private native void observeInvitation0(OnObserveListener onObserveListener) throws OcException;

    /**
     * Method to cancel observe to invitation resource on account server
     *
     * @throws OcException if failure
     */
    public void cancelObserveInvitation() throws OcException {
        this.cancelObserveInvitation0();
    }

    private native void cancelObserveInvitation0() throws OcException;

    /**
     * Method to send a invitation to invite a user into a group.
     *
     * @param groupId        Group ID for inviting.
     * @param userUuid       Identifier of the user to invite.
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     * @throws OcException if failure
     */
    public void sendInvitation(String groupId,
                               String userUuid,
                               OnPostListener onPostListener) throws OcException {
        this.sendInvitation0(groupId, userUuid, onPostListener);
    }

    private native void sendInvitation0(String groupId,
                                        String userUuid,
                                        OnPostListener onPostListener) throws OcException;

    /**
     * Method to cancel a invitation on account server you has sent before the invited user
     * replies.
     *
     * @param groupId          Group ID to cancel a invitation.
     * @param userUuid         Identifier of the user to cancel a invitation.
     * @param onDeleteListener event handler The event handler will have headerOptionList.
     * @throws OcException if failure
     */
    public void cancelInvitation(String groupId,
                                 String userUuid,
                                 OnDeleteListener onDeleteListener) throws OcException {
        this.cancelInvitation0(groupId, userUuid, onDeleteListener);
    }

    private native void cancelInvitation0(String groupId,
                                          String userUuid,
                                          OnDeleteListener onDeleteListener) throws OcException;

    /**
     * Method to reply to the invitation that you has received.
     * If you set accept as true, you will join the group as a member and the invitation will be
     * deleted on account server. If false, only the invitation will be deleted.
     *
     * @param groupId          Group ID to delete a invitation.
     * @param accept           boolean whether to join the group or not.
     * @param onDeleteListener event handler The event handler will have headerOptionList.
     * @throws OcException if failure
     */
    public void replyToInvitation(String groupId,
                                  boolean accept,
                                  OnDeleteListener onDeleteListener) throws OcException {
        this.replyToInvitation0(groupId, accept, onDeleteListener);
    }

    private native void replyToInvitation0(String groupId,
                                           boolean accept,
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

    /**
     * An OnObserveListener can be registered via the resource observe call.
     * Event listeners are notified asynchronously
     */
    public interface OnObserveListener {
        /**
         * To Register.
         */
        public static final int REGISTER = 0;
        /**
         * To Deregister.
         */
        public static final int DEREGISTER = 1;
        /**
         * Others.
         */
        public static final int NO_OPTION = 2;

        public void onObserveCompleted(List<OcHeaderOption> headerOptionList,
                                       OcRepresentation ocRepresentation,
                                       int sequenceNumber);

        public void onObserveFailed(Throwable ex);
    }

    private long mNativeHandle;
}

