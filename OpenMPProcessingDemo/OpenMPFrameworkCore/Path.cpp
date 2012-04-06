//
//  Path.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>

#include <assert.h>
#include "Path.h"
#include "PathFilteringCallback.h"
#include "FilterWrapper.h"


using namespace demo;

void
Path::run(Event& iEvent) {
  for(auto& filter : m_filters) {
    if(*m_fatalJobErrorOccurredPtr) {
      break;
    }
    bool result = filter->doFilter(iEvent);
    if(*m_fatalJobErrorOccurredPtr or not result) {
      break;
    }
  }
  return;
}

void Path::reset() {
}
  
void Path::addFilter(FilterWrapper* iFilter) {
  m_filters.push_back(iFilter);
}

Path* Path::clone(const std::vector<boost::shared_ptr<FilterWrapper> >& iWrappers) const {
  std::auto_ptr<Path> newPath(new Path);
  newPath->m_filters.reserve(m_filters.size());
  for (const FilterWrapper* fw: m_filters) {
    bool found = false;
     for(boost::shared_ptr<FilterWrapper> newFw: iWrappers) {
      if(newFw->label() == fw->label()) {
        found = true;
         newPath->m_filters.push_back(newFw.get());
        break;
      }
    }
    if(not found) {
      assert(0=="Could not find filter label");
      exit(1);
    }
  }
  return newPath.release();
}
