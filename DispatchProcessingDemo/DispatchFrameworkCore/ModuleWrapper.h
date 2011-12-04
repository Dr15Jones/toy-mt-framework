//
//  ModuleWrapper.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/8/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_ModuleWrapper_h
#define DispatchProcessingDemo_ModuleWrapper_h
#include <dispatch/dispatch.h>
#include "GroupHolder.h"

namespace demo {
  class Module;
  class Event;

  class ModuleWrapper {
  public:
    ModuleWrapper(Module* iModule,
                  Event* iEvent);
    ~ModuleWrapper();
    
    Module* module() const {
      return m_module;
    }

    Event* event() const {
      return m_event;
    }
    
    void reset() {
      m_requestedPrefetch=false;
    }

  protected:
    ModuleWrapper(const ModuleWrapper&, Event*);
    ModuleWrapper(const ModuleWrapper&);
    
    void doPrefetchAndWork();

  private:
    static void do_prefetch_task(void* iContext);
    void doPrefetch();
    static void do_work_task(void* iContext);
    static void do_work_and_resume_queues_task(void* iContext);
    static void do_suspend_thread_unsafe_queue_before_work_task(void* iContext);
    static void do_suspend_run_queue_before_work_task(void* iContext);
    virtual void doWork() =0;
    
    
    Module* m_module;
    Event* m_event;
    GroupHolder m_prefetchGroup;
    dispatch_queue_t m_prefetchQueue;
    dispatch_queue_t m_runQueue;
    bool m_requestedPrefetch;

  };
  
};


#endif
