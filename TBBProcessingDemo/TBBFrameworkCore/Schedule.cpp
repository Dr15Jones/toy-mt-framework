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
#include "Queues.h"

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
  for(boost::shared_ptr<FilterWrapper> fw: iOther.m_filters) {
    m_filters.push_back(boost::shared_ptr<FilterWrapper>(new FilterWrapper(*fw,&m_event)));
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
           s_thread_safe_queue->push([pPath,pThis]{pThis->processPresentPath(pPath);});
    }
  } else {
    m_scheduleCallback(true);
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
  m_filters.push_back(boost::shared_ptr<FilterWrapper>(new FilterWrapper(boost::shared_ptr<Filter>(iFilter),&m_event)));
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
Schedule::aPathHasFinished(bool iSuccess) {
   if(!iSuccess) {
     *(m_fatalJobErrorOccuredPtr) = true;
   }
   assert(0!=m_pathsStillRunning);
   if(0== --m_pathsStillRunning) {
     m_scheduleCallback(not *m_fatalJobErrorOccuredPtr);
   }
}


void
PathFilteringCallback::operator()(bool iSuccess) const
{
  m_schedule->aPathHasFinished(iSuccess);
}


void 
Schedule::processPresentPath(Path* iPath) {
  //printf("Schedule::processPresentPath %u\n",iIndex);      
  if(*(m_fatalJobErrorOccuredPtr)) {
    m_scheduleCallback(false);
    return;
  }
  iPath->runAsync(m_pathDoneCallback);
}

