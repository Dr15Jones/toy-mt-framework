/*
 *  WaitingTaskList.h
 *  TBBProcessingDemo
 *
 *  Created by Chris Jones on 9/18/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */
#ifndef TBBProcessingDemo_WaitingTaskList_h
#define TBBProcessingDemo_WaitingTaskList_h

#include <atomic>

namespace demo {
   class WaitableTask;
   class WaitingTaskList {
   public:
      WaitingTaskList():
      m_waiting{true},
      m_head{0}
      {  }

      void add(WaitableTask*);

      void doneWaiting();
      void reset() { m_waiting = true;}
  
   private:
      //safe to call from multiple threads
      void announce();

      std::atomic<WaitableTask*> m_head;
      std::atomic<bool> m_waiting;
   };
}

#endif