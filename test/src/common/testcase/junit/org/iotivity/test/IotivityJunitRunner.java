/******************************************************************
 * Copyright 2018 Samsung Electronics All Rights Reserved.
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

package org.iotivity.test;

import org.junit.runner.JUnitCore;
import org.junit.runner.Request;
import org.junit.runner.Result;

public class IotivityJunitRunner {

    public static void main(String[] args) throws ClassNotFoundException {
        Request request = null;
        Result result = null;
        if (args.length == 0) {
            System.err.println(
                    "No TestSuit/Testcase provided to run usning JUNIT. Exiting...");
            System.out.println("Please provide argument like below : ");
            System.out.println(
                    "java -Djava.library.path=<path_to_IoTivity_lib> -jar lib/<tc_jar> <package_name>.<test_suit_name>#<tc_name>");
            System.out.println(" or, ");
            System.out.println(
                    "java -Djava.library.path=<path_to_IoTivity_lib> -jar lib/<tc_jar> <package_name>.<test_suit_name>");
            return;
        }
        if (args[0].contains("#")) {
            String[] classAndMethod = args[0].split("#");
            request = Request.method(Class.forName(classAndMethod[0]),
                    classAndMethod[1]);
            result = new JUnitCore().run(request);
        } else {
            request = Request.aClass(Class.forName(args[0]));
            result = new JUnitCore().run(request);
        }

        if (result.wasSuccessful()) {
            System.out.println("Result : " + "\033[32m TC Execution Finished Successfully");
        } else {
            System.out.println("\033[37m Result : \033[31m"
                    + "Execution Failed, reason: ");
            for (int i = 0; i < result.getFailures().size(); i++) {
                System.out.println("Fail : " + (i + 1));
                System.out.println("\033[37m Description: \033[31m "
                        + result.getFailures().get(i).getDescription());
                System.out.println("\033[37m Message: \033[31m "
                        + result.getFailures().get(i).getMessage());
                System.out.println("\033[37m Stacktrace: \033[31m "
                        + result.getFailures().get(i).getTrace());
                System.out.println(" ");
            }
        }
    }

}
