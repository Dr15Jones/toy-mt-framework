//
//  PassFilter.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_PassFilter_h
#define DispatchProcessingDemo_PassFilter_h

#include "ConfiguredFilter.h"

namespace demo {
  class PassFilter : public ConfiguredFilter {
  public:
    PassFilter(const boost::property_tree::ptree& iConfig);
    
  private:
    virtual bool filter(const edm::Event& iEvent);
  };
}

#endif
