//
//  ConfiguredFilter.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/29/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_ConfiguredFilter_h
#define DispatchProcessingDemo_ConfiguredFilter_h

#include "Filter.h"
#include "Factory.h"

namespace demo {
  class ConfiguredFilter : public Filter {
  public:
    ConfiguredFilter(const boost::property_tree::ptree& iConfig):
    Filter(iConfig.get<std::string>("@label")) {}
  };
}

#define REGISTER_FILTER(_type_) DEFINE_DEMO_FACTORY(demo::Filter,_type_)
#endif
