//
//  PrefetchAndWorkWrapper.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/8/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <sstream>


#include "PrefetchAndWorkWrapper.h"
#include "ModuleWrapper.h"
#include "Module.h"
#include "GroupHolder.h"
#include "Queues.h"
#include "Event.h"

using namespace demo;


PrefetchAndWorkWrapper::PrefetchAndWorkWrapper(ModuleWrapper* iWrapper):
m_wrapper(iWrapper)
{
}

Module*
PrefetchAndWorkWrapper::module_() const {
  return m_wrapper->module();
}

void 
PrefetchAndWorkWrapper::do_work_task(void* iContext)
{
  PrefetchAndWorkWrapper* wrapper = reinterpret_cast<PrefetchAndWorkWrapper*>(iContext);
  wrapper->doWork();
}

void 
PrefetchAndWorkWrapper::do_work_and_resume_queues_task(void* iContext)
{
  PrefetchAndWorkWrapper* wrapper = reinterpret_cast<PrefetchAndWorkWrapper*>(iContext);
  wrapper->doWork();
  //now that our work is done, we allow other instances use the queues
  dispatch_resume(s_non_thread_safe_queue);
  dispatch_resume(wrapper->m_wrapper->runQueue());
}

void 
PrefetchAndWorkWrapper::do_suspend_thread_unsafe_queue_before_work_task(void* iContext)
{
  //This is running in the s_non_thread_safe_queue so we know this
  // instances is the one that has the 'lock'.
  dispatch_suspend(s_non_thread_safe_queue);
  dispatch_async_f(s_thread_safe_queue, iContext,
                   PrefetchAndWorkWrapper::do_work_and_resume_queues_task);
}

void 
PrefetchAndWorkWrapper::do_suspend_run_queue_before_work_task(void* iContext)
{
  //This is running in the m_runQueue so we know this instance is
  // the one that has the 'lock'.
  PrefetchAndWorkWrapper* wrapper = reinterpret_cast<PrefetchAndWorkWrapper*>(iContext);
  dispatch_suspend(wrapper->m_wrapper->runQueue());
  dispatch_async_f(s_non_thread_safe_queue, 
                   iContext,
                   PrefetchAndWorkWrapper::do_suspend_thread_unsafe_queue_before_work_task);

}

void 
PrefetchAndWorkWrapper::doPrefetchAndWork()
{
  m_wrapper->prefetchAsync();
  //when everything has been gotten, do our work
  if(module_()->threadType()!=kThreadUnsafe) {
    dispatch_group_notify_f(m_wrapper->prefetchGroup(),
                            m_wrapper->runQueue(),
                            static_cast<void*>(this),
                            PrefetchAndWorkWrapper::do_work_task);
  } else {
    //Must first acquire the 'run' lock (i.e. be the running block
    // in the 'run' queue and only after that acquire the 
    // non-thread-safe lock since we must avoid having the same
    // instance of this module be added to the non-thread-safe queue
    // in the case we unblock that queue when we do a getByLabel
    dispatch_group_notify_f(m_wrapper->prefetchGroup(),
                            m_wrapper->runQueue(),
                            this,
                            PrefetchAndWorkWrapper::do_suspend_run_queue_before_work_task);
      
  }
}
