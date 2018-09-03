package org.iotivity.cloud.base.healthcheck;

import java.util.concurrent.TimeUnit;

public class SimpleHealthHolder implements HealthHolder {


    private final Long intervalMs;

    private Long pingAccepted;

    public SimpleHealthHolder(TimeUnit timeUnit, Integer interval) {
        this.intervalMs = timeUnit.toMillis(interval);
        pingAccepted();
    }

    @Override
    public synchronized boolean isHealthy() {
        return (System.currentTimeMillis() - pingAccepted) < intervalMs;
    }

    @Override
    public synchronized void pingAccepted() {
        pingAccepted = System.currentTimeMillis();
    }
}
