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

void Path::runAsync(PathFilteringCallback iCallback) {
  m_callback = iCallback;
  if(!m_filters.empty()) {
    runFilterAsync(0);
  } else {
    m_callback(true);
  }
}


void Path::doNextIfSuccess(bool iKeep, bool iSuccess, size_t iPreviousIndex) {
  //something bad happened
  if(!iSuccess) {
    m_callback(false);
  }
  if(not *m_fatalJobErrorOccurredPtr && iKeep && iPreviousIndex+1 < m_filters.size()) {
    //go to next
    runFilterAsync(iPreviousIndex+1);
  } else {
    //finished path without an error
    m_callback(true);
  }  
}
  
void Path::runFilterAsync(size_t iIndex) {
  if (*m_fatalJobErrorOccurredPtr) {
    //There must have been a fatal problem on another path
    m_callback(true);
    return;
  }
    
  m_filters[iIndex].filterAsync();
}
  
void Path::reset() {
}
  
void Path::addFilter(FilterWrapper* iFilter,Event*iEvent) {
  m_filters.push_back(FilterOnPathWrapper(iFilter,this,m_filters.size()));
}

  
Path* Path::clone(const std::vector<boost::shared_ptr<FilterWrapper> >& iWrappers, Event*iEvent) const {
  std::auto_ptr<Path> newPath(new Path);
  newPath->m_filters.reserve(m_filters.size());
  for (const FilterOnPathWrapper& fw: m_filters) {
    bool found = false;
    for(boost::shared_ptr<FilterWrapper> newFw: iWrappers) {
      if(newFw->label() == fw.label()) {
        found = true;
        newPath->m_filters.push_back(FilterOnPathWrapper(newFw.get(),newPath.get(),newPath->m_filters.size()));
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
