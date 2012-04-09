//
//  ProducerWrapper.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/23/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_ProducerWrapper_h
#define DispatchProcessingDemo_ProducerWrapper_h

#if defined(PARALLEL_MODULES)  
#include <atomic>
#endif
#include <boost/shared_ptr.hpp>
#include "ModuleWrapper.h"

namespace demo {
  
  class Producer;
  class Event;
  
  class ProducerWrapper : private ModuleWrapper {
  public:
    explicit ProducerWrapper(Producer*);
    ProducerWrapper(const ProducerWrapper*);
    ProducerWrapper(const ProducerWrapper&);
    ~ProducerWrapper();
    
    void doProduce(Event&);

    void reset();
    
  private:
    ProducerWrapper& operator=(const ProducerWrapper&) = delete;
    
    boost::shared_ptr<Producer> m_producer;
#if defined(PARALLEL_MODULES)  
    std::atomic<bool> m_wasRun;
#else
    bool m_wasRun;
#endif
  };
}

#endif
