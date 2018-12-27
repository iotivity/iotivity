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

package org.iotivity.test.rfw.common.devicecontroller.linux;

import org.iotivity.test.rfw.common.devicecontroller.interfaces.IFileManager;
import org.iotivity.test.rfw.common.util.FileUtil;
import org.iotivity.test.rfw.common.util.SystemUtil;

import org.iotivity.test.rfw.common.devicecontroller.linux.LinuxProcessManager;

class LinuxFileManager implements IFileManager {

    private LinuxProcessManager fileProcessManager;

    public LinuxFileManager() {
        fileProcessManager = new LinuxProcessManager();
    }

    @Override
    public void changeWorkingDir(String path) {
        FileUtil.changeWorkingDir(path);
    }

    @Override
    public String getWorkingDir() {
        return FileUtil.getWorkingDir();
    }

    @Override
    public boolean copyFile(String source, String destination) {
        return FileUtil.copy(source, destination);
    }

    @Override
    public boolean mkdir(String path) {
        return FileUtil.makeDir(path);
    }

    @Override
    public boolean rmdir(String path) {
        return FileUtil.remove(path);
    }

    @Override
    public boolean deleteFile(String file) {
        return FileUtil.remove(file);
    }

    @Override
    public boolean moveFile(String source, String destination) {
        return FileUtil.move(source, destination);
    }

    @Override
    public boolean createFile(String desFile) {
        return FileUtil.createFile(desFile);
    }

    @Override
    public boolean copydir(String source, String destination) {
        return FileUtil.copy(source, destination);
    }

    @Override
    public boolean movedir(String source, String destination) {
        return FileUtil.move(source, destination);
    }

    @Override
    public boolean push(String source, String destination) {
        if (SystemUtil.isNullOrEmpty(source)
                || SystemUtil.isNullOrEmpty(destination))
            return false;
        String command = String.format("scp %s %s", source, destination);
        return fileProcessManager.createProcess("pushFileLinux", command);
    }

    @Override
    public boolean pull(String source, String destination) {
        if (SystemUtil.isNullOrEmpty(source)
                || SystemUtil.isNullOrEmpty(destination))
            return false;
        String command = String.format("scp %s %s", source, destination);
        return fileProcessManager.createProcess("pullFileLinux", command);
    }

    @Override
    public boolean isExistWordInFile(String word, String file) {
        return FileUtil.IsExistWordInFile(word, file);
    }

}
