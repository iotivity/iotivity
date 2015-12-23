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
import java.io.FilenameFilter;
import java.util.LinkedHashMap;

import oic.ctt.ui.util.CTLogger;
import oic.ctt.ui.util.TestCaseParser;
import static oic.ctt.ui.util.DatabaseUtil.EXTENTION_DB;

import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.Viewer;
import org.slf4j.Logger;

public class FileTreeContentProvider implements ITreeContentProvider {

    private static final String XML_EXT          = ".xml";
    private static final String PROJECT_EXT      = ".project";
    private static final String STR_TC_FULL_NAME = "TC FULL NAME";
    private Logger              logger           = CTLogger.getInstance(); ;
    private Object              calledElement;

    public FileTreeContentProvider(Object providedInstance) {
        calledElement = providedInstance;
        logger.info("Initialize " + this.getClass().getSimpleName()
                + " class for " + providedInstance.getClass().getSimpleName());
    }

    @Override
    public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
    }

    @Override
    public void dispose() {
    }

    @Override
    public Object[] getElements(Object inputElement) {
        return getChildren(inputElement);
    }

    @Override
    public Object[] getChildren(Object parentElement) {
        if (parentElement instanceof File) {
            Object[] child = ((File) parentElement)
                    .listFiles(new FilenameFilter() {

                        @Override
                        public boolean accept(File dir, String name) {
                            return isContainedAcceptedData(name);
                        }
                    });

            if (!(calledElement instanceof TestReportView) && child == null
                    && ((File) parentElement).isFile()) {
                File[] tcname = getTCName(parentElement);

                return tcname == null ? new Object[0] : tcname;
            }

            return child == null ? new Object[0] : child;
        }

        return new Object[0];
    }

    /**
     * This method is to get the testcase name
     * 
     * @param parentElement
     *            is the test file instance
     * @return tcName is testcase name
     */
    private File[] getTCName(Object parentElement) {
        File[] tcName = null;
        TestCaseParser parser = new TestCaseParser();
        LinkedHashMap<Integer, LinkedHashMap<String, String>> list = new LinkedHashMap<Integer, LinkedHashMap<String, String>>();
        list = parser.getDocumentHashMap(((File) parentElement)
                .getAbsolutePath());

        if (list != null) {
            if (list.size() > 0) {
                tcName = new File[list.size()];
                for (int i = 0; i < list.size(); i++) {
                    File temp = new File(
                            ((File) parentElement).getAbsolutePath(), list.get(
                                    i).get(STR_TC_FULL_NAME));
                    tcName[i] = temp;
                }
            }
        }

        return tcName;
    }

    /**
     * This method checked whether proper file is found.
     * 
     * @param fileName
     *            is the name of file which is to be checked for the accepted
     *            value
     * @return true if accepted data is found, false otherwise
     */
    private boolean isContainedAcceptedData(String fileName) {
        if ((fileName.contains(PROJECT_EXT) || fileName.endsWith("~"))
                || (calledElement instanceof TestPlanView && fileName
                        .contains(EXTENTION_DB))
                || (calledElement instanceof TestReportView && fileName
                        .contains(XML_EXT))) {
            return false;
        } else {
            return true;
        }
    }

    @Override
    public Object getParent(Object element) {
        return ((File) element).getParentFile();
    }

    @Override
    public boolean hasChildren(Object element) {
        Object[] object = getChildren(element);

        if (object == null) {
            logger.info(element.toString());
        }

        return getChildren(element).length > 0;
    }
}
