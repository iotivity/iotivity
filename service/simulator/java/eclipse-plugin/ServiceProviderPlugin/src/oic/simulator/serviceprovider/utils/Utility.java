package oic.simulator.serviceprovider.utils;

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
}