//
//  OMPLock.h
//  OpenMPProcessingDemo
//
//  Nicer interface to an OpenMP Lock
//
//  Created by Chris Jones on 4/5/12.
//  Copyright 2012 FNAL. All rights reserved.
//

#ifndef OpenMPProcessingDemo_OMPLock_h
#define OpenMPProcessingDemo_OMPLock_h
#include "omp.h"

namespace demo {
  class OMPLock {
  public:
    OMPLock() : m_lock{}
      { omp_init_lock(&m_lock); }
    OMPLock( OMPLock const& ) = delete;
    OMPLock& operator=(OMPLock const& ) = delete;

    ~OMPLock()
      { omp_destroy_lock(&m_lock);}

    void set() {omp_set_lock(&m_lock);}
    void unset() {omp_unset_lock(&m_lock);}
  private:
    omp_lock_t m_lock;
  };

  class OMPLockSentry {
  public:
    OMPLockSentry( OMPLock* iLock):
      m_lock(iLock)
      { m_lock->set();}
    OMPLockSentry( OMPLockSentry const&) = delete;
    OMPLockSentry& operator=( OMPLockSentry const&) = delete;
    ~OMPLockSentry() 
      { m_lock->unset();}
  private:
    OMPLock* m_lock;
  };
}

#endif
