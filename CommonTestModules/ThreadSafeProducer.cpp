//
//  ThreadSafeProducer.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include "ThreadSafeProducer.h"
#include "busyWait.h"
#include "Event.h"

using namespace demo;

ThreadSafeProducer::ThreadSafeProducer(const boost::property_tree::ptree& iConfig) :
  ConfiguredProducer(iConfig,kThreadSafeBetweenInstances),
  m_product(iConfig.get<std::string>("productLabel")),
  m_value(iConfig.get<int>("value")) {
  registerProduct(m_product);
}
  
void
ThreadSafeProducer::produce(edm::Event& iEvent) {
  //printf("Producer %s\n",label().c_str());
  float v = busyWait(100000);
  iEvent.put(this,m_product,static_cast<int>(m_value+(static_cast<unsigned long long>(v)%10)));
}
REGISTER_PRODUCER(demo::ThreadSafeProducer);