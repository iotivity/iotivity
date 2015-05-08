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

/// @file FelixManager.java

package org.iotivity.service.ppm;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Dictionary;
import java.util.HashMap;
import java.util.Map;

import org.apache.felix.framework.Felix;
import org.osgi.framework.BundleContext;
import org.osgi.framework.BundleException;
import org.osgi.framework.Constants;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.FileObserver;
import android.util.Log;

public class FelixManager {
    private static Felix                   m_felix                    = null;
    private static ArrayList<FileObserver> m_observer;
    private static FelixManager            m_felixmgr;
    private static Context                 m_context;
    private static final int               TRUE                       = 1;
    private static final int               FALSE                      = 0;

    static final String                    ANDROID_FRAMEWORK_PACKAGES = ("android,"
                                                                  + "android.app,"
                                                                  + "android.content,"
                                                                  + "android.content.pm,"
                                                                  + "android.content.res,"
                                                                  + "android.database,"
                                                                  + "android.database.sqlite,"
                                                                  + "android.graphics,"
                                                                  + "android.graphics.drawable,"
                                                                  + "android.graphics.glutils,"
                                                                  + "android.hardware,"
                                                                  + "android.location,"
                                                                  + "android.media,"
                                                                  + "android.net,"
                                                                  + "android.net.wifi,"
                                                                  + "android.net.http,"
                                                                  + "android.opengl,"
                                                                  + "android.os,"
                                                                  + "android.provider,"
                                                                  + "android.sax,"
                                                                  + "android.speech.recognition,"
                                                                  + "android.telephony,"
                                                                  + "android.telephony.gsm,"
                                                                  + "android.text,"
                                                                  + "android.text.format,"
                                                                  + "android.text.method,"
                                                                  + "android.text.style,"
                                                                  + "android.text.util,"
                                                                  + "android.util,"
                                                                  + "android.view,"
                                                                  + "android.view.animation,"
                                                                  + "android.webkit,"
                                                                  + "android.widget,"
                                                                  + "javax.xml.parsers,"
                                                                  + "org.xml.sax,"
                                                                  + "org.w3c.dom,"
                                                                  + "com.example.felixmanager,"
                                                                  + "org.iotivity.base");

    // FelixManager singleton
    public static FelixManager getInstance(Context ctx) {
        if (m_felixmgr == null) {
            m_felixmgr = new FelixManager(ctx);
            copyFiles("files");
        }

        return m_felixmgr;
    }

    public static void LogEx(String info) {
        Log.d("felix", info);
    }

    private FelixManager(Context ctx) {
        m_context = ctx;
        m_observer = new ArrayList<FileObserver>();

        Map configMap = new HashMap();
        String mCacheDir = ctx.getDir("org.osgi.framework.storage",
                Context.MODE_WORLD_WRITEABLE).toString();
        configMap.put("org.osgi.framework.storage", mCacheDir);
        configMap.put("felix.embedded.execution", "true");
        configMap.put("org.osgi.service.http.port", "9990");
        configMap.put("org.osgi.framework.startlevel.beginning", "5");
        configMap.put("felix.bootdelegation.implicit", "false");
        configMap.put("felix.service.urlhandlers", "false");
        configMap.put(Constants.FRAMEWORK_SYSTEMPACKAGES_EXTRA,
                ANDROID_FRAMEWORK_PACKAGES);
        try {
            m_felix = new Felix(configMap);
            m_felix.init();
            m_felix.start();

            for (org.osgi.framework.Bundle b : m_felix.getBundleContext()
                    .getBundles()) {
                LogEx("Bundle: " + b.getSymbolicName());
            }
        } catch (Throwable ex) {
            Log.d("Felix", "could not create framework: " + ex.getMessage(), ex);
        }
    }

    public static int registerPlugin(String path) {

        int flag = FALSE;

        flag = installPlugin(path);
        if (flag == TRUE)
            flag = loadPluginInfoToManager(path);

        return flag;
    }

    public static int registerAllPlugin(String path) {

        int flag = FALSE;

        flag = findPluginRecursive(path);
        if (flag == TRUE)
            flag = loadPluginInfoToManager(path);

        return flag;
    }

    public static int unregisterPlugin(String id) {
        int flag = TRUE;
        try {
            BundleContext bContext = m_felix.getBundleContext();
            org.osgi.framework.Bundle[] bundles = bContext.getBundles();
            for (org.osgi.framework.Bundle b : bundles) {
                if (b.getSymbolicName().equals(id)) {
                    Log.d("Felix", "bundle: " + b.getBundleId()
                            + "   symbolicName : " + b.getSymbolicName());
                    b.uninstall();
                    Log.d("Felix", "uninstall end");
                }
            }
        } catch (BundleException e) {
            e.printStackTrace();
            flag = FALSE;
        }

        return flag;
    }

    public static int unregisterAllPlugin() {
        int flag = TRUE;
        try {
            BundleContext bContext = m_felix.getBundleContext();
            org.osgi.framework.Bundle[] bundles = bContext.getBundles();
            for (org.osgi.framework.Bundle b : bundles) {
                if (!b.getSymbolicName().equals("org.apache.felix.framework")) {
                    Log.d("Felix", "bundle: " + b.getBundleId()
                            + "   symbolicName : " + b.getSymbolicName());
                    b.uninstall();
                    Log.d("Felix", "uninstall end");
                }
            }
        } catch (BundleException e) {
            e.printStackTrace();
            flag = FALSE;
        }

        return flag;
    }

    public static org.osgi.framework.Bundle[] getAllPlugins() {
        try {
            BundleContext bContext = m_felix.getBundleContext();
            org.osgi.framework.Bundle[] bundles = bContext.getBundles();
            return bundles;
        } catch (Exception e) {
            e.printStackTrace();
        }

        return null;
    }

    public static org.osgi.framework.Bundle[] findPulgins(String key,
            String value) {
        BundleContext bContext = m_felix.getBundleContext();
        org.osgi.framework.Bundle[] bundles = bContext.getBundles();

        return bundles;
    }

    public static org.osgi.framework.Bundle getPlugin(int ID) {
        BundleContext bContext = m_felix.getBundleContext();
        org.osgi.framework.Bundle[] bundles = bContext.getBundles();
        for (int i = 0; i < bundles.length; i++) {
            if (bundles[i].getBundleId() == ID) {
                return bundles[i];
            }
        }
        return null;
    }

    public static int start(String id) {
        int flag = TRUE;
        Log.d("Felix", "String id : " + id);
        try {
            BundleContext bContext = m_felix.getBundleContext();
            bContext.registerService(Context.class.getName(), m_context, null);

            org.osgi.framework.Bundle[] bundles = bContext.getBundles();
            for (org.osgi.framework.Bundle b : bundles) {
                Log.d("Felix", "symbolicName : " + b.getSymbolicName());
                if (b.getSymbolicName().equals(id)) {
                    Log.d("Felix", "bundle: " + b.getBundleId()
                            + "   symbolicName : " + b.getSymbolicName());
                    b.start();
                    Log.d("Felix", "start end");
                }
            }
        } catch (BundleException e) {
            e.printStackTrace();
            flag = FALSE;
        }
        return flag;
    }

    public static int stop(String id) {
        int flag = TRUE;
        try {
            BundleContext bContext = m_felix.getBundleContext();
            org.osgi.framework.Bundle[] bundles = bContext.getBundles();
            for (org.osgi.framework.Bundle b : bundles) {
                if (b.getSymbolicName().equals(id)) {
                    Log.d("Felix", "bundle: " + b.getBundleId()
                            + "   symbolicName : " + b.getSymbolicName());
                    b.stop();
                    b.uninstall();
                    Log.d("Felix", "stop end");
                }
            }
        } catch (BundleException e) {
            e.printStackTrace();
            flag = FALSE;
        }
        return flag;
    }

    public static boolean isStarted(String name) {
        String state = getState(name);
        if (state.equals("ACTIVE") || state.equals("STARTING"))
            return true;
        return false;
    }

    public static String getValue(String name, String key) {
        Log.d("FELIX", "getValue");
        BundleContext bContext = m_felix.getBundleContext();
        org.osgi.framework.Bundle[] bundles = bContext.getBundles();
        for (org.osgi.framework.Bundle b : bundles) {
            Dictionary<String, String> dic = b.getHeaders();
            String bundlename = b.getSymbolicName();
            Log.d("FELIX", "Bundlename: " + bundlename);

            if (bundlename.equals(name)) {
                if (dic.get("Bundle-" + key) == null) {
                    Log.d("FELIX", name + " null");
                    return "";
                }
                Log.d("FELIX", name + " " + dic.get("Bundle-" + key));
                return dic.get("Bundle-" + key);
            }
        }
        return "";
    }

    public static String getState(String name) {
        Log.d("FELIX", "getState");
        BundleContext bContext = m_felix.getBundleContext();
        org.osgi.framework.Bundle[] bundles = bContext.getBundles();
        for (org.osgi.framework.Bundle b : bundles) {
            Dictionary<String, String> dic = b.getHeaders();
            String bundlename = b.getSymbolicName();

            if (bundlename.equals(name)) {
                Log.d("FELIX", state_to_string(b.getState()));
                return state_to_string(b.getState());
            }
        }
        Log.d("FELIX", "null");
        return "";
    }

    public static String printPluginList() {
        String str = "";
        str += "id   |     state     |    symbolname | ";
        BundleContext bContext = m_felix.getBundleContext();
        org.osgi.framework.Bundle[] bundles = bContext.getBundles();
        for (org.osgi.framework.Bundle b : bundles) {
            str += "\n" + b.getBundleId() + "  "
                    + state_to_string(b.getState()) + " " + b.getSymbolicName();
        }
        return str;
    }

    public static void stop_felix() {
        try {
            m_felix.stop();
            m_felix.waitForStop(0);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static ArrayList<String> getFileList(String path, boolean recursive) {
        File file = new File(path);
        File[] files = file.listFiles();
        ArrayList<String> array = new ArrayList<String>();
        for (int i = 0; i < files.length; i++) {
            if (files[i].isDirectory()) {
                if (recursive == true) {
                    ArrayList<String> directory_list = getFileList(files[i]
                            + "/", true);
                    array.addAll(directory_list);
                }
                Log.v("Directory : ", files[i].getName());
            } else {
                if (files[i].getName().endsWith("jar")) {
                    Log.v("File :", files[i].getName());
                    if (path.charAt(path.length() - 1) != '/') {
                        path += "/";
                    }
                    array.add(path + files[i].getName());
                }
            }
        }
        return array;
    }

    private static String state_to_string(int state) {
        String str = state + " [unknown state]";

        if (state == org.osgi.framework.Bundle.ACTIVE) {
            return "ACTIVE";
        } else if (state == org.osgi.framework.Bundle.INSTALLED) {
            return "INSTALLED";
        } else if (state == org.osgi.framework.Bundle.RESOLVED) {
            return "RESOLVED";
        } else if (state == org.osgi.framework.Bundle.STARTING) {
            return "STARTING";
        } else if (state == org.osgi.framework.Bundle.STOPPING) {
            return "STOPPING";
        } else if (state == org.osgi.framework.Bundle.UNINSTALLED) {
            return "UNINSTALLED";
        }
        return str;
    }

    public static int installPlugin(String path) {

        int flag = TRUE;
        if (path == "") {
            System.out.println("PluginManager path is Null\n");
        }

        ArrayList<String> filearray;
        BundleContext bContext = m_felix.getBundleContext();
        String location = "";
        FileInputStream fin;
        if (path.charAt(0) != '/') {
            path = "/" + path;
        } else if (path.charAt(path.length() - 1) != '/') {
            path = path + "/";
        }

        filearray = getFileList(path, false);

        for (int i = 0; i < filearray.size(); i++) {
            try {
                location = "file:" + filearray.get(i);
                fin = new FileInputStream(new File(filearray.get(i)));
                bContext.installBundle(location, fin);
            } catch (BundleException e) {
                e.printStackTrace();
                flag = FALSE;
            } catch (FileNotFoundException e) {
                e.printStackTrace();
                flag = FALSE;
            }
        }

        return flag;
    }

    private static String getEventString(int event) {
        switch (event) {
            case FileObserver.ACCESS:
                return "ACCESS";
            case FileObserver.MODIFY:
                return "MODIFY";
            case FileObserver.ATTRIB:
                return "ATTRIB";
            case FileObserver.CLOSE_WRITE:
                return "CLOSE_WRITE";
            case FileObserver.CLOSE_NOWRITE:
                return "CLOSE_NOWRITE";
            case FileObserver.OPEN:
                return "OPEN";
            case FileObserver.MOVED_FROM:
                return "MOVED_FROM";
            case FileObserver.MOVED_TO:
                return "MOVED_TO";
            case FileObserver.CREATE:
                return "CREATE";
            case FileObserver.DELETE:
                return "DELETE";
            case FileObserver.DELETE_SELF:
                return "DELETE_SELF";
            case FileObserver.MOVE_SELF:
                return "MOVE_SELF";
            default:
                return "UNKNOWN";
        }
    }

    public static int ObservePluginPath(String path) {
        int flag = TRUE;
        Log.d("FELIX", "ObservePluginPath" + path);

        FileObserver observer = new FileObserver(path) {
            @Override
            public void onEvent(int event, String path) {
                Log.d("FELIX", "Observing start : " + path);
                Log.d("FELIX", "Observing event : " + getEventString(event));
            }
        };
        observer.startWatching();
        m_observer.add(observer);

        return flag;
    }

    public static int findPluginRecursive(String path) {
        int flag = TRUE;
        if (path == "") {
            System.out.println("PluginManager path is Null\n");
            Log.d("FELIX", "PluginManager path is Null\n");
            flag = FALSE;
            return flag;
        }

        ArrayList<String> filearray;
        BundleContext bContext = m_felix.getBundleContext();
        String location = "";
        FileInputStream fin;

        if (path.charAt(0) != '/') {
            path = "/" + path;
        } else if (path.charAt(path.length() - 1) != '/') {
            path = path + "/";
        }

        filearray = getFileList(path, true);

        for (int i = 0; i < filearray.size(); i++) {
            try {
                location = "file:" + filearray.get(i);
                fin = new FileInputStream(new File(filearray.get(i)));
                bContext.installBundle(location, fin);
            } catch (BundleException e) {
                e.printStackTrace();
                flag = FALSE;
            } catch (FileNotFoundException e) {
                e.printStackTrace();
                flag = FALSE;
            }
        }

        return flag;
    }

    public static int loadPluginInfoToManager(String path) {
        int flag = FALSE;

        flag = ObservePluginPath(path);
        FelixManager.printPluginList();

        return flag;
    }

    public static String getPackageName() {
        String packagename;

        packagename = m_context.getPackageName();

        return packagename;
    }

    private static void copyFiles(String path) {
        AssetManager assetManager = m_context.getAssets();
        String assets[] = null;

        try {
            assets = assetManager.list(path);

            if (assets.length == 0) {
                copyFile(path);
            } else {
                String fullPath = "/data/data/"
                        + m_context.getPackageName() + "/" + path;
                Log.d("FELIX", fullPath);
                File dir = new File(fullPath);

                if (!dir.exists())
                    dir.mkdir();
                for (int i = 0; i < assets.length; ++i) {
                    copyFiles(path + "/" + assets[i]);
                }
            }
        } catch (IOException ex) {
            Log.e("tag", "I/O Exception", ex);
        }
    }

    private static void copyFile(String filename) {
        AssetManager assetManager = m_context.getAssets();
        InputStream in = null;
        OutputStream out = null;

        try {
            in = assetManager.open(filename);
            out = m_context.openFileOutput(filename.split("/")[1], Context.MODE_PRIVATE);

            byte[] buffer = new byte[1024];
            
            int read;

            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }

            in.close();
            in = null;
            out.flush();
            out.close();
            out = null;
        } catch (Exception e) {
            Log.e("tag", e.getMessage());
        }
    }
}