/*
 *  WaitingList.h
 *  TBBProcessingDemo
 *
 *  Created by Chris Jones on 9/18/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */
#ifndef TBBProcessingDemo_WaitingList_h
#define TBBProcessingDemo_WaitingList_h

#include <atomic>

#include "tbb/task.h"
#include "tbb/concurrent_queue.h"

namespace demo {
   class WaitingList {
   public:
        WaitingList():
        m_waiting{true}
        {  }

        void add(tbb::task*);

        void doneWaiting();
        void reset() { m_waiting = true;}
        
      private:
        //safe to call from multiple threads
        void announce();

        tbb::concurrent_queue<tbb::task*> m_tasks;
        std::atomic<bool> m_waiting;
   };
}

#endif