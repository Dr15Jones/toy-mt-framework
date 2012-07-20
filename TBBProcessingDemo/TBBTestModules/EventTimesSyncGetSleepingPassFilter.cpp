//
//  EventTimesSyncGetSleepingPassFilter.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/12/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <unistd.h>

#include <vector>
#include "EventTimesSyncGetPassFilterBase.h"

namespace demo {
  class EventTimesSyncGetSleepingPassFilter : public EventTimesSyncGetPassFilterBase {
  public:
    EventTimesSyncGetSleepingPassFilter(const boost::property_tree::ptree& iConfig);
  private:
    void wait(double iSeconds);
  };
  
  EventTimesSyncGetSleepingPassFilter::EventTimesSyncGetSleepingPassFilter(const boost::property_tree::ptree& iConfig):
  EventTimesSyncGetPassFilterBase(iConfig) {}
  
  void
  EventTimesSyncGetSleepingPassFilter::wait(double iSeconds){
    usleep(iSeconds*1000000);
  }
}
REGISTER_FILTER(demo::EventTimesSyncGetSleepingPassFilter);