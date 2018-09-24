//
//  ThrowingFilter.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include <exception>

#include <vector>
#include "ConfiguredFilter.h"
#include "thread_type_from_config.h"
#include "Event.h"
#include "Waiter.h"

namespace demo {
  
  class ThrowingFilter : public ConfiguredFilter {
  public:
    ThrowingFilter(const boost::property_tree::ptree& iConfig);
  private:
    virtual bool filter(const edm::Event&) override;
    std::vector<const Getter*> m_getters;
  };
  
  ThrowingFilter::ThrowingFilter(const boost::property_tree::ptree& iConfig):
  ConfiguredFilter(iConfig,thread_type_from_config(iConfig))
  {
    
    for(const boost::property_tree::ptree::value_type& v: iConfig.get_child("toGet")) {
      m_getters.push_back(registerGet(v.second.get<std::string>("label"), 
                                      ""));
    }
  }
  
  bool 
  ThrowingFilter::filter(const edm::Event& iEvent) {
    printf("Producer %s\n",label().c_str());
    
    throw std::runtime_error("exception test");
    return true;
  }
}
REGISTER_FILTER(demo::ThrowingFilter);
