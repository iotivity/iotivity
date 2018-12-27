/******************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 *
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
 *
 ******************************************************************/

package org.iotivity.test.rfw.common.devicecontroller.android;

import java.io.IOException;
import java.net.ServerSocket;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.IDeviceBridgeManager;
import org.iotivity.test.rfw.common.devicecontroller.linux.LinuxProcessManager;
import org.iotivity.test.rfw.common.util.SystemUtil;
import org.iotivity.test.rfw.common.devicecontroller.android.AndroidCommand;
import org.iotivity.test.rfw.common.devicecontroller.android.AndroidScreenController;
import org.iotivity.test.rfw.common.devicecontroller.android.CommandOutputCapture;

import com.android.ddmlib.AdbCommandRejectedException;
import com.android.ddmlib.AndroidDebugBridge;
import com.android.ddmlib.IDevice;
import com.android.ddmlib.IShellOutputReceiver;
import com.android.ddmlib.InstallException;
import com.android.ddmlib.ShellCommandUnresponsiveException;
import com.android.ddmlib.TimeoutException;

class AndroidADBManager implements IDeviceBridgeManager {
    private static final int          CONNECTION_ITERATION_TIMEOUT_MS = 200;
    private static final int          CONNECTION_TIMEOUT_MS           = 20000;

    private CommandOutputCapture      capture;
    private final ExecutorService     executor                        = Executors
            .newCachedThreadPool();
    private Future<String>            executorResult;
    private final IDevice             device;
    private IDevice                   androidDevice;
    private AndroidScreenController   manager;
    private static AndroidDebugBridge bridge                          = null;
    private ServerSocket              monkeySocket;
    private LinuxProcessManager       adbProcessManager;
    private int                       monkeyPort;

    public AndroidADBManager(String deviceIdRegex) throws Exception {
        capture = new CommandOutputCapture();
        adbProcessManager = new LinuxProcessManager();
        startServer();

        this.device = getConnectedDevice(CONNECTION_TIMEOUT_MS, deviceIdRegex);
        if (this.device == null)
            throw new Exception("fail to connect device");

        monkeySocket = new ServerSocket(0);
        monkeyPort = monkeySocket.getLocalPort();
        monkeySocket.close();

        createMonkeyConnection(AndroidCommand.LOCAL_IP_ADDRESS, monkeyPort);
    }

    public int getMonkeyPort() {
        return monkeyPort;
    }

    public IDevice getDevice() {
        return this.device;
    }

    private IDevice findAttacedDevice(String deviceId) {
        for (IDevice device : bridge.getDevices()) {
            String serialNumber = device.getSerialNumber();
            if (deviceId.equals(serialNumber)) {
                return device;
            }
        }

        return null;
    }

    public IDevice getConnectedDevice(long timeoutMs, String deviceIdRegex) {
        do {
            IDevice device = findAttacedDevice(deviceIdRegex);
            // Only return the device when it is online
            if (device != null
                    && device.getState() == IDevice.DeviceState.ONLINE)
                return device;

            SystemUtil.sleep(CONNECTION_ITERATION_TIMEOUT_MS);

            timeoutMs -= CONNECTION_ITERATION_TIMEOUT_MS;
        } while (timeoutMs > 0);

        return null;
    }

    @Override
    public void startServer() {

        String command = String.format("%s %s", AndroidCommand.ADB,
                AndroidCommand.START_ADB_SERVER);
        adbProcessManager.createProcess(AndroidCommand.START_ADB_SERVER,
                command);
        SystemUtil.sleep(3000);

        if (bridge == null) {
            try {
                AndroidDebugBridge.initIfNeeded(false);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        if (bridge == null || !bridge.isConnected()) {
            bridge = AndroidDebugBridge.createBridge(AndroidCommand.ADB, true);
        }

        // Port should be unused port
    }

    @Override
    public void stopServer() {
        if (androidDevice != null)
            try {
                AndroidDebugBridge.disconnectBridge();
                manager.closeControlSocket();
            } catch (IOException e) {
                e.printStackTrace();
            }

        androidDevice = null;
        String command = String.format("%s %s", AndroidCommand.ADB,
                AndroidCommand.KILL_ADB_SERVER);
        adbProcessManager.createProcess(AndroidCommand.KILL_ADB_SERVER,
                command);

        SystemUtil.sleep(3000);
    }

    @Override
    public void executeAsyncCommand(final String command,
            final IShellOutputReceiver logger) {
        executorResult = executor.submit(new Callable<String>() {
            @Override
            public String call() throws Exception {
                try {
                    device.executeShellCommand(command, logger);
                } catch (TimeoutException e) {
                    throw new RuntimeException(e);
                } catch (AdbCommandRejectedException e) {
                    throw new RuntimeException(e);
                } catch (ShellCommandUnresponsiveException e) {
                    // This happens a lot
                    throw new RuntimeException(e);
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
                return logger.toString();
            }
        });
    }

    private void initialMonkey() {
        CommandOutputCapture logger = new CommandOutputCapture();
        String psList = "";
        executeAsyncCommand(AndroidCommand.PROCESS, logger);

        try {
            psList = executorResult.get();
        } catch (InterruptedException | ExecutionException e) {
            e.printStackTrace();
        }

        String[] result = psList.split("\n");
        for (String strPrint : result) {
            if (strPrint.contains("com.android.commands.monkey")) {
                Pattern pattern = Pattern.compile(
                        "([^ \\t\\r\\n\\v\\f]+)(\\s+)([^ \\t\\r\\n\\v\\f]+)(\\s+)([^ \\t\\r\\n\\v\\f]+)(\\s+)([^ \\t\\r\\n\\v\\f]+)(\\s+)([^ \\t\\r\\n\\v\\f]+)(\\s+)([^ \\t\\r\\n\\v\\f]+)(\\s+)([^ \\t\\r\\n\\v\\f]+)(\\s+)([^ \\t\\r\\n\\v\\f]+)(\\s+)([^ \\t\\r\\n\\v\\f]+)(\\s*)");

                if (strPrint.matches(pattern.pattern())) {
                    Matcher matcher = pattern.matcher(strPrint);

                    if (matcher.find()) {
                        executeAsyncCommand("kill " + matcher.group(3), logger);
                        break;
                    }
                }
            }
        }
    }

    private void createMonkeyConnection(String address, int port) {
        try {
            initialMonkey();
            device.createForward(port, port);
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        String command = "monkey --port " + port;
        executeAsyncCommand(command, new CommandOutputCapture());

        SystemUtil.sleep(1000);
    }

    private String shell(String cmd) {
        try {
            device.executeShellCommand(cmd, capture);
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }

        return capture.toString();
    }

    @Override
    public boolean installApplication(String path) {
        try {
            String result = device.installPackage(path, true);
            if (result != null) {
                return false;
            }

            return true;
        } catch (InstallException e) {
            e.printStackTrace();
            return false;
        }
    }

    @Override
    public void reboot(String into) {
        try {
            device.reboot(into);
        } catch (TimeoutException e) {
            e.printStackTrace();
        } catch (AdbCommandRejectedException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void startService(String serviceName, String className) {
        if (className.contains(".")) {
            shell("am start -n " + serviceName + "/" + className);
        } else {
            shell("am start -a " + "android.intent.action.MAIN" + " -n "
                    + serviceName + "/." + className);
        }
    }

    @Override
    public void stopService(String serviceName) {
        shell("am force-stop " + serviceName);
    }

    @Override
    public boolean isInstalledApp(String appName) {
        String result = shell("pm list packages -f " + appName);

        if (result.contains("package"))
            return true;

        return false;
    }

    @Override
    public boolean isRunningApp(String appName) {
        String result = shell("shell ps | find \"" + appName + "\"");

        if (result.contains(appName))
            return true;

        return false;
    }

    @Override
    public boolean unInstallApp(String packageName) {
        try {
            String result = device.uninstallPackage(packageName);
            if (result != null) {
                return false;
            }
            return true;
        } catch (InstallException e) {
            e.printStackTrace();
            return false;
        }
    }

    @Override
    public boolean connectWifiDevice() {
        return false;
    }
}
