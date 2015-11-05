// ******************************************************************
//
// Copyright 2014 Samsung Electronics All Rights Reserved.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

package oic.ctt.formatter;

import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.UIManager;
import javax.swing.UIManager.LookAndFeelInfo;

/**
 * 
 * This Class is responsible for Interacting with User
 *
 */
public class UserInterface {
    public JFrame frame = null;

    /**
     * Default Constructor
     */
    public UserInterface() {
        frame = new JFrame();

        try {
            for (LookAndFeelInfo info : UIManager.getInstalledLookAndFeels()) {
                if ("Nimbus".equals(info.getName())) {
                    UIManager.setLookAndFeel(info.getClassName());
                    break;
                }
            }
        } catch (Exception e) {
        }
    }

    /**
     * This Method is used to show a Input Dialog box
     * 
     * @param msg
     *            Message to Show in the Input Dialog
     * @param optionList
     *            Optionlist to show in the Input Dialog
     * @return String User Input
     */
    public String showInputDialog(String msg, String[] optionList) {
        if (optionList.length == 0) {
            return (String) JOptionPane.showInputDialog(frame, msg);
        } else {
            return (String) JOptionPane.showInputDialog(frame, msg,
                    "User Input", JOptionPane.OK_OPTION, null, optionList,
                    optionList[0]);
        }
    }

    /**
     * This Method is used to Show User a Message Dialog box
     * 
     * @param msg
     *            Messge to show in the Dialog
     */
    public void showMessageDialog(String msg) {
        String[] wordList = msg.split(" ");
        String finalMessage = "";
        int counter = 0;
        for (String word : wordList) {
            if (counter++ < 6) {
                finalMessage += " " + word;
            } else {
                finalMessage += "\n";
                counter = 0;
            }
        }
        JOptionPane.showMessageDialog(frame, finalMessage, "",
                JOptionPane.INFORMATION_MESSAGE);
    }
}
