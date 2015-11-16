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

namespace demo {
  class Module;
  class Event;
  class WaitableTask;
  class WaitingTask;

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
      m_requestedPrefetch.clear();
    }

    void prefetchAsync(WaitingTask* iPostPrefetchTask);

    SerialTaskQueue* runQueue() const {
      return m_runQueue.get();
    }

  protected:
    ModuleWrapper(const ModuleWrapper&, Event*);
    ModuleWrapper(const ModuleWrapper&);
    ModuleWrapper& operator=(const ModuleWrapper&);
    
  private:
    Module* m_module;
    Event* m_event;
    std::shared_ptr<SerialTaskQueue> m_runQueue;
    std::atomic_flag m_requestedPrefetch;

  };
  
};


#endif
