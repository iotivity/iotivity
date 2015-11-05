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
package oic.ctt.ui.log;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

/**
 * Container for a list of log messages. The list of messages are maintained in
 * a circular buffer (FIFO).
 */
public final class LogMessageList {
    /** Preference key for size of the FIFO. */
    public static final String        MAX_MESSAGES_PREFKEY = "Log.messagelist.max.size";
    /** Default value for max # of messages. */
    public static final int           MAX_MESSAGES_DEFAULT = 20000;
    private int                       mFifoSize;
    private BlockingQueue<LogMessage> mQ;
    private LogMessage[]              mQArray;

    /**
     * Construct an empty message list.
     * 
     * @param maxMessages
     *            capacity of the circular buffer
     */
    public LogMessageList(int maxMessages) {
        mFifoSize = maxMessages;
        mQ = new ArrayBlockingQueue<LogMessage>(mFifoSize);
        mQArray = new LogMessage[mFifoSize];
    }

    /**
     * Resize the message list.
     * 
     * @param n
     *            new size for the list
     */
    public synchronized void resize(int n) {
        mFifoSize = n;
        if (mFifoSize > mQ.size()) {
            mQ = new ArrayBlockingQueue<LogMessage>(mFifoSize, true, mQ);
        } else {
            LogMessage[] curMessages = mQ.toArray(new LogMessage[mQ.size()]);
            mQ = new ArrayBlockingQueue<LogMessage>(mFifoSize);
            for (int i = curMessages.length - mFifoSize; i < curMessages.length; i++) {
                mQ.offer(curMessages[i]);
            }
        }
        mQArray = new LogMessage[mFifoSize];
    }

    /**
     * Append a message to the list. If the list is full, the first message will
     * be popped off of it.
     * 
     * @param m
     *            log to be inserted
     */
    public synchronized void appendMessages(final List<LogMessage> messages) {
        ensureSpace(messages.size());
        Iterator<LogMessage> iterator = messages.iterator();
        while (iterator.hasNext()) {
            LogMessage next = iterator.next();
            mQ.offer(next);
        }
    }

    /**
     * Ensure that there is sufficient space for given number of messages.
     * 
     * @return list of messages that were deleted to create additional space.
     */
    public synchronized List<LogMessage> ensureSpace(int messageCount) {
        List<LogMessage> l = new ArrayList<LogMessage>(messageCount);

        while (mQ.remainingCapacity() < messageCount) {
            l.add(mQ.poll());
        }

        return l;
    }

    /**
     * Clear all messages in the list.
     */
    public synchronized void clear() {
        mQ.clear();
    }

    /**
     * Obtain all the messages currently present in the list.
     * 
     * @return array containing all the log messages
     */
    public Object[] toArray() {
        if (mQ.size() == mFifoSize) {
            /*
             * Once the queue is full, it stays full until the user explicitly
             * clears all the logs. Optimize for this case by not reallocating
             * the array.
             */
            return mQ.toArray(mQArray);
        }
        return mQ.toArray();
    }
}
