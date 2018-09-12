#ifndef OpenMPProcessingDemo_WaitingTaskHolder_h
#define OpenMPProcessingDemo_WaitingTaskHolder_h
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

// user include files
#include "WaitingTask.h"

// forward declarations

namespace demo {
  class WaitingTaskHolder
  {
    
  public:
    WaitingTaskHolder() = default;

    template <typename T>
      explicit WaitingTaskHolder(std::unique_ptr<T> iTask):
    m_task(wrapTask(iTask.release()) ) {}

    explicit WaitingTaskHolder(std::shared_ptr<WaitingTask> iTask):
    m_task(std::move(iTask) ) {}

    ~WaitingTaskHolder() = default;

    WaitingTaskHolder(const WaitingTaskHolder& iHolder) = default;
    WaitingTaskHolder(WaitingTaskHolder&& iOther) = default;
    
    WaitingTaskHolder& operator=(const WaitingTaskHolder& iRHS) = default;
    WaitingTaskHolder& operator=(WaitingTaskHolder&& iRHS) = default;
    
    // ---------- const member functions ---------------------
    bool taskHasFailed() const { return m_task->exceptionPtr() != nullptr; }
    
    // ---------- static member functions --------------------
    
    // ---------- member functions ---------------------------
    std::shared_ptr<WaitingTask> releaseTask() { 
      auto ret = std::move(m_task);
      return ret;
    }
    
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
      m_task.reset();
    }
    
  private:
    std::shared_ptr<WaitingTask> wrapTask( WaitingTask* iTask) {
      return std::shared_ptr<WaitingTask>(iTask, [](WaitingTask* iTask){ if(iTask) { demo::TaskBase::spawn(iTask); } } );
    }
    
    // ---------- member data --------------------------------
    std::shared_ptr<WaitingTask> m_task;
  };
}

#endif
