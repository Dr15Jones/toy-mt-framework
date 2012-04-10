//
//  ModuleWrapper.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/8/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <sstream>


#include "ModuleWrapper.h"
#include "Module.h"
#include "Queues.h"
#include "Event.h"
#include "Locks.h"

using namespace demo;

static
boost::shared_ptr<TaskYieldLock>
chooseLock(unsigned int iType)
{
#if defined(PARALLEL_MODULES)
  return iType != kThreadUnsafe? boost::shared_ptr<TaskYieldLock>(new TaskYieldLock{}) : s_thread_unsafe_lock;
#else
  boost::shared_ptr<OMPLock> returnValue;
  switch(iType) {
    case kThreadUnsafe:
    {
      returnValue = s_thread_unsafe_lock;
      break;
    }
    case kThreadSafeBetweenModules:
    {
      returnValue = boost::shared_ptr<TaskYieldLock>(new TaskYieldLock{});
      break;
    }
    case kThreadSafeBetweenInstances:
    {
      //no need for a lock
      break;
    }
    default:
    assert(false);
  }
  return returnValue;
#endif  
}

ModuleWrapper::ModuleWrapper(Module* iModule):
  m_module(iModule),
#if defined(PARALLEL_MODULES)
  m_prefetchLock{},
  m_runLock(chooseLock(m_module->threadType())),
  m_donePrefetch(false)
#else
  m_runLock(chooseLock(m_module->threadType()))
#endif
{
}

ModuleWrapper::ModuleWrapper(const ModuleWrapper* iOther):
  m_module(iOther->m_module),
#if defined(PARALLEL_MODULES)
  m_prefetchLock{},
  m_donePrefetch(false)
#else
  m_runLock(iOther->m_runLock)
#endif
{
#if defined(PARALLEL_MODULES)
  if(m_module->threadType() == kThreadSafeBetweenInstances) {
    //the same instance can be called reentrantly so each Schedule can have
    // its own lock for each instance rather than having to share one lock
    m_runLock = boost::shared_ptr<TaskYieldLock>(new TaskYieldLock{});
  } else {
    m_runLock = iOther->m_runLock;
  }
#endif
}


ModuleWrapper::ModuleWrapper(const ModuleWrapper& iOther):
  m_module(iOther.m_module),
#if defined(PARALLEL_MODULES)
  m_prefetchLock{},
  m_runLock{iOther.m_runLock},
  m_donePrefetch(static_cast<bool>(iOther.m_donePrefetch))
#else
  m_runLock{iOther.m_runLock}
#endif
{
}

ModuleWrapper::~ModuleWrapper()
{
}

void
ModuleWrapper::prefetch(Event& iEvent)
{
#if defined(PARALLEL_MODULES)
  if(!m_donePrefetch) {
    TaskYieldLockSentry sentry(&m_prefetchLock);
    if(!m_donePrefetch) {
      m_module->prefetch(iEvent);
      __sync_synchronize();
      m_donePrefetch=true;
    }
  }
#else
  m_module->prefetch(iEvent);
#endif
}

