/******************************************************************
 *
 * Copyright 2014 Samsung Electronics All Rights Reserved.
 *
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************/

#ifndef BASE_SLEP_UTIL_WORKER_POOL_H_
#define BASE_SLEP_UTIL_WORKER_POOL_H_
#include <map>
#include <string>

#include "base/bind.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop.h"
#include "base/threading/thread.h"

namespace slep_util {
static const base::PlatformThreadId kInvalidWorkerThreadID = 0;
// Default worker threads number per worker pool
static const int kDefaultWorkersNum = 5;

// 'WorkerPool' provides a thread pool which will execute the posted task to it.
class BASE_EXPORT WorkerPool {
 public:
    // this class observes the tasks on worker thread
  class WorkerThreadTaskObserver : public MessageLoop::TaskObserver {
    public:
      // constructor
      WorkerThreadTaskObserver();
      // destructor
      virtual ~WorkerThreadTaskObserver();
      // executed when a tsk is about to be executed
      void WillProcessTask(base::TimeTicks time_posted) OVERRIDE;
      // called when a task is done
      void DidProcessTask(base::TimeTicks time_posted) OVERRIDE;
      // called before posting a task to thread
      void TaskPosted();
      // returns pending tasks on this thread
      int PendingTasksNum() const;

    private:
      int num_pending_tasks_;
      DISALLOW_COPY_AND_ASSIGN(WorkerThreadTaskObserver);
  };

  // worker thread to process FastCGI request
  class WorkerThread : public base::Thread {
    public:
      // returns number of pending tasks on the thread
      unsigned int GetPendingTasksNum() const;
      // posts a task to the thread
      void PostTask(const tracked_objects::Location& from_here, const base::Closure& task, int64 delay_ms = 0) const;
      // posts task of assigning a Task oberver to thread
      void PostAddTaskObserverTask();
      // assigns a task observer to thread
      void AddTaskObserver();
      // constructor
      explicit WorkerThread(const std::string& name);
      // destructor
      ~WorkerThread();

    private:
      WorkerThreadTaskObserver *task_observer_;
  };
  // constructor
  explicit WorkerPool(const std::string& name);
  // destructor
  ~WorkerPool();
  // Creates and starts given number of worker threads
  bool Init(int workers_num = kDefaultWorkersNum);
  // Stops all worker threads in the pool
  void Stop();
  // Posts a task to worker pool
  // if a thead ID of workers is provide as argument for parameter 'thread_id' then
  // posts the task to that thread else posts task to least loaded worker thread
  bool PostTask(const tracked_objects::Location& from_here, const base::Closure& task, base::PlatformThreadId thread_id = kInvalidWorkerThreadID, int64 delay_ms = 0);
  // Returns thread ID of the least loaded worker thread in the pool
  base::PlatformThreadId GetLeastLoadedThread();

 private:
  // Map of WorkerThreads against its id
  // Thread pointer is maaped against it's thread ID
  std::map<base::PlatformThreadId, WorkerThread*> workers_;
  // name of the worker pool
  std::string name_;
  // posts task to least loaded thread in the pool
  void PostTaskToLeastLoadedThread(const tracked_objects::Location& from_here, const base::Closure& task, int64 delay_ms = 0);
};
}  // namespace slep_util
#endif  // BASE_SLEP_UTIL_WORKER_POOL_H_
