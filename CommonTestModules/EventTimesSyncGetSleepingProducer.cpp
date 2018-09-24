//
//  EventTimesSyncGetSleepingProducer.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/12/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include <iostream>
#include <vector>

#include "EventTimesSyncGetProducerBase.h"
#include "thread_type_from_config.h"
#include "busyWait.h"
#include "Event.h"

namespace demo {
  
  class EventTimesSyncGetSleepingProducer : public EventTimesSyncGetProducerBase {
  public:
    EventTimesSyncGetSleepingProducer(const boost::property_tree::ptree& iConfig);
  private:
    void wait(double iSeconds);
  };
  
  EventTimesSyncGetSleepingProducer::EventTimesSyncGetSleepingProducer(const boost::property_tree::ptree& iConfig):
  EventTimesSyncGetProducerBase(iConfig){
  }
  
  void
  EventTimesSyncGetSleepingProducer::wait(double iSeconds){
    usleep(iSeconds*1000000);
  }  
}
REGISTER_PRODUCER(demo::EventTimesSyncGetSleepingProducer);