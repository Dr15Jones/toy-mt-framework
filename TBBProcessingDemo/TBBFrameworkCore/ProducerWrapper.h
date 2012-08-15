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
#include <boost/shared_ptr.hpp>
#include "ModuleWrapper.h"
#include "PrefetchAndWorkWrapper.h"
#include "WaitingTaskList.h"

namespace demo {
  
  class Producer;
  class Event;
  class WaitableTask;
  
  class ProducerWrapper : private ModuleWrapper,PrefetchAndWorkWrapper {
  public:
    explicit ProducerWrapper(Producer*, Event*);
    ProducerWrapper(const ProducerWrapper&, Event*);
    ProducerWrapper(const ProducerWrapper&);
    ~ProducerWrapper();
    
    ///Pass task to be called when data has been produced
    void doProduceAsync(WaitableTask* iCallTaskWhenDone);

    void reset();

    bool wasRun() const {return m_wasRun;}
  private:
    void doProduceAsyncImpl();
    void doWork();
    Producer* producer() const;

    ProducerWrapper& operator=(const ProducerWrapper&);
    
    boost::shared_ptr<Producer> m_producer;
    WaitingTaskList m_waitingList;
    bool m_wasRun;

  };
}

#endif
