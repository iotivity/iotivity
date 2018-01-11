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

package org.iotivity.configuration;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

public class ReadConfigPropFile implements IConfiguration {

    private static File       s_mConfigFile;
    private static Properties s_mConfigProps;
    public static String      s_mGitHubUserId;
    public static String      s_mGitHubUserPassword;
    public static String      s_mDeafaultCiServerIp;
    public static String      s_mTcpCIServerIp;
    public static String      s_mTcpCIServerIpv6;

    public static void readConfigFile(String filePath) {
        System.out.println("=================FILE PATH " + filePath);
        InputStream inputStream = null;

        s_mConfigProps = new Properties();

        s_mConfigFile = new File(filePath + CONFIG_FILE_NAME);

        if (s_mConfigFile.exists()) {
            System.out.println("Config Properties File Exists.");
        }

        try {
            inputStream = new FileInputStream(s_mConfigFile);
            s_mConfigProps.load(inputStream);

            s_mGitHubUserId = "";
            s_mGitHubUserPassword = "";
            s_mDeafaultCiServerIp = "";

            s_mGitHubUserId = s_mConfigProps.getProperty(KEY_GIT_USER_ID);
            s_mGitHubUserPassword = s_mConfigProps
                    .getProperty(KEY_GIT_USER_PASSWORD);
            s_mDeafaultCiServerIp = s_mConfigProps
                    .getProperty(KEY_DEFAULT_CI_SERVER_IP);
            s_mTcpCIServerIp = s_mConfigProps.getProperty(KEY_TCP_SERVER_IP);
            s_mTcpCIServerIpv6 = s_mConfigProps.getProperty(KEY_TCP_SERVER_IPv6);

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