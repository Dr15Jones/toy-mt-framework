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
#include "task_helpers.h"

using namespace demo;


Schedule::Schedule()
  : m_event(),
  m_pathsStillRunning(0),
  m_pathDoneCallback(this),
  m_scheduleCallback(),
  m_fatalJobErrorOccuredPtr(0){
}

Schedule::Schedule(Event* iEvent):
m_event(*iEvent),
m_pathsStillRunning(0),
m_pathDoneCallback(this),
m_scheduleCallback(),
m_fatalJobErrorOccuredPtr(0)
{  
}

Schedule::Schedule(const Schedule& iOther):
m_event(iOther.m_event),
m_pathsStillRunning(0),
m_pathDoneCallback(this),
m_scheduleCallback(),
m_fatalJobErrorOccuredPtr(iOther.m_fatalJobErrorOccuredPtr)
{
  m_filters.reserve(iOther.m_filters.size());
  for(auto fw: iOther.m_filters) {
    m_filters.push_back(std::make_shared<FilterWrapper>(*fw,&m_event));
  }
  
  m_paths.reserve(iOther.m_paths.size());
  for(auto& itPath : iOther.m_paths) {
    addPath(itPath->clone(m_filters,&m_event));
  }
}

void 
Schedule::process(ScheduleFilteringCallback iCallback) {
  m_scheduleCallback = iCallback;
  //printf("Schedule::process\n");
  reset();
  if(!m_paths.empty()) {
     
    m_pathsStillRunning=m_paths.size();
    for(auto& path : m_paths) {
      Schedule* pThis = this; 
      Path* pPath = path.get();
      spawn_task_from([pPath,pThis]{pThis->processPresentPath(pPath);});
    }
  } else {
    m_scheduleCallback(std::exception_ptr{});
  }
}

void 
Schedule::addPath(Path* iPath) {
  m_paths.push_back(std::shared_ptr<Path>(iPath));
  iPath->setFatalJobErrorOccurredPointer(m_fatalJobErrorOccuredPtr);
}

void
Schedule::addPath(const std::vector<std::string>& iPath) {
  std::auto_ptr<Path> newPath(new Path);
  for(const auto& name: iPath) {
    FilterWrapper* fw = findFilter(name);
    if(0!=fw) {
      newPath->addFilter(fw,&m_event);
    } else {
      assert(0=="Failed to find filter name");
      exit(1);
    }
  }
  addPath(newPath.release());
}

void
Schedule::addFilter(Filter* iFilter) {
  m_filters.push_back(std::make_shared<FilterWrapper>(std::shared_ptr<Filter>(iFilter),&m_event));
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
  for(auto fw: m_filters) {
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
Schedule::aPathHasFinished(std::exception_ptr iException) {
   if(!iException) {
     *(m_fatalJobErrorOccuredPtr) = true;
   }
   assert(0!=m_pathsStillRunning);
   if(0== --m_pathsStillRunning) {
     m_scheduleCallback(iException);
   }
}


void
PathFilteringCallback::operator()(std::exception_ptr iException) const
{
  m_schedule->aPathHasFinished(iException);
}


void 
Schedule::processPresentPath(Path* iPath) {
  //printf("Schedule::processPresentPath %u\n",iIndex);      
  if(*(m_fatalJobErrorOccuredPtr)) {
    m_scheduleCallback(std::exception_ptr{} );
    return;
  }
  iPath->runAsync(m_pathDoneCallback);
}

