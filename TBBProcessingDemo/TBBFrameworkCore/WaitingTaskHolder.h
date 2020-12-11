#ifndef TBBProcessingDemo_WaitingTaskHolder_h
#define TBBProcessingDemo_WaitingTaskHolder_h
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
#include "tbb/task_group.h"

// user include files
#include "WaitingTask.h"

// forward declarations

namespace demo {
  class WaitingTaskList;

  class WaitingTaskHolder
  {
    
  public:
    friend class WaitingTaskList;
    WaitingTaskHolder():
    m_group{nullptr},
    m_task(nullptr) {}
    
    explicit WaitingTaskHolder(tbb::task_group& iGroup, WaitingTask* iTask):
    m_group(&iGroup),
    m_task(iTask)
    { if(m_task) { m_task->increment_ref_count(); } }
    ~WaitingTaskHolder() {
      if(m_task) {
        doneWaiting(std::exception_ptr{});
      }
    }

    WaitingTaskHolder(const WaitingTaskHolder& iHolder) :
    m_group(iHolder.m_group),
    m_task(iHolder.m_task) {
      m_task->increment_ref_count();
    }

    WaitingTaskHolder(WaitingTaskHolder&& iOther) :
    m_group(iOther.m_group),
    m_task(iOther.m_task) {
      iOther.m_task = nullptr;
    }
    
    WaitingTaskHolder& operator=(const WaitingTaskHolder& iRHS) {
      WaitingTaskHolder tmp(iRHS);
      std::swap(m_group, tmp.m_group);
      std::swap(m_task, tmp.m_task);
      return *this;
    }

    WaitingTaskHolder& operator=(WaitingTaskHolder&& iRHS) {
      WaitingTaskHolder tmp(std::move(iRHS));
      std::swap(m_group, tmp.m_group);
      std::swap(m_task, tmp.m_task);
      return *this;
    }
    
    // ---------- const member functions ---------------------
    bool taskHasFailed() const { return m_task->exceptionPtr() != nullptr; }
    
    // ---------- static member functions --------------------
    
    // ---------- member functions ---------------------------
    tbb::task_group& group() { return *m_group;}
    
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
      auto t = m_task;
      m_task = nullptr;
      if(0 == t->decrement_ref_count()){
	m_group->run([t]() {
	    TaskSentry s{t};
	    t->execute();
	  });
      }
    }
    
  private:
    WaitingTask* release_no_decrement() {
      auto t = m_task;
      m_task = nullptr;
      return t;
    }
    
    // ---------- member data --------------------------------
    tbb::task_group* m_group;
    WaitingTask* m_task;
  };
}

#endif
