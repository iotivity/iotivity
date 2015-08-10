package oic.simulator.logger;

import java.text.DateFormat;
import java.text.SimpleDateFormat;

import oic.simulator.serviceprovider.manager.LogManager;

import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;

public class LogLabelProvider extends LabelProvider implements
        ITableLabelProvider {

    DateFormat dateFormat = new SimpleDateFormat("HH:mm:ss.SSS");

    @Override
    public Image getColumnImage(Object element, int columnIndex) {
        if (columnIndex == 0) {
            LogEntry entry = (LogEntry) element;
            return LogManager.getSeverityIcon(entry.getSeverity());
        }
        return null;
    }

    @Override
    public String getColumnText(Object element, int columnIndex) {
        LogEntry entry = (LogEntry) element;
        if (columnIndex == 0) {
            return LogManager.getSeverityName(entry.getSeverity());
        } else if (columnIndex == 1) {
            return dateFormat.format(entry.getDate());
        } else {
            return entry.getMessage();
        }
    }

}