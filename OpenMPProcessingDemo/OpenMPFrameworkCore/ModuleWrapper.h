//
//  ModuleWrapper.h
//  OpenMPProcessingDemo
//
//  Created by Chris Jones on 10/8/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef OpenMPProcessingDemo_ModuleWrapper_h
#define OpenMPProcessingDemo_ModuleWrapper_h

#include <boost/shared_ptr.hpp>
#include "OMPLock.h"

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
    
    OMPLock* runLock() {
      return m_runLock.get();
    }
    
    void prefetch(Event&);
    
   private:
    ModuleWrapper& operator=(const ModuleWrapper&) = delete;
    
    Module* m_module;
    OMPLock m_prefetchLock;
    boost::shared_ptr<OMPLock> m_runLock;
  };
  
};


#endif
