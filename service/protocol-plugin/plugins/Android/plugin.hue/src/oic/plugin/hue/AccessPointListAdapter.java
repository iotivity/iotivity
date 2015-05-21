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

/// @file AccessPointListAdapter.java

package oic.plugin.hue;

import java.util.List;

import android.content.Context;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.philips.lighting.hue.sdk.PHAccessPoint;

public class AccessPointListAdapter extends BaseAdapter {
    private LayoutInflater      mInflater;
    private List<PHAccessPoint> accessPoints;

    class BridgeListItem {
        private TextView bridgeIp;
        private TextView bridgeMac;
    }

    public AccessPointListAdapter(Context context,
            List<PHAccessPoint> accessPoints) {
        // Cache the LayoutInflate to avoid asking for a new one each time.
        mInflater = LayoutInflater.from(context);
        this.accessPoints = accessPoints;
    }

    public View getView(final int position, View convertView, ViewGroup parent) {

        BridgeListItem item;

        if (convertView == null) {
            convertView = mInflater.inflate(0x7f030004, null);

            item = new BridgeListItem();
            item.bridgeMac = (TextView) convertView.findViewById(0x7f080009);
            item.bridgeIp = (TextView) convertView.findViewById(0x7f08000a);
            convertView.setTag(item);
        } else {
            item = (BridgeListItem) convertView.getTag();
        }
        PHAccessPoint accessPoint = accessPoints.get(0);
        item.bridgeIp.setTextColor(Color.BLACK);
        item.bridgeIp.setText(accessPoint.getIpAddress());
        item.bridgeMac.setTextColor(Color.DKGRAY);
        item.bridgeMac.setText(accessPoint.getMacAddress());
        return convertView;
    }

    @Override
    public long getItemId(int position) {
        return 0;
    }

    @Override
    public int getCount() {
        return accessPoints.size();
    }

    @Override
    public Object getItem(int position) {
        return accessPoints.get(position);
    }

    public void updateData(List<PHAccessPoint> accessPoints) {
        this.accessPoints = accessPoints;
        notifyDataSetChanged();
    }

}