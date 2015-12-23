//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
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
package oic.ctt.ui.log;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import oic.ctt.ui.log.ITableFocusListener.IFocusedTableActivator;
import oic.ctt.ui.util.CTLogger;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.IPreferenceStore;
import org.eclipse.jface.preference.PreferenceConverter;
import org.eclipse.jface.util.IPropertyChangeListener;
import org.eclipse.jface.util.PropertyChangeEvent;
import org.eclipse.jface.viewers.ColumnViewer;
import org.eclipse.jface.viewers.ColumnViewerToolTipSupport;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TableViewerColumn;
import org.eclipse.jface.viewers.ViewerCell;
import org.eclipse.jface.window.ToolTip;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.SashForm;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.dnd.TextTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.ScrollBar;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.slf4j.Logger;

import com.android.ddmlib.Log.LogLevel;

import static oic.ctt.ui.types.ToolTipTextType.*;
import static oic.ctt.ui.types.IDType.*;
import static oic.ctt.ui.types.ImageFilePathType.*;

public class LogPanel extends Panel implements ILogMessageEventListener {
    private static final String                FILE_DIALOG_TXT                 = "*.txt";
    private static final String                FILE_DIALOG_TEXT_FILES_TXT      = "Text Files (*.txt)";
    private static final String                FILE_DIALOG_LOG_TXT             = "log.txt";
    private static final String                FILE_DIALOG_SAVE_LOG            = "Save Log..";
    private static final String                SYMPLE_TEXT_VALUE               = " 00-00 00:00:00.0000 ";
    private static final String                SYMPLE_TEXT_MESSAGE             = " Log Message field should be pretty long by default.";
    private static final String                SYMPLE_TEXT_SERVER_CLIENT       = "SmartThingServerClient";
    private static final String                PROPERTY_MESSAGE                = "Message";
    private static final String                PROPERTY_TAG                    = "Tag";
    private static final String                PROPERTY_TIME                   = "Time";
    private static final String                PROPERTY_LEVEL                  = "Level";
    private static final String                THREAD_NAME                     = "Saving selected items to logfile..";
    private static final String                EXPORT_DIALOG_MASSAGE           = "Unexpected error while saving selected messages to file: ";
    private static final String                EXPORT_DIALOG_TITLE             = "Unable to export selection to file.";
    private Logger                             logger                          = CTLogger
                                                                                       .getInstance();
    public static final String                 LOGCAT_VIEW_FONT_PREFKEY        = "logcat.view.font";
    // public static final String LOGCAT_VIEW_FONT_PREFKEY = "Ubuntu";
    // Use a monospace font family
    // private static final String FONT_FAMILY = "Courier New";
    private static final String                FONT_FAMILY                     = "Ubuntu";
    private static final String                SEPARATOR_MSG                   = " - ";
    // Use the default system font size
    private static final FontData              DEFAULT_LOGCAT_FONTDATA;
    static {
        int h = Display.getDefault().getSystemFont().getFontData()[0]
                .getHeight();
        DEFAULT_LOGCAT_FONTDATA = new FontData(FONT_FAMILY, h, SWT.NORMAL);
    }
    public static final String                 LOG_FILTERS_LIST                = "log.view.filters.list";
    private static final String                LOG_VIEW_COLSIZE_PREFKEY_PREFIX = "logcat.view.colsize.";
    private static final String                DISPLAY_FILTERS_COLUMN_PREFKEY  = "logcat.view.display.filters";
    private static final String                DEFAULT_SEARCH_MESSAGE          = "Search for messages. Prefix with tag: to limit scope";
    private static final String                DEFAULT_SEARCH_TOOLTIP          = "Example search patterns:\n"
                                                                                       + "    sqlite (search for sqlite in text field)\n";

    private static final int[]                 WEIGHTS_SHOW_FILTERS            = new int[] {
            25, 75                                                            };
    private static final int[]                 WEIGHTS_LOGCAT_ONLY             = new int[] {
            0, 100                                                            };
    SashForm                                   mSash;
    ToolItem                                   mNewFilterToolItem;
    ToolItem                                   mDeleteFilterToolItem;
    ToolItem                                   mEditFilterToolItem;

    private ToolItem                           mScrollLockCheckBox;
    private boolean                            mShouldScrollToLatestLog        = false;

    private ToolItem                           mRobotSysLogCheckBox;
    private boolean                            showRobotSysLog                 = false;

    private List<LogFilter>                    mLogFilters;
    private TableViewer                        mFiltersTableViewer;
    private int                                mCurrentSelectedFilterIndex;
    private Combo                              mLiveFilterLevelCombo;
    private Text                               mLiveFilterText;
    private TableViewer                        mViewer;
    private IPreferenceStore                   mPrefStore;
    private String                             mLogFileExportFolder;
    private LogMessageLabelProvider            mLogMessageLabelProvider;
    private List<ILogMessageSelectionListener> mMessageSelectionListeners;
    private LogTableRefresherTask              mCurrentRefresher;
    private LogMessageList                     mLogMessages;

    public LogPanel(IPreferenceStore preferenceStore) {
        mPrefStore = preferenceStore;
        initializeFilters();
        setupDefaultPreferences();
        initializePreferenceUpdateListeners();

    }

    private void initializeFilters() {
        mLogFilters = new ArrayList<LogFilter>();
        /* add default filter matching all messages */
        String tag = "";
        String text = "";
        String robot = "";

        mLogFilters.add(new LogFilter("All messages (no filters)", tag, text,
                robot, LogLevel.VERBOSE));
        /* restore saved filters from prefStore */
        List<LogFilter> savedFilters = getSavedFilters();
        mLogFilters.addAll(savedFilters);
        mLogMessages = new LogMessageList(LogMessageList.MAX_MESSAGES_DEFAULT);
    }

    private void setupDefaultPreferences() {
        PreferenceConverter.setDefault(mPrefStore,
                LogPanel.LOGCAT_VIEW_FONT_PREFKEY, DEFAULT_LOGCAT_FONTDATA);
        mPrefStore.setDefault(LogMessageList.MAX_MESSAGES_PREFKEY,
                LogMessageList.MAX_MESSAGES_DEFAULT);
        mPrefStore.setDefault(DISPLAY_FILTERS_COLUMN_PREFKEY, true);
    }

    private void initializePreferenceUpdateListeners() {
        mPrefStore.addPropertyChangeListener(new IPropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent event) {
                String changedProperty = event.getProperty();
                if (changedProperty.equals(LogPanel.LOGCAT_VIEW_FONT_PREFKEY)) {
                    mLogMessageLabelProvider.setFont(getFontFromPrefStore());
                    refreshLogTable();
                } else if (changedProperty
                        .equals(LogMessageList.MAX_MESSAGES_PREFKEY)) {
                    refreshLogTable();
                }
            }
        });
    }

    private void refreshLogTable() {
        synchronized (this) {
            if (mCurrentRefresher == null) {
                mCurrentRefresher = new LogTableRefresherTask();
                Display.getDefault().asyncExec(mCurrentRefresher);
            }
        }
    }

    private class LogTableRefresherTask implements Runnable {
        @Override
        public void run() {
            if (mViewer.getTable().isDisposed()) {
                return;
            }
            synchronized (LogPanel.this) {
                mCurrentRefresher = null;
            }
            // find out if we need to scroll before refreshing the table
            boolean shouldScroll = shouldScrollToLatestLog();
            mViewer.refresh();
            if (shouldScroll) {
                scrollToLatestLog();
            }
        }
    }

    private boolean shouldScrollToLatestLog() {
        ScrollBar sb = mViewer.getTable().getVerticalBar();
        if (sb == null) {
            return true;
        }
        // On Mac & Linux, when the scroll bar is at the bottom,
        // sb.getSelection + sb.getThumb = sb.getMaximum
        // But on Windows 7, the scrollbar never touches the bottom, and as a
        // result
        // sb.getSelection + sb.getThumb is slightly less than sb.getMaximum.
        // So we assume that as long as the thumb is close to the bottom, we
        // want to scroll.
        return Math.abs(sb.getSelection() + sb.getThumb() - sb.getMaximum()) < 10;
    }

    private List<LogFilter> getSavedFilters() {
        LogCatFilterSettingsSerializer serializer = new LogCatFilterSettingsSerializer();
        String e = mPrefStore.getString(LOG_FILTERS_LIST);
        return serializer.decodeFromPreferenceString(e);
    }

    @Override
    protected void postCreation() {
    }

    @Override
    protected Control createControl(Composite parent) {
        GridLayout layout = new GridLayout(1, false);
        parent.setLayout(layout);
        createViews(parent);
        setupDefaults();
        return null;
    }

    private void createViews(Composite parent) {
        mSash = createSash(parent);
        createListOfFilters(mSash);
        createLogTableView(mSash);
        boolean showFilters = mPrefStore
                .getBoolean(DISPLAY_FILTERS_COLUMN_PREFKEY);
        updateFiltersColumn(showFilters);
    }

    private SashForm createSash(Composite parent) {
        SashForm sash = new SashForm(parent, SWT.HORIZONTAL);
        sash.setLayoutData(new GridData(GridData.FILL_BOTH));
        return sash;
    }

    @Override
    public void setFocus() {
    }

    private void createListOfFilters(SashForm sash) {
        Composite c = new Composite(sash, SWT.BORDER);
        GridLayout layout = new GridLayout(2, false);
        c.setLayout(layout);
        c.setLayoutData(new GridData(GridData.FILL_BOTH));
        createFiltersToolbar(c);
        createFiltersTable(c);
    }

    private void createFiltersToolbar(Composite parent) {
        Label l = new Label(parent, SWT.NONE);
        l.setText("Saved Filters");
        GridData gd = new GridData();
        gd.horizontalAlignment = SWT.LEFT;
        l.setLayoutData(gd);
        ToolBar t = new ToolBar(parent, SWT.FLAT);
        gd = new GridData();
        gd.horizontalAlignment = SWT.RIGHT;
        t.setLayoutData(gd);
        /* new filter */
        mNewFilterToolItem = new ToolItem(t, SWT.PUSH);
        mNewFilterToolItem.setImage(AbstractUIPlugin.imageDescriptorFromPlugin(
                PLUGIN_ID.toString(),
                IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_CREATE_FILTERS_TOOLBAR
                        .toString()).createImage());
        mNewFilterToolItem.setToolTipText(TOOLTIP_TEXT_CREATE_FILTERS_TOOLBAR
                .toString());
        // mNewFilterToolItem.setToolTipText("Add a new log filter");
        mNewFilterToolItem.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent arg0) {
                addNewFilter();
            }
        });
        /* delete filter */
        mDeleteFilterToolItem = new ToolItem(t, SWT.PUSH);
        mDeleteFilterToolItem.setImage(AbstractUIPlugin
                .imageDescriptorFromPlugin(
                        PLUGIN_ID.toString(),
                        IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_DELETE_LOG_FILTER
                                .toString()).createImage());
        // mDeleteFilterToolItem.setToolTipText("Delete selected log filter");
        mDeleteFilterToolItem.setToolTipText(TOOLTIP_TEXT_DELETE_LOG_FILTER
                .toString());
        mDeleteFilterToolItem.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent arg0) {
                deleteSelectedFilter();
            }
        });
        /* edit filter */
        mEditFilterToolItem = new ToolItem(t, SWT.PUSH);
        mEditFilterToolItem.setImage(AbstractUIPlugin
                .imageDescriptorFromPlugin(
                        PLUGIN_ID.toString(),
                        IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_EDIT_LOG_FILTER
                                .toString()).createImage());
        mEditFilterToolItem.setToolTipText(TOOLTIP_TEXT_EDIT_LOG_FILTER
                .toString());
        // mEditFilterToolItem.setToolTipText("Edit selected log filter");
        mEditFilterToolItem.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent arg0) {
                editSelectedFilter();
            }
        });
    }

    private void addNewFilter() {
        LogFilterSettingsDialog d = new LogFilterSettingsDialog(Display
                .getCurrent().getActiveShell());
        if (d.open() != Window.OK) {
            return;
        }
        LogFilter f = new LogFilter(d.getFilterName().trim(), d.getTag(), d
                .getText().trim(), "", LogLevel.getByString(d.getLogLevel()));
        mLogFilters.add(f);
        mFiltersTableViewer.refresh();
        /* select the newly added entry */
        int idx = mLogFilters.size() - 1;
        mFiltersTableViewer.getTable().setSelection(idx);
        filterSelectionChanged();
        saveFilterPreferences();
    }

    private void deleteSelectedFilter() {
        int selectedIndex = mFiltersTableViewer.getTable().getSelectionIndex();
        if (selectedIndex <= 0) {
            /*
             * return if no selected filter, or the default filter was selected
             * (0th).
             */
            return;
        }
        mLogFilters.remove(selectedIndex);
        mFiltersTableViewer.refresh();
        mFiltersTableViewer.getTable().setSelection(selectedIndex - 1);
        filterSelectionChanged();
        saveFilterPreferences();
    }

    private void editSelectedFilter() {
        int selectedIndex = mFiltersTableViewer.getTable().getSelectionIndex();
        if (selectedIndex < 0) {
            return;
        }
        LogFilter curFilter = mLogFilters.get(selectedIndex);
        LogFilterSettingsDialog dialog = new LogFilterSettingsDialog(Display
                .getCurrent().getActiveShell());
        dialog.setDefaults(curFilter.getName(), curFilter.getTag(),
                curFilter.getText(), curFilter.getLogLevel());
        if (dialog.open() != Window.OK) {
            return;
        }
        LogFilter f = new LogFilter(dialog.getFilterName(), dialog.getTag(),
                dialog.getText(), "",
                LogLevel.getByString(dialog.getLogLevel()));
        mLogFilters.set(selectedIndex, f);
        mFiltersTableViewer.refresh();
        mFiltersTableViewer.getTable().setSelection(selectedIndex);
        filterSelectionChanged();
        saveFilterPreferences();
    }

    private void filterSelectionChanged() {
        int idx = getSelectedSavedFilterIndex();
        if (idx == -1) {
            /*
             * One of the filters should always be selected. On Linux, there is
             * no way to deselect an item. On Mac, clicking inside the list
             * view, but not an any item will result in all items being
             * deselected. In such a case, we simply reselect the first entry.
             */
            idx = 0;
            mFiltersTableViewer.getTable().setSelection(idx);
        }
        mCurrentSelectedFilterIndex = idx;
        resetUnreadCountForSelectedFilter();
        updateFiltersToolBar();
        updateAppliedFilters();
    }

    private int getSelectedSavedFilterIndex() {
        return mFiltersTableViewer.getTable().getSelectionIndex();
    }

    private void resetUnreadCountForSelectedFilter() {
        int index = getSelectedSavedFilterIndex();
        mLogFilters.get(index).resetUnreadCount();
        refreshFiltersTable();
    }

    private void updateFiltersToolBar() {
        /* The default filter at index 0 can neither be edited, nor removed. */
        boolean en = getSelectedSavedFilterIndex() != 0;
        mEditFilterToolItem.setEnabled(en);
        mDeleteFilterToolItem.setEnabled(en);
    }

    private void updateAppliedFilters() {
        /* list of filters to apply = saved filter + live filters */
        List<LogViewerFilter> filters = new ArrayList<LogViewerFilter>();
        filters.add(getSelectedSavedFilter());
        filters.addAll(getCurrentLiveFilters());
        mViewer.setFilters(filters.toArray(new LogViewerFilter[filters.size()]));
        /*
         * whenever filters are changed, the number of displayed logs changes
         * drastically. Display the latest log in such a situation.
         */
        scrollToLatestLog();
    }

    // live filter
    private List<LogViewerFilter> getCurrentLiveFilters() {
        List<LogViewerFilter> liveFilters = new ArrayList<LogViewerFilter>();
        String tempRobot = " robot:";

        if (!showRobotSysLog) {
            tempRobot += "^(?!(RobotSysLog))";
        }

        List<LogFilter> liveFilterSettings = LogFilter.fromString(
                mLiveFilterText.getText() + tempRobot,
                LogLevel.getByString(mLiveFilterLevelCombo.getText()));

        for (LogFilter s : liveFilterSettings) {
            liveFilters.add(new LogViewerFilter(s));
        }
        return liveFilters;
    }

    private LogViewerFilter getSelectedSavedFilter() {
        int index = getSelectedSavedFilterIndex();
        return new LogViewerFilter(mLogFilters.get(index));
    }

    private void scrollToLatestLog() {
        if (!mShouldScrollToLatestLog) {
            mViewer.getTable().setTopIndex(
                    mViewer.getTable().getItemCount() - 1);
        }
    }

    private void refreshFiltersTable() {
        Display.getDefault().asyncExec(new Runnable() {
            public void run() {
                if (mFiltersTableViewer.getTable().isDisposed()) {
                    return;
                }
                mFiltersTableViewer.refresh();
            }
        });
    }

    private void saveFilterPreferences() {
        LogCatFilterSettingsSerializer serializer = new LogCatFilterSettingsSerializer();
        /* save all filter settings except the first one which is the default */
        String e = serializer.encodeToPreferenceString(mLogFilters.subList(1,
                mLogFilters.size()));
        mPrefStore.setValue(LOG_FILTERS_LIST, e);
    }

    private void createFiltersTable(Composite parent) {
        final Table table = new Table(parent, SWT.FULL_SELECTION);
        GridData gd = new GridData(GridData.FILL_BOTH);
        gd.horizontalSpan = 2;
        table.setLayoutData(gd);
        table.setLinesVisible(true);
        mFiltersTableViewer = new TableViewer(table);
        mFiltersTableViewer.setContentProvider(new LogFilterContentProvider());
        mFiltersTableViewer.setLabelProvider(new LogFilterLabelProvider());
        mFiltersTableViewer.setInput(mLogFilters);
        mFiltersTableViewer.getTable().addSelectionListener(
                new SelectionAdapter() {
                    @Override
                    public void widgetSelected(SelectionEvent event) {
                        filterSelectionChanged();
                    }
                });
    }

    private void setupDefaults() {
        int defaultFilterIndex = 0;
        mFiltersTableViewer.getTable().setSelection(defaultFilterIndex);
        filterSelectionChanged();
    }

    private void createLogTableView(SashForm sash) {
        Composite c = new Composite(sash, SWT.NONE);
        c.setLayout(new GridLayout());
        c.setLayoutData(new GridData(GridData.FILL_BOTH));
        createLiveFilters(c);

        createLogViewTable(c);
    }

    /**
     * Create the search bar at the top of the logcat messages table. FIXME:
     * Currently, this feature is incomplete: The UI elements are created, but
     * they are all set to disabled state.
     */
    private void createLiveFilters(Composite parent) {
        Composite c = new Composite(parent, SWT.NONE);
        c.setLayout(new GridLayout(3, false));
        c.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        mLiveFilterText = new Text(c, SWT.BORDER | SWT.SEARCH);
        mLiveFilterText.setLayoutData(new GridData(GridData.FILL_HORIZONTAL));
        mLiveFilterText.setMessage(DEFAULT_SEARCH_MESSAGE);
        mLiveFilterText.setToolTipText(DEFAULT_SEARCH_TOOLTIP);
        mLiveFilterText.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent arg0) {
                updateAppliedFilters();
            }
        });
        mLiveFilterLevelCombo = new Combo(c, SWT.READ_ONLY | SWT.DROP_DOWN);
        mLiveFilterLevelCombo.setItems(LogFilterSettingsDialog.getLogLevels()
                .toArray(new String[0]));
        mLiveFilterLevelCombo.select(0);

        // filter eventlistener
        mLiveFilterLevelCombo.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent arg0) {
                updateAppliedFilters();
            }
        });
        ToolBar toolBar = new ToolBar(c, SWT.FLAT);

        // robot sys log
        mRobotSysLogCheckBox = new ToolItem(toolBar, SWT.CHECK);
        mRobotSysLogCheckBox.setImage(AbstractUIPlugin
                .imageDescriptorFromPlugin(
                        PLUGIN_ID.toString(),
                        IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_ROBOT_SYS_LOG
                                .toString()).createImage());

        mRobotSysLogCheckBox.setSelection(false);
        mRobotSysLogCheckBox.setToolTipText(TOOLTIP_TEXT_ROBOT_SYS_LOG
                .toString());
        mRobotSysLogCheckBox.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent event) {
                boolean bRobot = mRobotSysLogCheckBox.getSelection();
                showRobotSysLog = bRobot;
                updateAppliedFilters();
            }
        });

        ToolItem saveToLog = new ToolItem(toolBar, SWT.PUSH);
        saveToLog.setImage(AbstractUIPlugin.imageDescriptorFromPlugin(
                PLUGIN_ID.toString(),
                IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_SAVE_TO_LOG.toString())
                .createImage());
        saveToLog.setToolTipText(TOOLTIP_TEXT_SAVE_TO_LOG.toString());

        saveToLog.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent arg0) {
                saveLogToFile();
            }
        });
        ToolItem clearLog = new ToolItem(toolBar, SWT.PUSH);
        clearLog.setToolTipText(TOOLTIP_TEXT_CLEAR_LOG.toString());
        clearLog.setImage(AbstractUIPlugin.imageDescriptorFromPlugin(
                PLUGIN_ID.toString(),
                IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_CLEAR_LOG.toString())
                .createImage());
        clearLog.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent arg0) {
                mLogMessages.clear();
                mViewer.setInput(mLogMessages);
            }
        });

        // scroll lock
        mScrollLockCheckBox = new ToolItem(toolBar, SWT.CHECK);
        mScrollLockCheckBox
                .setImage(AbstractUIPlugin
                        .imageDescriptorFromPlugin(
                                PLUGIN_ID.toString(),
                                IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_SCROLL_LOCK
                                        .toString()).createImage());

        mScrollLockCheckBox.setSelection(false);
        mScrollLockCheckBox.setToolTipText(TOOLTIP_TEXT_SCROLL_LOCK.toString());

        mScrollLockCheckBox.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent event) {
                boolean scrollLock = mScrollLockCheckBox.getSelection();
                mShouldScrollToLatestLog = scrollLock;
            }
        });

        final ToolItem showFiltersColumn = new ToolItem(toolBar, SWT.CHECK);
        showFiltersColumn.setImage(AbstractUIPlugin
                .imageDescriptorFromPlugin(
                        PLUGIN_ID.toString(),
                        IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_SHOW_FILTERS_COLUMN
                                .toString()).createImage());
        showFiltersColumn.setSelection(mPrefStore
                .getBoolean(DISPLAY_FILTERS_COLUMN_PREFKEY));
        showFiltersColumn.setToolTipText(TOOLTIP_TEXT_SHOW_FILTERS_COLUMN
                .toString());

        showFiltersColumn.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent event) {
                boolean showFilters = showFiltersColumn.getSelection();
                mPrefStore
                        .setValue(DISPLAY_FILTERS_COLUMN_PREFKEY, showFilters);
                updateFiltersColumn(showFilters);
            }
        });
    }

    private void saveLogToFile() {
        /* show dialog box and get target file name */
        final String fName = getLogFileTargetLocation();
        if (fName == null) {
            return;
        }

        /* obtain list of selected messages */
        final List<LogMessage> selectedMessages = getSelectedLogMessages();
        /* save messages to file in a different (non UI) thread */
        Thread t = new Thread(new Runnable() {
            public void run() {
                try {
                    BufferedWriter w = new BufferedWriter(new FileWriter(fName));
                    for (LogMessage m : selectedMessages) {
                        if (m != null) {
                            String temp = "";
                            temp = appendLogMessage(m, temp);
                            w.append(temp);
                            w.newLine();
                        }
                    }
                    w.close();
                } catch (final IOException e) {
                    Display.getDefault().asyncExec(new Runnable() {
                        public void run() {
                            MessageDialog.openError(Display.getCurrent()
                                    .getActiveShell(), EXPORT_DIALOG_TITLE,
                                    EXPORT_DIALOG_MASSAGE + e.getMessage());
                        }
                    });
                }
            }

        });
        t.setName(THREAD_NAME);
        t.start();
    }

    private static String appendLogMessage(LogMessage m, String temp) {
        if (!m.getTime().isEmpty()) {
            temp += m.getTime() + SEPARATOR_MSG;
        }
        if (!m.getLogLevel().toString().isEmpty()) {
            temp += m.getLogLevel().toString() + SEPARATOR_MSG;
        }
        if (!m.getTag().isEmpty()) {
            temp += m.getTag() + SEPARATOR_MSG;
        }
        if (!m.getMessage().isEmpty()) {
            temp += m.getMessage();
        }
        return temp;
    }

    private String getLogFileTargetLocation() {
        FileDialog fd = new FileDialog(Display.getCurrent().getActiveShell(),
                SWT.SAVE);
        fd.setText(FILE_DIALOG_SAVE_LOG);
        fd.setFileName(FILE_DIALOG_LOG_TXT);
        if (mLogFileExportFolder == null) {
            mLogFileExportFolder = System.getProperty("user.home");
        }
        fd.setFilterNames(new String[] { FILE_DIALOG_TEXT_FILES_TXT });
        fd.setFilterExtensions(new String[] { FILE_DIALOG_TXT });
        String fName = fd.open();
        if (fName != null) {
            mLogFileExportFolder = fd.getFilterPath();
        }
        return fName;
    }

    private List<LogMessage> getSelectedLogMessages() {
        Table table = mViewer.getTable();
        int[] indices = table.getSelectionIndices();
        Arrays.sort(indices);
        List<LogMessage> selectedMessages = new ArrayList<LogMessage>(
                indices.length);
        for (int i : indices) {
            LogMessage m = (LogMessage) table.getItem(i).getData();
            selectedMessages.add(m);
        }
        return selectedMessages;
    }

    private void updateFiltersColumn(boolean showFilters) {
        if (showFilters) {
            mSash.setWeights(WEIGHTS_SHOW_FILTERS);
        } else {
            mSash.setWeights(WEIGHTS_LOGCAT_ONLY);
        }
    }

    // log table
    private void createLogViewTable(Composite parent) {
        // The SWT.VIRTUAL bit causes the table to be rendered faster. However
        // it makes all rows
        // to be of the same height, thereby clipping any rows with multiple
        // lines of text.
        // In such a case, users can view the full text by hovering over the
        // item and looking at
        // the tooltip.
        final Table table = new Table(parent, SWT.FULL_SELECTION | SWT.MULTI
                | SWT.VIRTUAL | SWT.H_SCROLL);

        table.setLayoutData(new GridData(GridData.FILL_BOTH));
        table.getHorizontalBar().setVisible(true);

        mViewer = new TableViewer(table);

        /** Columns to show in the table. */
        String[] properties = { PROPERTY_LEVEL, PROPERTY_TIME, PROPERTY_TAG,
                PROPERTY_MESSAGE };
        int[] widths = { 50, 150, 10, 400 };
        /**
         * The sampleText for each column is used to determin,e the default
         * widths for each column. The contents do not matter, only their
         * lengths are needed.
         */
        String[] sampleText = { PROPERTY_LEVEL, SYMPLE_TEXT_VALUE,
                SYMPLE_TEXT_SERVER_CLIENT, SYMPLE_TEXT_MESSAGE };
        mLogMessageLabelProvider = new LogMessageLabelProvider(
                getFontFromPrefStore());

        int firstColumnStyle = 0;
        for (int i = 0; i < properties.length; i++) {
            if (i == 3) {
                firstColumnStyle = SWT.LEFT;
            } else {
                firstColumnStyle = SWT.CENTER;
            }

            TableColumn tc = TableHelper.createTableColumn(mViewer.getTable(),
                    properties[i], /* Column title */
                    firstColumnStyle, /* Column Style */
                    sampleText[i], /* String to compute default col width */
                    getColPreferenceKey(properties[i]), /*
                                                         * Preference Store key
                                                         * for this column
                                                         */
                    mPrefStore);
            if (tc.getWidth() == 0)
                tc.setWidth(widths[i]);
            TableViewerColumn tvc = new TableViewerColumn(mViewer, tc);
            tvc.setLabelProvider(mLogMessageLabelProvider);
        }
        mViewer.getTable().showSelection();
        mViewer.getTable().setLinesVisible(true); /* zebra stripe the table */
        mViewer.getTable().setHeaderVisible(true);
        mViewer.setContentProvider(new LogMessageContentProvider());
        WrappingToolTipSupport.enableFor(mViewer, ToolTip.NO_RECREATE);

        // Set the row height to be sufficient enough to display the current
        // font.
        // This is not strictly necessary, except that on WinXP, the rows showed
        // up clipped. So
        // we explicitly set it to be sure.
        mViewer.getTable().addListener(SWT.MeasureItem, new Listener() {
            public void handleEvent(Event event) {
                event.height = event.gc.getFontMetrics().getHeight();
            }
        });
        // Update the label provider whenever the text column's width changes
        TableColumn textColumn = mViewer.getTable().getColumn(
                properties.length - 1);
        textColumn.addControlListener(new ControlAdapter() {
            @Override
            public void controlResized(ControlEvent event) {
                TableColumn tc = (TableColumn) event.getSource();
                int width = tc.getWidth();
                GC gc = new GC(tc.getParent());
                int avgCharWidth = gc.getFontMetrics().getAverageCharWidth();
                gc.dispose();
                if (mLogMessageLabelProvider != null) {
                    mLogMessageLabelProvider.setMinimumLengthForToolTips(width
                            / avgCharWidth);
                }
            }
        });

        initDoubleClickListener();
        mViewer.setInput(mLogMessages);
    }

    private void initDoubleClickListener() {
        mMessageSelectionListeners = new ArrayList<ILogMessageSelectionListener>(
                1);
        mViewer.getTable().addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetDefaultSelected(SelectionEvent arg0) {
                List<LogMessage> selectedMessages = getSelectedLogMessages();
                if (selectedMessages.size() == 0) {
                    return;
                }
                for (ILogMessageSelectionListener l : mMessageSelectionListeners) {
                    l.messageDoubleClicked(selectedMessages.get(0));
                }
            }
        });

    }

    private Font getFontFromPrefStore() {
        FontData fd = PreferenceConverter.getFontData(mPrefStore,
                LogPanel.LOGCAT_VIEW_FONT_PREFKEY);

        return new Font(Display.getDefault(), fd);
    }

    private String getColPreferenceKey(String field) {
        return LOG_VIEW_COLSIZE_PREFKEY_PREFIX + field;
    }

    public void addLogMessage(List<LogMessage> lm) {
        mLogMessages.appendMessages(lm);
        Display.getDefault().syncExec(new Runnable() {
            @Override
            public void run() {
                mViewer.setInput(mLogMessages);
                scrollToLatestLog();
            }
        });

    }

    private static class WrappingToolTipSupport extends
            ColumnViewerToolTipSupport {
        protected WrappingToolTipSupport(ColumnViewer viewer, int style,
                boolean manualActivation) {
            super(viewer, style, manualActivation);
        }

        @Override
        protected Composite createViewerToolTipContentArea(Event event,
                ViewerCell cell, Composite parent) {
            Composite comp = new Composite(parent, SWT.NONE);
            GridLayout l = new GridLayout(1, false);
            l.horizontalSpacing = 0;
            l.marginWidth = 1;
            l.marginHeight = 1;
            l.verticalSpacing = 0;
            comp.setLayout(l);
            Text text = new Text(comp, SWT.BORDER | SWT.V_SCROLL | SWT.WRAP);
            text.setEditable(false);

            LogMessage m = (LogMessage) cell.getElement();

            String temp = "";
            if (m != null) {
                temp = appendLogMessage(m, temp);
            }
            text.setText(temp);
            text.setLayoutData(new GridData(500, 150));
            return comp;
        }

        @Override
        public boolean isHideOnMouseDown() {
            return false;
        }

        public static final void enableFor(ColumnViewer viewer, int style) {
            new WrappingToolTipSupport(viewer, style, false);
        }
    }

    private ITableFocusListener mTableFocusListener;

    public void setTableFocusListener(ITableFocusListener listener) {
        mTableFocusListener = listener;
        final Table table = mViewer.getTable();
        final IFocusedTableActivator activator = new IFocusedTableActivator() {
            public void copy(Clipboard clipboard) {
                copySelectionToClipboard(clipboard);
            }

            public void selectAll() {
                table.selectAll();
            }
        };
        table.addFocusListener(new FocusListener() {
            public void focusGained(FocusEvent e) {
                mTableFocusListener.focusGained(activator);
            }

            public void focusLost(FocusEvent e) {
                mTableFocusListener.focusLost(activator);
            }
        });
    }

    /** Copy all selected messages to clipboard. */
    public void copySelectionToClipboard(Clipboard clipboard) {

        if (!mViewer.getTable().isFocusControl())
            return;

        Display.getDefault().asyncExec(new Runnable() {
            public void run() {
                try {
                    StringBuilder sb = new StringBuilder();

                    for (LogMessage m : getSelectedLogMessages()) {
                        try {
                            if (m != null) {
                                String temp = "";
                                temp = appendLogMessage(m, temp);
                                sb.append(temp);
                                sb.append('\n');
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                            logger.error("LogPanel copySelectionToClipboard :"
                                    + e.toString());
                        }
                    }
                    clipboard.setContents(new Object[] { sb.toString() },
                            new Transfer[] { TextTransfer.getInstance() });

                } catch (Exception e) {
                    e.printStackTrace();
                    logger.error("LogPanel copySelectionToClipboard :"
                            + e.toString());
                }
            }
        });

    }

    /** Select all items in the logcat table. */
    public void selectAll() {
        if (!mViewer.getTable().isFocusControl())
            return;
        mViewer.getTable().selectAll();
    }

    @Override
    public void messageReceived(List<LogMessage> receivedMessages) {
    }

    public void clearLogMessage() {
        mLogMessages.clear();
        mViewer.setInput(mLogMessages);
    }
}