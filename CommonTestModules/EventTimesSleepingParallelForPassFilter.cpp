//
//  EventTimesSleepingParallelForPassFilter.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/12/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include <cassert>
#include <atomic>

#include <vector>
#include "EventTimesPassFilterBase.h"
#include "parallel_for.h"

namespace demo {
  class EventTimesSleepingParallelForPassFilter : public EventTimesPassFilterBase {
  public:
    explicit EventTimesSleepingParallelForPassFilter(const boost::property_tree::ptree& iConfig);
  private:
    void wait(double iSeconds) override;
    const unsigned int m_nRanges;
  };
  
  EventTimesSleepingParallelForPassFilter::EventTimesSleepingParallelForPassFilter(const boost::property_tree::ptree& iConfig):
    EventTimesPassFilterBase(iConfig),
    m_nRanges{iConfig.get<unsigned int>("nRanges")}
  {
  }
  
  void
  EventTimesSleepingParallelForPassFilter::wait(double iSeconds){
    std::atomic<unsigned int> nRangesDone{0};
    parallel_for(m_nRanges,[this,iSeconds,&nRangesDone](int) {
        usleep((iSeconds*1000000)/m_nRanges);
        ++nRangesDone;
      });
    assert(nRangesDone == m_nRanges);
  }
}
REGISTER_FILTER(demo::EventTimesSleepingParallelForPassFilter);
