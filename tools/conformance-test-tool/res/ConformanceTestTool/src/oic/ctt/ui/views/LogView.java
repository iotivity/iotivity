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
package oic.ctt.ui.views;

import oic.ctt.ui.log.LogHandler;
import oic.ctt.ui.log.LogPanel;

import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.ViewPart;

public class LogView extends ViewPart {

    /**
     * The ID of the view as specified by the extension.
     */
    public static final String ID        = "oic.ctt.ui.views.LogView";

    List                       list;
    Text                       text;
    String                     temp;
    Clipboard                  mClipboard;
    public final LogPanel      mLogPanel = new LogPanel(
                                                 PlatformUI
                                                         .getPreferenceStore());

    public LogView() {
    }

    @Override
    public void createPartControl(Composite parent) {
        parent.setLayout(new FillLayout());

        getViewSite().getActionBars().getStatusLineManager()
                .setMessage("End Pasting");

        mLogPanel.createPanel(parent);

        Display.getCurrent().addFilter(SWT.KeyDown, new Listener() {

            @Override
            public void handleEvent(Event e) {
                if (((e.stateMask & SWT.CTRL) == SWT.CTRL)
                        && (e.keyCode == 'c' || e.keyCode == 'C')) {
                    mLogPanel.copySelectionToClipboard(new Clipboard(null));
                }
                if (((e.stateMask & SWT.CTRL) == SWT.CTRL)
                        && (e.keyCode == 'a' || e.keyCode == 'A')) {
                    mLogPanel.selectAll();
                }
            }
        });
    }

    @Override
    public void setFocus() {
    }

    public void clearLog() {
        Display.getDefault().asyncExec(new Runnable() {
            @Override
            public void run() {
                mLogPanel.clearLogMessage();
            }
        });
    }

    @Override
    public void dispose() {
        LogHandler.stopRealTimeLogging();
        LogHandler.stopLogViewer();
        System.out.println("LogView dispose");
    }
}
