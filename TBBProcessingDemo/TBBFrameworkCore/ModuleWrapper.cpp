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
#include "SerialTaskQueue.h"

using namespace demo;

ModuleWrapper::ModuleWrapper(Module* iModule, Event* iEvent):
m_module(iModule),
m_event(iEvent),
m_runQueue( new SerialTaskQueue{}),
m_requestedPrefetch{ATOMIC_FLAG_INIT}
{
}

ModuleWrapper::ModuleWrapper(const ModuleWrapper& iOther,
                             Event* iEvent):
m_module(iOther.m_module),
m_event(iEvent),
m_requestedPrefetch{ATOMIC_FLAG_INIT}
{
  if(m_module->threadType() == kThreadSafeBetweenInstances) {
    //the same instance can be called reentrantly so each Schedule can have
    // its own queue for each instance rather than having to share one queue
    m_runQueue = boost::shared_ptr<SerialTaskQueue>(new SerialTaskQueue{});
  } else {
    m_runQueue=iOther.m_runQueue;
  }
}

ModuleWrapper::ModuleWrapper(const ModuleWrapper& iOther):
m_module(iOther.m_module),
m_event(iOther.m_event),
m_runQueue(iOther.m_runQueue),
m_requestedPrefetch{ATOMIC_FLAG_INIT}
{
}

ModuleWrapper&
ModuleWrapper::operator=(const ModuleWrapper& iOther)
{
  if(&iOther!=this) {
    m_module =iOther.m_module;
    m_event = iOther.m_event;
    m_runQueue=iOther.m_runQueue;
    //leave it the way it was
    //m_requestedPrefetch = iOther.m_requestedPrefetch;  
  }
  return *this;
}

ModuleWrapper::~ModuleWrapper()
{
}

void 
ModuleWrapper::prefetchAsync(tbb::task* iPrefetchDoneTask)
{
  //NOTE: we must guarantee that we've entered the group before leaving
  // this routine OR that the group has already been entered by another thread
  // which we can tell because m_requestedPrefetch can only be set to true
  // in the atomic operation which would have been done by the dispatch_group_async_f.
  // Having multiple threads call ModuleWrapper::doPrefetch is OK since only the
  // first one will actually do the prefetching. However it is not desirable since
  // we still have to wait until all the outstanding tasks created by this dispatch_group_async_f
  // have been run before the next step can be taken.
  assert(iPrefetchDoneTask->ref_count()==0);
  
  if(module()->hasPrefetchItems()) {
    
    if (not m_requestedPrefetch.test_and_set()) {
      module()->prefetchAsync(*m_event, iPrefetchDoneTask); 
    } else {
      //we already have one prefetch task running and the done task is identical 
      // so it can be gotten rid of
      tbb::task::destroy(*iPrefetchDoneTask);
    }
  } else {
    //if everything finishes before we leave this routine, we need to launch the task
    tbb::task::spawn(*iPrefetchDoneTask);
  }
}
