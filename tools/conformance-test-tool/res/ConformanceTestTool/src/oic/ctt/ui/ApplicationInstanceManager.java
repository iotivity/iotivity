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
package oic.ctt.ui;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;

public class ApplicationInstanceManager {
    private static ApplicationInstanceListener subListener;

    /** Randomly chosen, but static, high socket number */
    public static final int                    SINGLE_INSTANCE_NETWORK_SOCKET = 44841;

    /** Must end with newline */
    public static final String                 SINGLE_INSTANCE_SHARED_KEY     = "$$NewInstance$$\n";

    /**
     * Registers this instance of the application.
     *
     * @return true if first instance, false if not.
     */
    public static boolean registerInstance() {
        boolean returnValueonerror = true;
        try {
            final ServerSocket socket = new ServerSocket(
                    SINGLE_INSTANCE_NETWORK_SOCKET, 10,
                    InetAddress.getLocalHost());
            System.out.println("Listening for application instances on socket "
                    + SINGLE_INSTANCE_NETWORK_SOCKET);
            Thread instanceListenerThread = new Thread(new Runnable() {
                public void run() {
                    boolean socketClosed = false;
                    while (!socketClosed) {
                        if (socket.isClosed()) {
                            socketClosed = true;
                        } else {
                            try {
                                Socket client = socket.accept();
                                BufferedReader in = new BufferedReader(
                                        new InputStreamReader(client
                                                .getInputStream()));
                                String message = in.readLine();
                                if (SINGLE_INSTANCE_SHARED_KEY.trim().equals(
                                        message.trim())) {
                                    System.out
                                            .println("Shared key matched - new application instance found");
                                    fireNewInstance();
                                }
                                in.close();
                                client.close();
                            } catch (IOException e) {
                                socketClosed = true;
                            }
                        }
                    }
                }
            });
            instanceListenerThread.start();
            // listen
        } catch (UnknownHostException e) {
            System.out.println(e.getMessage());
            return returnValueonerror;
        } catch (IOException e) {
            System.out
                    .println("Port is already taken.  Notifying first instance.");
            try {
                Socket clientSocket = new Socket(InetAddress.getLocalHost(),
                        SINGLE_INSTANCE_NETWORK_SOCKET);
                OutputStream out = clientSocket.getOutputStream();
                out.write(SINGLE_INSTANCE_SHARED_KEY.getBytes());
                out.close();
                clientSocket.close();
                System.out.println("Successfully notified first instance.");
                return false;
            } catch (UnknownHostException e1) {
                System.out.println(e.getMessage());
                return returnValueonerror;
            } catch (IOException e1) {
                System.out
                        .println("Error connecting to local port for single instance notification");
                System.out.println(e1.getMessage());
                return returnValueonerror;
            }

        }
        return true;
    }

    public static void setApplicationInstanceListener(
            ApplicationInstanceListener listener) {
        subListener = listener;
    }

    private static void fireNewInstance() {
        if (subListener != null) {
            subListener.newInstanceCreated();
        }
    }
}
