package org.iotivity.cloud.ciserver.resources;

import org.iotivity.cloud.base.connector.ConnectionEstablishedListener;
import org.iotivity.cloud.base.connector.ConnectorPool;
import org.iotivity.cloud.base.device.IRequestChannel;
import org.iotivity.cloud.base.protocols.MessageBuilder;
import org.iotivity.cloud.base.protocols.enums.ContentFormat;
import org.iotivity.cloud.base.protocols.enums.RequestMethod;
import org.iotivity.cloud.ciserver.Constants;
import org.iotivity.cloud.util.Cbor;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.*;
import java.util.stream.Collectors;

public class UpdateDeviceStateListener implements ConnectionEstablishedListener {

    private final static Logger Log = LoggerFactory.getLogger(UpdateDeviceStateListener.class);

    private final DevicePresenter devicePresenter;

    public UpdateDeviceStateListener(DevicePresenter devicePresenter) {
        this.devicePresenter = devicePresenter;
    }

    @Override
    public void onConnectionEstablished(IRequestChannel requestChannel) {

        final Set<String> deviceIds = devicePresenter.getDeviceIds().stream()
                .filter(Objects::nonNull).collect(Collectors.toSet());
        if(deviceIds == null || deviceIds.size() == 0){
            return;
        }
        Log.debug("RD is connected. Update presence table of actually connected device");
        Cbor<HashMap<String, Object>> cbor = new Cbor<>();
        HashMap<String, Object> payload = new HashMap<String, Object>();
        List<Map<String,Object>> devicePresenceList = new LinkedList<>();
        payload.put(Constants.PRESENCE_LIST, devicePresenceList);
        deviceIds.stream().forEach(deviceId -> {
            final HashMap<String, Object> deviceStatusRecord = new HashMap<>();
            deviceStatusRecord.put(Constants.DEVICE_ID, deviceId);
            deviceStatusRecord.put(Constants.PRESENCE_STATE, Constants.PRESENCE_ON);
            devicePresenceList.add(deviceStatusRecord);
        });

        Log.debug("Payload for presence update: {} ", payload);
        StringBuffer uriPath = new StringBuffer();
        uriPath.append("/" + Constants.PREFIX_OIC);
        uriPath.append("/" + Constants.DEVICE_PRESENCE_URI);
        requestChannel.sendRequest(MessageBuilder.createRequest(
                        RequestMethod.PUT, uriPath.toString(), null,
                        ContentFormat.APPLICATION_CBOR,
                        cbor.encodingPayloadToCbor(payload)), null);
        Log.debug("Presence table of RD successfully updated");
    }
}
