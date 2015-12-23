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
package oic.ctt.ui.types;

public enum ImageFilePathType {
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_CREATE_TEST_PLAN("icons/folder_add.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_CREATE_TEST_SUITE("icons/document_new.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_DELETE_TEST_SUITE("icons/erase.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_IMPORT_TEST_PLAN("icons/import.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_PAUSE_TEST("icons/pause.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_START_TEST("icons/run.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_STOP_TEST("icons/stop.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_CREATE_FILTERS_TOOLBAR("icons/add_filter.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_DELETE_LOG_FILTER("icons/erase.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_EDIT_LOG_FILTER("icons/edit_filter.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_ROBOT_SYS_LOG("icons/robot.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_SAVE_TO_LOG("icons/save_log.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_CLEAR_LOG("icons/delete_log.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_SCROLL_LOCK("icons/scroll_lock.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_SHOW_FILTERS_COLUMN("icons/filter_log.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_RESUME("icons/resume.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_FILE_ICON("icons/file2.gif"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_FOLDER_ICON("icons/folder.gif"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_TESTFILE_ICON("icons/test.gif"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_SEQUENCE_ICON("icons/sequence.gif"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_REFRESH_ICON("icons/refresh.gif"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_SETTINGS_ICON("icons/settings.gif"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_SAVE_ICON("icons/save.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_PLAY_ICON("icons/play2.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_EXCLAMATION_ICON("icons/exclamation.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_PASS_ICON("icons/pass.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_FAIL_ICON("icons/fail.png"),
    IMAGE_FILE_PATH_IMAGE_DESCRIPTOR_NOTTESTED_ICON("icons/nottested.gif");

    private final String text;

    private ImageFilePathType(String text) {
        this.text = text;
    }

    public String toString() {
       return this.text;
    }
}
