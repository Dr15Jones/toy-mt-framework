//
//  EventTimesSleepingPassFilter.cpp
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 10/13/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <iostream>
#include <unistd.h>

#include <vector>
#include "ConfiguredFilter.h"
#include "Event.h"

namespace demo {
  
  class EventTimesSleepingPassFilter : public ConfiguredFilter {
  public:
    EventTimesSleepingPassFilter(const boost::property_tree::ptree& iConfig);
  private:
    void wait(double iSeconds);
    virtual bool filter(const edm::Event&);
    std::vector<const Getter*> m_getters;
    std::vector<float> m_eventTimes;
  };
  
  EventTimesSleepingPassFilter::EventTimesSleepingPassFilter(const boost::property_tree::ptree& iConfig):
  ConfiguredFilter(iConfig){
    
    for(const boost::property_tree::ptree::value_type& v: iConfig.get_child("toGet")) {
      m_getters.push_back(registerGet(v.second.get<std::string>("label"), 
                                      ""));
    }
    m_eventTimes.reserve(iConfig.get_child("eventTimes").size());
    for(const boost::property_tree::ptree::value_type& v: iConfig.get_child("eventTimes")) {
      m_eventTimes.push_back(v.second.get_value<float>());
    }
  }
  
  void
  EventTimesSleepingPassFilter::wait(double iSeconds){
    usleep(iSeconds*1000000);
  }
  
  bool 
  EventTimesSleepingPassFilter::filter(const edm::Event& iEvent) {
    //printf("Producer %s\n",label().c_str());
    int sum=0;
    for(std::vector<const Getter*>::iterator it = m_getters.begin(), itEnd=m_getters.end();
        it != itEnd;
        ++it) {
      sum +=iEvent.get(*it);
      //printf("%s got %s with value %i\n",label().c_str(), (*it)->label().c_str(), iEvent.get((*it)));
    }
    unsigned long index = iEvent.index() % m_eventTimes.size();
    
    wait(m_eventTimes[index]);
    return true;
  }
}
REGISTER_FILTER(demo::EventTimesSleepingPassFilter);