//
//  EventTimesBusyWaitPassFilter.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/13/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include "busy_wait_scale_factor.h"
#include "busyWait.h"

#include <vector>
#include "EventTimesPassFilterBase.h"

namespace demo {
  class EventTimesBusyWaitPassFilter : public EventTimesPassFilterBase {
  public:
    EventTimesBusyWaitPassFilter(const boost::property_tree::ptree& iConfig);
  private:
    void wait(double iSeconds);
  };
  
  EventTimesBusyWaitPassFilter::EventTimesBusyWaitPassFilter(const boost::property_tree::ptree& iConfig):
  EventTimesPassFilterBase(iConfig) {}
  
  void
  EventTimesBusyWaitPassFilter::wait(double iSeconds){
    busyWait(iSeconds*busy_wait_scale_factor);
  }
}
REGISTER_FILTER(demo::EventTimesBusyWaitPassFilter);