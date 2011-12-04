//
//  ModuleWrapper.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/8/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <Block.h>

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
ModuleWrapper::do_work_task(void* iContext)
{
  ModuleWrapper* wrapper = reinterpret_cast<ModuleWrapper*>(iContext);
  wrapper->doWork();
}

void 
ModuleWrapper::do_work_and_resume_queues_task(void* iContext)
{
  ModuleWrapper* wrapper = reinterpret_cast<ModuleWrapper*>(iContext);
  wrapper->doWork();
  //now that our work is done, we allow other instances use the queues
  dispatch_resume(s_non_thread_safe_queue);
  dispatch_resume(wrapper->m_runQueue);
}

void 
ModuleWrapper::do_suspend_thread_unsafe_queue_before_work_task(void* iContext)
{
  //This is running in the s_non_thread_safe_queue so we know this
  // instances is the one that has the 'lock'.
  dispatch_suspend(s_non_thread_safe_queue);
  dispatch_async_f(s_thread_safe_queue, iContext,
                   ModuleWrapper::do_work_and_resume_queues_task);
}

void 
ModuleWrapper::do_suspend_run_queue_before_work_task(void* iContext)
{
  //This is running in the m_runQueue so we know this instance is
  // the one that has the 'lock'.
  ModuleWrapper* wrapper = reinterpret_cast<ModuleWrapper*>(iContext);
  dispatch_suspend(wrapper->m_runQueue);
  dispatch_async_f(s_non_thread_safe_queue, 
                   iContext,
                   ModuleWrapper::do_suspend_thread_unsafe_queue_before_work_task);

}

void 
ModuleWrapper::doPrefetchAndWork()
{
  if(module()->hasPrefetchItems() and (not m_requestedPrefetch)) {
      dispatch_group_async_f(m_prefetchGroup.get(),m_prefetchQueue,
                             static_cast<void*>(this),
                             ModuleWrapper::do_prefetch_task);
  }
  //when everything has been gotten, do our work
  if(module()->threadType()!=kThreadUnsafe) {
    dispatch_group_notify_f(m_prefetchGroup.get(),
                            m_runQueue,
                            static_cast<void*>(this),
                            ModuleWrapper::do_work_task);
  } else {
    //Must first acquire the 'run' lock (i.e. be the running block
    // in the 'run' queue and only after that acquire the 
    // non-thread-safe lock since we must avoid having the same
    // instance of this module be added to the non-thread-safe queue
    // in the case we unblock that queue when we do a getByLabel
    dispatch_group_notify_f(m_prefetchGroup.get(),
                            m_runQueue,
                            this,
                            ModuleWrapper::do_suspend_run_queue_before_work_task);
      
  }
}
