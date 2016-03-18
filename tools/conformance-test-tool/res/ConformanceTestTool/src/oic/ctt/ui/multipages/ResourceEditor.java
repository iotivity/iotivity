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

package oic.ctt.ui.multipages;

import static oic.ctt.ui.types.IDType.RESOURCE_EDITOR_ID;
import static oic.ctt.ui.util.PopUpUtil.ACTIVE_WORKBENCH_WINDOW_SHELL;
import static oic.ctt.ui.util.PopUpUtil.displayMessageBox;
import static oic.ctt.ui.util.PopUpUtil.RESOURCE_SAVE_TEXT;
import static oic.ctt.ui.util.ResourceUtil.*;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;

import oic.ctt.ui.UIConst;
import oic.ctt.ui.util.DUTDescriptorUtil;
import oic.ctt.ui.util.ResourceDescription;
import oic.ctt.ui.util.ResourceProvider;
import oic.ctt.ui.util.ResourceUtil;
import oic.ctt.ui.views.DoubleClickListenerThread;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.action.Action;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;

public class ResourceEditor extends EditorPart {
	private static final String RESOURCE_EDITOR_PART_NAME = "Resource Editor";
	public static final String ID = RESOURCE_EDITOR_ID.toString();
	private ResourceDescription resourceDescription;
	private ResourceProvider resourceProvider;
	private int count;
	private Text diText;
	private Text hrefText;
	private Text rtText;
	private Combo disCombo;
	private Combo obsCombo;
	private Text ifText;
	private Combo ccCombo;
	private Combo csCombo;
	private Combo rCombo;
	private Combo ucCombo;
	private Combo upCombo;
	private Combo dCombo;
	private Combo[] ccCsRUcUpDComboList = { ccCombo, csCombo, rCombo, ucCombo,
			upCombo, dCombo };
	private ArrayList<Map<String, Text>> repTextList;
	private Map<String, Control> allControl;
	private Text payloadTypeText;
	private Combo dutTypeCombo;
	private Button addRepButton;

	public ResourceEditor() {

	}

	@Override
	public void doSave(IProgressMonitor monitor) {

	}

	@Override
	public void doSaveAs() {

	}

	@Override
	public void init(IEditorSite site, IEditorInput input)
			throws PartInitException {
		setSite(site);
		setInput(input);
		site.getActionBars();
		setPartName(RESOURCE_EDITOR_PART_NAME);
		getResourceDescription(DoubleClickListenerThread.resourceHref);
		repTextList = new ArrayList<Map<String, Text>>();
		allControl = new HashMap<String, Control>();
		count = 1;
	}

	@Override
	public boolean isDirty() {
		return false;
	}

	@Override
	public boolean isSaveAsAllowed() {
		return false;
	}

	@Override
	public void createPartControl(Composite parent) {

		final Action editResourceAction = new Action(UIConst.SAVE_ACTION_TEXT) {
			@Override
			public void run() {
				super.run();
				editResource();
			}
		};

		editResourceAction.setToolTipText(UIConst.SAVE_ACTION_TEXT);
		getEditorSite().getActionBars().getToolBarManager()
				.add(editResourceAction);
		GridLayout layout = new GridLayout();
		layout.numColumns = 4;
		GridData gridData = new GridData(SWT.FILL, SWT.BEGINNING, true, false);
		ScrolledComposite scrolledComposite = new ScrolledComposite(parent,
				SWT.H_SCROLL | SWT.V_SCROLL);
		Composite composite = new Composite(scrolledComposite, SWT.NONE);
		scrolledComposite.setContent(composite);
		composite.setLayout(layout);
		composite.layout();
		scrolledComposite.setExpandHorizontal(true);
		scrolledComposite.setExpandVertical(true);
		scrolledComposite.setMinSize(composite.computeSize(SWT.DEFAULT,
				SCROLL_GRID_MIN_SIZE));

		/*
		 * adding control to grid
		 */

		// di row start
		new Label(composite, SWT.NONE).setText(KEY_DI);
		ResourceUtil.skipCell(composite, 2);
		diText = ResourceUtil.addTextBox(composite, gridData,
				resourceProvider.getDI());
		diText.setEditable(false);
		// oc row start
		new Label(composite, SWT.NONE).setText(KEY_OC);
		ResourceUtil.skipCell(composite, 3);
		// href row start
		new Label(composite, SWT.NONE);
		new Label(composite, SWT.NONE).setText(KEY_HREF);
		ResourceUtil.skipCell(composite, 1);
		hrefText = ResourceUtil.addTextBox(composite, gridData,
				DoubleClickListenerThread.resourceHref);
		allControl.put(KEY_HREF, hrefText);
		// rt row start
		ResourceUtil.skipCell(composite, 1);
		new Label(composite, SWT.NONE).setText(KEY_RT);
		ResourceUtil.skipCell(composite, 1);
		rtText = ResourceUtil.addTextBox(composite, gridData,
				resourceDescription.getRt());
		allControl.put(KEY_RT, rtText);
		// dis row start
		ResourceUtil.skipCell(composite, 1);
		new Label(composite, SWT.NONE).setText(KEY_DIS);
		ResourceUtil.skipCell(composite, 1);
		disCombo = ResourceUtil.addComboBox(composite, gridData, OBS_DIS_VALUE,
				Integer.toString(resourceDescription.getDis()));
		allControl.put(KEY_DIS, disCombo);
		// obs row start
		ResourceUtil.skipCell(composite, 1);
		new Label(composite, SWT.NONE).setText(KEY_OBS);
		ResourceUtil.skipCell(composite, 1);
		obsCombo = ResourceUtil.addComboBox(composite, gridData, OBS_DIS_VALUE,
				Integer.toString(resourceDescription.getObs()));
		allControl.put(KEY_OBS, obsCombo);
		// if row start
		ResourceUtil.skipCell(composite, 1);
		new Label(composite, SWT.NONE).setText(KEY_IF);
		ResourceUtil.skipCell(composite, 1);
		// start if row
		ifText = ResourceUtil
				.addTextBox(composite, gridData, getIfTextValues());
		allControl.put(KEY_IF, ifText);
		// start CcCsRUcUpD rows
		ArrayList<Boolean> ccCsRUcUpDList = resourceDescription.getCcCsRUcUpD();
		ResourceUtil.addCcCsRUcUpDRows(gridData, composite,
				ccCsRUcUpDComboList, ccCsRUcUpDList);
		// payload row
		ResourceUtil.skipCell(composite, 1);
		new Label(composite, SWT.NONE).setText(KEY_PAYLOAD_TYPE);
		ResourceUtil.skipCell(composite, 1);
		payloadTypeText = ResourceUtil.addTextBox(composite, gridData,
				resourceDescription.getPayloadType());
		allControl.put(KEY_PAYLOAD_TYPE, payloadTypeText);
		// dutType row
		ResourceUtil.skipCell(composite, 1);
		new Label(composite, SWT.NONE).setText(KEY_DUT_TYPE);
		ResourceUtil.skipCell(composite, 1);
		dutTypeCombo = ResourceUtil.addComboBox(composite, gridData,
				DUT_TYPE_VALUE, resourceDescription.getDutType());
		allControl.put(KEY_DUT_TYPE, dutTypeCombo);
		// rep row start
		ResourceUtil.skipCell(composite, 1);
		new Label(composite, SWT.NONE).setText(KEY_REP);
		ResourceUtil.skipCell(composite, 1);
		addRepButton = ResourceUtil.addButton(composite, gridData,
				REP_BUTTON_TEXT);

		addRepButton.addSelectionListener(new SelectionListener() {

			@Override
			public void widgetSelected(SelectionEvent e) {
				repTextList.add(ResourceUtil.addNewRep(composite, gridData,
						count++));
			}

			@Override
			public void widgetDefaultSelected(SelectionEvent e) {

			}
		});

		addRepRows(gridData, composite);
	}

	private void addRepRows(GridData gridData, Composite composite) {
		ArrayList<Map<String, String>> repList = resourceDescription.getRep();
		for (Map<String, String> m : repList) {
			ResourceUtil.skipCell(composite, 2);
			new Label(composite, SWT.NONE).setText(KEY_REP + " : "
					+ Integer.toString(count++));
			new Label(composite, SWT.NONE);
			Map<String, Text> mapTextList = new LinkedHashMap<String, Text>();
			for (String repKey : m.keySet()) {
				ResourceUtil.skipCell(composite, 2);
				new Label(composite, SWT.NONE).setText(repKey);
				Text repText = null;
				repText = ResourceUtil.addTextBox(composite, gridData,
						m.get(repKey));
				mapTextList.put(repKey, repText);
			}
			repTextList.add(mapTextList);
		}
	}

	@Override
	public void setFocus() {
	}

	public void getResourceDescription(String resourceHref) {
		resourceProvider = new ResourceProvider(DESCRIPTOR_PATH);
		resourceDescription = resourceProvider
				.getResourceDescription(resourceHref);
	}

	private String getIfTextValues() {
		ArrayList<String> ifList = resourceDescription.getIfList();
		String ifTextValues = "";
		for (int i = 0; i < ifList.size(); i++) {
			ifTextValues += ifList.get(i);
			if (i != ifList.size() - 1) {
				ifTextValues += ",";
			}
		}
		return ifTextValues;
	}

	private void editResource() {
		ResourceProvider resourceProvider = new ResourceProvider(
				ResourceUtil.DESCRIPTOR_PATH);
		String saveMessage = resourceProvider.saveResourceDescription(
				ResourceUtil.setResourceDescription(allControl,
						ccCsRUcUpDComboList, repTextList),
				DoubleClickListenerThread.resourceHref);
		displayMessageBox(ACTIVE_WORKBENCH_WINDOW_SHELL, saveMessage,
				RESOURCE_SAVE_TEXT, SWT.OK);
		if (saveMessage == DUTDescriptorUtil.SUCCESS) {
			UIConst.closeOpenedEditors(UIConst.getActivePage());
			ResourceUtil.refreshResourceView();
		}

	}

}
