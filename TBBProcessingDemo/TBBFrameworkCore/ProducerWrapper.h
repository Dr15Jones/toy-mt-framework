//
//  ProducerWrapper.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/23/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_ProducerWrapper_h
#define DispatchProcessingDemo_ProducerWrapper_h
#include <atomic>
#include <memory>
#include <exception>
#include "ModuleWrapper.h"
#include "Producer.h"

namespace demo {
  
  class Event;
  
  class ProducerWrapper : public ModuleWrapper {
  public:
    explicit ProducerWrapper(Producer*, Event*);
    ProducerWrapper(const ProducerWrapper&, Event*);
    ProducerWrapper(const ProducerWrapper&);
    
    ///Pass task to be called when data has been produced
    void doProduceAsync(WaitingTask* iCallTaskWhenDone);

    unsigned int id() const {return m_producer->id();}
  private:
    void implDoWork() override;
    Producer* producer() const;

    ProducerWrapper& operator=(const ProducerWrapper&) = delete;
    
    std::shared_ptr<Producer> m_producer;

  };
}

#endif
