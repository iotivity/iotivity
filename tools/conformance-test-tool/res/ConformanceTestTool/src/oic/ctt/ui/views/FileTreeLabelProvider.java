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
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import oic.ctt.ui.UIConst;
import oic.ctt.ui.util.CTLogger;
import static oic.ctt.ui.types.ImageFilePathType.*;

import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.swt.graphics.Image;
import org.slf4j.Logger;

public class FileTreeLabelProvider implements ILabelProvider {
    private int                          max_size;
    private boolean                      isGraph;
    private List<ILabelProviderListener> listeners;
    private Image                        file;
    private Image                        dir;
    private Image                        tc;
    private Image                        graph;
    private Logger                       logger = CTLogger.getInstance();

    public FileTreeLabelProvider(Object parent) {
        logger.info("Initialize " + this.getClass().getSimpleName()
                + " class for " + parent.getClass().getSimpleName());

        if (parent instanceof TestReportView) {
            max_size = 4;
            isGraph = true;
        } else {
            max_size = 3;
        }

        listeners = new ArrayList<ILabelProviderListener>();
        FileInputStream[] fileInputStream = new FileInputStream[max_size];
        try {
            File f = new File(UIConst.PROJECT_PATH + "icons/");
            if (f.exists()) {
                try {
                    fileInputStream[0] = new FileInputStream(
                            UIConst.PROJECT_PATH
                                    + IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_FILE_ICON
                                            .toString());
                    file = new Image(null, fileInputStream[0]);

                    fileInputStream[1] = new FileInputStream(
                            UIConst.PROJECT_PATH
                                    + IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_FOLDER_ICON
                                            .toString());
                    dir = new Image(null, fileInputStream[1]);

                    fileInputStream[2] = new FileInputStream(
                            UIConst.PROJECT_PATH
                                    + IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_TESTFILE_ICON
                                            .toString());
                    tc = new Image(null, fileInputStream[2]);

                    if (parent instanceof TestReportView) {
                        fileInputStream[3] = new FileInputStream(
                                UIConst.PROJECT_PATH
                                        + IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_SEQUENCE_ICON
                                                .toString());
                        graph = new Image(null, fileInputStream[3]);
                    }

                    logger.info("Found icons directory(PROJECT_PATH). PATH = "
                            + UIConst.PROJECT_PATH + "icons/");

                } catch (FileNotFoundException e) {
                    logger.error("icon error. " + e.getLocalizedMessage());
                }
            }
            for (int i = 0; i < max_size; i++) {
                if (fileInputStream[i] != null) {
                    fileInputStream[i].close();
                }
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public Image getImage(Object arg0) {
        Image image = null;

        if (arg0 instanceof File) {
            if (((File) arg0).isDirectory()) {
                return dir;
            } else if (isGraph || ((File) arg0).isFile()) {
                String filename = ((File) arg0).getName();
                if (filename.equals(UIConst.FLOWGRAPH_FILE_NAME)) {
                    return graph;
                } else {
                    return file;
                }
            } else {
                image = tc;
            }
        }

        return image;
    }

    @Override
    public String getText(Object arg0) {
        if (arg0 instanceof File) {
            String text = ((File) arg0).getName();
            if (text.length() == 0) {
                text = ((File) arg0).getPath();
            }
            return text;
        } else {
            return (String) arg0;
        }
    }

    @Override
    public void addListener(ILabelProviderListener arg0) {
        listeners.add(arg0);
    }

    @Override
    public void dispose() {
        Image[] disposableImages = { dir, file, tc, graph };
        disposeImageObjects(disposableImages);
    }

    @Override
    public boolean isLabelProperty(Object arg0, String arg1) {
        return false;
    }

    @Override
    public void removeListener(ILabelProviderListener arg0) {
        listeners.remove(arg0);
    }

    /**
     * This method is for disposing image objects
     * 
     * @param imageObjects
     *            is a set of imageObjects have to be disposed
     * @return nothing
     */
    public static void disposeImageObjects(Image[] imageObjects) {
        for (Image imageObject : imageObjects) {
            try {
                if (imageObject != null)
                    imageObject.dispose();
            } catch (Exception e) {
            }
        }
    }
}
