//
//  Schedule.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>
#include <dispatch/dispatch.h>

#include <assert.h>

#include "Schedule.h"
#include "Path.h"
#include "FilterWrapper.h"

using namespace demo;


Schedule::Schedule()
  : m_event(),
  m_allPathsDoneGroup(dispatch_group_create()),
  m_callback(this),
  m_scheduleCallback(),
  m_fatalJobErrorOccuredPtr(0){
}

Schedule::Schedule(Event* iEvent):
m_event(*iEvent),
m_allPathsDoneGroup(dispatch_group_create()),
m_callback(this),
m_scheduleCallback(),
m_fatalJobErrorOccuredPtr(0)
{  
}

Schedule::Schedule(const Schedule& iOther):
m_event(iOther.m_event),
m_allPathsDoneGroup(dispatch_group_create()),
m_callback(this),
m_scheduleCallback(),
m_fatalJobErrorOccuredPtr(iOther.m_fatalJobErrorOccuredPtr)
{
  m_filters.reserve(iOther.m_filters.size());
  for(boost::shared_ptr<FilterWrapper> fw: iOther.m_filters) {
    m_filters.push_back(boost::shared_ptr<FilterWrapper>(new FilterWrapper(*fw,&m_event)));
  }
  
  m_paths.reserve(iOther.m_paths.size());
  for(std::vector<PathContext>::const_iterator it = iOther.m_paths.begin(), itEnd = iOther.m_paths.end();
      it != itEnd;
      ++it) {
    addPath((*it).path->clone(m_filters,&m_event));
  }
}

void
Schedule::do_schedule_callback_f(void* iContext){
  Schedule* that = reinterpret_cast<Schedule*>(iContext);
  that->m_scheduleCallback(not *that->m_fatalJobErrorOccuredPtr);
}

void 
Schedule::process(ScheduleFilteringCallback iCallback) {
  m_scheduleCallback = iCallback;
  //printf("Schedule::process\n");
  reset();
  if(!m_paths.empty()) {
    for(std::vector<PathContext>::iterator it = m_paths.begin(), itEnd = m_paths.end(); 
        it != itEnd; ++it) {
      dispatch_group_async_f(m_allPathsDoneGroup.get(),dispatch_get_global_queue(0, 0),
                             &(*it),&Schedule::processPresentPath);
    }
    dispatch_group_notify_f(m_allPathsDoneGroup.get(), dispatch_get_global_queue(0, 0), this, do_schedule_callback_f);
  } else {
    m_scheduleCallback(true);
  }
}

void 
Schedule::addPath(Path* iPath) {
  m_paths.push_back(PathContext(this,iPath));
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
Schedule::reset_f(void* iContext, size_t iIndex){
  Schedule* that = reinterpret_cast<Schedule*>(iContext);
  that->m_paths[iIndex].path->reset();
}

void 
Schedule::reset() {
  for(auto fw: m_filters) {
    fw->reset();
  }
  for(auto& p: m_paths) {
    p.path->reset();
  }
  //dispatch_apply_f(m_paths.size(), dispatch_get_global_queue(0, 0), this, &Schedule::reset_f);
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
PathFilteringCallback::operator()(bool iSuccess) const
{
  if(!iSuccess) {
    *(m_schedule->m_fatalJobErrorOccuredPtr) = true;
  }
  dispatch_group_leave(m_schedule->m_allPathsDoneGroup.get());
}

void 
Schedule::processPresentPath(void*iContext) {
  PathContext* pc = reinterpret_cast<PathContext*>(iContext);
  //printf("Schedule::processPresentPath %u\n",iIndex);      
  if(*(pc->schedule->m_fatalJobErrorOccuredPtr)) {
    return;
  }
  //Enter the group here and leave once the path has finished
  dispatch_group_enter(pc->schedule->m_allPathsDoneGroup.get());
  
  //dispatch_group_t groupPtr = m_allPathsDoneGroup.get();
  pc->path->runAsync(pc->schedule->m_callback);
}
