//
//  Filter.cpp
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include "Filter.h"
#include "Event.h"

using namespace demo;

bool 
Filter::doFilter(const Event& iEvent) {
  if(not wasRun()) {
    prefetch(iEvent);
    edm::Event event = edm::Event(const_cast<Event*>(&iEvent));
    m_keep=filter(event);
    setWasRun();
  }
  return m_keep;
}
