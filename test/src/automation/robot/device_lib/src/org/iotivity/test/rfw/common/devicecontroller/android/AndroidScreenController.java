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

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.StringTokenizer;
import java.util.logging.Logger;

import org.iotivity.test.rfw.common.devicecontroller.datamodel.Coordinate;
import org.iotivity.test.rfw.common.devicecontroller.datamodel.KeyEventType;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IScreenController;
import org.iotivity.test.rfw.common.util.IoTivityLogger;
import org.iotivity.test.rfw.common.util.SystemUtil;

import org.iotivity.test.rfw.common.devicecontroller.android.AndroidCommand;

class AndroidScreenController implements IScreenController {

    private static final long MANAGER_CREATE_TIMEOUT_MS   = 30 * 1000;    // 30
                                                                          // seconds
    private static final long MANAGER_CREATE_WAIT_TIME_MS = 1000;         // wait
                                                                          // 1
                                                                          // second
    private static Logger     LOG                         = IoTivityLogger
            .GetLogger();

    private Socket            monkeySocket;
    private BufferedWriter    monkeyWriter;
    private BufferedReader    monkeyReader;

    private final int         MoveStep                    = 10;
    private final int         MoveTime                    = 30;

    public AndroidScreenController(int port) throws Exception {
        try {

            InetAddress addr;
            addr = InetAddress.getByName(AndroidCommand.LOCAL_IP_ADDRESS);
            boolean success = false;
            long start = System.currentTimeMillis();
            while (!success) {
                long now = System.currentTimeMillis();
                long diff = now - start;
                if (diff > MANAGER_CREATE_TIMEOUT_MS) {
                    LOG.severe(
                            "Timeout while trying to create monkey mananger");
                }

                SystemUtil.sleep(MANAGER_CREATE_WAIT_TIME_MS);
                this.monkeySocket = new Socket(addr, port);
                this.monkeySocket.setSoTimeout(5000);
                monkeyWriter = new BufferedWriter(
                        new OutputStreamWriter(monkeySocket.getOutputStream()));
                monkeyReader = new BufferedReader(
                        new InputStreamReader(monkeySocket.getInputStream()));
                success = true;
                SystemUtil.sleep(MANAGER_CREATE_WAIT_TIME_MS);
            }

        } catch (IOException e) {
            throw new Exception("Fail to make monkey connection");
        }
    }

    @Override
    public boolean touchDown(Coordinate coordinate) throws IOException {
        return sendMonkeyEvent("touch down " + (int) coordinate.getX() + " "
                + (int) coordinate.getY());
    }

    @Override
    public boolean touchUp(Coordinate coordinate) throws IOException {
        return sendMonkeyEvent("touch up " + (int) coordinate.getX() + " "
                + (int) coordinate.getY());
    }

    @Override
    public boolean touchMove(Coordinate coordinate) throws IOException {
        return sendMonkeyEvent("touch move " + (int) coordinate.getX() + " "
                + (int) coordinate.getY());
    }

    @Override
    public boolean touch(Coordinate coordinate) throws IOException {
        String command = "touch down " + coordinate.getX() + " "
                + coordinate.getY() + "\r\n" + "touch up " + coordinate.getX()
                + " " + coordinate.getY() + "\r\n";
        return sendMonkeyEvent(command);
    }

    @Override
    public boolean pressKey(KeyEventType key) throws IOException {
        return sendMonkeyEvent("press " + key);
    }

    @Override
    public void closeControlSocket() throws IOException {
        closeMonkey();
        monkeySocket.close();
        monkeyReader.close();
        monkeyWriter.close();
    }

    @Override
    public boolean tap(Coordinate coordinate) throws IOException {
        return sendMonkeyEvent("tap " + (int) coordinate.getX() + " "
                + (int) coordinate.getY());
    }

    @Override
    public boolean sendString(String text) throws IOException {
        // The network protocol can't handle embedded line breaks, so we have to
        // handle it
        // here instead
        StringTokenizer tok = new StringTokenizer(text, "\n", true);
        while (tok.hasMoreTokens()) {
            String line = tok.nextToken();
            if ("\n".equals(line) == false) {
                boolean success = sendMonkeyEvent("type " + line);
                if (!success) {
                    return false;
                }
            }
        }
        return true;
    }

    private String sendMonkeyEventAndGetResponse(String command)
            throws IOException {
        command = command.trim();
        // LOG.info("Monkey Command: " + command + ".");

        // send a single command and get the response
        monkeyWriter.write(command + "\n");
        monkeyWriter.flush();
        SystemUtil.sleep(1000);
        return monkeyReader.readLine();
    }

    private boolean parseResponseForSuccess(String monkeyResponse) {
        if (monkeyResponse == null) {
            return false;
        }
        if (monkeyResponse.startsWith("OK")) {
            return true;
        }

        return false;
    }

    private String parseResponseForExtra(String monkeyResponse) {
        int offset = monkeyResponse.indexOf(':');
        if (offset < 0) {
            return "";
        }
        return monkeyResponse.substring(offset + 1);
    }

    private boolean sendMonkeyEvent(String command) throws IOException {
        synchronized (this) {
            String monkeyResponse = sendMonkeyEventAndGetResponse(command);
            return parseResponseForSuccess(monkeyResponse);
        }
    }

    public String getVariable(String name) throws IOException {
        synchronized (this) {
            String response = sendMonkeyEventAndGetResponse("getvar " + name);
            if (!parseResponseForSuccess(response)) {
                return null;
            }
            return parseResponseForExtra(response);
        }
    }

    public Collection<String> listVariable() throws IOException {
        synchronized (this) {
            String response = sendMonkeyEventAndGetResponse("listvar");
            if (!parseResponseForSuccess(response)) {
                Collections.emptyList();
            }
            String extras = parseResponseForExtra(response);
            ArrayList<String> list = new ArrayList<String>();

            String[] strings = extras.split(" ");

            for (int n = 0; n < strings.length; n++) {
                list.add(strings[n]);
            }

            return list;
        }
    }

    public void wake() throws IOException {
        sendMonkeyEvent("wake");
    }

    private void closeMonkey() throws IOException {
        // this command drops the connection, so handle it here
        synchronized (this) {
            sendMonkeyEventAndGetResponse("quit");
        }
    }

    @Override
    public boolean drag(Coordinate from, Coordinate to) throws IOException {
        if (from == null || to == null)
            return false;

        float xLength = (to.getX() - from.getX()) / MoveStep;
        float yLength = (to.getY() - from.getY()) / MoveStep;

        touchDown(from);

        for (int i = 1; i <= MoveStep; i++) {

            move(new Coordinate(from.getX() + xLength * i,
                    from.getY() + yLength * i));

            SystemUtil.sleep(MoveTime);
        }

        touchUp(to);
        return true;
    }

    private boolean move(Coordinate point) throws IOException {
        return sendMonkeyEvent(
                "touch move " + point.getX() + " " + point.getY());
    }
}
