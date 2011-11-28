//
//  Path.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <Block.h>
#include <assert.h>
#include "Path.h"
#include "filtering_callback.h"
#include "FilterWrapper.h"


using namespace demo;

void Path::runAsync(Event& iEvent, filtering_callback_t iCallback) {
  if(!m_filters.empty()) {
    //need to make a copy since iCallback can be on the stack
    filtering_callback_t heapCallback = Block_copy(iCallback);
    
    runFilterAsync(iEvent,0,heapCallback);
  } else {
    iCallback(true);
  }
}
  
void Path::runFilterAsync(Event& iEvent, 
                      unsigned int iIndex,
                      filtering_callback_t iCallback) const {
  if (*m_fatalJobErrorOccurredPtr) {
    //There must have been a fatal problem on another path
    iCallback(true);
    Block_release(iCallback);
    return;
  }
    
  //items in a block become 'const' but we need non-const access to the event, hence use the pointer
  Event* nonConstEvent = &iEvent;
  m_filters[iIndex]->filterAsync(iEvent, ^(bool iKeep, bool iSuccess) {
    //something bad happened
    if(!iSuccess) {
      iCallback(false);
      Block_release(iCallback);
    }
    if(not *m_fatalJobErrorOccurredPtr && iKeep && iIndex+1 < m_filters.size()) {
      //go to next
      runFilterAsync(*nonConstEvent,iIndex+1,iCallback);
    } else {
      //finished path without an error
      iCallback(true);
      Block_release(iCallback);
    }
  });
}
  
void Path::reset() {
  dispatch_apply(m_filters.size(), dispatch_get_global_queue(0, 0), ^(size_t iIndex){
    m_filters[iIndex]->reset();
  });         
}
  
void Path::addFilter(FilterWrapper* iFilter) {
  m_filters.push_back(iFilter);
}

  
Path* Path::clone(const std::vector<FilterWrapper*>& iWrappers) const {
  std::auto_ptr<Path> newPath(new Path);
  newPath->m_filters.reserve(m_filters.size());
  for (FilterWrapper* fw: m_filters) {
    bool found = false;
    for(FilterWrapper* newFw: iWrappers) {
      if(newFw->label() == fw->label()) {
        found = true;
        newPath->m_filters.push_back(newFw);
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
  
Path::Path(const Path& iOther):m_fatalJobErrorOccurredPtr(0) {
  m_filters.reserve(iOther.m_filters.size());
  for(std::vector<FilterWrapper*>::const_iterator it=iOther.m_filters.begin(), itEnd = iOther.m_filters.end();
      it != itEnd;
      ++it) {
    m_filters.push_back(new FilterWrapper(*(*it)));
  }
}
