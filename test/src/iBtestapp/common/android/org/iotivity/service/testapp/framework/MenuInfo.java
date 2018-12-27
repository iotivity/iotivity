package org.iotivity.service.testapp.framework;

import android.util.Log;

public class MenuInfo {

    protected String            menuText;
    protected String            actionName;
    private static final String TAG     = "MenuInfo";
    private static final String MESSAGE = "Invalid menu title or action name";

    public MenuInfo(String menuText, String actionName) {
        if (!menuText.isEmpty() && menuText != null && !actionName.isEmpty()
                && actionName != null) {
            this.menuText = menuText;
            this.actionName = actionName;
        } else {
            Log.d(TAG, MESSAGE);
        }
    }
}
