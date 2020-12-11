//
//  Schedule.cpp
//  DispatchProcessingDemo
//
//  Created by Chris Jones on 9/18/11.
//  Copyright 2011 FNAL. All rights reserved.
//

#include <iostream>

#include <cassert>

#include "Schedule.h"
#include "Path.h"
#include "FilterWrapper.h"
#include "WaitingTaskHolder.h"
#include "task_helpers.h"

using namespace demo;


Schedule::Schedule()
  : m_event(),
  m_allPathsDoneTask(nullptr){
}

Schedule::Schedule(Event* iEvent):
m_event(*iEvent),
m_allPathsDoneTask(nullptr)
{  
}

Schedule::Schedule(const Schedule& iOther):
m_event(iOther.m_event),
m_allPathsDoneTask(iOther.m_allPathsDoneTask.fatalJobErrorOccurredPtr())
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

Schedule::AllPathsDoneTask::AllPathsDoneTask(std::atomic<bool>* iFatalError):
  m_fatalJobErrorOccuredPtr(iFatalError) {}
inline void Schedule::AllPathsDoneTask::set(WaitingTaskHolder iTask) {
  m_task = std::move(iTask);
}
void Schedule::AllPathsDoneTask::execute() {
  if(exceptionPtr()) {
    *(m_fatalJobErrorOccuredPtr) = true;
    m_task.doneWaiting(*exceptionPtr());
  } else {
    m_task.doneWaiting(std::exception_ptr{});
  }
}
void Schedule::AllPathsDoneTask::recycle() {}

void 
Schedule::processAsync(WaitingTaskHolder iCallback) {
  //printf("Schedule::process\n");
  reset();

  if(!m_paths.empty()) {
    auto& group = iCallback.group();
    m_allPathsDoneTask.set(std::move(iCallback));
/*    auto allPathsDone = make_waiting_task(
                                          [this, h=std::move(iCallback)](std::exception_ptr* const iExcept) mutable
                                          {
                                            if(iExcept) {
                                              *(m_fatalJobErrorOccuredPtr) = true;
                                              h.doneWaiting(*iExcept);
                                            } else {
                                              h.doneWaiting(std::exception_ptr{});
                                            }
					    }); */
    WaitingTaskHolder tmp(group, &m_allPathsDoneTask);
     
    for(auto& path : m_paths) {
      path->runAsync( tmp );
    }
  } else {
    iCallback.doneWaiting(std::exception_ptr{});
  }
}

void 
Schedule::addPath(Path* iPath) {
  m_paths.push_back(std::shared_ptr<Path>(iPath));
  iPath->setFatalJobErrorOccurredPointer(m_allPathsDoneTask.fatalJobErrorOccurredPtr());
}

void
Schedule::addPath(const std::vector<std::string>& iPath) {
  auto newPath = std::make_unique<Path>();
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


