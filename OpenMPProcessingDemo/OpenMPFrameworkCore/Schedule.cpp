//
//  Schedule.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>

#include <assert.h>

#include "Schedule.h"
#include "Path.h"
#include "FilterWrapper.h"

using namespace demo;


Schedule::Schedule()
  : m_event(),
  m_fatalJobErrorOccuredPtr(0){
}

Schedule::Schedule(Event* iEvent):
m_event(*iEvent),
m_fatalJobErrorOccuredPtr(0)
{  
}

Schedule::Schedule(const Schedule& iOther):
m_event(iOther.m_event),
m_fatalJobErrorOccuredPtr(iOther.m_fatalJobErrorOccuredPtr)
{
  m_filters.reserve(iOther.m_filters.size());
  for(boost::shared_ptr<FilterWrapper> fw: iOther.m_filters) {
    m_filters.push_back(boost::shared_ptr<FilterWrapper>(new FilterWrapper(fw.get())));
  }
  
  for(std::vector<boost::shared_ptr<Path>>::const_iterator it = iOther.m_paths.begin(), itEnd = iOther.m_paths.end();
      it != itEnd;
      ++it) {
    addPath((*it)->clone(m_filters));
  }
}

bool
Schedule::process() {
  //printf("Schedule::process\n");
  reset();
  if(!m_paths.empty()) {
    for(auto it = m_paths.begin(), itEnd = m_paths.end(); 
	it != itEnd; ++it) {
    auto temp = it->get();
#pragma omp task default(shared) firstprivate(temp)
      assert(temp);
      processPresentPath(temp);
    }
#pragma omp taskwait
    return not *m_fatalJobErrorOccuredPtr;
  } else {
    return true;
  }
}

void 
Schedule::addPath(Path* iPath) {
  m_paths.push_back(boost::shared_ptr<Path>(iPath));
  iPath->setFatalJobErrorOccurredPointer(m_fatalJobErrorOccuredPtr);
}

void
Schedule::addPath(const std::vector<std::string>& iPath) {
  std::auto_ptr<Path> newPath(new Path);
  for(const std::string& name: iPath) {
    FilterWrapper* fw = findFilter(name);
    if(0!=fw) {
      newPath->addFilter(fw);
    } else {
      assert(0=="Failed to find filter name");
      exit(1);
    }
  }
  addPath(newPath.release());
}

void
Schedule::addFilter(Filter* iFilter) {
  m_filters.push_back(boost::shared_ptr<FilterWrapper>(new FilterWrapper(boost::shared_ptr<Filter>(iFilter))));
}

void 
Schedule::reset() {
  for(auto fw: m_filters) {
    fw->reset();
  }
  for(auto& p: m_paths) {
    p->reset();
  }
}

Event*
Schedule::event()
{
  return &m_event;
}

FilterWrapper*
Schedule::findFilter(const std::string& iLabel){
  for(boost::shared_ptr<FilterWrapper> fw: m_filters) {
    if (fw->label() == iLabel) {
      return fw.get();
    }
  }
  return 0;
}

Schedule* 
Schedule::clone() {
  return new Schedule(*this);
}


void 
Schedule::processPresentPath(Path* iPath) {
  if(*m_fatalJobErrorOccuredPtr) {
    return;
  }
  iPath->run(m_event);
}
