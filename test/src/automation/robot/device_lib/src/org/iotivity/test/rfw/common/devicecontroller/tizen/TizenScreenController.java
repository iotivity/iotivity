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

package org.iotivity.test.rfw.common.devicecontroller.tizen;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.logging.Logger;

import org.iotivity.test.rfw.common.devicecontroller.android.AndroidCommand;
import org.iotivity.test.rfw.common.devicecontroller.datamodel.Coordinate;
import org.iotivity.test.rfw.common.devicecontroller.datamodel.KeyEventType;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IScreenController;
import org.iotivity.test.rfw.common.devicecontroller.linux.LinuxProcess;
import org.iotivity.test.rfw.common.util.IoTivityLogger;
import org.iotivity.test.rfw.common.util.SystemUtil;

import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenCommand;
import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenSDBManager;

class TizenScreenController implements IScreenController {

    private TizenSDBManager sdbManager;
    private LinuxProcess    sendPythonManager;
    private Logger          logger = IoTivityLogger.GetLogger();

    public TizenScreenController(TizenSDBManager sdbManager) {
        this.sdbManager = sdbManager;
        initializeSendVTKeyconnection();
    }

    @Override
    public boolean touch(Coordinate point) throws IOException {
        String points = " " + (int) point.getX() + " " + (int) point.getY();
        String command = String.format("%s %s %s %s %s", TizenCommand.SHELL,
                TizenCommand.PYTHON_PATH, TizenCommand.SCREEN_CONTROLLER,
                "touchDown", points);
        getSdbManager().executeAsyncCommand(command, null);
        // touchUp doesn't work
        command = String.format("%s %s %s %s %s", TizenCommand.SHELL,
                TizenCommand.PYTHON_PATH, TizenCommand.SCREEN_CONTROLLER,
                "touchUp", points);
        // getSdbManager().executeAsyncCommand(command, null);
        executeCommand(command);
        return false;
    }

    @Override
    public boolean touchDown(Coordinate point) throws IOException {
        String points = " " + (int) point.getX() + " " + (int) point.getY();
        String command = String.format("%s %s %s %s %s", TizenCommand.SHELL,
                TizenCommand.PYTHON_PATH, TizenCommand.SCREEN_CONTROLLER,
                "touchDown", points);
        getSdbManager().executeAsyncCommand(command, null);
        return true;
    }

    @Override
    public boolean touchUp(Coordinate point) throws IOException {
        String points = " " + (int) point.getX() + " " + (int) point.getY();
        // this touchUp doesn't work
        String command = String.format("%s %s %s %s %s", TizenCommand.SHELL,
                TizenCommand.PYTHON_PATH, TizenCommand.SCREEN_CONTROLLER,
                "touchUp", points);
        getSdbManager().executeAsyncCommand(command, null);
        return false;
    }

    @Override
    public boolean touchMove(Coordinate point) throws IOException {
        String points = " " + (int) point.getX() + " " + (int) point.getY();
        // this move doesn't work
        String command = String.format("%s %s %s %s %s", TizenCommand.SHELL,
                TizenCommand.PYTHON_PATH, TizenCommand.SCREEN_CONTROLLER,
                "move", points);
        // getSdbManager().executeAsyncCommand(command, null);
        // SystemUtil.sleep(10000);

        executeCommand(command);
        return false;
    }

    @Override
    public boolean pressKey(KeyEventType key) throws IOException {
        String keyStr = "";
        switch (key) {
            case Home:
                keyStr = "KEY_HOME";
                break;
            case Back:
                keyStr = "KEY_BACK";
                break;
            case VolumeUp:
                keyStr = "KEY_VOLUMEUP";
                break;
            case VolumeDown:
                keyStr = "KEY_VOLUMEDOWN";
                break;
            case Power:
                keyStr = "KEY_POWER";
                break;
            case Menu:
                keyStr = "KEY_MENU";
                break;
            case RecentApps:
                keyStr = "KEY_HOME";
                break;
            case None:
            default:
                return false;
        }
        String pythonCommand = "sendKey";
        if (KeyEventType.RecentApps == key)
            pythonCommand = "sendLongKey";

        String command = String.format("%s %s %s %s %s", TizenCommand.SHELL,
                TizenCommand.PYTHON_PATH, TizenCommand.SCREEN_CONTROLLER,
                pythonCommand, keyStr);
        // getSdbManager().executeAsyncCommand(command, null);
        // SystemUtil.sleep(10000);
        executeCommand(command);
        return true;
    }

    private TizenSDBManager getSdbManager() {
        return sdbManager;
    }

    private boolean initializeSendVTKeyconnection() {
        try {

            String command = String.format("%s %s %s %s", TizenCommand.SDB,
                    AndroidCommand.SPECOPTION, this.sdbManager.getDeviceID(),
                    TizenCommand.SHELL);
            logger.info(command);

            sendPythonManager = new LinuxProcess("SENDPYTHON", command);
            sendPythonManager.inputCommand("cd /var/python/bin");
            Thread.sleep(1000);
            sendPythonManager.inputCommand("chmod 777 *");
            Thread.sleep(1000);
            sendPythonManager.inputCommand("./python2.7");
            Thread.sleep(1000);
            sendPythonManager.inputCommand("import fmbttizenagent");
            Thread.sleep(1000);
            return true;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        return false;
    }

    @Override
    public boolean sendString(String text) throws IOException {
        try {
            if (!sendPythonManager.isProcessRunning())
                initializeSendVTKeyconnection();
            sendPythonManager.inputCommand(
                    "fmbttizenagent.typeCharHw(\"" + text + "\")");
            Thread.sleep(2000);
            return true;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        return false;
    }

    @Override
    public boolean tap(Coordinate point) throws IOException {
        String points = " " + (int) point.getX() + " " + (int) point.getY();

        String command = String.format("%s %s %s %s %s", TizenCommand.SHELL,
                TizenCommand.PYTHON_PATH, TizenCommand.SCREEN_CONTROLLER, "tap",
                points);
        logger.info(command);

        // getSdbManager().executeAsyncCommand(command, null);
        executeCommand(command);
        return true;
    }

    /*
     * @Override public void openControlSocket(Socket socket) throws IOException
     * { }
     */
    @Override
    public void closeControlSocket() throws IOException {
        sendPythonManager.destroy();
    }

    @Override
    public boolean drag(Coordinate from, Coordinate to) throws IOException {
        String points = " " + (int) from.getX() + " " + (int) from.getY() + " "
                + (int) to.getX() + " " + (int) to.getY();
        String command = String.format("%s %s %s %s %s", TizenCommand.SHELL,
                TizenCommand.PYTHON_PATH, TizenCommand.SCREEN_CONTROLLER,
                "swipe", points);
        logger.info(command);

        // getSdbManager().executeAsyncCommand(command, null);
        executeCommand(command);
        return true;
    }

    public void executeCommand(String command) {
        try {
            command = "sdb -s " + sdbManager.getDeviceID() + " " + command;
            logger.info(command);
            Process process = Runtime.getRuntime().exec(command);
            BufferedReader in = new BufferedReader(
                    new InputStreamReader(process.getInputStream()));
            SystemUtil.sleep(1000);
            String line;
            while ((line = in.readLine()) != null) {
                logger.info("ROOT ON output : " + line);
            }
            SystemUtil.sleep(1000);
        } catch (IOException e) {
            e.printStackTrace();
            logger.info("Exception occured in executing command " + command
                    + "ERROR::::" + e.toString());
        }
    }

}
