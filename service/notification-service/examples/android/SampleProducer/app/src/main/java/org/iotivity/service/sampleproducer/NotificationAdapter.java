package org.iotivity.service.sampleproducer;

import android.app.Activity;
import android.content.Context;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.TextView;

import org.iotivity.service.notificationservice.NotificationObject;
import org.iotivity.service.notificationservice.NotificationType;
import org.iotivity.service.notificationservice.impl.ImageNotification;
import org.iotivity.service.notificationservice.impl.TextNotification;
import org.iotivity.service.notificationservice.impl.VideoNotification;

import java.util.ArrayList;
import java.util.HashMap;

public class NotificationAdapter extends BaseAdapter {

    private Activity ProducerActivty;
    HashMap<Integer, Integer> notificationRead;
    private ArrayList<NotificationObject> notifications;
    private static LayoutInflater inflater = null;

    public NotificationAdapter(Activity activity, ArrayList<NotificationObject> data, HashMap<Integer, Integer> data_read) {
        ProducerActivty = activity;
        notifications = data;
        notificationRead=data_read;
        inflater = (LayoutInflater) ProducerActivty.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    }

    @Override
    public int getCount() {
        return notifications.size();
    }

    @Override
    public Object getItem(int position) {
        return position;
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    public static class ViewHolder {

        TextView textViewId, textViewObject, textViewRead;
        int read_count = 544;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {

        /*Log.d("get view", position + ""+notifications.size());
        View v=convertView;
        ViewHolder viewHolder;
        if(convertView==null)
        {
            v=inflater.inflate(R.layout.listitem,null);
            viewHolder=new ViewHolder();

            viewHolder.textViewId=(TextView)v.findViewById(R.id.notification_id);
            viewHolder.textViewObject=(TextView)v.findViewById(R.id.notification_object);
            viewHolder.textViewRead=(TextView)v.findViewById(R.id.notification_read);
        }
        else viewHolder=(ViewHolder)v.getTag();

        if(notifications.size()>0)
        {
            viewHolder.textViewId.setText("id");//notifications.get(position).getNotifcationId());
            viewHolder.textViewObject.setText(notifications.get(position).getNotifcationSender());
            viewHolder.textViewRead.setText("0");
        }
        */
        View v;
        v = inflater.inflate(R.layout.listitem, null);

        ViewHolder viewHolder = new ViewHolder();
        viewHolder.textViewId = (TextView) v.findViewById(R.id.notification_id);
        viewHolder.textViewObject = (TextView) v.findViewById(R.id.notification_object);
        viewHolder.textViewRead = (TextView) v.findViewById(R.id.notification_read);

        if (notifications.size() <= 0) return null;
        int id=notifications.get(position).getNotifcationId();
        viewHolder.textViewId.setText( id+ "");

        NotificationObject notificationObject = notifications.get(position);
        String message = "";
        if (notificationObject.getNotificationType() == NotificationType.TYPE_TEXT)
            message = ((TextNotification) notificationObject).getNotificationMessage();
        else if (notificationObject.getNotificationType() == NotificationType.TYPE_IMAGE)
            message = ((ImageNotification) notificationObject).getNotificationMessage();
        else if (notificationObject.getNotificationType() == NotificationType.TYPE_VIDEO)
            message = "Video";//((VideoNotification)notificationObject).getNotificationVideoUrl();
        viewHolder.textViewObject.setText(message);
        viewHolder.textViewRead.setText(notificationRead.get(id)+"");

        v.setOnClickListener(new OnItemClickListener(position));
        return v;
    }

    private class OnItemClickListener implements View.OnClickListener {
        private int mPosition;

        OnItemClickListener(int position) {
            mPosition = position;
        }

        @Override
        public void onClick(View arg0) {


            SampleProducer sp = (SampleProducer) ProducerActivty;

            sp.onItemClick(mPosition);
            ///ViewHolder viewHolder=(ViewHolder)arg0.getTag();
            //Log.d("read_count=",viewHolder.read_count+"");
        }
    }
}
