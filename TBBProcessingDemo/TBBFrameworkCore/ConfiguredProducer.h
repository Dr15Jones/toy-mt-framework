//
//  ConfiguredProducer.h
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/29/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#ifndef DispatchProcessingDemo_ConfiguredProducer_h
#define DispatchProcessingDemo_ConfiguredProducer_h
#include "Producer.h"
#include "Factory.h"

namespace demo {
  class ConfiguredProducer : public Producer {
  public:
    ConfiguredProducer(const boost::property_tree::ptree& iConfig, 
                       ThreadType iThreadType= kThreadUnsafe):
    Producer(iConfig.get<std::string>("@label"),iThreadType) {}
  };
}

#define REGISTER_PRODUCER(_type_) DEFINE_DEMO_FACTORY(demo::Producer,_type_)

#endif
