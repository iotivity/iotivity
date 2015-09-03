package oic.simulator.serviceprovider.resource;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import oic.simulator.serviceprovider.utils.Constants;

import org.oic.simulator.AutomationType;

public class AutomationSettingHelper {
    private String       settingID;
    private String       settingValue;
    private List<String> allowedValues;

    public String getSettingID() {
        return settingID;
    }

    public void setSettingID(String settingID) {
        this.settingID = settingID;
    }

    public String getSettingValue() {
        return settingValue;
    }

    public void setSettingValue(String settingValue) {
        this.settingValue = settingValue;
    }

    public List<String> getAllowedValues() {
        return allowedValues;
    }

    public void setAllowedValues(List<String> allowedValues) {
        this.allowedValues = allowedValues;
    }

    public void addAllowedValue(String newText) {
        if (null != allowedValues) {
            allowedValues.add(newText);
        }
    }

    public static List<AutomationSettingHelper> getAutomationSettings(
            LocalResourceAttribute attribute) {
        List<AutomationSettingHelper> settingList = null;
        boolean invalidSetting;
        if (null != attribute) {
            settingList = new ArrayList<AutomationSettingHelper>();
            for (int count = 0; count < Constants.AUTOMATION_SETTINGS_COUNT; count++) {
                invalidSetting = false;
                AutomationSettingHelper setting = new AutomationSettingHelper();
                if (Constants.AUTOMATION_SETTINGS[count]
                        .equals(Constants.AUTOMATION_TYPE)) {
                    setting.setSettingID(Constants.AUTOMATION_TYPE);
                    setting.setSettingValue(attribute.getAutomationType()
                            .toString());
                    List<String> valueList = new ArrayList<String>();
                    valueList.add(AutomationType.NORMAL.toString());
                    valueList.add(AutomationType.RECURRENT.toString());
                    setting.setAllowedValues(valueList);
                } else if (Constants.AUTOMATION_SETTINGS[count]
                        .equals(Constants.UPDATE_INTERVAL_IN_MS)) {
                    setting.setSettingID(Constants.UPDATE_INTERVAL_IN_MS);
                    setting.setSettingValue(String.valueOf(attribute
                            .getAutomationUpdateInterval()));
                    List<String> valueList = new ArrayList<String>();
                    for (int index = 1; index <= 10; index++) {
                        valueList.add(String.valueOf(index * 500));
                    }
                    setting.setAllowedValues(valueList);
                } else {
                    invalidSetting = true;
                }
                if (!invalidSetting) {
                    settingList.add(setting);
                }
            }
        }
        return settingList;
    }

    public static void updateAutomationStatus(
            List<AutomationSettingHelper> localSettingList, String status) {
        if (null != localSettingList && null != status) {
            Iterator<AutomationSettingHelper> settingItr = localSettingList
                    .iterator();
            AutomationSettingHelper setting;
            while (settingItr.hasNext()) {
                setting = settingItr.next();
                if (null != setting) {
                    if (setting.getSettingID().equals(Constants.AUTOMATION)) {
                        setting.setSettingValue(status);
                        break;
                    }
                }
            }
        }
    }
}