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

package OC.Cm;

public interface ICMListener {
    public void onStarted(String address);

    public void onStopped();

    public void onError(OC.Cm.CMErrorCode errorCode, String errorMessage);

    public String getMyDeviceToken();

    public void updateUUIDAndTokenMap(OC.Cm.DeviceToken token);

    public void tokenRequestNotification(String uuid);

    public void easySetupNotification(OC.Cm.EasySetupNotification notif);

    public void scsErrorNotification(int errorCode);

    public void OnChannelCreated(String channelName, int connID);

    public String getAuthCode();

    public String getAccessToken();

    public String getRefreshToken();

    public String getUserID();
}
