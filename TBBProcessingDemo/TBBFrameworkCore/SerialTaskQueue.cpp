/*
 *  SerialTaskQueue.cpp
 *  TBBProcessingDemo
 *
 *  Created by Chris Jones on 9/17/09.
 *  Copyright 2009 FNAL. All rights reserved.
 *
 */

#include "SerialTaskQueue.h"
#include "ThreadingHelpers.h"

using namespace demo;

bool
SerialTaskQueue::resume() {
  if(0==--m_pauseCount) {
    tbb::task* t = pickNextTask();
    if(0 != t) {
      tbb::task::spawn(*t);
    }
    return true;
  }
  return false;
}

void
SerialTaskQueue::pushTask(SerialTaskQueue::TaskBase* iTask) {
  tbb::task* t = pushAndGetNextTask(iTask);
  if(0!=t) {
    tbb::task::spawn(*t);      
  }
}

tbb::task* 
SerialTaskQueue::pushAndGetNextTask(TaskBase* iTask) {
  tbb::task* returnValue{0};
  if(0!=iTask) {
     iTask->setNext(0);
     TaskBase* oldTail = m_tail.exchange(iTask);
     __sync_synchronize();
     if(nullptr == oldTail){
        m_head=iTask;
     } else {
        oldTail->setNext(iTask);
     }
     returnValue = pickNextTask();
  }
  return returnValue;
}


tbb::task*
SerialTaskQueue::finishedTask() {
  m_taskChosen.clear();
  return pickNextTask();
}

SerialTaskQueue::TaskBase*
SerialTaskQueue::pickNextTask() {
  
  if(0 == m_pauseCount and not m_taskChosen.test_and_set()) {
    TaskBase* oldHead = m_head.load();
    if(nullptr != oldHead) {       
       if(m_tail.compare_exchange_strong(oldHead,nullptr)) {
          //head was same as tail
         //If a new head value hasn't yet been assigned
          m_head.compare_exchange_strong(oldHead,nullptr);
          return oldHead;
       }
       TaskBase* nextHead;
       while(nullptr == (nextHead = oldHead->next()) ) {
          hardware_pause();
          //NOTE: empirically this is needed, but I haven't yet figured out
          // the case which causes the head to change
          oldHead = m_head.load();
       }
       m_head.exchange(nextHead);
       return oldHead;
    }
    //no task was actually pulled
    m_taskChosen.clear();
    
    //was a new entry added after we called 'm_head.load' but before we did the clear?
    if(nullptr !=m_head.load() and not m_taskChosen.test_and_set()) {
       TaskBase* oldHead = m_head.load();
       if(nullptr != oldHead) {       
          if(m_tail.compare_exchange_strong(oldHead,nullptr)) {
            //If a new head value hasn't yet been assigned
             m_head.compare_exchange_strong(oldHead,nullptr);
             return oldHead;
          }
          TaskBase* nextHead;
          while(nullptr == (nextHead = oldHead->next()) ) {
             hardware_pause();
             oldHead = m_head.load();
          }
          m_head.exchange(nextHead);
          return oldHead;
       }
       //no task was still pulled since a different thread beat us to it
       m_taskChosen.clear();      
    }
  }
  return nullptr;
}
