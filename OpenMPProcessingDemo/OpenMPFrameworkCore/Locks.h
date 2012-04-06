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
#include "OMPLock.h"

namespace demo {
  extern boost::shared_ptr<OMPLock> s_thread_unsafe_lock;
}

#endif
