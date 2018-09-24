//
//  EventTimesBusyWaitProducer.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/13/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include <iostream>
#include <vector>

#include "EventTimesProducerBase.h"
#include "busy_wait_scale_factor.h"
#include "busyWait.h"

namespace demo {
  
  class EventTimesBusyWaitProducer : public EventTimesProducerBase {
  public:
    EventTimesBusyWaitProducer(const boost::property_tree::ptree& iConfig);
  private:
    void wait(double iSeconds);
  };
  
  EventTimesBusyWaitProducer::EventTimesBusyWaitProducer(const boost::property_tree::ptree& iConfig):
  EventTimesProducerBase(iConfig){
  }
  
  void
  EventTimesBusyWaitProducer::wait(double iSeconds){
    busyWait(iSeconds*busy_wait_scale_factor);
  }  
}
REGISTER_PRODUCER(demo::EventTimesBusyWaitProducer);