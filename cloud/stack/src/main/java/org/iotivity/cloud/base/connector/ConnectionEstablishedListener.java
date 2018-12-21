package org.iotivity.cloud.base.connector;

import org.iotivity.cloud.base.device.IRequestChannel;

public interface ConnectionEstablishedListener {

    void onConnectionEstablished(final IRequestChannel requestChannel);

}
