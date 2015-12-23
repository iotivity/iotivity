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

import java.io.File;

import oic.ctt.ui.UIConst;
import oic.ctt.ui.actions.CreateTestPlanAction;
import oic.ctt.ui.util.CTLogger;

import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.slf4j.Logger;

public class CheckStateListenerThread implements ICheckStateListener {
    private CheckboxTreeViewer   treeViewer;
    private Object               parentClass;
    private CreateTestPlanAction createTestPlanAction;
    private Logger               logger = CTLogger.getInstance();

    public CheckStateListenerThread(CheckboxTreeViewer checkboxTreeViewer,
            Object parent, CreateTestPlanAction createTestPlanAction) {
        treeViewer = checkboxTreeViewer;
        parentClass = parent;
        this.createTestPlanAction = createTestPlanAction;

        logger.info("Initialize " + this.getClass().getSimpleName()
                + " class for " + parent.getClass().getSimpleName());
    }

    @Override
    public void checkStateChanged(CheckStateChangedEvent event) {
        if (event.getChecked()) {
            treeViewer.setSubtreeChecked(event.getElement(), true);
        } else {
            treeViewer.setSubtreeChecked(event.getElement(), false);
        }

        File element = (File) event.getElement();
        if (element != null) {
            if (treeViewer.getChecked(element)) {
                while (true) {
                    File elementParent = element.getParentFile();
                    if (elementParent != null) {
                        element = elementParent;
                        if (element != null) {
                            if (element.getName()
                                    .equals(UIConst.TESTSUITE_PATH)) {
                                break;
                            }
                            logger.info("Test element path: "
                                    + element.getName());
                            treeViewer.setChecked(element, true);
                        }
                    } else {
                        break;
                    }
                }

            } else {
                while (true) {
                    if (element != null) {
                        element = element.getParentFile();
                        if (element != null) {
                            logger.info("Test element path while element is not null: "
                                    + element.getName());
                            if (element.getName()
                                    .equals(UIConst.TESTSUITE_PATH)) {
                                break;
                            }
                            boolean check = false;
                            File[] children = (File[]) (((ITreeContentProvider) treeViewer
                                    .getContentProvider()).getChildren(element));
                            for (int i = 0; i < children.length; i++) {
                                if (treeViewer.getChecked(children[i])) {
                                    check = true;
                                    break;
                                }
                            }
                            if (check == false) {
                                treeViewer.setChecked(element, false);
                            }
                        } else {
                            logger.debug("element is null while its parent.");
                        }
                    } else {
                        logger.debug("element is null");
                    }

                }
            }

            if (((TestSuiteView) parentClass).getSelectedItems().length > 0) {
                createTestPlanAction.setEnabled(true);
            } else {
                createTestPlanAction.setEnabled(false);
            }
        }
    }

}
