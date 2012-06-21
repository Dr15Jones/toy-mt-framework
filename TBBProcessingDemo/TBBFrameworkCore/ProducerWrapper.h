//
//  ProducerWrapper.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/23/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_ProducerWrapper_h
#define DispatchProcessingDemo_ProducerWrapper_h

#include <boost/shared_ptr.hpp>
#include "ModuleWrapper.h"
#include "PrefetchAndWorkWrapper.h"
#include "WaitingList.h"

namespace demo {
  
  class Producer;
  class Event;
  
  class ProducerWrapper : private ModuleWrapper,PrefetchAndWorkWrapper {
  public:
    explicit ProducerWrapper(Producer*, Event*);
    ProducerWrapper(const ProducerWrapper&, Event*);
    ProducerWrapper(const ProducerWrapper&);
    ~ProducerWrapper();
    
    //returns the WaitingList assigned to when produce has finished
    // this way callers can add a task
    WaitingList& doProduceAsync();

    void reset();
    
  private:
    void doProduceAsyncImpl();
    void doWork();
    Producer* producer() const;

    ProducerWrapper& operator=(const ProducerWrapper&);
    
    boost::shared_ptr<Producer> m_producer;
    WaitingList m_waitingList;
    bool m_wasRun;

  };
}

#endif
