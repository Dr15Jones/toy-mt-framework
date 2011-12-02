//
//  ProducerWrapper.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/23/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_ProducerWrapper_h
#define DispatchProcessingDemo_ProducerWrapper_h

#include <dispatch/dispatch.h>
#include "GroupHolder.h"
#include "ModuleWrapper.h"

namespace demo {
  
  class Producer;
  class Event;
  
  class ProducerWrapper : private ModuleWrapper {
  public:
    explicit ProducerWrapper(Producer*, Event*);
    ProducerWrapper(const ProducerWrapper&, Event*);
    ProducerWrapper(const ProducerWrapper&);
    ~ProducerWrapper();
    
    //returns the dispatch group assigned to when produce has finished
    // this way callers can either wait on that group or can associate
    // a callback to that group
    GroupHolder doProduceAsync();

    void reset();
    
  private:
    typedef void(^WorkBlock_t)();
    static void do_produceAsyncImpl_task(void*);
    void doProduceAsyncImpl();
    void doWork();
    Producer* producer() const;

    WorkBlock_t createWorkBlock();
    ProducerWrapper& operator=(const ProducerWrapper&);
    
    dispatch_queue_t m_runQueue;
    GroupHolder m_group;
    WorkBlock_t m_work;
    bool m_wasRun;
  };
}

#endif
