//
//  Producer.cpp
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 8/5/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>

#include "Producer.h"
#include "Event.h"
using namespace demo;

Producer::Producer(const std::string& iLabel):
Module(iLabel) {}

Producer::~Producer() {}

void
Producer::doProduce(Event& iEvent)
{
  prefetch(iEvent);
  edm::Event event(&iEvent);
  produce(event);
  setWasRun();   
}
