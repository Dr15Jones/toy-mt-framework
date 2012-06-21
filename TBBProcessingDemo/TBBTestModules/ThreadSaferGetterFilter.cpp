//
//  ThreadSaferGetterFilter.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include "ThreadSaferGetterFilter.h"
#include "Event.h"
#include "Getter.h"

using namespace demo;

ThreadSaferGetterFilter::ThreadSaferGetterFilter(const boost::property_tree::ptree& iConfig):
  ConfiguredFilter(iConfig,kThreadSafeBetweenModules) {
    for(const boost::property_tree::ptree::value_type& v: iConfig.get_child("toGet")) {
      m_getters.push_back(registerGet(v.second.get<std::string>("label"), 
                                      v.second.get<std::string>("product")));
    }
  }
bool 
ThreadSaferGetterFilter::filter(const edm::Event&) {
  //printf("starting filter %s\n",label().c_str());
  for(std::vector<const Getter*>::iterator it = m_getters.begin(), itEnd=m_getters.end();
      it != itEnd;
      ++it) {
    //printf("  %s got %s with value %i\n",label().c_str(), (*it)->label().c_str(), (*it)->value());
  }
  return true;
}

REGISTER_FILTER(demo::ThreadSaferGetterFilter);