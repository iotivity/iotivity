//******************************************************************
//
// Copyright 2016 Samsung Electronics All Rights Reserved.
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

package org.iotivity.service.ns.common;

import android.util.Log;
import org.iotivity.base.OcRepresentation;

/**
  * @class   Message
  * @brief   This class provides implementation of Notification Message object.
  */
public class Message
{
    private static final String LOG_TAG = "NotificationService_Message";

    public enum MessageType
    {
        ALERT(1),
        NOTICE(2),
        EVENT(3),
        INFO(4),
        WARNING(5);
        private int type;

        private MessageType(int type)
        {
            this.type = type;
        }

        public int getMessageType()
        {
            return this.type;
        }
    };
    public long mMessageId                     = 0;
    public String mProviderId                  = null;

    public String mSourceName                  = null;
    public MessageType mType                   = MessageType.ALERT;
    public String mTime                        = null;
    public long mTTL                           = 0;
    public String mTitle                       = null;
    public String mContentText                 = null;
    public MediaContents mMediaContents        = null;
    public String mTopic                       = null;
    public OcRepresentation mExtraInfo         = null;

    private long mNativeHandle                 = 0;

    public Message(String title, String contentText, String sourceName)
    {
        Log.i (LOG_TAG, "Message()");

        mTitle = title;
        mContentText = contentText;
        mSourceName = sourceName;
    }

    public long getMessageId()
    {
        return mMessageId;
    }

    public String getProviderId()
    {
        return mProviderId;
    }

    public String getSourceName ()
    {
        return mSourceName;
    }

    public MessageType getType()
    {
        return mType;
    }

    public String getTime()
    {
        return mTime;
    }

    public long getTTL()
    {
        return mTTL;
    }

    public String getTitle()
    {
        return mTitle;
    }

    public String getContentText()
    {
        return mContentText;
    }

    public MediaContents getMediaContents()
    {
        return mMediaContents;
    }

    public String getTopic()
    {
        return mTopic;
    }

    public OcRepresentation getExtraInfo()
    {
        return mExtraInfo;
    }

    public void setSourceName (String sourceName)
    {
        mSourceName = sourceName;
    }

    public void setType(MessageType type)
    {
        mType = type;
    }

    public void setTime(String time)
    {
        mTime = time;
    }

    public void setTTL(long ttl)
    {
        mTTL = ttl;
    }

    public void setTitle(String title)
    {
        mTitle = title;
    }

    public void setContentText(String contextText)
    {
        mContentText = contextText;
    }

    public void setMediaContents(MediaContents mediaContents)
    {
        mMediaContents = mediaContents;
    }

    public void setTopic(String topic)
    {
        mTopic = topic;
    }

    public void setExtraInfo(OcRepresentation extraInfo)
    {
        mExtraInfo = extraInfo;
    }
}
