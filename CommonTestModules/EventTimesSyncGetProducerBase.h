//
//  EventTimesSyncGetProducerBase.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/13/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_EventTimesSyncGetProducerBase_h
#define DispatchProcessingDemo_EventTimesSyncGetProducerBase_h

#include <vector>
#include <string>

#include "ConfiguredProducer.h"

namespace demo {
  
  class EventTimesSyncGetProducerBase : public ConfiguredProducer {
  public:
    EventTimesSyncGetProducerBase(const boost::property_tree::ptree& iConfig);
  private:
    virtual void wait(double iSeconds) =0;
    virtual void produce(edm::Event&);
    std::vector<std::string> m_toGet;
    std::vector<float> m_eventTimes;
  };
}

#endif
