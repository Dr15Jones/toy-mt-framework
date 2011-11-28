//
//  ThreadSafeProducer.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_ThreadSafeProducer_h
#define DispatchProcessingDemo_ThreadSafeProducer_h
#include <string>
#import "ConfiguredProducer.h"

namespace demo {
  class ThreadSafeProducer : public ConfiguredProducer {
  public:
    ThreadSafeProducer(const boost::property_tree::ptree& iConfig);
    
  private:
    void produce(edm::Event& iEvent);
    std::string m_product;
    int m_value;
  };
}


#endif
