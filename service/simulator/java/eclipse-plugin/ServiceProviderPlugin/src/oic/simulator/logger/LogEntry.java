package oic.simulator.logger;

import java.util.Date;

import oic.simulator.serviceprovider.manager.LogManager;

public class LogEntry {
    private final int    severity;
    private final Date   date;
    private final String message;

    public LogEntry(int severity, Date date, String message) {
        this.severity = severity;
        this.date = date;
        this.message = message;
    }

    public String getMessage() {
        return message;
    }

    public Date getDate() {
        return date;
    }

    public int getSeverity() {
        return severity;
    }

    @Override
    public String toString() {
        String newline = System.getProperty("line.separator");
        String out = date.toString() + newline;
        out += "Severity: " + LogManager.getSeverityName(severity) + newline;
        out += "Message: " + message + newline;
        out += "===============================" + newline + newline;
        return out;
    }
}