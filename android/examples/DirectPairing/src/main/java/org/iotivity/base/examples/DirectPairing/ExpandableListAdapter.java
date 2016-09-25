/*
 * //******************************************************************
 * //
 * // Copyright 2016 Samsung Electronics All Rights Reserved.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * //
 * // Licensed under the Apache License, Version 2.0 (the "License");
 * // you may not use this file except in compliance with the License.
 * // You may obtain a copy of the License at
 * //
 * //      http://www.apache.org/licenses/LICENSE-2.0
 * //
 * // Unless required by applicable law or agreed to in writing, software
 * // distributed under the License is distributed on an "AS IS" BASIS,
 * // WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * // See the License for the specific language governing permissions and
 * // limitations under the License.
 * //
 * //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples.DirectPairing;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import org.iotivity.base.examples.DirectPairing.Led;

import java.util.List;

public class ExpandableListAdapter extends BaseExpandableListAdapter {
    private List<String> mGroupList;
    private List<Object> mChilditemsList;
    private Context mContext;

    public ExpandableListAdapter(List<String> groupList, List<Object> childitemsList,
                                 Context context) {
        super();
        this.mContext = context;
        this.mGroupList = groupList;
        this.mChilditemsList = childitemsList;
    }

    @Override
    public int getGroupCount() {
        return mGroupList.size();
    }

    @Override
    public int getChildrenCount(int groupPosition) {
        return 1;
    }

    @Override
    public Object getGroup(int groupPosition) {
        return mGroupList.get(groupPosition);
    }

    @Override
    public Object getChild(int groupPosition, int childPosition) {
        return this.mChilditemsList.get(childPosition);
    }

    @Override
    public long getGroupId(int groupPosition) {
        return groupPosition;
    }

    @Override
    public long getChildId(int groupPosition, int childPosition) {
        return childPosition;
    }

    @Override
    public boolean hasStableIds() {
        return false;
    }

    @Override
    public View getGroupView(int groupPosition, boolean isExpanded,
                             View convertView, ViewGroup parent) {

        String data = (String) getGroup(groupPosition);
        if (convertView == null) {
            LayoutInflater infalInflater = (LayoutInflater) this.mContext
                    .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView = infalInflater.inflate(R.layout.list_group, null);
        }

        ImageView img = (ImageView) convertView.findViewById(R.id.image);
        if (isExpanded) {
            img.setImageResource(R.drawable.arrow_down);

        } else {
            img.setImageResource(R.drawable.arrow_right);
        }

        TextView groupText = (TextView) convertView.findViewById(R.id.title);
        groupText.setText(mGroupList.get(groupPosition));

        return convertView;
    }

    @Override
    public View getChildView(int groupPosition, int childPosition, boolean isLastChild,
                             View convertView, ViewGroup parent) {
        LayoutInflater inflater = (LayoutInflater) mContext
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        convertView = inflater.inflate(R.layout.list_item, parent, false);

        Led ledResource = (Led) getChild(groupPosition, childPosition);
        TextView textItem = (TextView) convertView
                .findViewById(R.id.textViewItem);
        TextView textSource = (TextView) convertView
                .findViewById(R.id.textViewSource);
        TextView textStatus = (TextView) convertView
                .findViewById(R.id.textViewStatus);
        textStatus.setText("URI:" + "/a/led");
        textItem.setText("State: " + ledResource.getState());
        textSource.setText("Power: " + ledResource.getPower());

        return convertView;

    }

    @Override
    public boolean isChildSelectable(int groupPosition, int childPosition) {
        return true;
    }

}
