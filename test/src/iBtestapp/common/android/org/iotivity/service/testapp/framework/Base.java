/******************************************************************
*
* Copyright 2016 Samsung Electronics All Rights Reserved.
*
*
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************/

package org.iotivity.service.testapp.framework;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.drawable.GradientDrawable;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import android.text.method.ScrollingMovementMethod;

public class Base extends Activity {
    private static final String LOG_HEADER                  = "Log:\n";
    private static final String INVOCATION_TARGET_EXCEPTION = "InvocationTargetException";
    private static final String ILLEGAL_ARGUMENT_EXCEPTION  = "IllegalArgumentException";
    private static final String ILLEGAL_ACCESS_EXCEPTION    = "IllegalAccessException";
    private static Context      context;
    private static final String EMPTY_TEXT_STRING           = "";
    private static final String ACTION_NOT_FOUND_MESSAGE    = "Corresponding action not found";
    private static final String ACTIONS_REGISTER_MESSAGE    = "Actions are not registered";
    private static final String INVALID_MENU_OPTION_MESSAGE = "Menu option does not exist";
    private static final String ACTION_BUTTON_TEXT          = "Execute";
    private static final String INPUT_HINT_TEXT             = "Input menu option number";
    private static String       outPutString                = LOG_HEADER;
    private Object              actionObject;
    private static TextView     outputTextView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        context = getApplicationContext();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        return super.onOptionsItemSelected(item);

    }

    public static Context getContext() {
        return context;
    }

    public void RegisterApp(String appTitle,
            final Map<String, MenuInfo> menuMap, Object actionClassObj) {
        createMenu(appTitle, menuMap);
        registerActions(actionClassObj);
    }

    private void createMenu(String appTitle,
            final Map<String, MenuInfo> menuMap) {

        TableLayout.LayoutParams tableParams = new TableLayout.LayoutParams(
                TableLayout.LayoutParams.WRAP_CONTENT,
                TableLayout.LayoutParams.WRAP_CONTENT);
        TableRow.LayoutParams rowParams = new TableRow.LayoutParams(
                TableRow.LayoutParams.WRAP_CONTENT,
                TableRow.LayoutParams.WRAP_CONTENT);

        TableLayout parentLayout = new TableLayout(this);
        parentLayout.setLayoutParams(tableParams);

        TableRow titleBarRow = new TableRow(this);
        titleBarRow.setLayoutParams(tableParams);

        TableRow menuRow = new TableRow(this);
        menuRow.setLayoutParams(tableParams);

        TableRow inputRow = new TableRow(this);
        inputRow.setLayoutParams(rowParams);

        TableRow outputRow = new TableRow(this);
        outputRow.setLayoutParams(rowParams);

        GradientDrawable gd = new GradientDrawable();
        gd.setColor(Color.GRAY);
        gd.setCornerRadius(5);
        gd.setStroke(1, 0xFF000000);

        TextView titleBar = new TextView(this);
        titleBar.setText(appTitle);
        titleBar.setTextSize(20);
        titleBar.setGravity(Gravity.CENTER_VERTICAL);
        titleBar.setHeight(200);
        titleBar.setTypeface(Typeface.SERIF);
        titleBar.setTextColor(Color.BLACK);
        titleBarRow.setBackgroundDrawable(gd);
        titleBarRow.addView(titleBar);

        TextView menuTextView = new TextView(this);
        menuTextView.setLayoutParams(rowParams);
        menuTextView.setText(getMenuString(menuMap));
        menuRow.addView(menuTextView);
        menuTextView.setTextSize(18);

        final EditText inputEditTextView = new EditText(this);
        menuTextView.setLayoutParams(rowParams);
        inputRow.addView(inputEditTextView);
        inputEditTextView.setHint(INPUT_HINT_TEXT);
        inputEditTextView.setSingleLine();

        final Button actionButton = new Button(this);
        actionButton.setLayoutParams(rowParams);
        actionButton.setText(ACTION_BUTTON_TEXT);
        inputRow.addView(actionButton);

        outputTextView = new TextView(this);
        outputTextView.setLayoutParams(rowParams);
        outputTextView.setVerticalScrollBarEnabled(true);
        outputTextView.setMovementMethod(new ScrollingMovementMethod());
        outputTextView.setMaxWidth(outputTextView.getWidth());
        outputRow.addView(outputTextView);

        parentLayout.addView(titleBarRow,
                new TableLayout.LayoutParams(
                        TableLayout.LayoutParams.MATCH_PARENT,
                        TableLayout.LayoutParams.WRAP_CONTENT));
        parentLayout.addView(menuRow,
                new TableLayout.LayoutParams(
                        TableLayout.LayoutParams.MATCH_PARENT,
                        TableLayout.LayoutParams.WRAP_CONTENT));
        parentLayout.addView(inputRow,
                new TableLayout.LayoutParams(
                        TableLayout.LayoutParams.MATCH_PARENT,
                        TableLayout.LayoutParams.WRAP_CONTENT));
        parentLayout.addView(outputRow,
                new TableLayout.LayoutParams(
                        TableLayout.LayoutParams.MATCH_PARENT,
                        TableLayout.LayoutParams.WRAP_CONTENT));
        setContentView(parentLayout);
        inputEditTextView.addTextChangedListener(new TextWatcher() {
            public void afterTextChanged(Editable s) {
            }

            public void beforeTextChanged(CharSequence s, int start, int count,
                    int after) {
            }

            public void onTextChanged(CharSequence s, int start, int before,
                    int count) {
            }
        });

        actionButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View arg0) {
                String inputText = inputEditTextView.getText().toString();
                if (!inputText.isEmpty()) {
                    String key = "";
                    String argument = null;
                    if (inputText.contains(" ")) {
                        String temptext[] = inputText.split("\\ ");
                        key = temptext[0];
                        if (temptext.length > 1)
                            argument = temptext[1];
                    } else {
                        key = inputText;
                    }
                    if (menuMap.containsKey(key)) {
                        executeAction(menuMap, key, argument,
                                inputEditTextView);
                    } else
                        showOutPut(INVALID_MENU_OPTION_MESSAGE);
                }

            }

        });
    }

    private String getMenuString(Map<String, MenuInfo> menuMap) {
        String text = EMPTY_TEXT_STRING;
        Set<Entry<String, MenuInfo>> set = menuMap.entrySet();
        Iterator<Entry<String, MenuInfo>> i = set.iterator();
        while (i.hasNext()) {
            Map.Entry<String, MenuInfo> me = (Map.Entry<String, MenuInfo>) i
                    .next();
            text += me.getKey() + ". ";
            MenuInfo menuInfo = (MenuInfo) me.getValue();
            text += menuInfo.menuText + "\n";
        }
        return text;
    }

    protected MenuInfo getMenuInfo(Map<String, MenuInfo> menuMap, String key) {
        MenuInfo menuInfo = menuMap.get(key);
        return menuInfo;

    }

    public static void showOutPut(String outputText) {
        Base.outPutString += outputText + "\n";
        outputTextView.setText(outPutString);
    }

    public static void clearOutPut() {
        outPutString = LOG_HEADER;
        outputTextView.setText(outPutString);
    }

    private void registerActions(Object actionClassObj) {
        this.actionObject = actionClassObj;
    }

    private boolean isRegistered() {
        if (actionObject == null) {
            return false;
        }
        return true;
    }

    private Method getDeclaredMethod(Object obj, String name,
            Class<?>... parameterTypes) {
        if (isRegistered()) {
            try {
                return obj.getClass().getDeclaredMethod(name, parameterTypes);
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
            } catch (SecurityException e) {
                e.printStackTrace();
            }
        } else
            showOutPut(ACTIONS_REGISTER_MESSAGE);

        return null;
    }

    private void executeAction(Map<String, MenuInfo> menuMap, String key,
            String argument, EditText inputEditView) {
        MenuInfo menuInfo = getMenuInfo(menuMap, key);
        Method action = null;

        if (argument == null) {
            action = getDeclaredMethod(actionObject, menuInfo.actionName);
        } else {
            Class[] cArg = new Class[1];
            cArg[0] = String.class;
            action = getDeclaredMethod(actionObject, menuInfo.actionName,
                    cArg[0]);
        }
        if (null != action) {
            try {
                if (argument == null) {
                    action.invoke(actionObject);
                } else
                    action.invoke(actionObject, new String(argument));
            } catch (IllegalAccessException e) {
                showOutPut(ILLEGAL_ACCESS_EXCEPTION);
            } catch (IllegalArgumentException e) {
                showOutPut(ILLEGAL_ARGUMENT_EXCEPTION);
            } catch (InvocationTargetException e) {
                showOutPut(INVOCATION_TARGET_EXCEPTION);
            }
        } else {
            showOutPut(ACTION_NOT_FOUND_MESSAGE);
        }

        InputMethodManager inputManager = (InputMethodManager) getSystemService(
                Context.INPUT_METHOD_SERVICE);
        inputManager.hideSoftInputFromWindow(inputEditView.getWindowToken(), 0);
    }
}
