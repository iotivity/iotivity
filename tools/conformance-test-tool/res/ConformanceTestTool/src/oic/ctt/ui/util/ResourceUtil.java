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

package oic.ctt.ui.util;

import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;

import static oic.ctt.ui.UIConst.ROOT_PATH;
import oic.ctt.ui.views.ResourceView;

public class ResourceUtil {

	public static final String DESCRIPTOR_PATH = ROOT_PATH
			+ "libs/DUTDescriptor.json";
	private static final String[] CcCsRUcUpD_VALUE = { "true", "false" };
	public static final String initText = "";
	public static final String[] OBS_DIS_VALUE = { "0", "1" };
	public static final String[] DUT_TYPE_VALUE = { "SERVER", "CLIENT" };
	public static final String DEFAULT_PAYLOAD = "Object";
	public static final String REP_BUTTON_TEXT = "ADD REP";
	public static final int SCROLL_GRID_MIN_SIZE = 2000;
	public static final String KEY_DI = "di";
	public static final String KEY_OC = "oc";
	public static final String KEY_HREF = "href";
	public static final String KEY_RT = "rt";
	public static final String KEY_DIS = "dis";
	public static final String KEY_OBS = "obs";
	public static final String KEY_IF = "if";
	public static final String KEY_PAYLOAD_TYPE = "payloadType";
	public static final String KEY_DUT_TYPE = "dutType";
	public static final String KEY_REP = "rep";

	/**
	 * Create a text box and set text and layout.
	 * 
	 * @param parent
	 *            parent control to which text box will be added
	 * @param gridData
	 *            style of the text box
	 * @return a text box object
	 */
	public static Text addTextBox(Composite parent, GridData gridData,
			String text) {
		Text textBox = new Text(parent, SWT.BORDER);
		textBox.setText(text);
		textBox.setLayoutData(gridData);
		return textBox;
	}

	/**
	 * Create a button and set text and layout.
	 * 
	 * @param parent
	 *            parent control to which text box will be added
	 * @param gridData
	 *            style of the button
	 * @return a button object
	 */
	public static Button addButton(Composite parent, GridData gridData,
			String text) {
		Button button = new Button(parent, SWT.PUSH);
		button.setText(text);
		button.setLayoutData(gridData);
		return button;
	}

	/**
	 * Create a combo box and set text and layout.
	 * 
	 * @param parent
	 *            parent control to which combo box will be added
	 * @param gridData
	 *            style of the combo box
	 * @param items
	 *            to be displayed in combo box drop down list
	 * @return a combo box object
	 */
	public static Combo addComboBox(Composite parent, GridData gridData,
			String[] items) {
		return addComboBox(parent, gridData, items, "");
	}

	/**
	 * Create a combo box and set text and layout.
	 * 
	 * @param parent
	 *            parent control to which combo box will be added
	 * @param gridData
	 *            style of the combo box
	 * @param items
	 *            to be displayed in combo box drop down list
	 * @param selectedValue
	 *            to be displayed in combo box first time loading
	 * @return a combo box object
	 */
	public static Combo addComboBox(Composite parent, GridData gridData,
			String[] items, String selectedValue) {
		Combo combo = new Combo(parent, SWT.READ_ONLY);
		combo.setLayoutData(gridData);
		for (int i = 0; i < items.length; i++) {
			combo.add(items[i], i);
		}
		if (selectedValue.isEmpty() || selectedValue == null) {
			combo.select(0);
		} else {
			combo.select(combo.indexOf(selectedValue));
		}
		return combo;
	}

	/**
	 * To fill a cell of a grid with label control having no value and style
	 * 
	 * @param parent
	 *            parent control to which combo box will be added
	 * @param noOfCell
	 *            to be skipped
	 * @return none
	 */

	public static void skipCell(Composite parent, int noOfCell) {
		for (int i = 0; i < noOfCell; i++)
			new Label(parent, SWT.NONE);
	}

	/**
	 * To create rep controls("key", "type", "testdata", "accessmode")
	 * dynamically
	 * 
	 * @param parent
	 *            parent control to which combo box will be added
	 * @param gridData
	 *            style of the combo box
	 * @param count
	 *            serial no of rep controls to be added
	 * @return map of rep controls
	 */
	public static Map<String, Text> addNewRep(Composite composite,
			GridData gridData, int count) {
		ResourceUtil.skipCell(composite, 2);
		new Label(composite, SWT.NONE).setText(KEY_REP + " : "
				+ Integer.toString(count));
		new Label(composite, SWT.NONE);
		String[] repKeyList = { "key", "type", "testdata", "testdata2",
				"accessmode" };
		Map<String, Text> mapTextList = new LinkedHashMap<String, Text>();
		for (String repKey : repKeyList) {
			ResourceUtil.skipCell(composite, 2);
			new Label(composite, SWT.NONE).setText(repKey);
			Text repText = null;
			repText = ResourceUtil.addTextBox(composite, gridData, "");
			mapTextList.put(repKey, repText);
		}
		composite.layout();
		return mapTextList;
	}

	/**
	 * To get all rep controls text to a list dynamically
	 * 
	 * @param ArrayList
	 *            list of rep controls
	 * @return list of rep controls text
	 */
	public static ArrayList<Map<String, String>> getRepList(
			final ArrayList<Map<String, Text>> repTextList) {

		ArrayList<Map<String, String>> repList = new ArrayList<Map<String, String>>();
		for (Map<String, Text> rep : repTextList) {
			Map<String, String> tempMap = new LinkedHashMap<String, String>();
			for (String repKey : rep.keySet()) {
				tempMap.put(repKey, rep.get(repKey).getText());
			}
			repList.add(tempMap);
		}
		return repList;
	}

	/**
	 * To get all text values of CcCsRUcUpD Combo box
	 * 
	 * @param ArrayList
	 *            list of CcCsRUcUpD controls
	 * @return list of CcCsRUcUpD combo box controls text
	 */
	public static ArrayList<Boolean> getCcCsRUcUpDList(
			final Combo[] ccCsRUcUpDComboList) {
		ArrayList<Boolean> CcCsRUcUpDList = new ArrayList<Boolean>();
		for (Combo combo : ccCsRUcUpDComboList) {
			CcCsRUcUpDList.add(Boolean.valueOf(combo.getText()));
		}
		return CcCsRUcUpDList;
	}

	/**
	 * Split if text box text by comma and add to a list
	 * 
	 * @param ArrayList
	 *            list of CcCsRUcUpD controls
	 * @return list of if text box text split by comma
	 */
	public static ArrayList<String> getifList(final Text ifText) {
		ArrayList<String> ifList = new ArrayList<String>();
		String ifValue = ifText.getText();
		String[] ifValues = ifValue.split(",");
		for (String value : ifValues) {
			ifList.add(value);
		}
		return ifList;
	}

	/**
	 * add all Cc Cs R Uc Up D rows at a time and show combo box for each row
	 * 
	 * @param gridData
	 *            style of the controls
	 * @param parent
	 *            parent control to which combo box will be added
	 * @param ComboList
	 *            list of CcCsRUcUpD combo controls
	 * @param ArrayList
	 *            list of CcCsRUcUpD Values to be displayed in combo box when
	 *            loaded first time
	 * @return none
	 */
	public static void addCcCsRUcUpDRows(GridData gridData,
			Composite composite, Combo[] ccCsRUcUpDCombotList,
			ArrayList<Boolean> ccCsRUcUpDList) {
		String[] labelText = { "Cc", "Cs", "R", "Uc", "Up", "D" };
		if (ccCsRUcUpDList.size() == 0) {
			List<Boolean> tempList = new ArrayList<Boolean>(
					Collections.nCopies(6, true));
			ccCsRUcUpDList.addAll(tempList);
		}
		for (int i = 0; i < labelText.length; i++) {
			ResourceUtil.skipCell(composite, 1);
			new Label(composite, SWT.NONE).setText(labelText[i]);
			ResourceUtil.skipCell(composite, 1);
			ccCsRUcUpDCombotList[i] = ResourceUtil.addComboBox(composite,
					gridData, CcCsRUcUpD_VALUE, ccCsRUcUpDList.get(i)
							.toString());
		}
	}

	/**
	 * refresh resource view
	 * 
	 * @return none
	 */
	public static void refreshResourceView() {
		IWorkbenchPage page = PlatformUI.getWorkbench()
				.getActiveWorkbenchWindow().getActivePage();
		ResourceView resourceView = (ResourceView) page
				.findView(ResourceView.ID);
		resourceView.refreshResourceView();
	}

	/**
	 * set all resource description
	 * 
	 * @param Map
	 *            map of all controls
	 * @param ComboList
	 *            list of CcCsRUcUpD combo controls
	 * @param ArrayList
	 *            list of rep text controls
	 * @return none
	 */
	public static ResourceDescription setResourceDescription(
			final Map<String, Control> allText,
			final Combo[] ccCsRUcUpDComboList,
			final ArrayList<Map<String, Text>> repTextList) {

		ResourceDescription resourceDescription = new ResourceDescription();
		resourceDescription.setHref(((Text) allText.get(KEY_HREF)).getText());
		resourceDescription.setRt(((Text) allText.get(KEY_RT)).getText());
		resourceDescription.setDis(Integer.valueOf(((Combo) allText
				.get(KEY_DIS)).getText()));
		resourceDescription.setObs(Integer.valueOf(((Combo) allText
				.get(KEY_OBS)).getText()));
		resourceDescription.setIf(ResourceUtil.getifList((Text) allText
				.get(KEY_IF)));
		resourceDescription.setCcCsRUcUpD(ResourceUtil
				.getCcCsRUcUpDList(ccCsRUcUpDComboList));
		resourceDescription.setRep(ResourceUtil.getRepList(repTextList));
		resourceDescription.setPayloadType(((Text) allText
				.get(KEY_PAYLOAD_TYPE)).getText());
		resourceDescription.setDutType(((Combo) allText.get(KEY_DUT_TYPE))
				.getText());
		return resourceDescription;

	}
}
