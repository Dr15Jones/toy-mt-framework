/*
 *  ConcurrentTaskQueue.h
 *  TBBProcessingDemo
 *
 *  Created by Chris Jones on 9/18/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */
#ifndef TBBProcessingDemo_ConcurrentTaskQueue_h
#define TBBProcessingDemo_ConcurrentTaskQueue_h

#include <atomic>

#include "tbb/task.h"
#include "tbb/concurrent_queue.h"

#include "TaskQueueBase.h"
namespace demo {
   class ConcurrentTaskQueue : public TaskQueueBase {
   public:
        ConcurrentTaskQueue()
        {  }

      private:
        virtual void pushTask(TaskBase*) /*final*/;
        virtual tbb::task* pushAndGetNextTask(TaskBase*) /*final*/;
        tbb::task* finishedTask() /*final*/;
   };
}

#endif