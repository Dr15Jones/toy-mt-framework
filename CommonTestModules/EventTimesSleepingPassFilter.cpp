//
//  EventTimesSleepingPassFilter.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/12/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <unistd.h>

#include <vector>
#include "EventTimesPassFilterBase.h"

namespace demo {
  class EventTimesSleepingPassFilter : public EventTimesPassFilterBase {
  public:
    EventTimesSleepingPassFilter(const boost::property_tree::ptree& iConfig);
  private:
    void wait(double iSeconds);
  };
  
  EventTimesSleepingPassFilter::EventTimesSleepingPassFilter(const boost::property_tree::ptree& iConfig):
  EventTimesPassFilterBase(iConfig) {}
  
  void
  EventTimesSleepingPassFilter::wait(double iSeconds){
    usleep(iSeconds*1000000);
  }
}
REGISTER_FILTER(demo::EventTimesSleepingPassFilter);