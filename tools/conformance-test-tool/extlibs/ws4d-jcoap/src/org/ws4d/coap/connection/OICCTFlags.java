package org.ws4d.coap.connection;

import java.util.ArrayList;
import java.util.HashMap;

public class OICCTFlags {
    private static ArrayList<String> unReachableTokens = new ArrayList<String>();
    private static HashMap<Integer, String> notificationTokens = new HashMap<Integer, String>();
    private static ArrayList<String> toRemove = new ArrayList<String>();
    
    
    
    public static void addToken(int msgId, String token)
    {
        notificationTokens.put(msgId, token);
    }
    
    public static void markForRemoval(int msgId)
    {
        if(notificationTokens.containsKey(msgId))
            toRemove.add(notificationTokens.get(msgId));
    }
    
    public static boolean isMarkedForRemoval(String token)
    {
        return toRemove.contains(token);
    }
    
    public static void addAsUnreachable(String token)
    {
        unReachableTokens.add(token);
    }
    
    public static boolean isUnreachable(String token)
    {
       return unReachableTokens.contains(token);
    }
}
