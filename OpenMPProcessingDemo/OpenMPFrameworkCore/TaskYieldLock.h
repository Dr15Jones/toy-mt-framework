//
//  TaskYieldLock.h
//  OpenMPProcessingDemo
//
//  A Lock that will do an OpenMP taskyield between checks that the resource is available
//
//  Created by Chris Jones on 4/5/12.
//  Copyright 2012 FNAL. All rights reserved.
//

#ifndef OpenMPProcessingDemo_TaskYieldLock_h
#define OpenMPProcessingDemo_TaskYieldLock_h
#include <atomic>

namespace demo {
  class TaskYieldLock {
  public:
    TaskYieldLock() : m_lock{ATOMIC_FLAG_INIT}
      {}
    TaskYieldLock( TaskYieldLock const& ) = delete;
    TaskYieldLock& operator=(TaskYieldLock const& ) = delete;

    ~TaskYieldLock()
      {}

    void set() {
      while(m_lock.test_and_set(std::memory_order_acquire)) {
#pragma omp taskyield
      }
    }
    void unset() {m_lock.clear(std::memory_order_release);}
  private:
    std::atomic_flag m_lock;
  };

  class TaskYieldLockSentry {
  public:
    TaskYieldLockSentry( TaskYieldLock* iLock):
      m_lock(iLock)
      {
#if !defined(PARALLEL_MODULES)  
        if(m_lock) {
          m_lock->set();
        }
#else
        m_lock->set();
#endif
      }
    TaskYieldLockSentry( TaskYieldLockSentry const&) = delete;
    TaskYieldLockSentry& operator=( TaskYieldLockSentry const&) = delete;
    ~TaskYieldLockSentry() 
      { 
#if !defined(PARALLEL_MODULES)  
        if(m_lock) {
          m_lock->unset();
        }
#else
        m_lock->unset();
#endif
      }
  private:
    TaskYieldLock* m_lock;
  };
}

#endif
