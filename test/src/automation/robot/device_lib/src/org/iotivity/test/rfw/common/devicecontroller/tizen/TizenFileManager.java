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

import java.util.logging.Logger;

import org.iotivity.test.rfw.common.devicecontroller.android.AndroidCommand;
import org.iotivity.test.rfw.common.devicecontroller.interfaces.IFileManager;
import org.iotivity.test.rfw.common.devicecontroller.linux.LinuxProcessManager;
import org.iotivity.test.rfw.common.util.IoTivityLogger;
import org.iotivity.test.rfw.common.util.SystemUtil;

import org.iotivity.test.rfw.common.devicecontroller.tizen.TizenCommand;

class TizenFileManager implements IFileManager {

    private String              deviceID;
    private final String        deviceBridge = "sdb";
    private LinuxProcessManager fileProcessManager;
    private Logger              logger       = IoTivityLogger.GetLogger();

    public TizenFileManager(String deviceID) {
        fileProcessManager = new LinuxProcessManager();
        this.deviceID = deviceID;
    }

    @Override
    public boolean copyFile(String source, String destination) {
        if (SystemUtil.isNullOrEmpty(source)
                || SystemUtil.isNullOrEmpty(destination))
            return false;

        String command = String.format("%s %s %s %s %s %s %s", deviceBridge,
                AndroidCommand.SPECOPTION, deviceID, TizenCommand.SHELL,
                TizenCommand.COPY, source, destination);
        logger.info(command);

        return fileProcessManager.createProcess("copyFile", command);
    }

    @Override
    public boolean deleteFile(String file) {
        if (SystemUtil.isNullOrEmpty(file))
            return false;

        String command = String.format("%s %s %s %s %s %s", deviceBridge,
                AndroidCommand.SPECOPTION, deviceID, TizenCommand.SHELL,
                TizenCommand.DELETE, file);
        logger.info(command);

        return fileProcessManager.createProcess("deleteFile", command);
    }

    @Override
    public boolean moveFile(String source, String destination) {
        if (SystemUtil.isNullOrEmpty(source)
                || SystemUtil.isNullOrEmpty(destination))
            return false;

        String command = String.format("%s %s %s %s %s %s %s", deviceBridge,
                AndroidCommand.SPECOPTION, deviceID, TizenCommand.SHELL,
                TizenCommand.MOVE, source, destination);
        logger.info(command);

        return fileProcessManager.createProcess("moveFile", command);
    }

    @Override
    public boolean createFile(String newFile) {
        if (SystemUtil.isNullOrEmpty(newFile))
            return false;

        String command = String.format("%s %s %s %s %s %s", deviceBridge,
                AndroidCommand.SPECOPTION, deviceID, TizenCommand.SHELL,
                TizenCommand.CREATE_FILE, newFile);
        logger.info(command);

        return fileProcessManager.createProcess("reateFile", command);
    }

    @Override
    public void changeWorkingDir(String path) {
        if (SystemUtil.isNullOrEmpty(path))
            return;

        String command = String.format("%s %s %s %s %s %s", deviceBridge,
                AndroidCommand.SPECOPTION, deviceID, TizenCommand.SHELL,
                TizenCommand.CHANGE_DIR, path);
        logger.info(command);

        fileProcessManager.createProcess("moveFile", command);
    }

    @Override
    public String getWorkingDir() {
        return "\\";
    }

    @Override
    public boolean mkdir(String path) {
        if (SystemUtil.isNullOrEmpty(path))
            return false;

        String command = String.format("%s %s %s %s %s %s", deviceBridge,
                AndroidCommand.SPECOPTION, deviceID, TizenCommand.SHELL,
                TizenCommand.MAKE_DIR, path);
        logger.info(command);

        return fileProcessManager.createProcess("makeDirectory", command);
    }

    @Override
    public boolean copydir(String source, String destination) {
        if (SystemUtil.isNullOrEmpty(source)
                || SystemUtil.isNullOrEmpty(source))
            return false;

        String command = String.format("%s %s %s %s %s %s %s %s", deviceBridge,
                AndroidCommand.SPECOPTION, deviceID, TizenCommand.SHELL,
                TizenCommand.COPY, TizenCommand.RECURSIVE_FORCE, source,
                destination);
        logger.info(command);

        return fileProcessManager.createProcess("copyDirectory", command);
    }

    @Override
    public boolean movedir(String source, String destination) {
        if (SystemUtil.isNullOrEmpty(source)
                || SystemUtil.isNullOrEmpty(source))
            return false;

        String command = String.format("%s %s %s %s %s %s %s", deviceBridge,
                AndroidCommand.SPECOPTION, deviceID, TizenCommand.SHELL,
                TizenCommand.MOVE, source, destination);
        logger.info(command);

        return fileProcessManager.createProcess("moveDirectory", command);
    }

    @Override
    public boolean rmdir(String path) {
        if (SystemUtil.isNullOrEmpty(path))
            return false;

        String command = String.format("%s %s %s %s %s %s %s", deviceBridge,
                AndroidCommand.SPECOPTION, deviceID, TizenCommand.SHELL,
                TizenCommand.DELETE, TizenCommand.RECURSIVE_FORCE, path);
        logger.info(command);

        return fileProcessManager.createProcess("removeDirectory", command);
    }

    @Override
    public boolean push(String source, String destination) {
        if (SystemUtil.isNullOrEmpty(source)
                || SystemUtil.isNullOrEmpty(destination))
            return false;

        String command = String.format("%s %s %s %s %s %s", deviceBridge,
                AndroidCommand.SPECOPTION, deviceID, TizenCommand.PUSH, source,
                destination);
        logger.info(command);

        return fileProcessManager.createProcess("push", command);
    }

    @Override
    public boolean pull(String source, String destination) {
        if (SystemUtil.isNullOrEmpty(source)
                || SystemUtil.isNullOrEmpty(destination))
            return false;

        String command = String.format("%s %s %s %s %s %s", deviceBridge,
                AndroidCommand.SPECOPTION, deviceID, TizenCommand.PULL, source,
                destination);
        logger.info(command);

        return fileProcessManager.createProcess("pull", command);
    }

    @Override
    public boolean isExistWordInFile(String word, String file) {
        return false;
    }
}
