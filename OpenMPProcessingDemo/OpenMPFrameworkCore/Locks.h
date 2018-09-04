//
//  Locks.h
//  OpenMPProcessingDemo
//
//  Global locks
//
//  Created by Chris Jones on 4/5/12.
//  Copyright 2012 FNAL. All rights reserved.
//

#ifndef OpenMPProcessingDemo_Locks_h
#define OpenMPProcessingDemo_Locks_h
#include <memory>
#if defined(PARALLEL_MODULES)
#include "OMPLock.h"
#else
#include "OMPLock.h"
#endif

namespace demo {
#if defined(PARALLEL_MODULES)
  extern std::shared_ptr<OMPLock> s_thread_unsafe_lock;
#else
  extern std::shared_ptr<OMPLock> s_thread_unsafe_lock;
#endif

}

#endif
