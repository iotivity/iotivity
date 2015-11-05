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

import org.eclipse.jface.viewers.ColumnLabelProvider;
import org.eclipse.jface.viewers.ViewerCell;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;

import com.android.ddmlib.Log.LogLevel;
import com.android.ddmlib.logcat.LogCatMessage;

/**
 * A JFace Column label provider for the LogCat log messages. It expects
 * elements of type {@link LogCatMessage}.
 */
public final class LogMessageLabelProvider extends ColumnLabelProvider {
    private static final int   INDEX_LOGLEVEL    = 0;
    private static final int   INDEX_LOGTIME     = 1;
    private static final int   INDEX_TAG         = 2;
    private static final int   INDEX_TEXT        = 3;

    // gray
    private static final Color INFO_MSG_COLOR    = new Color(null, 102, 102,
                                                         102);
    // green
    private static final Color VERBOSE_MSG_COLOR = new Color(null, 051, 102,
                                                         051);
    // blue
    private static final Color DEBUG_MSG_COLOR   = new Color(null, 051, 051,
                                                         153);
    // red
    private static final Color ERROR_MSG_COLOR   = new Color(null, 204, 0, 0);
    // orange
    private static final Color WARN_MSG_COLOR    = new Color(null, 204, 102, 0);

    private Font               mLogFont;
    private int                mWrapWidth        = 100;

    /**
     * Construct a column label provider for the logcat table.
     * 
     * @param font
     *            default font to use
     */
    public LogMessageLabelProvider(Font font) {
        mLogFont = font;
    }

    private String getCellText(LogMessage m, int columnIndex) {
        switch (columnIndex) {
            case INDEX_LOGLEVEL:
                return Character.toString(m.getLogLevel().getPriorityLetter());
                // return m.getLogLevel().toString();
            case INDEX_LOGTIME:
                return m.getTime();
            case INDEX_TAG:
                return m.getTag();
            case INDEX_TEXT:
                return m.getMessage();
            default:
                return "";
        }
    }

    @Override
    public void update(ViewerCell cell) {
        Object element = cell.getElement();
        if (!(element instanceof LogMessage)) {
            return;
        }
        LogMessage m = (LogMessage) element;
        String text = getCellText(m, cell.getColumnIndex());
        cell.setText(text);
        cell.setFont(mLogFont);
        cell.setForeground(getForegroundColor(m));
    }

    private Color getForegroundColor(LogMessage m) {
        LogLevel l = m.getLogLevel();
        if (l.equals(LogLevel.VERBOSE)) {
            return VERBOSE_MSG_COLOR;
        } else if (l.equals(LogLevel.INFO)) {
            return INFO_MSG_COLOR;
        } else if (l.equals(LogLevel.DEBUG)) {
            return DEBUG_MSG_COLOR;
        } else if (l.equals(LogLevel.ERROR)) {
            return ERROR_MSG_COLOR;
        } else if (l.equals(LogLevel.WARN)) {
            return WARN_MSG_COLOR;
        }
        return null;
    }

    public void setFont(Font preferredFont) {
        if (mLogFont != null) {
            mLogFont.dispose();
        }
        mLogFont = preferredFont;
    }

    public void setMinimumLengthForToolTips(int widthInChars) {
        mWrapWidth = widthInChars;
    }

    /**
     * Obtain the tool tip to show for a particular logcat message. We display a
     * tool tip only for messages longer than the width set by
     * {@link #setMinimumLengthForToolTips(int)}.
     */
    @Override
    public String getToolTipText(Object element) {
        String text = element.toString();
        if (text.length() > mWrapWidth) {
            return text;
        } else {
            return null;
        }
    }
}
