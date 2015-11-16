//
//  PrefetchAndWorkWrapper.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/8/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_PrefetchAndWorkWrapper_h
#define DispatchProcessingDemo_PrefetchAndWorkWrapper_h
#include <exception>

namespace demo {
  class ModuleWrapper;
  class Module;
  class SerialTaskQueue;

  class PrefetchAndWorkWrapper {
     
  public:
    PrefetchAndWorkWrapper(ModuleWrapper* iWrapper);

    template<typename Task>
      static void callWrapperDoWork(PrefetchAndWorkWrapper*,std::exception_ptr);

    SerialTaskQueue* runQueue() const;
  protected:
    void doPrefetchAndWork();

  private:
    Module* module_() const;
    
    virtual void doWork(std::exception_ptr) =0;
    
    ModuleWrapper* m_wrapper;

  };
  
};


#endif
