//
//  SleepingWithAcquireProducer.cpp
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
#include "Event.h"


namespace demo {
  
  class SleepingWithAcquireProducer : public ConfiguredProducer {
  public:
    SleepingWithAcquireProducer(const boost::property_tree::ptree& iConfig);

    bool hasAcquire() const final { return true;}

    void acquire(edm::Event const&, WaitingTaskWithArenaHolder) final;
  private:
    virtual void produce(edm::Event&);
    std::vector<const Getter*> m_getters;
    double m_sleep;
  };
  
  SleepingWithAcquireProducer::SleepingWithAcquireProducer(const boost::property_tree::ptree& iConfig):
  ConfiguredProducer(iConfig,thread_type_from_config(iConfig)),
  m_sleep(iConfig.get<double>("averageTime")){
    registerProduct(demo::DataKey());
    
    for(const boost::property_tree::ptree::value_type& v: iConfig.get_child("toGet")) {
      m_getters.push_back(registerGet(v.second.get<std::string>("label"), 
                                      ""));
    }
  }
  
  void
  SleepingWithAcquireProducer::acquire(edm::Event const&, WaitingTaskWithArenaHolder h) {
    usleep(m_sleep*1000000);
  }
  
  void 
  SleepingWithAcquireProducer::produce(edm::Event& iEvent) {
    //printf("Producer %s\n",label().c_str());
    int sum=0;
    for(std::vector<const Getter*>::iterator it = m_getters.begin(), itEnd=m_getters.end();
        it != itEnd;
        ++it) {
      sum +=iEvent.get(*it);
      //printf("%s got %s with value %i\n",label().c_str(), (*it)->label().c_str(), iEvent.get((*it)));
    }
    iEvent.put(this,"",static_cast<int>(sum));
  }
}
REGISTER_PRODUCER(demo::SleepingWithAcquireProducer);
