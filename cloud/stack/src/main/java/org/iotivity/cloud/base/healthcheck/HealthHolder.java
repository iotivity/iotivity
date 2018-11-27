package org.iotivity.cloud.base.healthcheck;

public interface HealthHolder {

    boolean isHealthy();

    void pingAccepted();

}
