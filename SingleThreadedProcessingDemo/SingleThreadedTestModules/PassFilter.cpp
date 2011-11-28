//
//  PassFilter.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include "PassFilter.h"
#include "busyWait.h"

using namespace demo;

PassFilter::PassFilter(const boost::property_tree::ptree& iConfig):
  ConfiguredFilter(iConfig) {}
  
bool 
PassFilter::filter(const edm::Event& iEvent) {
  //printf("filter %s was Run on event %lu\n",label().c_str(),iEvent.index());
  float v = busyWait(10000);
  return v>0;
};

REGISTER_FILTER(demo::PassFilter);