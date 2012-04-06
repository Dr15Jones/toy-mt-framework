//
//  SleepingProducer.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <unistd.h>
#include <iostream>
#include <vector>

#include "ConfiguredProducer.h"
#include "thread_type_from_config.h"
#include "busyWait.h"
#include "Event.h"
#include "Waiter.h"

namespace demo {
  
  class SleepingProducer : public ConfiguredProducer,Waiter {
  public:
    SleepingProducer(const boost::property_tree::ptree& iConfig);
  private:
    void doWait(double iSeconds);
    virtual void produce(edm::Event&);
    std::vector<const Getter*> m_getters;
  };
  
  SleepingProducer::SleepingProducer(const boost::property_tree::ptree& iConfig):
  ConfiguredProducer(iConfig,thread_type_from_config(iConfig)),
  Waiter(iConfig){
    registerProduct(demo::DataKey());
    
    for(const boost::property_tree::ptree::value_type& v: iConfig.get_child("toGet")) {
      m_getters.push_back(registerGet(v.second.get<std::string>("label"), 
                                      ""));
    }
  }
  
  void
  SleepingProducer::doWait(double iSeconds){
    usleep(iSeconds*1000000);
  }
  
  void 
  SleepingProducer::produce(edm::Event& iEvent) {
    //printf("Producer %s\n",label().c_str());
    int sum=0;
    for(std::vector<const Getter*>::iterator it = m_getters.begin(), itEnd=m_getters.end();
        it != itEnd;
        ++it) {
      sum +=iEvent.get(*it);
      //printf("%s got %s with value %i\n",label().c_str(), (*it)->label().c_str(), iEvent.get((*it)));
    }
    wait(iEvent);
    iEvent.put(this,"",static_cast<int>(sum));
  }
}
REGISTER_PRODUCER(demo::SleepingProducer);