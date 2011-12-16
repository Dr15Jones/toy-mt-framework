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
m_runQueue(dispatch_queue_create(unique_name(kPrefix+iModule->label()).c_str(), NULL)),
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
    m_runQueue = dispatch_queue_create(unique_name(kPrefix+m_module->label()).c_str(), NULL);
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
  if (not m_requestedPrefetch) {
    module()->prefetchAsync(*m_event, m_prefetchGroup); 
    m_requestedPrefetch=true;
  }  
}

void 
ModuleWrapper::prefetchAsync()
{
  if(module()->hasPrefetchItems() and (not m_requestedPrefetch)) {
    dispatch_group_async_f(m_prefetchGroup.get(),m_prefetchQueue,
                           static_cast<void*>(this),
                           ModuleWrapper::do_prefetch_task);
  }
}
