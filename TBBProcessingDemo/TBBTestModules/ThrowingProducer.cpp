//
//  ThrowingProducer.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <vector>
#include <exception>
#include "ConfiguredProducer.h"
#include "thread_type_from_config.h"
#include "Event.h"

namespace demo {

  class ThrowingProducer : public ConfiguredProducer {
  public:
    ThrowingProducer(const boost::property_tree::ptree& iConfig);
  private:
    virtual void produce(edm::Event&);
    std::vector<const Getter*> m_getters;
  };

  ThrowingProducer::ThrowingProducer(const boost::property_tree::ptree& iConfig):
  ConfiguredProducer(iConfig,thread_type_from_config(iConfig)) {
    registerProduct(demo::DataKey());
    
    for(const boost::property_tree::ptree::value_type& v: iConfig.get_child("toGet")) {
      m_getters.push_back(registerGet(v.second.get<std::string>("label"), 
                                      ""));
    }
  }
  
  void 
  ThrowingProducer::produce(edm::Event& iEvent) {
    //printf("Producer %s\n",label().c_str());
    int sum=0;
    for(std::vector<const Getter*>::iterator it = m_getters.begin(), itEnd=m_getters.end();
        it != itEnd;
        ++it) {
      sum +=iEvent.get(*it);
      //printf("%s got %s with value %i\n",label().c_str(), (*it)->label().c_str(), iEvent.get((*it)));
    }
    throw std::runtime_error("producer throw test");
    iEvent.put(this,"",static_cast<int>(sum));
  }
}
REGISTER_PRODUCER(demo::ThrowingProducer);