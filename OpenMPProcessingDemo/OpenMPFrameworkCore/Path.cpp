//
//  Path.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>

#include <cassert>
#include <memory>
#include "Path.h"
#include "FilterWrapper.h"


using namespace demo;

void Path::runAsync(WaitingTaskHolder iCallback) {
  m_callback.reset();
  m_callback.add(std::move(iCallback));
  if(!m_filters.empty()) {
    runFilterAsync(0);
  } else {
    m_callback.doneWaiting(std::exception_ptr{});
  }
}


void Path::filterFinished(std::exception_ptr const* iException, size_t iPreviousIndex) {
  if(not iException && not *m_fatalJobErrorOccurredPtr && m_filters[iPreviousIndex].checkResultsOfRunFilter() && iPreviousIndex+1 < m_filters.size()) {
    //go to next
    runFilterAsync(iPreviousIndex+1);
  } else {
    //finished path without an error
    std::exception_ptr ptr;
    if(iException) {
      ptr = *iException;
    }
    m_callback.doneWaiting(ptr);
  }  
}
  
void Path::runFilterAsync(size_t iIndex) {
  if (*m_fatalJobErrorOccurredPtr) {
    //There must have been a fatal problem on another path
    m_callback.doneWaiting(std::exception_ptr{});
    return;
  }
  auto nextTask = make_waiting_task([this, iIndex](std::exception_ptr const* iPtr) 
  {
    this->filterFinished(iPtr, iIndex);
  });
  m_filters[iIndex].filterAsync(WaitingTaskHolder{std::move(nextTask)});
}
  
void Path::reset() {
}
  
void Path::addFilter(FilterWrapper* iFilter,Event*iEvent) {
  m_filters.emplace_back(iFilter,m_filters.size());
}

  
Path* Path::clone(const std::vector<std::shared_ptr<FilterWrapper> >& iWrappers, Event*iEvent) const {
  auto newPath = std::make_unique<Path>();
  newPath->m_filters.reserve(m_filters.size());
  for (const auto& fw: m_filters) {
    bool found = false;
    for(auto newFw: iWrappers) {
      if(newFw->label() == fw.label()) {
        found = true;
        newPath->m_filters.emplace_back(newFw.get(),newPath->m_filters.size());
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
