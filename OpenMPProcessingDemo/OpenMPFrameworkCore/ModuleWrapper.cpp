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

ModuleWrapper::ModuleWrapper(Module* iModule):
  m_module(iModule),
  m_prefetchLock{},
  m_runLock(m_module->threadType() != kThreadUnsafe? boost::shared_ptr<OMPLock>(new OMPLock{}) : s_thread_unsafe_lock),
  m_donePrefetch(false)
{
}

ModuleWrapper::ModuleWrapper(const ModuleWrapper* iOther):
  m_module(iOther->m_module),
  m_prefetchLock{},
  m_donePrefetch(false)
{
  if(m_module->threadType() == kThreadSafeBetweenInstances) {
    //the same instance can be called reentrantly so each Schedule can have
    // its own lock for each instance rather than having to share one lock
    m_runLock = boost::shared_ptr<OMPLock>(new OMPLock{});
  } else {
    m_runLock = iOther->m_runLock;
  }
}


ModuleWrapper::ModuleWrapper(const ModuleWrapper& iOther):
  m_module(iOther.m_module),
  m_prefetchLock{},
  m_runLock{iOther.m_runLock},
  m_donePrefetch(static_cast<bool>(iOther.m_donePrefetch))
{
}

ModuleWrapper::~ModuleWrapper()
{
}

void
ModuleWrapper::prefetch(Event& iEvent)
{
  if(!m_donePrefetch) {
    OMPLockSentry sentry(&m_prefetchLock);
    m_module->prefetch(iEvent);
    __sync_synchronize();
    m_donePrefetch=true;
  }
}


