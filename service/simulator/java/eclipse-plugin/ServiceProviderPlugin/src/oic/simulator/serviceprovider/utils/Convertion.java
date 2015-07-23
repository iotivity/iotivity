package oic.simulator.serviceprovider.utils;

public class Convertion {

    public static String uriToDisplayName(String uri) {
        String result = null;
        if (null != uri) {
            String tokens[] = uri.split(Constants.FORWARD_SLASH);
            System.out.println(uri);
            System.out.println(tokens.length);
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
            if (2 == tokens.length) {
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
            System.out.println(uri);
            System.out.println(tokens.length);
            if (Constants.PROPER_RESOURCE_URI_TOKEN_COUNT == tokens.length) {
                uriComplete = true;
            }
        }
        return uriComplete;
    }
}
