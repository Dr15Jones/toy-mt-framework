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
#include "GroupHolder.h"
#include "Queues.h"
#include "Event.h"

using namespace demo;

static const std::string kPrefix("gov.fnal.prefetch.");
static const std::string kRunPrefix("gov.fnal.run.");

static unsigned long nextID() {
  static unsigned long s_id =0;
  ++s_id;
  return s_id;
}

static std::string unique_name(const std::string& iString) {
  std::ostringstream os;
  os<<iString<<nextID();
  return os.str();
}


ModuleWrapper::ModuleWrapper(Module* iModule, Event* iEvent):
m_module(iModule),
m_event(iEvent),
m_prefetchGroup(dispatch_group_create()),
m_prefetchQueue(dispatch_queue_create(unique_name(kPrefix+iModule->label()).c_str(), NULL)),
m_runQueue(dispatch_queue_create(unique_name(kRunPrefix+iModule->label()).c_str(), NULL)),
m_requestedPrefetch(false)
{
}

ModuleWrapper::ModuleWrapper(const ModuleWrapper& iOther,
                             Event* iEvent):
m_module(iOther.m_module),
m_event(iEvent),
m_prefetchGroup(dispatch_group_create()),
m_prefetchQueue(dispatch_queue_create(unique_name(kPrefix+iOther.m_module->label()).c_str(), NULL)),
m_requestedPrefetch(false)
{
  if(m_module->threadType() == kThreadSafeBetweenInstances) {
    //the same instance can be called reentrantly so each Schedule can have
    // its own queue for each instance rather than having to share one queue
    m_runQueue = dispatch_queue_create(unique_name(kRunPrefix+m_module->label()).c_str(), NULL);
  } else {
    m_runQueue=iOther.m_runQueue;
    dispatch_retain(m_runQueue);
  }
}

ModuleWrapper::ModuleWrapper(const ModuleWrapper& iOther):
m_module(iOther.m_module),
m_event(iOther.m_event),
m_prefetchGroup(iOther.m_prefetchGroup),
m_prefetchQueue(iOther.m_prefetchQueue),
m_runQueue(iOther.m_runQueue),
m_requestedPrefetch(iOther.m_requestedPrefetch)
{
  dispatch_retain(m_prefetchQueue);
  dispatch_retain(m_runQueue);
}

ModuleWrapper&
ModuleWrapper::operator=(const ModuleWrapper& iOther)
{
  if(&iOther!=this) {
    m_module =iOther.m_module;
    m_event = iOther.m_event;
    dispatch_release(m_prefetchQueue);
    m_prefetchGroup = iOther.m_prefetchGroup;
    dispatch_retain(m_prefetchQueue);
    dispatch_release(m_runQueue);
    m_runQueue=iOther.m_runQueue;
    dispatch_retain(m_runQueue);
    m_requestedPrefetch = iOther.m_requestedPrefetch;  
  }
  return *this;
}

ModuleWrapper::~ModuleWrapper()
{
  dispatch_release(m_prefetchQueue);
  dispatch_release(m_runQueue);
}

void 
ModuleWrapper::do_prefetch_task(void* iContext)
{
  ModuleWrapper* wrapper = reinterpret_cast<ModuleWrapper*>(iContext);
  wrapper->doPrefetch();
}

void
ModuleWrapper::doPrefetch()
{
  //This routine is only called from within a queue so its
  // operations are atomic
  if (not m_requestedPrefetch) {
    m_requestedPrefetch = true;
    __sync_synchronize(); //tell other threads we are prefetching
    module()->prefetchAsync(*m_event, m_prefetchGroup); 
  }  
}

void 
ModuleWrapper::prefetchAsync()
{
  //NOTE: we must guarantee that we've entered the group before leaving
  // this routine OR that the group has already been entered by another thread
  // which we can tell because m_requestedPrefetch can only be set to true
  // in the atomic operation which would have been done by the dispatch_group_async_f.
  // Having multiple threads call ModuleWrapper::doPrefetch is OK since only the
  // first one will actually do the prefetching. However it is not desirable since
  // we still have to wait until all the outstanding tasks created by this dispatch_group_async_f
  // have been run before the next step can be taken.
  
  //NOTE: If m_requestedPrefetch were atomic, we could do
  // dispatch_group_enter(m_refetchGroup.get());
  // if(module()->hasPrefetchItems() and ( not atomic_get_previous_and_set(m_requestedPrefetch,true)) {
  //    module()->prefetchAsync(*m_event,m_prefetchGroup);
  //  }
  //  dispatch_group_leave(m_prefetchGroup.get());
  // This would work because even if atomic_get_previous_and_set(m_requestedPrefetch,true) returned true
  // we'd know that another thread must either still have an outstanding dispatch_group_enter and is
  // running module()->prefetchAsync(...) or that a thread has already finished module()->prefetchAsync
  // and all prefetchAsync has already acquired the dispatch_group_enter or all prefetches have finished
  
  __sync_synchronize(); //make sure we have the most up to date value of m_requestedPrefetch
  if(module()->hasPrefetchItems() and (not m_requestedPrefetch)) {
    dispatch_group_async_f(m_prefetchGroup.get(),m_prefetchQueue,
                           static_cast<void*>(this),
                           ModuleWrapper::do_prefetch_task);
  }
}
