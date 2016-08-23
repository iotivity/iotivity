/******************************************************************
 *
 * Copyright 2016 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

package org.iotivity.service.rc;

import static org.iotivity.service.rc.RCTestAppStaticUtil.*;

import org.iotivity.service.resourcecontainer.RcsBundleInfo;
import org.iotivity.service.resourcecontainer.RcsResourceContainer;
import org.iotivity.service.resourcecontainer.BundleResource;

import java.util.*;

import android.content.Context;
import static org.iotivity.service.testapp.framework.Base.*;

public class RCServer {
    private Context             appContext;
    private String              sdCardPath;
    private String              configFile;
    public static boolean       isContainerStarted    = false;
    private static boolean      isBundleStarted       = false;
    private int                 intBundleStartCounter = 0;

    private static String       m_errorMessage        = "";
    private static String       m_successfullMessage  = "";
    private static String       m_ResourceListString  = "";
    private static String       m_BundleListString    = "";

    public RcsResourceContainer rcsResourceContainer;

    public RCServer() {
        appContext = getContext();
        rcsResourceContainer = new RcsResourceContainer(appContext);
    }

    public void rcStartContainer() {
        clearMessagelog();
        try {

            sdCardPath = appContext.getFilesDir().getPath();

            configFile = sdCardPath + PATH_INDICATOR
                    + RESOURCE_CONTAINER_CONFIG_XML;

            if (configFile.isEmpty()) {
                showOutPut("Configuration file path is empty.Please check...");
                return;
            }

            if (!isContainerStarted) {
                List<RcsBundleInfo> bundles = rcsResourceContainer
                        .startContainer(configFile);
                if (bundles.size() > 0) {
                    isContainerStarted = true;
                    showOutPut("Resource container has been started succesfully.");
                } else {
                    showOutPut("Sorry.Container is not started.Please check config file path.");
                }
            } else {
                showOutPut("Resource container already started.");
            }
        } catch (IllegalArgumentException ex) {
            m_errorMessage = "IllegalArgument exception occurred: "
                    + ex.getMessage();
            showOutPut(m_errorMessage);
        } catch (Exception e) {
            m_errorMessage = e.getMessage();
            showOutPut(m_errorMessage);
        }
    }

    public void rcStopContainer() {
        clearMessagelog();
        try {
            if (!checkIsContainerStarted())
                return;

            rcsResourceContainer.stopContainer();
            isContainerStarted = false;
            intBundleStartCounter = 0;
            showOutPut("Resource container has been stopped succesfully.");
        } catch (Exception e) {
            m_errorMessage = e.getMessage();
            showOutPut(m_errorMessage);
        }
    }

    public void rcAddBundle(String bundleId, String bundleUri,
            String bundlePath, String activator, Map<String, String> params) {
        clearMessagelog();
        try {
            if (!checkIsContainerStarted())
                return;

            rcsResourceContainer.addBundle(bundleId, bundleUri, bundlePath,
                    activator, params);

            m_successfullMessage = "Bundle ";
            m_successfullMessage += "(";
            m_successfullMessage += bundleId;
            m_successfullMessage += ")";
            m_successfullMessage += " has been added successfully having\n";
            m_successfullMessage += "Uri: " + bundleUri + "\n";
            m_successfullMessage += "Path : " + bundlePath + "\n";

            showOutPut(m_successfullMessage);
        } catch (IllegalArgumentException ex) {
            m_errorMessage = "IllegalArgument exception occurred: "
                    + ex.getMessage();
            showOutPut(m_errorMessage);
        } catch (Exception e) {
            m_errorMessage = e.getMessage();
            showOutPut(m_errorMessage);
        }
    }

    public void rcRemoveBundle(String bundleId) {
        clearMessagelog();
        try {
            if (!checkIsContainerStarted())
                return;
            if (checkBundleRegistered(bundleId)) {

                rcsResourceContainer.removeBundle(bundleId);
                intBundleStartCounter = (intBundleStartCounter < 0) ? 0
                        : intBundleStartCounter--;

                isBundleStarted = false;

                m_successfullMessage = "Bundle ";
                m_successfullMessage += "(";
                m_successfullMessage += bundleId;
                m_successfullMessage += ")";
                m_successfullMessage += " has been removed successfully...\n";

                showOutPut(m_successfullMessage);
            } else {
                showOutPut("Bundle \'" + bundleId + "\'is not registered.");
            }

        } catch (IllegalArgumentException ex) {
            m_errorMessage = "IllegalArgument exception occurred: "
                    + ex.getMessage();
            showOutPut(m_errorMessage);
        } catch (Exception e) {
            m_errorMessage = e.getMessage();
            showOutPut(m_errorMessage);
        }
    }

    public void rcStartBundle(String bundleId) {
        clearMessagelog();
        try {
            if (!checkIsContainerStarted())
                return;

            if (checkBundleRegistered(bundleId) && (intBundleStartCounter <= 2)) {

                rcsResourceContainer.startBundle(bundleId);

                isBundleStarted = true;
                intBundleStartCounter++;

                m_successfullMessage = "Bundle ";
                m_successfullMessage += "(";
                m_successfullMessage += bundleId;
                m_successfullMessage += ")";
                m_successfullMessage += " has been started successfully...\n";

                showOutPut(m_successfullMessage);
            } else {
                showOutPut("Bundle \'" + bundleId + "\' is not registered.");
            }

        } catch (IllegalArgumentException ex) {
            m_errorMessage = "IllegalArgument exception occurred: "
                    + ex.getMessage();
            showOutPut(m_errorMessage);
        } catch (Exception e) {
            m_errorMessage = e.getMessage();
            showOutPut(m_errorMessage);
        }
    }

    public void rcStopBundle(String bundleId) {
        clearMessagelog();
        try {
            if (!checkIsContainerStarted())
                return;
            if (!checkIsBundleStarted(bundleId))
                return;

            rcsResourceContainer.stopBundle(bundleId);

            isBundleStarted = false;
            intBundleStartCounter--;

            m_successfullMessage = "Bundle ";
            m_successfullMessage += "(";
            m_successfullMessage += bundleId;
            m_successfullMessage += ")";
            m_successfullMessage += " has been stopped successfully...\n";

            showOutPut(m_successfullMessage);
        } catch (IllegalArgumentException ex) {
            m_errorMessage = "IllegalArgument exception occurred: "
                    + ex.getMessage();
            showOutPut(m_errorMessage);
        } catch (Exception e) {
            m_errorMessage = e.getMessage();
            showOutPut(m_errorMessage);
        }
    }

    public void rcAddResourceConfig(String bundleId, String resourceUri,
            Map<String, String> params, boolean flag) {
        clearMessagelog();
        try {
            if (!checkIsContainerStarted())
                return;
            if (!checkIsBundleStarted(bundleId))
                return;

            rcsResourceContainer.addResourceConfig(bundleId, resourceUri,
                    params);

            m_successfullMessage = "Resources(" + resourceUri + ")"
                    + " has been added successfully to Bundle ";
            m_successfullMessage = m_successfullMessage + "ID : " + bundleId
                    + "\n";

            if (flag)
                showOutPut(m_successfullMessage);
        } catch (IllegalArgumentException ex) {
            m_errorMessage = "IllegalArgument exception occurred: "
                    + ex.getMessage();
            if (flag)
                showOutPut(m_errorMessage);
        } catch (Exception e) {
            m_errorMessage = e.getMessage();
            if (flag)
                showOutPut(m_errorMessage);
        }
    }

    public void rcRemoveResourceConfig(String bundleId, String resourceUri) {
        clearMessagelog();
        try {
            if (!checkIsContainerStarted())
                return;
            if (isBundleStarted) {
                rcsResourceContainer
                        .removeResourceConfig(bundleId, resourceUri);

                m_successfullMessage = "Resources(" + resourceUri + ")"
                        + " has been removed successfully from Bundle ";
                m_successfullMessage = m_successfullMessage + "ID : "
                        + bundleId + "\n";

                showOutPut(m_successfullMessage);
            }

        } catch (IllegalArgumentException ex) {
            m_errorMessage = "IllegalArgument exception occurred: "
                    + ex.getMessage();
            showOutPut(m_errorMessage);
        } catch (Exception e) {
            m_errorMessage = e.getMessage();
            showOutPut(m_errorMessage);
        }
    }

    public void rcRegisterResource(String bundleId, BundleResource resource,
            boolean flag) {
        clearMessagelog();
        try {
            if (!checkIsContainerStarted())
                return;
            if (!checkIsBundleStarted(bundleId))
                return;

            rcsResourceContainer.registerResource(bundleId, resource);

            m_successfullMessage = "Resources(" + resource.getURI() + ")"
                    + " with bundle ID : " + bundleId
                    + " has been registered successfully.";
            if (flag)
                showOutPut(m_successfullMessage);
        } catch (IllegalArgumentException ex) {
            m_errorMessage = "IllegalArgument exception occurred: "
                    + ex.getMessage();
            if (flag)
                showOutPut(m_errorMessage);
        } catch (Exception e) {
            m_errorMessage = e.getMessage();
            if (flag)
                showOutPut(m_errorMessage);
        }
    }

    public void rcUnregisterResource(BundleResource resource, String bundleId,
            boolean flag) {
        clearMessagelog();
        try {
            if (!checkIsContainerStarted())
                return;
            if (!checkIsBundleStarted(bundleId))
                return;

            rcsResourceContainer.unregisterResource(resource);
            m_successfullMessage = "Resources(" + resource.getURI() + ")"
                    + " bundle ID : " + bundleId
                    + " has been unregistered successfully.";
            if (flag)
                showOutPut(m_successfullMessage);
        } catch (IllegalArgumentException ex) {
            m_errorMessage = "IllegalArgument exception occurred: "
                    + ex.getMessage();
            if (flag)
                showOutPut(m_errorMessage);
        } catch (Exception e) {
            m_errorMessage = e.getMessage();
            if (flag)
                showOutPut(m_errorMessage);
        }
    }

    public void rcDisplayCurrentBundleList() {
        clearMessagelog();
        try {
            if (!checkIsContainerStarted())
                return;

            List<RcsBundleInfo> bundleList = rcsResourceContainer.listBundles();

            if (bundleList.size() <= 0) {
                showOutPut("Sorry.Bundle list is empty...");
                return;
            }

            Iterator<RcsBundleInfo> it = bundleList.iterator();
            int i = 0;
            m_BundleListString = "";
            m_BundleListString = m_BundleListString + "size of bundleList : "
                    + bundleList.size() + "\n\n";

            while (it.hasNext()) {
                i++;
                RcsBundleInfo object = (RcsBundleInfo) it.next();
                m_BundleListString += "Bundle : " + i + " -: \n";
                m_BundleListString += "ID : " + object.getID() + "\n";
                m_BundleListString += "Lib Path: " + object.getPath() + "\n";
                if (!(object.getVersion().equalsIgnoreCase("")))
                    m_BundleListString += "version : " + object.getVersion()
                            + "\n";
                else
                    m_BundleListString += "\n";
            }

            showOutPut(m_BundleListString);
        } catch (Exception e) {
            m_errorMessage = e.getMessage();
            showOutPut(m_errorMessage);
        }
    }

    public void rcDisplayCurrentResourceList(String bundleId) {
        clearMessagelog();
        try {
            if (!checkIsContainerStarted())
                return;

            if (checkBundleRegistered(bundleId)) {
                List<String> bundleResources = rcsResourceContainer
                        .listBundleResources(bundleId);
                if (bundleResources.size() <= 0) {
                    showOutPut("Sorry.Bundle Resource list is empty...");
                    return;
                }
                
                m_ResourceListString = "";
               
                for(String element: bundleResources)
                {
                    m_ResourceListString += element + "\n";
                }
                
                showOutPut(m_ResourceListString);
            } else {
                showOutPut("Bundle \'" + bundleId + "\' is not registered.\n");
            }

        } catch (IllegalArgumentException ex) {
            m_errorMessage = "IllegalArgument exception occurred: "
                    + ex.getMessage();
            showOutPut(m_errorMessage);
        } catch (Exception e) {
            m_errorMessage = e.getMessage();
            showOutPut(m_errorMessage);
        }
    }

    public void rcExitTestApp() {
        clearAll();
        int pid = android.os.Process.myPid();
        android.os.Process.killProcess(pid);
    }

    // ****************************************** Local function
    // ***********************************//

    private boolean checkIsContainerStarted() {
        if (isContainerStarted) {
            return true;
        } else {
            m_errorMessage = "Please Start Container before perform this action...\n";
            showOutPut(m_errorMessage);
            return false;
        }
    }

    private boolean checkIsBundleStarted(String bundleId) {
        if (checkBundleRegistered(bundleId) && (intBundleStartCounter <= 2)) {
            return true;
        } else {
            m_errorMessage = "Please Start Bundle before perform this action...\n";
            showOutPut(m_errorMessage);
            return false;
        }
    }

    private boolean checkBundleRegistered(String bundleId) {
        if (!checkIsContainerStarted())
            return false;

        List<RcsBundleInfo> bundleList = rcsResourceContainer.listBundles();
        Iterator<RcsBundleInfo> iterator = bundleList.iterator();

        while (iterator.hasNext()) {
            if (iterator.next().getID().equalsIgnoreCase(bundleId))
                return true;
        }

        return false;
    }

    private void clearMessagelog() {
        m_errorMessage = "";
        m_successfullMessage = "";
    }

    private void clearAll() {
        if (isContainerStarted)
            rcsResourceContainer.stopContainer();
        clearMessagelog();
        isContainerStarted = false;
        isBundleStarted = false;
        m_errorMessage = "";
        m_BundleListString = "";
        intBundleStartCounter = 0;
    }
}