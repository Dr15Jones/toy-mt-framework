//
//  ModuleWrapper.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/8/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_ModuleWrapper_h
#define DispatchProcessingDemo_ModuleWrapper_h
#include <atomic>
#include "SerialTaskQueue.h"
#include "WaitingTaskList.h"
#include "WaitingTaskHolder.h"
#include "WaitingTaskWithArenaHolder.h"

namespace demo {
  class Module;
  class Event;

  class ModuleWrapper {
  public:
    ModuleWrapper(Module* iModule,
                  Event* iEvent);
    virtual ~ModuleWrapper();
    
    Module* module() const {
      return m_module;
    }

    Event* event() const {
      return m_event;
    }
    
    void reset() {
      m_workStarted = false;
      m_waitingTasks.reset();
    }


    void doWorkAsync(WaitingTaskHolder);

    SerialTaskQueue* runQueue() const {
      return m_runQueue.get();
    }

  protected:
    ModuleWrapper(const ModuleWrapper&, Event*);
    ModuleWrapper(const ModuleWrapper&);
    ModuleWrapper& operator=(const ModuleWrapper&);
    
  private:
    void prefetchAsync(WaitingTaskHolder iPostPrefetchTask);
    void runModuleAfterAsyncPrefetch(std::exception_ptr);
    void runModuleAcquireAfterAsyncPrefetch(tbb::task_group* iGroup, std::exception_ptr);
    virtual void implDoWork() = 0;
    virtual void implDoAcquire(WaitingTaskWithArenaHolder) = 0;
    Module* m_module;
    Event* m_event;
    WaitingTaskList m_waitingTasks;
    std::shared_ptr<SerialTaskQueue> m_runQueue;

    class RunModuleTask : public WaitingTask {
    public:
      RunModuleTask(ModuleWrapper* iWrapper);
      void execute() final;
      void setGroup(tbb::task_group* iGroup);
      void recycle() final;
    private:
      ModuleWrapper* m_wrapper;
      tbb::task_group* m_group;
    };
    RunModuleTask m_runTask;
    std::atomic<bool> m_workStarted;

  };
  
};


#endif
