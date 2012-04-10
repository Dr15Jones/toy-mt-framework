//
//  ModuleWrapper.h
//  OpenMPProcessingDemo
//
//  Created by Chris Jones on 10/8/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef OpenMPProcessingDemo_ModuleWrapper_h
#define OpenMPProcessingDemo_ModuleWrapper_h

#if defined(PARALLEL_MODULES)
#include <atomic>
#endif
#include <boost/shared_ptr.hpp>
#if defined(PARALLEL_MODULES)
#include "TaskYieldLock.h"
#else
#include "OMPLock.h"
#endif

namespace demo {
  class Module;
  class Event;

  class ModuleWrapper {
  public:
    ModuleWrapper(Module* iModule);
    virtual ~ModuleWrapper();
    
  protected:
    ModuleWrapper(const ModuleWrapper*);
    ModuleWrapper(const ModuleWrapper&);
    
#if defined(PARALLEL_MODULES)
    TaskYieldLock* runLock() {
#else
    OMPLock* runLock() {
#endif
      return m_runLock.get();
    }
    
#if defined(PARALLEL_MODULES)
    void reset() { m_donePrefetch=false; }
#endif
    
    void prefetch(Event&);
    
   private:
    ModuleWrapper& operator=(const ModuleWrapper&) = delete;
    
    Module* m_module;
#if defined(PARALLEL_MODULES)
    TaskYieldLock m_prefetchLock;
    boost::shared_ptr<TaskYieldLock> m_runLock;
    std::atomic<bool> m_donePrefetch;
#else
    boost::shared_ptr<OMPLock> m_runLock;
#endif
  };
  
};


#endif
