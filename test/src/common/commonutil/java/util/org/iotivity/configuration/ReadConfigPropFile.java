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

package org.iotivity.configuration;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

public class ReadConfigPropFile implements IConfiguration {

    private static File       configFile;

    private static Properties configProps;
    public static String      s_GitHubUserId;
    public static String      s_GitHubUserPassword;
    public static String      s_DeafaultCiServerIp;
    static {
        InputStream inputStream = null;

        configProps = new Properties();

        configFile = new File(TEMP_DIR + CONFIG_FILE_NAME);

        if (configFile.exists()) {
            System.out.println("Config Properties File Exists.");
        }

        try {
            inputStream = new FileInputStream(configFile);
            configProps.load(inputStream);

            s_GitHubUserId = "";
            s_GitHubUserPassword = "";
            s_DeafaultCiServerIp = "";

            s_GitHubUserId = configProps.getProperty(KEY_GIT_USER_ID);
            s_GitHubUserPassword = configProps
                    .getProperty(KEY_GIT_USER_PASSWORD);
            s_DeafaultCiServerIp = configProps
                    .getProperty(KEY_DEFAULT_CI_SERVER_IP);

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {

            if (inputStream != null) {
                try {
                    inputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}