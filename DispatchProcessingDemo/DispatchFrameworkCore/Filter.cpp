//
//  Filter.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 8/13/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <dispatch/dispatch.h>
#include <Block.h>

#include "Filter.h"
#include "Event.h"

namespace demo {
  namespace edm {
    class Event;
  }
  
  Filter::Filter(const std::string& iLabel,
         ThreadType iThreadType):
  Module(iLabel,iThreadType) {}
  
  Filter::Filter( const Filter& iOther):
  Module(iOther.label(),iOther.threadType()) 
  {}
  Filter::~Filter() {
  }
  
  bool Filter::doFilter(const Event& iEvent) {
    edm::Event event = edm::Event(const_cast<Event*>(&iEvent),threadType()!=kThreadUnsafe);
    return filter(event);
  }
}