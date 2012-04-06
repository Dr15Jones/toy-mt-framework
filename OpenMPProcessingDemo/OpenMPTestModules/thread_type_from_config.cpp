//
//  thread_type_from_config.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include "thread_type_from_config.h"
#include <iostream>

static const std::string kThreadUnsafeName("ThreadUnsafe");
static const std::string kThreadSafeBetweenModulesName("ThreadSafeBetweenModules");
static const std::string kThreadSafeBetweenInstancesName("ThreadSafeBetweenInstances");

demo::ThreadType 
thread_type_from_config(const boost::property_tree::ptree& iConfig) {
  const std::string& config = iConfig.get<std::string>("threadType");
  
  if(config == kThreadUnsafeName) {
    return demo::kThreadUnsafe;
  }
  if(config == kThreadSafeBetweenModulesName) {
    return demo::kThreadSafeBetweenModules;
  }
  if(config == kThreadSafeBetweenInstancesName) {
    return demo::kThreadSafeBetweenInstances;
  }
  std::cout <<"unknown '"<<config<<"'"<<std::endl;
  assert(0=="unknown thread type");
  return demo::kThreadUnsafe;
}
