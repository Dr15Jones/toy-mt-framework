//
//  EventTimesProducerBase.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/13/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include <iostream>
#include <vector>

#include "EventTimesProducerBase.h"
#include "busyWait.h"
#include "Event.h"

static const std::string s_blank("");

namespace demo {
  
  EventTimesProducerBase::EventTimesProducerBase(const boost::property_tree::ptree& iConfig):
  ConfiguredProducer(iConfig){
    registerProduct(demo::DataKey());
    
    for(const boost::property_tree::ptree::value_type& v: iConfig.get_child("toGet")) {
      m_getters.push_back(registerGet(v.second.get<std::string>("label"), 
                                      s_blank));
    }
    m_eventTimes.reserve(iConfig.get_child("eventTimes").size());
    for(const boost::property_tree::ptree::value_type& v: iConfig.get_child("eventTimes")) {
      m_eventTimes.push_back(v.second.get_value<float>());
    }
  }
  
  void 
  EventTimesProducerBase::produce(edm::Event& iEvent) {
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
    iEvent.put(this,s_blank,static_cast<int>(sum));
  }
}
