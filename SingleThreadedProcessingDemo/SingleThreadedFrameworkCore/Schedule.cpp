//
//  Schedule.cpp
//  SingleThreadedProcessingDemo
//
//  Created by Chris Jones on 10/3/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <cassert>
#include "Schedule.h"
#include "Path.h"
#include "Filter.h"

using namespace demo;

void 
Schedule::process(Event& iEvent) {
  //printf("Schedule::process\n");
  reset();
  for(std::vector<Path*>::iterator it = m_paths.begin(), itEnd = m_paths.end();
      it != itEnd;
      ++it) {
    (*it)->run(iEvent);
  }
}

void 
Schedule::addPath(Path* iPath) {
  m_paths.push_back(iPath);
  iPath->setFatalJobErrorOccurredPointer(m_fatalJobErrorOccuredPtr);
}

void
Schedule::addPath(const std::vector<std::string>& iPath) {
  std::auto_ptr<Path> newPath(new Path);
  for(const std::string& name: iPath) {
    Filter* fw = findFilter(name);
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
Schedule::reset() {
  for(std::vector<Path*>::iterator it = m_paths.begin(), itEnd = m_paths.end();
      it != itEnd;
      ++it) {
    (*it)->reset();
  }
}

Filter*
Schedule::findFilter(const std::string& iLabel){
  for(Filter* fw: m_filters) {
    if (fw->label() == iLabel) {
      return fw;
    }
  }
  return 0;
}


void
Schedule::addFilter(Filter* iFilter) {
  m_filters.push_back(iFilter);
}

