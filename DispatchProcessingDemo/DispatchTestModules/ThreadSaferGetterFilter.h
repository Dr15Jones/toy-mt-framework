//
//  ThreadSaferGetterFilter.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_ThreadSaferGetterFilter_h
#define DispatchProcessingDemo_ThreadSaferGetterFilter_h
#include <string>
#include <vector>

#include "ConfiguredFilter.h"

namespace demo {
  class Getter;
  
  class ThreadSaferGetterFilter : public ConfiguredFilter {
  public:
    ThreadSaferGetterFilter(const boost::property_tree::ptree& iConfig);
    
  private:
    ThreadSaferGetterFilter(const ThreadSaferGetterFilter& iOther);    
    bool filter(const edm::Event&);

    std::vector<const Getter*> m_getters;
  };
}


#endif
