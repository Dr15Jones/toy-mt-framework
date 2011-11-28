//
//  Path.cpp
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include "Path.h"
#include "Event.h"
#include "Filter.h"
using namespace demo;

void 
Path::run(Event& iEvent) {
  for(std::vector<Filter*>::iterator it=m_filters.begin(), itEnd = m_filters.end();
      it != itEnd;
      ++it) {
    if (not (*it)->doFilter(iEvent)) {
      break;
    }
  }
}

void 
Path::reset() {
  for(std::vector<Filter*>::iterator it=m_filters.begin(), itEnd = m_filters.end();
      it != itEnd;
      ++it) {
    (*it)->reset();
  }
}

void 
Path::setFatalJobErrorOccurredPointer(bool* iPtr) {
  m_fatalJobErrorOccurredPtr = iPtr;
}

void 
Path::addFilter(Filter* iFilter) {
  m_filters.push_back(iFilter);
}
