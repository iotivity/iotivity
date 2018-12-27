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

package org.iotivity.test.rfw.common.util;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;

public class FileUtil {

    public static void changeWorkingDir(String path) {
        System.setProperty("user.dir", path);
    }

    public static String getWorkingDir() {
        return new File(".").getAbsolutePath();
    }

    public static boolean isDir(String path) {
        File file = new File(path);

        return file.isDirectory();
    }

    public static boolean isExist(String path) {
        File file = new File(path);

        return file.exists();
    }

    public static boolean copy(String source, String dest) {
        if (isDir(source))
            return copyDir(source, dest);
        else
            return copyFile(source, dest);
    }

    public static boolean move(String source, String dest) {
        File srcFile = new File(source);

        return srcFile.renameTo(new File(dest));
    }

    public static boolean remove(String path) {
        if (isDir(path))
            return removeDir(path);
        else
            return removeFile(path);
    }

    public static boolean createFile(String desFile) {
        File newFile = new File(desFile);

        try {
            return newFile.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
    }

    public static boolean makeDir(String path) {
        File desDir = new File(path);
        if (desDir.exists())
            return true;

        return desDir.mkdirs();
    }

    @SuppressWarnings("resource")
    public static boolean IsExistWordInFile(String word, String path) {
        FileInputStream fis = null;
        boolean isSucceed = false;
        try {
            fis = new FileInputStream(path);
            BufferedReader br = new BufferedReader(new InputStreamReader(fis));
            String line;
            while ((line = br.readLine()) != null) {
                if (line.contains(word)) {
                    isSucceed = true;
                    break;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (fis != null)
                    fis.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return isSucceed;
    }

    private static boolean copyFile(String source, String destination) {
        InputStream inStream = null;
        OutputStream outStream = null;
        boolean isSucceed = false;
        try {
            File desFile = new File(destination);
            File srcFile = new File(source);

            inStream = new FileInputStream(srcFile);
            outStream = new FileOutputStream(desFile);
            byte[] buffer = new byte[1024];

            int length;
            while ((length = inStream.read(buffer)) > 0) {
                outStream.write(buffer, 0, length);
            }
            isSucceed = true;
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (inStream != null)
                    inStream.close();
                if (outStream != null)
                    outStream.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return isSucceed;
    }

    private static boolean copyDir(String source, String destination) {
        File desFile = new File(destination);
        File srcFile = new File(source);

        if (!desFile.exists()) {
            desFile.mkdir();
        }

        if (srcFile.listFiles() != null) {
            for (File f : srcFile.listFiles()) {
                return copy(f.getAbsolutePath(),
                        new File(desFile, f.getName()).getAbsolutePath());
            }
        }
        return false;
    }

    private static boolean removeDir(String path) {
        if (path == null)
            return false;

        File desDir = new File(path);
        if (desDir.exists() && desDir.listFiles() != null) {
            for (File f : desDir.listFiles()) {
                if (f.isDirectory()) {
                    removeDir(f.getAbsolutePath());
                    f.delete();
                } else {
                    f.delete();
                }
            }

            return desDir.delete();
        }

        return true;
    }

    private static boolean removeFile(String file) {
        File desFile = new File(file);
        boolean result = false;
        if (desFile.exists()) {
            try {
                result = desFile.delete();
            } catch (Exception ex) {
                return false;
            }
        }

        return result;
    }
}
