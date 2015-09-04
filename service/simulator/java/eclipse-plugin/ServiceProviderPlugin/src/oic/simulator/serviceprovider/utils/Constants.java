package oic.simulator.serviceprovider.utils;

import org.oic.simulator.AutomationType;

public class Constants {

    public static final String         CONFIG_DIRECTORY_PATH            = "/resource";

    public static final String         UNDERSCORE                       = "_";
    public static final String         FORWARD_SLASH                    = "/";

    public static final String         OIC                              = "oic";
    public static final String         SIMULATOR                        = "simulator";

    public static final String         RESOURCE_URI                     = "Resource URI";
    public static final String         RESOURCE_TYPE                    = "Resource Type";
    public static final String         RESOURCE_UID                     = "Resource ID";
    public static final String         CONNECTIVITY_TYPE                = "Connectivity Type";

    public static final String[]       META_PROPERTIES                  = {
            RESOURCE_URI, RESOURCE_TYPE, CONNECTIVITY_TYPE             };

    public static final int            META_PROPERTY_COUNT              = META_PROPERTIES.length;

    public static final String         ENABLE                           = "Enable";
    public static final String         DISABLE                          = "Disable";
    public static final String         ENABLED                          = "Enabled";
    public static final String         DISABLED                         = "Disabled";

    public static final String         AUTOMATION                       = "Automation";
    public static final String         AUTOMATION_TYPE                  = "Automation Type";
    public static final String         UPDATE_INTERVAL_IN_MS            = "Update Interval(ms)";

    public static final String[]       AUTOMATION_SETTINGS              = {
            AUTOMATION_TYPE, UPDATE_INTERVAL_IN_MS                     };

    public static final int            AUTOMATION_SETTINGS_COUNT        = AUTOMATION_SETTINGS.length;

    public static final String         START_RESOURCE_AUTOMATION        = "Start Automation";
    public static final String         STOP_RESOURCE_AUTOMATION         = "Stop Automation";

    public static final int            PROPER_RESOURCE_URI_TOKEN_COUNT  = 5;
    public static final int            DISPLAY_RESOURCE_URI_TOKEN_COUNT = 2;

    public static final AutomationType DEFAULT_AUTOMATION_TYPE          = AutomationType.NORMAL;

    public static final int            DEFAULT_AUTOMATION_INTERVAL      = 500;

    public static final int            PROPER_LOG_TIME_TOKEN_LENGTH     = 3;

    public static final String[]       BROWSE_RAML_FILTER_EXTENSIONS    = new String[] { "*.raml" };
    public static final String[]       SAVE_LOG_FILTER_EXTENSIONS       = new String[] {
            "*.log", "*"                                               };

    public static final int            LOG_SIZE                         = 1000;

    public static final String         INFO_LOG                         = "info_log";
    public static final String         WARNING_LOG                      = "warning_log";
    public static final String         ERROR_LOG                        = "error_log";
    public static final String         DEBUG_LOG                        = "debug_log";
    public static final String         UNKNOWN_LOG                      = "unknown_log";

    public static final String         INFO                             = "Info";
    public static final String         WARNING                          = "Warning";
    public static final String         ERROR                            = "Error";
    public static final String         DEBUG                            = "Debug";
    public static final String         UNKNOWN                          = "Unknown";

    public static final String         CHECKED                          = "Checked";
    public static final String         UNCHECKED                        = "Unchecked";
    public static final String         NOTIFY_BUTTON_SELECTED           = "Notify_Selected";
    public static final String         NOTIFY_BUTTON_UNSELECTED         = "Notify_Unselected";

    public static final String         CREATE_PAGE_TITLE                = "Create Resource";
    public static final String         CREATE_PAGE_MESSAGE              = "Select a standard resource or custom resource to be created";

    public static final String         DELETE_PAGE_TITLE                = "Delete Resource";
    public static final String         DELETE_PAGE_MESSAGE              = "Select the resource(s) to be deleted";

    public static final String         RAML_FILE_PREFIX                 = "";

    public static final String         RAML_FILE_EXTENSION              = ".raml";

    public static final String         SPLIT_BY_DOT_PATTERN             = "\\.";

    public static final String         OIC_LIGHT                        = "oic.light";
}