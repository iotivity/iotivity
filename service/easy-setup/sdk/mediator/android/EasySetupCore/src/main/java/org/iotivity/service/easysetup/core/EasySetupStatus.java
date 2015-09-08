package org.iotivity.service.easysetup.core;

public interface EasySetupStatus {

        public void onFinished(EnrolleeDevice enrolledevice);
        public void onProgress(EnrolleeState state);

}
