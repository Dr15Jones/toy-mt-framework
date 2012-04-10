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
#include <boost/shared_ptr.hpp>
#if defined(PARALLEL_MODULES)
#include "TaskYieldLock.h"
#else
#include "OMPLock.h"
#endif

namespace demo {
#if defined(PARALLEL_MODULES)
  extern boost::shared_ptr<TaskYieldLock> s_thread_unsafe_lock;
#else
  extern boost::shared_ptr<OMPLock> s_thread_unsafe_lock;
#endif

}

#endif
