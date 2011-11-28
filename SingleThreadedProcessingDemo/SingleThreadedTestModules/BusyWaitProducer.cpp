//
//  BusyWaitProducer.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <vector>
#include "ConfiguredProducer.h"
#include "busy_wait_scale_factor.h"
#include "busyWait.h"
#include "Event.h"
#include "Waiter.h"

namespace demo {

  class BusyWaitProducer : public ConfiguredProducer,Waiter {
  public:
    BusyWaitProducer(const boost::property_tree::ptree& iConfig);
  private:
    void doWait(double iSeconds);
    virtual void produce(edm::Event&);
    std::vector<const Getter*> m_getters;
  };

  BusyWaitProducer::BusyWaitProducer(const boost::property_tree::ptree& iConfig):
  ConfiguredProducer(iConfig),
  Waiter(iConfig){
    registerProduct(demo::DataKey());
    
    for(const boost::property_tree::ptree::value_type& v: iConfig.get_child("toGet")) {
      m_getters.push_back(registerGet(v.second.get<std::string>("label"), 
                                      ""));
    }
  }
  
  void
  BusyWaitProducer::doWait(double iSeconds){
    busyWait(iSeconds*busy_wait_scale_factor);
  }
  
  void 
  BusyWaitProducer::produce(edm::Event& iEvent) {
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
REGISTER_PRODUCER(demo::BusyWaitProducer);