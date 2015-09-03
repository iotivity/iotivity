package oic.simulator.serviceprovider.utils;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

public class Utility {

    public static String uriToDisplayName(String uri) {
        String result = null;
        if (null != uri) {
            String tokens[] = uri.split(Constants.FORWARD_SLASH);
            if (Constants.PROPER_RESOURCE_URI_TOKEN_COUNT == tokens.length) {
                // Proper URI
                result = tokens[2] + Constants.UNDERSCORE + tokens[4];
            }
        }
        return result;
    }

    public static String displayNameToUri(String displayName) {
        String result = null;
        if (null != displayName) {
            String tokens[] = displayName.split(Constants.UNDERSCORE);
            if (Constants.DISPLAY_RESOURCE_URI_TOKEN_COUNT == tokens.length) {
                // Proper Display Name
                result = Constants.FORWARD_SLASH + Constants.OIC
                        + Constants.FORWARD_SLASH + tokens[0]
                        + Constants.FORWARD_SLASH + Constants.SIMULATOR
                        + Constants.FORWARD_SLASH + tokens[1];
            }
        }
        return result;
    }

    public static String fileNameToDisplay(String fileName) {
        if (null == fileName || fileName.length() < 1) {
            return null;
        }
        // Remove the RAML file standard prefix
        int len = Constants.RAML_FILE_PREFIX.length();
        if (len > 0) {
            if (fileName.startsWith(Constants.RAML_FILE_PREFIX)) {
                fileName = fileName.substring(len);
            }
        }

        // Removing the file extension
        String[] token = fileName.split(Constants.SPLIT_BY_DOT_PATTERN);
        int tokLen = token.length;
        String result = null;
        if (tokLen - 2 >= 0) {
            result = token[tokLen - 2];
        }
        return result;
    }

    public static String displayToFileName(String displayName) {
        if (null == displayName || displayName.length() < 1) {
            return null;
        }
        String fileName;
        // Adding the prefix
        fileName = Constants.RAML_FILE_PREFIX + displayName;

        // Adding the file extension
        fileName = fileName + Constants.RAML_FILE_EXTENSION;

        return fileName;
    }

    public static boolean isUriComplete(String uri) {
        boolean uriComplete = false;
        if (null != uri) {
            String tokens[] = uri.split(Constants.FORWARD_SLASH);
            if (Constants.PROPER_RESOURCE_URI_TOKEN_COUNT == tokens.length) {
                uriComplete = true;
            }
        }
        return uriComplete;
    }

    public static String getAutomationStatus(boolean status) {
        if (status) {
            return Constants.ENABLED;
        } else {
            return Constants.DISABLED;
        }
    }

    public static String getAutomationString(boolean status) {
        if (status) {
            return Constants.ENABLE;
        } else {
            return Constants.DISABLE;
        }
    }

    public static boolean getAutomationBoolean(String status) {
        if (null != status) {
            if (status.equals(Constants.ENABLE)) {
                return true;
            }
        }
        return false;
    }

    public static int getUpdateIntervalFromString(String value) {
        int result = Constants.DEFAULT_AUTOMATION_INTERVAL;
        if (null != value) {
            try {
                result = Integer.parseInt(value);
            } catch (NumberFormatException nfe) {
                // Do nothing
            }
        }
        return result;
    }

    public static List<String> convertSetToList(Set<String> typeSet) {
        if (null == typeSet) {
            return null;
        }
        List<String> list = new ArrayList<String>();
        Iterator<String> typeItr = typeSet.iterator();
        while (typeItr.hasNext()) {
            list.add(typeItr.next());
        }
        return list;
    }
}