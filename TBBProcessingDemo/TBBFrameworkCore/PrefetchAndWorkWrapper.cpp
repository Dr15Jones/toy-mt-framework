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

/*
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
  s_non_thread_safe_queue.resume();
  wrapper->m_wrapper->runQueue().resume();
}

void 
PrefetchAndWorkWrapper::do_suspend_thread_unsafe_queue_before_work_task(void* iContext)
{
  //This is running in the s_non_thread_safe_queue so we know this
  // instances is the one that has the 'lock'.
  PrefetchAndWorkWrapper* wrapper = reinterpret_cast<PrefetchAndWorkWrapper*>(iContext);
  s_non_thread_safe_queue.pause();
  s_thread_safe_queue.add([]{
     wrapper->doWork();
     //now that our work is done, we allow other instances use the queues
     s_non_thread_safe_queue.resume();
     wrapper->m_wrapper->runQueue().resume();     
  });
  
  //dispatch_async_f(s_thread_safe_queue, iContext,
  //                 PrefetchAndWorkWrapper::do_work_and_resume_queues_task);
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
*/
namespace demo {
   namespace pnw {
      class DoWorkTask : public tbb::task {
      public:
         DoWorkTask(PrefetchAndWorkWrapper* iWrapper):
         m_wrapper(iWrapper) {}
      
         tbb::task* execute() {
            auto wrapper = this->m_wrapper;
            m_wrapper->m_wrapper->runQueue()->push([wrapper]{
               wrapper->doWork();
               });
            return nullptr;
         }
      private:
         PrefetchAndWorkWrapper* m_wrapper;
      };
   
      class NonThreadSafeDoWorkTask : public tbb::task {
      public:
         NonThreadSafeDoWorkTask(PrefetchAndWorkWrapper* iWrapper):
         m_wrapper(iWrapper) {}
      
         tbb::task* execute() {
            auto wrapper = m_wrapper;
            m_wrapper->m_wrapper->runQueue()->push([wrapper]{
               //This is running in the m_runQueue so we know this instance is
               // the one that has the 'lock'.
               wrapper->m_wrapper->runQueue()->pause();
            
               s_non_thread_safe_queue->push([wrapper]{
                  s_non_thread_safe_queue->pause();
                  //we need to leave the non_thread_safe_queue so if our 'doWork' requests
                  // data from another thread unsafe module that module can be given the 'lock'
                  // on the thread unsafe queue and run its task.
                  s_thread_safe_queue->push([wrapper]{
                     wrapper->doWork();
                     //now that our work is done, we allow other instances use the queues
                     s_non_thread_safe_queue->resume();
                     wrapper->m_wrapper->runQueue()->resume();     
                  });
               });
            });
            return nullptr;
         }
      public:
         PrefetchAndWorkWrapper* m_wrapper;
      };
   }
}
void 
PrefetchAndWorkWrapper::doPrefetchAndWork()
{
  //when everything has been gotten, do our work
  if(module_()->threadType()!=kThreadUnsafe) {
     m_wrapper->prefetchAsync( new (tbb::task::allocate_root()) pnw::DoWorkTask(this));
     
     
/*    dispatch_group_notify_f(m_wrapper->prefetchGroup(),
                            m_wrapper->runQueue(),
                            static_cast<void*>(this),
                            PrefetchAndWorkWrapper::do_work_task); */
  } else {
    //Must first acquire the 'run' lock (i.e. be the running block
    // in the 'run' queue and only after that acquire the 
    // non-thread-safe lock since we must avoid having the same
    // instance of this module be added to the non-thread-safe queue
    // in the case we unblock that queue when we do a getByLabel
     m_wrapper->prefetchAsync( new (tbb::task::allocate_root()) pnw::NonThreadSafeDoWorkTask(this));
    
    /*
    dispatch_group_notify_f(m_wrapper->prefetchGroup(),
                            m_wrapper->runQueue(),
                            this,
                            PrefetchAndWorkWrapper::do_suspend_run_queue_before_work_task);
      */
  }
}
