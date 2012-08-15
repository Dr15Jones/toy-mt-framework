/*
 *  WaitableTask.h
 *  TBBProcessingDemo
 *
 *  Created by Chris Jones on 9/18/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */
#ifndef TBBProcessingDemo_WaitableTas_h
#define TBBProcessingDemo_WaitableTask_h

#include <atomic>

#include "tbb/task.h"

namespace demo {
   class WaitableTask : public tbb::task {
   public:
      WaitableTask():
      m_next{this} {}

      inline WaitableTask* nextTask() const {
        return m_next;
      }

      inline void setNextTask(WaitableTask* iNext) {
        m_next = iNext;
      }
   private:
      std::atomic<WaitableTask*> m_next;      
   };
   
   class EmptyWaitableTask : public WaitableTask {
   public:
      tbb::task* execute() {}      
   };
}

#endif