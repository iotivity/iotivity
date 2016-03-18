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

public enum ToolTipTextType {
    TOOLTIP_TEXT_CREATE_TEST_PLAN("Create Test Plan"),
    TOOLTIP_TEXT_CREATE_TEST_SUITE("Create New Test Suite."),
    TOOLTIP_TEXT_DELETE_TEST_SUITE("Delete Selected Item"),
    TOOLTIP_TEXT_IMPORT_TEST_PLAN("Import Test Plan."),
    TOOLTIP_TEXT_IMPORT_RESOURCE_FILE("Import Resource File."),
    TOOLTIP_TEXT_CREATE_RESOURCE_ACTION("Create Resource."),
    TOOLTIP_TEXT_MAKE_DESCRIPTOR_FILE("Make Descriptor File From RAML Files"),
    TOOLTIP_TEXT_PAUSE_TEST("Pause Test "),
    TOOLTIP_TEXT_RESUME_TEST("Resume Test "),
    TOOLTIP_TEXT_START_TEST("Start Test"),
    TOOLTIP_TEXT_STOP_TEST("Stop Test"),
    TOOLTIP_TEXT_CREATE_FILTERS_TOOLBAR("Add a new log filter"),
    TOOLTIP_TEXT_DELETE_LOG_FILTER("Delete selected log filter"),
    TOOLTIP_TEXT_EDIT_LOG_FILTER("Edit selected log filter"),
    TOOLTIP_TEXT_DEFAULT_SEARCH("Example search patterns:\n"
            + "    sqlite (search for sqlite in text field)\n"),
    TOOLTIP_TEXT_ROBOT_SYS_LOG("Show Robotframework System Log"),
    TOOLTIP_TEXT_SAVE_TO_LOG("Export Selected Items To Text File.."),
    TOOLTIP_TEXT_CLEAR_LOG("Clear Log"),
    TOOLTIP_TEXT_SCROLL_LOCK("Scroll Lock"),
    TOOLTIP_TEXT_REFRESH_VIEW("Refresh (F5)"),
    TOOLTIP_TEXT_SHOW_FILTERS_COLUMN("Display Saved Filters View");

    private final String text;

    private ToolTipTextType(String text) {
        this.text = text;
    }

    public boolean equalsName(String otherName) {
        return (otherName == null) ? false : text.equals(otherName);
    }

    public String toString() {
       return this.text;
    }
}
