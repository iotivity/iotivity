/******************************************************************
 * Copyright 2017 Samsung Electronics All Rights Reserved.
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
 ******************************************************************/
package org.iotivity.test.tcpserver;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.util.ArrayList;

import org.iotivity.configuration.IConfiguration;

public class SocketMainServer extends Thread implements IConfiguration {
    private ServerSocket              mServerSocket;
    private Socket                    mClient        = null;
    private boolean                   mRunning       = false;
    private static PrintWriter        mOut;
    private OnMessageReceived         mMessageListener;
    private static ArrayList<Process> mProcessVector = new ArrayList<Process>();
    private final String              TAG            = "TCP SERVER";

    public static void main(String[] args) {
        SocketMainServer mServer;

        mServer = new SocketMainServer(
                new SocketMainServer.OnMessageReceived() {
                    @Override
                    public void messageReceived(String message) {
                        System.out.println("Msg Recieved : " + message);

                        new Thread() {
                            public void run() {

                                try {

                                    if (message.equals(KILL_ALL_SERVER)) {
                                        System.out.println("Total Process : "
                                                + mProcessVector.size());

                                        for (int i = 0; i < mProcessVector
                                                .size(); i++) {
                                            Process p = mProcessVector.get(i);
                                            if (null != p) {

                                                if (p.isAlive()) {

                                                    System.out.println(
                                                            "Process Is Alive");
                                                    p.destroy();
                                                } else {
                                                    System.out.println(
                                                            "Process Is Dead");
                                                }

                                            }

                                        }

                                        mProcessVector.clear();

                                    } else {
                                        System.out.println("File Name : "
                                                + message.split(" ")[1]);
                                        String src = "../"
                                                + message.split(" ")[1];
                                        String dest = "./"
                                                + message.split(" ")[1];
                                        File filesrc = new File(src);
                                        File fileDest = new File(dest);

                                        Files.copy(filesrc.toPath(),
                                                fileDest.toPath(),
                                                StandardCopyOption.REPLACE_EXISTING);

                                        Process server = Runtime.getRuntime()
                                                .exec(message.split(" "));
                                        mProcessVector.add(server);
                                        BufferedReader b = new BufferedReader(
                                                new InputStreamReader(server
                                                        .getInputStream()));
                                        String line;
                                        while ((line = b.readLine()) != null) {
                                            System.out.println(line);
                                        }

                                    }

                                } catch (IOException e) {
                                    // TODO Auto-generated catch block
                                    e.printStackTrace();
                                }
                            }
                        }.start();
                        sendMessage(message);
                    }
                });
        mServer.start();

        System.out.println("Server Started, now start Android Client");
    }

    /**
     * Constructor of the class
     *
     * @param mMessageListener
     *            listens for the messages
     */
    public SocketMainServer(OnMessageReceived messageListener) {
        this.mMessageListener = messageListener;
    }

    /**
     * Method to send the messages from server to mClient
     *
     * @param message
     *            the message sent by the server
     */
    public static void sendMessage(String message) {
        try {
            if (mOut != null && !mOut.checkError()) {
                System.out.println(message);
                // Here you can connect with database or else you can do what
                // you want with static message
                mOut.println(message);
                mOut.flush();
            }
        } catch (Exception e) {
        }
    }

    @Override
    public void run() {
        super.run();
        mRunning = true;
        try {
            System.out.println("PA: Connecting...");

            // create a server socket. A server socket waits for requests to
            // come in over the network.
            mServerSocket = new ServerSocket(SERVERPORT);

            // create mClient socket... the method accept() listens for a
            // connection to be made to this socket and accepts it.
            try {
                mClient = mServerSocket.accept();
                System.out.println("S: Receiving...");
                // sends the message to the mClient
                mOut = new PrintWriter(new BufferedWriter(
                        new OutputStreamWriter(mClient.getOutputStream())),
                        true);
                System.out.println("PA: Sent");
                System.out.println("PA: Connecting Done.");
                // read the message received from mClient
                BufferedReader in = new BufferedReader(
                        new InputStreamReader(mClient.getInputStream()));

                sendMessage(
                        "Server connected with Android Client now you can chat with socket server.");

                // in this while we wait to receive messages from mClient (it's
                // an infinite loop)
                // this while it's like a listener for messages
                while (mRunning) {
                    String message = in.readLine();
                    if (message != null && mMessageListener != null) {
                        // call the method messageReceived from ServerBoard
                        // class
                        mMessageListener.messageReceived(message);
                    }
                }
            } catch (Exception e) {
                System.out.println("PA: Error: " + e.getMessage());
                e.printStackTrace();
            } finally {
                mClient.close();
                System.out.println("PA: Done.");
            }
        } catch (Exception e) {
            System.out.println("PA: Error");
            e.printStackTrace();
        }

    }

    /**
     * Declare the interface. The method messageReceived(String message) will
     */
    public interface OnMessageReceived {
        public void messageReceived(String message);
    }

}
