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
package oic.ctt.logmanager;

public class LogManager {
    private static Thread threadLogManagerProducer = null;

    public LogManager() {

    }

    public static void main(String[] args) throws InterruptedException {

    }

    public static void startLogManager(int port) {
        LogCollector.startLogCollector();
        while (true) {
            if (LogCollector.mergeQueue.isEmpty()) {
                try {
                    Thread.sleep(500);
                    System.out.println("wating RobotFramework.......");
                } catch (Exception e) {
                    e.printStackTrace();
                    System.out.println("check startLogManager init : "
                            + e.toString());
                }
            } else {
                break;
            }
        }
        startLogManagerProducer(port);
        System.out.println("startLogManager");
    }

    public static void stopLogManager() {
        LogCollector.stopLogCollector();
        LogManagerProducer.stopLogManagerProducer();
        System.out.println("stopLogManager");
    }

    public static void startLogManagerProducer(int port) {
        threadLogManagerProducer = new Thread(new LogManagerProducer(
                LogCollector.mergeQueue, port));
        threadLogManagerProducer.start();
        System.out.println("startLogManagerProducer");
    }
}
