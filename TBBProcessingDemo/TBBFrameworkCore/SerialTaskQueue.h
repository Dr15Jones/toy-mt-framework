/*
 *  SerialTaskQueue.h
 *  TBBProcessingDemo
 *
 *  Created by Chris Jones on 9/18/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */
#ifndef TBBProcessingDemo_SerialTaskQueue_h
#define TBBProcessingDemo_SerialTaskQueue_h

#include <atomic>

#include "tbb/task.h"

#include "TaskQueueBase.h"
namespace demo {
   class SerialTaskQueue : public TaskQueueBase {
   public:
        SerialTaskQueue():
        m_head{0},
        m_tail{0},
        m_taskChosen{ATOMIC_FLAG_INIT},
        m_pauseCount{0}
        {  }

        //returns true if not already paused
        bool pause() {
           return 1 == ++m_pauseCount;
        }

        //returns true if this really restarts the queue
        bool resume();

      private:
        virtual void pushTask(TaskBase*) /*final*/;
        virtual tbb::task* pushAndGetNextTask(TaskBase*) /*final*/;
        virtual tbb::task* finishedTask() /*final*/;
        //returns nullptr if a task is already being processed
        TaskBase* pickNextTask();

        std::atomic<TaskBase*> m_head;
        std::atomic<TaskBase*> m_tail;
        std::atomic_flag m_taskChosen;
        std::atomic<unsigned long> m_pauseCount;
   };
}

#endif