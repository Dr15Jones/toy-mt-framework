//
//  PrefetchAndWorkWrapper.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/8/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_PrefetchAndWorkWrapper_h
#define DispatchProcessingDemo_PrefetchAndWorkWrapper_h
#include <dispatch/dispatch.h>
#include "GroupHolder.h"

namespace demo {
  class ModuleWrapper;
  class Module;

  class PrefetchAndWorkWrapper {
  public:
    PrefetchAndWorkWrapper(ModuleWrapper* iWrapper);
    
  protected:
    void doPrefetchAndWork();

  private:
    Module* module_() const;
    
    static void do_work_task(void* iContext);
    static void do_work_and_resume_queues_task(void* iContext);
    static void do_suspend_thread_unsafe_queue_before_work_task(void* iContext);
    static void do_suspend_run_queue_before_work_task(void* iContext);
    virtual void doWork() =0;
    
    
    ModuleWrapper* m_wrapper;

  };
  
};


#endif
