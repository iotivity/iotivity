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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

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

import oic.ctt.ui.UIConst;
import oic.ctt.ui.util.DUTDescriptorUtil;
import oic.ctt.ui.util.ResourceProvider;
import oic.ctt.ui.util.ResourceUtil;
import static oic.ctt.ui.types.IDType.RESOURCE_CREATOR_ID;
import static oic.ctt.ui.util.ResourceUtil.*;
import static oic.ctt.ui.util.PopUpUtil.ACTIVE_WORKBENCH_WINDOW_SHELL;
import static oic.ctt.ui.util.PopUpUtil.RESOURCE_SAVE_TEXT;
import static oic.ctt.ui.util.PopUpUtil.displayMessageBox;

public class ResourceCreator extends EditorPart {
	public static final String ID = RESOURCE_CREATOR_ID.toString();
	private static final String RESOURCE_CREATOR_PART_NAME = "Resource Creator";
	private int count;
	private Text diText;
	private Text hrefText;
	private Text rtText;
	private Text payloadTypeText;
	private Text ifText;
	private Combo disText;
	private Combo obsText;
	private Combo CcText;
	private Combo CsText;
	private Combo RText;
	private Combo UcText;
	private Combo UpText;
	private Combo DText;
	private Combo dutTypeText;
	private Combo[] ccCsRUcUpDComboList = { CcText, CsText, RText, UcText,
			UpText, DText };
	private ArrayList<Map<String, Text>> repTextList;
	private Map<String, Control> allText;
	private Button addRepButton;

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
		setPartName(RESOURCE_CREATOR_PART_NAME);
		count = 1;
		repTextList = new ArrayList<Map<String, Text>>();
		allText = new HashMap<String, Control>();
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
		ResourceProvider resourceProvider = new ResourceProvider(
				DESCRIPTOR_PATH);

		final Action createResourceAction = new Action(
				UIConst.CREATE_RESOURCE_ACTION_TEXT) {
			@Override
			public void run() {
				super.run();
				createResource();
			}
		};

		createResourceAction
				.setToolTipText(UIConst.CREATE_RESOURCE_ACTION_TEXT);
		createResourceAction
				.setDescription(UIConst.CREATE_RESOURCE_ACTION_TEXT);
		getEditorSite().getActionBars().getToolBarManager()
				.add(createResourceAction);
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
		allText.put(KEY_DI, diText);
		// oc row start
		new Label(composite, SWT.NONE).setText(KEY_OC);
		ResourceUtil.skipCell(composite, 3);
		// href row start
		new Label(composite, SWT.NONE);
		new Label(composite, SWT.NONE).setText(KEY_HREF);
		ResourceUtil.skipCell(composite, 1);
		hrefText = ResourceUtil.addTextBox(composite, gridData, initText);
		allText.put(KEY_HREF, hrefText);
		// rt row start
		ResourceUtil.skipCell(composite, 1);
		new Label(composite, SWT.NONE).setText(KEY_RT);
		ResourceUtil.skipCell(composite, 1);
		rtText = ResourceUtil.addTextBox(composite, gridData, initText);
		allText.put(KEY_RT, rtText);
		// dis row start
		ResourceUtil.skipCell(composite, 1);
		new Label(composite, SWT.NONE).setText(KEY_DIS);
		ResourceUtil.skipCell(composite, 1);
		disText = ResourceUtil.addComboBox(composite, gridData, OBS_DIS_VALUE);
		allText.put(KEY_DIS, disText);
		// obs row start
		ResourceUtil.skipCell(composite, 1);
		new Label(composite, SWT.NONE).setText(KEY_OBS);
		ResourceUtil.skipCell(composite, 1);
		obsText = ResourceUtil.addComboBox(composite, gridData, OBS_DIS_VALUE);
		allText.put(KEY_OBS, obsText);
		// if row start
		ResourceUtil.skipCell(composite, 1);
		new Label(composite, SWT.NONE).setText(KEY_IF);
		ResourceUtil.skipCell(composite, 1);
		ifText = ResourceUtil.addTextBox(composite, gridData, initText);
		allText.put(KEY_IF, ifText);
		// start CcCsRUcUpD rows
		ArrayList<Boolean> ccCsRUcUpDList = new ArrayList<Boolean>();
		ResourceUtil.addCcCsRUcUpDRows(gridData, composite,
				ccCsRUcUpDComboList, ccCsRUcUpDList);
		// payload row
		ResourceUtil.skipCell(composite, 1);
		new Label(composite, SWT.NONE).setText(KEY_PAYLOAD_TYPE);
		ResourceUtil.skipCell(composite, 1);
		payloadTypeText = ResourceUtil.addTextBox(composite, gridData,
				DEFAULT_PAYLOAD);
		allText.put(KEY_PAYLOAD_TYPE, payloadTypeText);
		// dutType row
		ResourceUtil.skipCell(composite, 1);
		new Label(composite, SWT.NONE).setText(KEY_DUT_TYPE);
		ResourceUtil.skipCell(composite, 1);
		dutTypeText = ResourceUtil.addComboBox(composite, gridData,
				DUT_TYPE_VALUE);
		allText.put(KEY_DUT_TYPE, dutTypeText);
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

		repTextList.add(ResourceUtil.addNewRep(composite, gridData, count++));

	}

	private void createResource() {
		ResourceProvider resourceProvider = new ResourceProvider(
				DESCRIPTOR_PATH);
		String resourceCreationMessage = resourceProvider
				.addNewResource(ResourceUtil.setResourceDescription(allText,
						ccCsRUcUpDComboList, repTextList));
		displayMessageBox(ACTIVE_WORKBENCH_WINDOW_SHELL,
				resourceCreationMessage, RESOURCE_SAVE_TEXT, SWT.OK);
		if (resourceCreationMessage == DUTDescriptorUtil.SUCCESS) {
			ResourceUtil.refreshResourceView();
		}
	}

	@Override
	public void setFocus() {

	}

}
