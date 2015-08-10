package oic.simulator.serviceprovider.listener;

import java.util.List;

import oic.simulator.logger.LogEntry;

public interface ILogUIListener {
    public void logAdded(LogEntry added);

    public void logChanged(List<LogEntry> entry);
}
