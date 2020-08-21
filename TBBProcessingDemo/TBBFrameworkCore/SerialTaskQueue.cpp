// -*- C++ -*-
//
// Package:     Concurrency
// Class  :     SerialTaskQueue
// 
// Implementation:
//     [Notes on implementation]
//
// Original Author:  Chris Jones
//         Created:  Thu Feb 21 11:31:52 CST 2013
// $Id$
//

// system include files

// user include files
#include "SerialTaskQueue.h"

#include "Likely.h"

using namespace demo;

//
// member functions
//
SerialTaskQueue::~SerialTaskQueue() {
  //be certain all tasks have completed
  bool isEmpty = m_tasks.empty();
  bool isTaskChosen = m_taskChosen;
  if ((not isEmpty and not isPaused()) or isTaskChosen) {
    tbb::task_group g;
    std::atomic<bool> done = false;
    push(g, [&done]() { 
	done = true;
	return; });
    do {
      g.wait();
    } while(not done.load());
  }
}

void SerialTaskQueue::spawn(TaskBase& iTask) {
  auto pTask = &iTask;
  iTask.group()->run([pTask, this]() {
      TaskBase* t = pTask;
      auto g = pTask->group();
      do {
	t->execute();
	delete t;
	t = finishedTask();
	if(t and t->group() != g) {
	  spawn(*t);
	  t=nullptr;
	}
      } while(t!=nullptr);
    });
}

bool
SerialTaskQueue::resume() {
  if(0==--m_pauseCount) {
    auto* t = pickNextTask();
    if(0 != t) {
      spawn(*t);
    }
    return true;
  }
  return false;
}

void
SerialTaskQueue::pushTask(TaskBase* iTask) {
  auto* t = pushAndGetNextTask(iTask);
  if(0!=t) {
    spawn(*t);      
  }
}

SerialTaskQueue::TaskBase* 
SerialTaskQueue::pushAndGetNextTask(TaskBase* iTask) {
  TaskBase* returnValue{nullptr};
  if likely(nullptr!=iTask) {
    m_tasks.push(iTask);
    returnValue = pickNextTask();
  }
  return returnValue;
}

SerialTaskQueue::TaskBase*
SerialTaskQueue::finishedTask() {
  m_taskChosen.store(false);
  return pickNextTask();
}

SerialTaskQueue::TaskBase*
SerialTaskQueue::pickNextTask() {
  bool expected = false;
  if likely(0 == m_pauseCount and not m_taskChosen.compare_exchange_strong(expected,true)) {
    TaskBase* t=0;
    if likely(m_tasks.try_pop(t)) {
      return t;
    }
    //no task was actually pulled
    m_taskChosen.store(false);
    
    //was a new entry added after we called 'try_pop' but before we did the clear?
    expected = false;
    if(not m_tasks.empty() and not m_taskChosen.compare_exchange_strong(expected,true)) {
      TaskBase* t=0;
      if(m_tasks.try_pop(t)) {
        return t;
      }
      //no task was still pulled since a different thread beat us to it
      m_taskChosen.store(false);
      
    }
  }
  return 0;
}


//
// const member functions
//

//
// static member functions
//
