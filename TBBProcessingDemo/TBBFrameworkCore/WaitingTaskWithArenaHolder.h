#ifndef TBBProcessingDemo_WaitingTaskWithArenaHolder_h
#define TBBProcessingDemo_WaitingTaskWithArenaHolder_h
// -*- C++ -*-
//
// 
/**

 Description: [one line class summary]

 Usage:
    <usage>

*/
//
// Original Author:  FWCore
//         Created:  Fri, 18 Nov 2016 20:30:42 GMT
//

// system include files
#include <cassert>
#include <memory>
#include "tbb/task_group.h"
#include "tbb/task_arena.h"

// user include files
#include "WaitingTask.h"

// forward declarations

namespace demo {
  class WaitingTaskWithArenaHolder
  {
    
  public:
    WaitingTaskWithArenaHolder():
    m_group(nullptr),
    m_task(nullptr) {}
    
    explicit WaitingTaskWithArenaHolder(tbb::task_group& iGroup, WaitingTask* iTask):
    m_group(&iGroup),
    m_task(iTask),
    m_arena(std::make_shared<tbb::task_arena>(tbb::task_arena::attach()))
    { if(m_task) { m_task->increment_ref_count(); } }
    ~WaitingTaskWithArenaHolder() {
      if(m_task) {
        doneWaiting(std::exception_ptr{});
      }
    }

    WaitingTaskWithArenaHolder(const WaitingTaskWithArenaHolder& iHolder) :
    m_group(iHolder.m_group),
    m_task(iHolder.m_task), m_arena(iHolder.m_arena) {
      m_task->increment_ref_count();
    }

    WaitingTaskWithArenaHolder(WaitingTaskWithArenaHolder&& iOther) :
    m_group(iOther.m_group),
    m_task(iOther.m_task), m_arena(std::move(iOther.m_arena)) {
      iOther.m_task = nullptr;
    }
    
    WaitingTaskWithArenaHolder& operator=(const WaitingTaskWithArenaHolder& iRHS) {
      WaitingTaskWithArenaHolder tmp(iRHS);
      std::swap(m_group, tmp.m_group);
      std::swap(m_task, tmp.m_task);
      std::swap(m_arena, tmp.m_arena);
      return *this;
    }

    WaitingTaskWithArenaHolder& operator=(WaitingTaskWithArenaHolder&& iRHS) {
      WaitingTaskWithArenaHolder tmp(std::move(iRHS));
      std::swap(m_group, tmp.m_group);
      std::swap(m_task, tmp.m_task);
      std::swap(m_arena, tmp.m_arena);
      return *this;
    }
    
    // ---------- const member functions ---------------------
    bool taskHasFailed() const { return m_task->exceptionPtr() != nullptr; }
    
    // ---------- static member functions --------------------
    
    // ---------- member functions ---------------------------
    
    /** Use in the case where you need to inform the parent task of a
     failure before some other child task which may be run later reports
     a different, but related failure. You must later call doneWaiting
     in the same thread passing the same exceptoin.
     */
    void presetTaskAsFailed(std::exception_ptr iExcept) {
      if(iExcept) {
        m_task->dependentTaskFailed(iExcept);
      }
    }
    
    void doneWaiting(std::exception_ptr iExcept) {
      if(iExcept) {
        m_task->dependentTaskFailed(iExcept);
      }
      //enqueue can run the task before we finish
      // doneWaiting and some other thread might
      // try to reuse this object. Resetting
      // before enqueue avoids problems
      auto task = m_task;
      m_task = nullptr;
      if(0==task->decrement_ref_count()){
	// The enqueue call will cause a worker thread to be created in
	// the arena if there is not one already.
	m_arena->enqueue([task = task, group=m_group]() { 
	    group->run([task]() {
		TaskSentry s{task};
		task->execute();
	      });
	  });
      }
    }
    
  private:
    
    // ---------- member data --------------------------------
    tbb::task_group* m_group;
    WaitingTask* m_task;
    std::shared_ptr<tbb::task_arena> m_arena;
  };
}

#endif
