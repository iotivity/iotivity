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

import java.util.logging.Logger;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.IFileManager;
import org.iotivity.test.rfw.common.devicecontroller.linux.LinuxProcessManager;
import org.iotivity.test.rfw.common.util.IoTivityLogger;
import org.iotivity.test.rfw.common.util.SystemUtil;

import org.iotivity.test.rfw.common.devicecontroller.android.AndroidCommand;

class AndroidFileManager implements IFileManager {
    private String              deviceID;
    private LinuxProcessManager fileProcessManager;
    private Logger              logger = IoTivityLogger.GetLogger();

    public AndroidFileManager(String deviceID) {
        fileProcessManager = new LinuxProcessManager();
        this.deviceID = deviceID;
    }

    @Override
    public boolean copyFile(String source, String destination) {
        if (SystemUtil.isNullOrEmpty(source)
                || SystemUtil.isNullOrEmpty(destination))
            return false;

        String command = String.format("%s %s %s %s %s %s %s",
                AndroidCommand.ADB, AndroidCommand.SPECOPTION, deviceID,
                AndroidCommand.SHELL, AndroidCommand.COPY, source, destination);
        return fileProcessManager.createProcess("copyFile", command);
    }

    @Override
    public boolean deleteFile(String file) {
        if (SystemUtil.isNullOrEmpty(file))
            return false;

        String command = String.format("%s %s %s %s %s %s", AndroidCommand.ADB,
                AndroidCommand.SPECOPTION, deviceID, AndroidCommand.SHELL,
                AndroidCommand.DELETE, file);
        return fileProcessManager.createProcess("deletefile", command);
    }

    @Override
    public boolean moveFile(String source, String destination) {
        if (SystemUtil.isNullOrEmpty(source)
                || SystemUtil.isNullOrEmpty(source))
            return false;

        String command = String.format("%s %s %s %s %s %s %s",
                AndroidCommand.ADB, AndroidCommand.SPECOPTION, deviceID,
                AndroidCommand.SHELL, AndroidCommand.MOVE, source, destination);

        return fileProcessManager.createProcess("moveFile", command);
    }

    @Override
    public boolean createFile(String newFile) {
        if (SystemUtil.isNullOrEmpty(newFile))
            return false;

        String command = String.format("%s %s %s %s %s %s", AndroidCommand.ADB,
                AndroidCommand.SPECOPTION, deviceID, AndroidCommand.SHELL,
                AndroidCommand.CREATE_FILE, newFile);
        logger.info(command);
        return fileProcessManager.createProcess("createFile", command);
    }

    @Override
    public void changeWorkingDir(String path) {
        if (SystemUtil.isNullOrEmpty(path))
            return;

        String command = String.format("%s %s %s %s %s %s", AndroidCommand.ADB,
                AndroidCommand.SPECOPTION, deviceID, AndroidCommand.SHELL,
                AndroidCommand.CHANGE_DIR, path);
        logger.info(command);

        fileProcessManager.createProcess("moveFile", command);
    }

    @Override
    public String getWorkingDir() {
        return null;
    }

    @Override
    public boolean mkdir(String path) {
        if (SystemUtil.isNullOrEmpty(path))
            return false;

        String command = String.format("%s %s %s %s %s %s", AndroidCommand.ADB,
                AndroidCommand.SPECOPTION, deviceID, AndroidCommand.SHELL,
                AndroidCommand.MAKE_DIR, path);
        logger.info(command);

        return fileProcessManager.createProcess("makeDirectory", command);
    }

    @Override
    public boolean copydir(String source, String destination) {
        if (SystemUtil.isNullOrEmpty(source)
                || SystemUtil.isNullOrEmpty(source))
            return false;

        String command = String.format("%s %s %s %s %s %s %s %s",
                AndroidCommand.ADB, AndroidCommand.SPECOPTION, deviceID,
                AndroidCommand.SHELL, AndroidCommand.COPY,
                AndroidCommand.RECURSIVE_FORCE, source, destination);
        logger.info(command);

        return fileProcessManager.createProcess("copyDirectory", command);
    }

    @Override
    public boolean movedir(String source, String destination) {
        if (SystemUtil.isNullOrEmpty(source)
                || SystemUtil.isNullOrEmpty(destination))
            return false;

        String command = String.format("%s %s %s %s %s %s %s",
                AndroidCommand.ADB, AndroidCommand.SPECOPTION, deviceID,
                AndroidCommand.SHELL, AndroidCommand.MOVE, source, destination);
        return fileProcessManager.createProcess("deletefile", command);
    }

    @Override
    public boolean rmdir(String path) {
        if (SystemUtil.isNullOrEmpty(path))
            return false;

        String command = String.format("%s %s %s %s %s %s %s",
                AndroidCommand.ADB, AndroidCommand.SPECOPTION, deviceID,
                AndroidCommand.SHELL, AndroidCommand.DELETE,
                AndroidCommand.RECURSIVE_FORCE, path);
        return fileProcessManager.createProcess("rmdir", command);
    }

    @Override
    public boolean push(String source, String destination) {
        if (SystemUtil.isNullOrEmpty(source)
                || SystemUtil.isNullOrEmpty(destination))
            return false;

        String command = String.format("%s %s %s %s %s %s", AndroidCommand.ADB,
                AndroidCommand.SPECOPTION, deviceID, AndroidCommand.PUSH,
                source, destination);
        return fileProcessManager.createProcess(AndroidCommand.PUSH, command);
    }

    @Override
    public boolean pull(String source, String destination) {
        if (SystemUtil.isNullOrEmpty(source)
                || SystemUtil.isNullOrEmpty(destination))
            return false;

        String command = String.format("%s %s %s %s %s %s", AndroidCommand.ADB,
                AndroidCommand.SPECOPTION, deviceID, AndroidCommand.PULL,
                source, destination);
        return fileProcessManager.createProcess(AndroidCommand.PULL, command);
    }

    @Override
    public boolean isExistWordInFile(String word, String file) {
        return false;
    }

}
