//
//  EventTimesPassFilterBase.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/13/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_EventTimesPassFilterBase_h
#define DispatchProcessingDemo_EventTimesPassFilterBase_h
#include <vector>
#include "ConfiguredFilter.h"

namespace demo {
  
  class EventTimesPassFilterBase : public ConfiguredFilter {
  public:
    EventTimesPassFilterBase(const boost::property_tree::ptree& iConfig);
  private:
    virtual void wait(double iSeconds) = 0;
    virtual bool filter(const edm::Event&);
    std::vector<const Getter*> m_getters;
    std::vector<float> m_eventTimes;
  };
}

#endif
