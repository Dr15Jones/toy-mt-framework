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
#include <exception>
#include "WaitingTask.h"


namespace demo {
   struct WaitNode {
      WaitingTask* m_task;
      std::atomic<WaitNode*> m_next;
      bool m_fromCache;
      
      void setNextNode(WaitNode* iNext){
         m_next = iNext;
      }
      
      WaitNode* nextNode() const {
         return m_next;
      }
   };
   
   class WaitingTaskList {
   public:
      WaitingTaskList(unsigned int iInitialSize = 2):
      m_head{0},
      m_nodeCache{new WaitNode[iInitialSize]},
      m_nodeCacheSize{iInitialSize},
      m_lastAssignedCacheIndex{0},
      m_waiting{true}
      {
         for(auto it = m_nodeCache, itEnd = m_nodeCache+m_nodeCacheSize; it!=itEnd; ++it) {
            it->m_fromCache=true;
         }
      }

      ~WaitingTaskList() {
         delete [] m_nodeCache;
      }
      void add(WaitingTask*);

      //Not thread safe
      // If the task we were waiting on threw an exception
      // iPtr will not be 'null'
      void doneWaiting(std::exception_ptr iPtr);
      
      ///NOTE: reset is not thread safe
      void reset();
  
   private:
      //safe to call from multiple threads
      void announce();

      WaitNode* createNode(WaitingTask* iTask);
      std::atomic<WaitNode*> m_head;
      WaitNode* m_nodeCache;
      std::exception_ptr m_exceptionPtr; //guarded by m_waiting
      unsigned int m_nodeCacheSize;
      std::atomic<unsigned int> m_lastAssignedCacheIndex;
      std::atomic<bool> m_waiting;
   };
}

#endif