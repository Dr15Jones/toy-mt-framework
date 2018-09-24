//
//  EventTimesProducerBase.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/13/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_EventTimesProducerBase_h
#define DispatchProcessingDemo_EventTimesProducerBase_h

#include <vector>

#include "ConfiguredProducer.h"

namespace demo {
  
  class EventTimesProducerBase : public ConfiguredProducer {
  public:
    EventTimesProducerBase(const boost::property_tree::ptree& iConfig);
  private:
    virtual void wait(double iSeconds) =0;
    virtual void produce(edm::Event&);
    std::vector<const Getter*> m_getters;
    std::vector<float> m_eventTimes;
  };
}

#endif
