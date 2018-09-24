//
//  EventTimesSyncGetPassFilterBase.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/13/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_EventTimesSyncGetPassFilterBase_h
#define DispatchProcessingDemo_EventTimesSyncGetPassFilterBase_h
#include <vector>
#include "ConfiguredFilter.h"

namespace demo {
  
  class EventTimesSyncGetPassFilterBase : public ConfiguredFilter {
  public:
    EventTimesSyncGetPassFilterBase(const boost::property_tree::ptree& iConfig);
  private:
    virtual void wait(double iSeconds) = 0;
    virtual bool filter(const edm::Event&);
    std::vector<std::string> m_toGet;
    std::vector<float> m_eventTimes;
  };
}

#endif
