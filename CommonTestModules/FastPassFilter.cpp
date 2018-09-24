//
//  FastPassFilter.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <unistd.h>

#include <vector>
#include "ConfiguredFilter.h"
#include "thread_type_from_config.h"
#include "Event.h"
#include "Waiter.h"

namespace demo {
  
  class FastPassFilter : public ConfiguredFilter {
  public:
    FastPassFilter(const boost::property_tree::ptree& iConfig);
  private:
    virtual bool filter(const edm::Event&) override;
    std::vector<const Getter*> m_getters;
  };
  
  FastPassFilter::FastPassFilter(const boost::property_tree::ptree& iConfig):
  ConfiguredFilter(iConfig,thread_type_from_config(iConfig))
  {
    
    for(const boost::property_tree::ptree::value_type& v: iConfig.get_child("toGet")) {
      m_getters.push_back(registerGet(v.second.get<std::string>("label"), 
                                      ""));
    }
  }
  
  bool 
  FastPassFilter::filter(const edm::Event& iEvent) {
    //printf("Producer %s\n",label().c_str());
    int sum=0;
    for(std::vector<const Getter*>::iterator it = m_getters.begin(), itEnd=m_getters.end();
        it != itEnd;
        ++it) {
      sum +=iEvent.get(*it);
      //printf("%s got %s with value %i\n",label().c_str(), (*it)->label().c_str(), iEvent.get((*it)));
    }
    return true;
  }
}
REGISTER_FILTER(demo::FastPassFilter);
