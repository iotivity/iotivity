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
     * Method to delete the device registered on the account signed-in
     *
     * @param groupType      Group type that can be used for referencing default group ACL creation.
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     * @throws OcException if failure
     */
    public void createGroup(AclGroupType groupType,
                            OnPostListener onPostListener) throws OcException {
        this.createGroup0(groupType.getValue(), onPostListener);
    }

    private native void createGroup0(int groupType,
                                     OnPostListener onPostListener) throws OcException;

    /**
     * Method to get a list of groups joined from account server
     *
     * @param onGetListener  The event handler will be invoked with a map of attribute name and
     *                       values. The event handler will also have the result from this Get
     *                       operation This will have error codes
     *
     * @return Returns ::OC_STACK_OK if success
     */
    public void getGroupList(OnGetListener onGetListener) throws OcException {
        this.getGroupList0(onGetListener);
    }

    private native void getGroupList0(OnGetListener onGetListener) throws OcException;

    /**
     * Method to delete the group from account server
     *
     * @param groupId          Group ID to delete.
     * @param onDeleteListener event handler The event handler will have headerOptionList.
     *
     * @return Returns ::OC_STACK_OK if success
     */
    public void deleteGroup(String groupId,
                            OnDeleteListener onDeleteListener) throws OcException {
        this.deleteGroup0(groupId, onDeleteListener);
    }

    private native void deleteGroup0(String groupId,
                                     OnDeleteListener onDeleteListener) throws OcException;

    /**
     * Method to join the group on account server
     *
     * @param groupId        Group ID to join
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     *
     * @return Returns ::OC_STACK_OK if success
     */
    public void joinGroup(String groupId,
                          OnPostListener onPostListener) throws OcException {
        this.joinGroup0(groupId, onPostListener);
    }

    private native void joinGroup0(String groupId,
                                   OnPostListener onPostListener) throws OcException;

    /**
     * Method to add devices to the group on account server
     *
     * @param groupId        Group ID to add devices.
     * @param deviceId       List of devices to add.
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     *
     * @return Returns ::OC_STACK_OK if success
     */
    public void addDeviceToGroup(String groupId,
                                 List<String> deviceId,
                                 OnPostListener onPostListener) throws OcException {
        this.addDeviceToGroup0(groupId, deviceId.toArray(new String[deviceId.size()]),
                               onPostListener);
    }

    private native void addDeviceToGroup0(String groupId,
                                          String[] deviceId,
                                          OnPostListener onPostListener) throws OcException;

    /**
     * Method to get information of the group from account server
     *
     * @param groupId        Group ID to get information.
     * @param onGetListener  The event handler will be invoked with a map of attribute name and
     *                       values. The event handler will also have the result from this Get
     *                       operation This will have error codes
     *
     * @return Returns ::OC_STACK_OK if success
     */
    public void getGroupInfo(String groupId,
                             OnGetListener onGetListener) throws OcException {
        this.getGroupInfo0(groupId, onGetListener);
    }

    private native void getGroupInfo0(String groupId,
                                      OnGetListener onGetListener) throws OcException;

    /**
     * Method to leave the group joined on account server
     *
     * @param groupId          Group ID to leave.
     * @param onDeleteListener event handler The event handler will have headerOptionList.
     *
     * @return Returns ::OC_STACK_OK if success
     */
    public void leaveGroup(String groupId,
                           OnDeleteListener onDeleteListener) throws OcException {
        this.leaveGroup0(groupId, onDeleteListener);
    }

    private native void leaveGroup0(String groupId,
                                    OnDeleteListener onDeleteListener) throws OcException;

    /**
     * Method to delete devices from the group on account server
     *
     * @param groupId          Group ID to delete devices.
     * @param deviceId         List of devices to delete.
     * @param onDeleteListener event handler The event handler will have headerOptionList.
     *
     * @return Returns ::OC_STACK_OK if success
     */
    public void deleteDeviceFromGroup(String groupId,
                                      List<String> deviceId,
                                      OnDeleteListener onDeleteListener) throws OcException {
        this.deleteDeviceFromGroup0(groupId, deviceId.toArray(new String[deviceId.size()]),
                                    onDeleteListener);
    }

    private native void deleteDeviceFromGroup0(String groupId,
                                               String[] deviceId,
                                               OnDeleteListener onDeleteListener) throws OcException;

    /**
     * Method to register observe to the group on account server
     * User can receive a notify when the group get changed (eg. new user/device added)
     *
     * @param groupId             Group ID to observe.
     * @param cloudConnectHandler Callback function that will get the result of the operation.
     *
     * @return Returns ::OC_STACK_OK if success
     */
    public void observeGroup(String groupId,
                             OnObserveListener onObserveListener) throws OcException {
        this.observeGroup0(groupId, onObserveListener);
    }

    private native void observeGroup0(String groupId,
                                      OnObserveListener onObserveListener) throws OcException;

    /**
     * Method to cancel observe to the group on account server
     *
     * @param groupId Group ID to observe.
     *
     * @return Returns ::OC_STACK_OK if success
     */
    public void cancelObserveGroup(String groupId) throws OcException {
        this.cancelObserveGroup0(groupId);
    }

    private native void cancelObserveGroup0(String groupId) throws OcException;

    /**
     * Method to register observe to invitation resource on account server
     * User can receive a invitation which is including group ID to join
     * Once receive a invitation, user should call 'deleteInvitation' to delete a invitation
     * on account server.
     *
     * @param cloudConnectHandler Callback function that will get the result of the operation.
     *
     * @return Returns ::OC_STACK_OK if success
     */
    public void observeInvitation(OnObserveListener onObserveListener) throws OcException {
        this.observeInvitation0(onObserveListener);
    }

    private native void observeInvitation0(OnObserveListener onObserveListener) throws OcException;

    /**
     * Method to cancel observe to invitation resource on account server
     *
     * @return Returns ::OC_STACK_OK if success
     */
    public void cancelObserveInvitation() throws OcException {
        this.cancelObserveInvitation0();
    }

    private native void cancelObserveInvitation0() throws OcException;

    /**
     * Method to send a invitation to invite a user into a group
     *
     * @param groupId        Group ID for inviting.
     * @param userUuid       Identifier of the user to invite.
     * @param onPostListener event handler The event handler will be invoked with a map of
     *                       attribute name and values.
     *
     * @return Returns ::OC_STACK_OK if success
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
     * Method to cancel a invitation on account server that user has sent
     *
     * @param groupId          Group ID to cancel a invitation.
     * @param userUuid         Identifier of the user to cancel a invitation.
     * @param onDeleteListener event handler The event handler will have headerOptionList.
     *
     * @return Returns ::OC_STACK_OK if success
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
     * Method to delete a invitation on account server that user has received
     *
     * @param groupId          Group ID to delete a invitation.
     * @param onDeleteListener event handler The event handler will have headerOptionList.
     *
     * @return Returns ::OC_STACK_OK if success
     */
    public void deleteInvitation(String groupId,
                                 OnDeleteListener onDeleteListener) throws OcException {
        this.deleteInvitation0(groupId, onDeleteListener);
    }

    private native void deleteInvitation0(String groupId,
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

