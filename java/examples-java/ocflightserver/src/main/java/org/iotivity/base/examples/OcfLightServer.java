/*
 *******************************************************************
 *
 * Copyright 2018 Intel Corporation.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples;

import org.iotivity.base.ModeType;
import org.iotivity.base.OcException;
import org.iotivity.base.OcPlatform;
import org.iotivity.base.OcPlatformInfo;
import org.iotivity.base.OcResourceHandle;
import org.iotivity.base.PayloadType;
import org.iotivity.base.PlatformConfig;
import org.iotivity.base.QualityOfService;
import org.iotivity.base.ServiceType;

import java.awt.FlowLayout;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.File;
import java.net.URISyntaxException;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.TimeZone;
import java.util.UUID;

import javax.swing.JFrame;
import javax.swing.WindowConstants;

/**
 * OcfLightServer
 */
public class OcfLightServer {

    static Light light;
    static String name;
    static boolean powerOn = true;
    static int dimmingValue = 100;

    static LightControlPanel lightControlPanel;

    public static void main(String args[]) throws IOException, InterruptedException {

        parseNameAndInitialSettings(args);

        File jarFile = null;
        try {
            jarFile = new File(OcfLightServer.class.getProtectionDomain().getCodeSource().getLocation().toURI().getPath());

        } catch (URISyntaxException e) {
            OcfLightServer.msgError("Failed to find jar file path.");
            e.printStackTrace();
            System.exit(-1);
        }

        String jarFilePath = jarFile.getParent();
        jarFilePath = (jarFilePath != null) ? jarFilePath : "";

        File securityFile = new File(jarFilePath+"/ocflightserver_security.dat");
        if (! (securityFile.exists() && securityFile.isFile())) {
            OcfLightServer.msgError("Security file " + securityFile.getAbsolutePath() + " not found");
            System.exit(-1);
        }

        File introspectionFile = new File(jarFilePath+"/ocflightserver_introspection.dat");
        if (! (introspectionFile.exists() && introspectionFile.isFile())) {
            OcfLightServer.msgError("Introspection file " + introspectionFile.getAbsolutePath() + " not found");
            System.exit(-1);
        }

        PlatformConfig platformConfig = new PlatformConfig(ServiceType.IN_PROC, ModeType.SERVER,
                QualityOfService.LOW, securityFile.getAbsolutePath(), introspectionFile.getAbsolutePath());

        OcPlatform.Configure(platformConfig);

        msg("Platform configured");

        String uuid = null;
        if (NamesPropertyFile.getInstance().hasName(name)) {
            uuid = NamesPropertyFile.getInstance().getUuidForName(name);
        } else {
            uuid = UUID.randomUUID().toString();
            NamesPropertyFile.getInstance().updateNamesProperty(name, uuid);
        }

        TimeZone timeZone = TimeZone.getTimeZone("UTC");
        DateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss'Z'");
        dateFormat.setTimeZone(timeZone);
        String nowAsIso8601 = dateFormat.format(new Date());

        OcPlatformInfo platformInfo = new OcPlatformInfo(
                uuid,
                "manufacturerName",
                "http://www.manufacturer.com",
                "modelNumber",
                "2018-03-15",
                "1.0",
                "1.0",
                "1.0",
                "1.0",
                "http://www.support.com",
                nowAsIso8601
        );

        try {
            msg("Registering platform info");
            OcPlatform.registerPlatformInfo(platformInfo);

        } catch (OcException e) {
            OcfLightServer.msgError("Failed to register platform info.");
            e.printStackTrace();
        }

        try {
            OcPlatform.setPropertyValue(PayloadType.DEVICE.getValue(), "n", name);
            OcPlatform.setPropertyValue(PayloadType.DEVICE.getValue(), "piid", uuid);
            OcPlatform.setPropertyValue(PayloadType.DEVICE.getValue(), "icv", "ocf.1.0.0");
            OcPlatform.setPropertyValue(PayloadType.DEVICE.getValue(), "dmv", "ocf.res.1.3.0,ocf.sh.1.3.0");

        } catch (OcException e) {
            OcfLightServer.msgError("Failed to create device properties");
            e.printStackTrace();
        }

        try {
            OcResourceHandle deviceResourceHandle = OcPlatform.getResourceHandleAtUri(OcPlatform.WELL_KNOWN_DEVICE_QUERY);
            if (deviceResourceHandle != null) {
                OcPlatform.bindTypeToResource(deviceResourceHandle, Light.RESOURCE_TYPE);
            }

        } catch (OcException e) {
            OcfLightServer.msgError("Failed to bind device type to /oic/d resource");
            e.printStackTrace();
        }

        JFrame frame = new JFrame(name);
        frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);

        frame.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                if (light != null) {
                    light.unregister();
                }
                msg("Shutdown");
                e.getWindow().dispose();
                System.exit(0);
            }
        });

        frame.setResizable(true);
        frame.setLayout(new FlowLayout());

        lightControlPanel = new LightControlPanel(powerOn, dimmingValue);
        light = new Light(name, uuid, powerOn, dimmingValue, lightControlPanel);

        frame.setContentPane(lightControlPanel);
        frame.pack();
        frame.setVisible(true);
    }

    private static void parseNameAndInitialSettings(String args[]) {
        if (args.length > 0) {
            name = args[0];
        }

        if (args.length > 1) {
            String arg = args[1];
            if (!arg.isEmpty()) {
                powerOn = arg.equalsIgnoreCase("true") || arg.equalsIgnoreCase("on") || arg.equalsIgnoreCase("yes")
                        || arg.equals("1");
            }
        }

        if (args.length > 2) {
            try {
                dimmingValue = Integer.valueOf(args[2]);
            } catch (NumberFormatException e) {
                msg("Dimming setting must be an integer in the range (0, 100), using default 100.");
            }

            dimmingValue = Math.max(0, dimmingValue);
            dimmingValue = Math.min(100, dimmingValue);
        }

        if (name == null || name.isEmpty()) {
            name = "Light " + (System.currentTimeMillis() % 10000);
        }
    }

    public static boolean useSlowResponse() {
        return lightControlPanel.useSlowResponse();
    }

    public static void msg(final String text) {
        DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
        Date date = new Date();
        System.out.println(dateFormat.format(date) + " " + text);
    }

    public static void msgError(final String text) {
        msg("[Error] " + text);
    }
}
