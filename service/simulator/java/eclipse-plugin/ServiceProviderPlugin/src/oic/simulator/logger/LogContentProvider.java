package oic.simulator.logger;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.Viewer;

public class LogContentProvider implements ITreeContentProvider {

    List<LogEntry> logEntryList = new ArrayList<LogEntry>();

    @SuppressWarnings("unchecked")
    @Override
    public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
        logEntryList = (List<LogEntry>) newInput;
    }

    @Override
    public Object[] getChildren(Object element) {
        return new Object[0];
    }

    @Override
    public Object[] getElements(Object input) {
        return logEntryList.toArray();
    }

    @Override
    public Object getParent(Object element) {
        return null;
    }

    @Override
    public boolean hasChildren(Object element) {
        return false;
    }

    @Override
    public void dispose() {
    }

    public synchronized void addLog(LogEntry newElement) {
        logEntryList.add(newElement);
    }
}