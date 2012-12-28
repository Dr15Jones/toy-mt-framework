//
//  PrefetchAndWorkWrapper.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/8/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <sstream>
#include "tbb/task.h"

#include "PrefetchAndWorkWrapper.h"
#include "ModuleWrapper.h"
#include "ModuleThreadStack.h"
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

inline
SerialTaskQueue*
PrefetchAndWorkWrapper::runQueue() const {
  return m_wrapper->runQueue();
}

static const unsigned int kNoDependenciesReturnCode = 0xFFFFFFFF;

static
unsigned int checkStackForDependencies(unsigned int iTransitionID,Module const* iModule) {
   for(auto const& transMod: demo::ModuleThreadStack::stack()) {
      if(transMod.transitionID == iTransitionID) {
         if(iModule->isADependentModule(transMod.moduleID)) {
            return transMod.moduleID;
         }
      }
   }
   return kNoDependenciesReturnCode;
}

namespace {
   //NOTE: the compiler things tbb::task::allocate_root requires a 'this' pointer when called from within a task.
   // That doesn't work inside my lambda's which are not capturing 'this'. This template function avoids the problem.
   template <typename T, typename W> T* createTask(W* iWrapper) { return new (tbb::task::allocate_root()) T(iWrapper);}
}

namespace demo {
   namespace pnw {
      class DoWorkTask : public tbb::task {
      public:
         DoWorkTask(PrefetchAndWorkWrapper* iWrapper):
         m_wrapper(iWrapper) {}
      
         tbb::task* execute() {
            auto wrapper = this->m_wrapper;
            auto queue = wrapper->runQueue();
            tbb::task* nextTask=0;
            if(queue) {
               nextTask = wrapper->runQueue()->pushAndGetNextTaskToRun([wrapper]{
                  unsigned int depModule = checkStackForDependencies(wrapper->m_wrapper->event()->transitionID(),wrapper->module_());
                  if(kNoDependenciesReturnCode!= depModule) {
                     wrapper->m_wrapper->event()->mustWaitFor(depModule,createTask<pnw::DoWorkTask>(wrapper));
                  } else {
                     ModuleThreadStackSentry s(wrapper->m_wrapper->event()->transitionID(), wrapper->module_()->id());
                     wrapper->doWork();
                  }
                  });
            } else {
               unsigned int depModule = checkStackForDependencies(wrapper->m_wrapper->event()->transitionID(),wrapper->module_());
               if(kNoDependenciesReturnCode!= depModule) {
                  wrapper->m_wrapper->event()->mustWaitFor(depModule,createTask<pnw::DoWorkTask>(wrapper));
               } else {
                  ModuleThreadStackSentry s(wrapper->m_wrapper->event()->transitionID(), wrapper->module_()->id());
                  wrapper->doWork();
               }
            }
            return nextTask;
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
            auto nextTask = wrapper->runQueue()->pushAndGetNextTaskToRun([wrapper]{
               //This is running in the m_runQueue so we know this instance is
               // the one that has the 'lock'.
               wrapper->runQueue()->pause();
            
               s_non_thread_safe_queue->push([wrapper]{
                  s_non_thread_safe_queue->pause();
                  //we need to leave the non_thread_safe_queue so if our 'doWork' requests
                  // data from another thread unsafe module that module can be given the 'lock'
                  // on the thread unsafe queue and run its task.
                  s_thread_safe_queue->push([wrapper]{
                     unsigned int depModule = checkStackForDependencies(wrapper->m_wrapper->event()->transitionID(),wrapper->module_());
                     if(kNoDependenciesReturnCode!= depModule) {
                        wrapper->m_wrapper->event()->mustWaitFor(depModule,createTask<pnw::NonThreadSafeDoWorkTask>(wrapper));
                     } else {
                        ModuleThreadStackSentry s(wrapper->m_wrapper->event()->transitionID(), wrapper->module_()->id());
                        wrapper->doWork();
                     }
                     //now that our work is done, we allow other instances use the queues
                     s_non_thread_safe_queue->resume();
                     wrapper->runQueue()->resume();     
                  });
               });
            });
            return nextTask;
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
  } else {
    //Must first acquire the 'run' lock (i.e. be the running block
    // in the 'run' queue and only after that acquire the 
    // non-thread-safe lock since we must avoid having the same
    // instance of this module be added to the non-thread-safe queue
    // in the case we unblock that queue when we do a getByLabel
     m_wrapper->prefetchAsync( new (tbb::task::allocate_root()) pnw::NonThreadSafeDoWorkTask(this));
  }
}
