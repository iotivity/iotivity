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

package org.iotivity.test.rfw.common.devicecontroller.interfaces;

public interface IFileManager {
    public boolean copyFile(String source, String destination);

    public boolean deleteFile(String file);

    public boolean moveFile(String source, String destination);

    public boolean createFile(String newFile);

    public void changeWorkingDir(String path);

    public String getWorkingDir();

    public boolean mkdir(String path);

    public boolean copydir(String source, String destination);

    public boolean movedir(String source, String destination);

    public boolean rmdir(String path);

    public boolean push(String source, String destination);

    public boolean pull(String source, String destination);

    public boolean isExistWordInFile(String word, String file);
}
