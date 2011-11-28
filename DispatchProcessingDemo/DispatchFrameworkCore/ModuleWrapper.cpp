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
}

ModuleWrapper::ModuleWrapper(const ModuleWrapper& iOther):
m_module(iOther.m_module),
m_event(iOther.m_event),
m_prefetchGroup(iOther.m_prefetchGroup),
m_prefetchQueue(iOther.m_prefetchQueue),
m_requestedPrefetch(iOther.m_requestedPrefetch)
{
  dispatch_retain(m_prefetchQueue);
}

ModuleWrapper::~ModuleWrapper()
{
  dispatch_release(m_prefetchQueue);
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
    m_requestedPrefetch=true;
    module()->prefetchAsync(*m_event, m_prefetchGroup); 
  }  
}

typedef void(^WorkBlock_t)();
void 
ModuleWrapper::doPrefetchAndWork(dispatch_queue_t iQueue, WorkBlock_t iWork)
{
  if(module()->hasPrefetchItems()) {
    if(not m_requestedPrefetch) {
      /*dispatch_group_async(m_prefetchGroup.get(),m_prefetchQueue, 
                           ^{
                             if (not m_requestedPrefetch) {
                               m_requestedPrefetch=true;
                               module()->prefetchAsync(*m_event, m_prefetchGroup); 
                             }
                           }
                           );*/
      dispatch_group_async_f(m_prefetchGroup.get(),m_prefetchQueue,
                             static_cast<void*>(this),
                             ModuleWrapper::do_prefetch_task);
    }
    {
      //NOTE: since iWork could be resident on the stack in the calling program we need
      // to make a copy of it on the heap so that it can be called at a later time in the notify
      WorkBlock_t workCopy= Block_copy(iWork);
      
      //when everything has been gotten, do our work
      if(module()->threadType()!=kThreadUnsafe) {
        dispatch_group_notify(m_prefetchGroup.get(),
                              iQueue,
                              workCopy);
        Block_release(workCopy);
      } else {
        //Must first acquire the 'run' lock (i.e. be the running block
        // in the 'run' queue and only after that acquire the 
        // non-thread-safe lock since we must avoid having the same
        // instance of this module be added to the non-thread-safe queue
        // in the case we unblock that queue when we do a getByLabel
        
        //NOTE: need to copy this block so that it is no longer on the
        // stack
        WorkBlock_t temp = ^{
          workCopy();
          Block_release(workCopy);
          dispatch_resume(s_non_thread_safe_queue);
          dispatch_resume(iQueue);
        };
        WorkBlock_t tempCopy = Block_copy(temp);
        
        WorkBlock_t nonThreadSafeBlock = ^{
          dispatch_suspend(s_non_thread_safe_queue);
          dispatch_async(s_thread_safe_queue, tempCopy);
          //NOTE: the dispatch_async has already done a 'retain'
          Block_release(tempCopy);
          
        };
        WorkBlock_t nonThreadSafeBlockCopy = Block_copy(nonThreadSafeBlock);
        
        dispatch_group_notify(m_prefetchGroup.get(),
                              iQueue,
                              ^{ dispatch_suspend(iQueue);
                                dispatch_async(s_non_thread_safe_queue, 
                                               nonThreadSafeBlockCopy);
                                //NOTE: the dispatch_async has already done a 'retain'
                                Block_release(nonThreadSafeBlockCopy);
                              });
        
      }
    }
  } else {
    //just do the work
    //NOTE: since iWork could be resident on the stack in the calling program we need
    // to make a copy of it on the heap so that it can be called at a later time in the notify
    WorkBlock_t workCopy= Block_copy(iWork);
    if(module()->threadType()!=kThreadUnsafe) {
      //Q: Could we just call 'iWork' right here? Are we always running in the thread safe queue already?
      dispatch_async(iQueue,
                     workCopy);
      Block_release(workCopy);
    } else {
      WorkBlock_t temp = ^{
        workCopy();
        Block_release(workCopy);
        dispatch_resume(s_non_thread_safe_queue);
        dispatch_resume(iQueue);
      };
      WorkBlock_t tempCopy = Block_copy(temp);
      
      WorkBlock_t nonThreadSafeBlock = ^{
        dispatch_suspend(s_non_thread_safe_queue);
        dispatch_async(s_thread_safe_queue, tempCopy);
        //NOTE: the dispatch_async has already done a 'retain'
        Block_release(tempCopy);
        
      };
      WorkBlock_t nonThreadSafeBlockCopy = Block_copy(nonThreadSafeBlock);
      
      //std::cout <<"not thread safe"<<std::endl;
      dispatch_async(iQueue,
                     ^{ dispatch_suspend(iQueue);
                       dispatch_async(s_non_thread_safe_queue, 
                                      nonThreadSafeBlockCopy);
                       //NOTE: the dispatch_async has already done a 'retain'
                       Block_release(nonThreadSafeBlockCopy);
                     });
    }
  }
}
